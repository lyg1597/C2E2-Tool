import ply.yacc as yacc
import ply.lex as lex
from node import Node
from guardParse import parse_guardExp

class ExpLexer2:
    
    tokens = ('STRING', 'LAND', 'LOR',)
    
    states = ( ('comment','exclusive'), )
    
    def t_comment(self,t):
        r'//'
        t.lexer.begin('comments')
    
    def t_comment_nline(self,t):
        r'\\n'
        t.lexer.begin('INITIAL')

    def t_comment_error(self,t):
        t.lexer.skip(1)

    t_ignore_NEWLINE = r'\\n | \n'

    def t_LAND(self,t):
        r'&& | and | &'
        t.value = "and"
        return t
        
    def t_LOR(self,t):
        r'\|\| | or | \|'
        t.value = "or"
        return t

    t_STRING = r'[^\[\]\{\};\|&]+'

    def t_error(self,t):
        t.lexer.skip(1)
       
    def build(self,**kwargs):
        self.lexer = lex.lex(module=self, **kwargs)
        return self.lexer
        
    def test(self,data):
        self.lexer.input(data)
        while True:
            tok = self.lexer.token()
            if not tok: break
            print tok
    
class ExpParser2:
    
    tokens = ExpLexer2().tokens

    def p_totExpr(self,p):
        '''totExpr : STRING LAND totExpr
                   | STRING LOR totExpr
                   | STRING'''
        # for i,j in enumerate(p):
        #     print str(i) + ': ' + str(j)
        if (len(p) == 4):
            p[0] = [Node('Logical',[parse_guardExp(p[1])]+p[3],p[2])]
        elif (len(p) == 2):
            p[0] = [parse_guardExp(p[1])]
    
    def p_error(self,p):
        print "Syntax Error in GuardParse at " + str(p)
        yacc.error()
    
    def build_and_parse(self,data,**kwargs):
        parser = yacc.yacc(module=self)
        result = parser.parse(data,**kwargs)
        return result

def parse_function2(data):
    
    expressionLexer = ExpLexer2().build()
    result = ExpParser2().build_and_parse(data, lexer=expressionLexer)
    #result.prints()
    
    return result
    

def parse_guardLogicalExp(data):

    next = parse_function2(data)
    return next

# N = parse_guardLogicalExp("x<=1 || vx>=0")
# N1 = parse_guardLogicalExp("x>=1 && vx <= 0")
# print N[0].prints()
