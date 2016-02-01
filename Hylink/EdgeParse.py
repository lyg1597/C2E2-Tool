import ply.yacc as yacc
import ply.lex as lex
from tree import print_tree, show_tree
from ModeParse import parse_action,parse_guard
from guardParse import parse_guardExp
from node import Node
import logging
#logging.basicConfig(
 #   level = logging.DEBUG,
  #  filename = "edparselog.txt",
   # filemode = "w",
    #format = "%(filename)10s:%(lineno)4d:%(message)s"
#)
#log = logging.getLogger()
        
class LSLexer:
    tokens = ('LPAREN',
              'RPAREN',
              'LBRACK',
              'RBRACK',
              'STRING',
              'SEMICOLON',
              'LAND',
              'LOR'
              )
    
    states = (
        ('comment','exclusive'),
    )
    
    def t_comment(self,t):
        r'//'
        t.lexer.begin('comment')
        
    def t_comment_nline(self,t):
        r'\\n'
        t.lexer.begin('INITIAL')
        
    def t_comment_error(self,t):
        t.lexer.skip(1)
        
    t_ignore_NEWLINE = r'\\n|\n'
    t_LPAREN = r'\{'
    t_RPAREN = r'\}'
    t_LBRACK = r'\['
    t_RBRACK = r'\]'
    t_STRING = r'[^\[\]\{\};\|&]+'
    t_SEMICOLON = r';'

    def t_LAND(self,t):
        r'&& | and | &'
        t.value = "and"
        return t
        
    def t_LOR(self,t):
        r'\|\| | or | \|'
        t.value = "or"
        return t

    def t_error(self,t):
        t.lexer.skip(1)
    
    def build(self,**kwargs):
        self.lexer = lex.lex(module=self, **kwargs)
        #self.lexer=lex.lex(module=self,debug=True,debuglog=log)
        return self.lexer
            
class LSParser:
    
    tokens = LSLexer().tokens
    
    def p_trans(self,p):
        '''trans : LBRACK precondn RBRACK LPAREN actions RPAREN
                | LBRACK precondn RBRACK
                | LPAREN actions RPAREN'''
        if (len(p)==7):
            p[0] = Node("Transition",p[2]+p[5])
        else:
            p[0] = Node("Transition",p[2])
                    
    def p_actions(self,p):
        '''actions : STRING SEMICOLON actions
                    | STRING SEMICOLON
                    | STRING'''
        if (len(p)==4):
            p[0] = parse_action(p[1])+p[3]
        else:
            p[0] = parse_action(p[1])
        
    def p_precondn(self,p):
        '''precondn : STRING
                    | STRING LOR precondn
                    | STRING LAND precondn'''
        if len(p)==2:
            p[0] = [parse_guardExp(p[1])]
        else:
            p[0] = [Node('Logical',[parse_guardExp(p[1])]+p[3],p[2])]
            
    def p_error(self,p):
        print "Syntax Error in EdgeParse at " + str(p)
        yacc.errok()
        
    def build_and_parse(self,data,**kwargs):
        #log = logging.getLogger()
        #parser = yacc.yacc(module=self,debug = True, debuglog = log)
        #result = parser.parse(data, debug=log, **kwargs)
        #return result
        parser = yacc.yacc(module=self)
        result = parser.parse(data,**kwargs)
        return result
    
def lstring3(data):
    lxr = LSLexer().build()
    #print data+" \n"
    result = LSParser().build_and_parse(data, lexer=lxr)
    return result

#data = '[x < xmin && y > z || z < 10*x]{x = xmin+(2*z)}'
#lxr = LSLexer().build()
#lxr.input(data)
#for tok in lxr:
#    print tok
#node = lstring3(data)
#show_tree(node)
#print foo(node.children[0])
#negate(node.children[0])
#print foo(node.children[0])
#show_tree(node)