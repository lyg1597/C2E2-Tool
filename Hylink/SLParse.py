__author__ = 'Kjell Magne Fauske'
__license__ = 'MIT'
__version__ = '1.3'

# A high level grammar of the Simulink mdl file format
SIMULINK_BNF = """
object {
     members
}
members
    variablename  value
    object {
        members
    }
variablename

array
    [ elements ]
matrix
    [elements ; elements]
elements
    value
    elements , value
value
    string
    doublequotedstring
    float
    integer
    object
    array
    matrix
"""

from pyparsing import *
import re
import pprint

# parse actions
def convertNumbers(s,l,toks):
    """Convert tokens to int or float"""
    # Taken from jsonParser.py
    n = toks[0]
    try:
        return int(n)
    except ValueError, ve:
        return float(n)

def joinStrings(s,l,toks):
    """Join string split over multiple lines"""
    return ["".join(toks)]

# Define grammar

# Parse double quoted strings. Ideally we should have used the simple statement:
#    dblString = dblQuotedString.setParseAction( removeQuotes )
# Unfortunately dblQuotedString does not handle special chars like \n \t,
# so we have to use a custom regex instead.
# See http://pyparsing.wikispaces.com/message/view/home/3778969 for details. 
dblString = Regex(r'\"(?:\\\"|\\\\|[^"])*\"', re.MULTILINE)
dblString.setParseAction( removeQuotes )


mdlNumber = Combine( Optional('-') + ( '0' | Word('123456789',nums) ) +
                    Optional( '.' + Word(nums) ) +
                    Optional( Word('eE',exact=1) + Word(nums+'+-',nums) ) )
mdlObject = Forward()
mdlName = Word('$'+'.'+'_'+alphas+nums)
mdlValue = Forward()
# Strings can be split over multiple lines
mdlString = (dblString + Optional(OneOrMore(Suppress(LineEnd()) + LineStart()
             + dblString)))
mdlElements = delimitedList( mdlValue )
mdlArray = Group(Suppress('[') + Optional(mdlElements) + Suppress(']') )
mdlMatrix =Group(Suppress('[') + (delimitedList(Group(mdlElements),';')) \
              + Suppress(']') )
mdlValue << ( mdlNumber | mdlName| mdlString  | mdlArray | mdlMatrix )
memberDef = Group( mdlName  + mdlValue ) | Group(mdlObject)
mdlMembers = OneOrMore( memberDef)
mdlObject << ( mdlName+Suppress('{') + Optional(mdlMembers) + Suppress('}') )
mdlNumber.setParseAction( convertNumbers )
mdlString.setParseAction(joinStrings)
# Some mdl files from Mathworks start with a comment. Ignore all
# lines that start with a #
singleLineComment = Group("#" + restOfLine)
mdlObject.ignore(singleLineComment)
mdlparser = mdlObject


def extract_sl(data):
    result = mdlparser.parseString(data)
    return result
#    pprint.pprint(result.asList())