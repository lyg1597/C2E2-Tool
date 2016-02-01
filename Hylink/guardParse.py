import ply.yacc as yacc
import ply.lex as lex
#from tree import print_tree, show_tree
from node import Node
import re


class ExpLexer:
    tokens = ('IDENTIFIER',
          'INTEGER',
          'FLOAT',
          'PLUS',
          'MINUS',
          'TIMES',
          'DIVIDE',
          'LPAREN',
          'RPAREN',
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

    t_ignore_NEWLINE = r'\\n | \n'
    t_PLUS = r'\+'
    t_MINUS = r'-'
    t_TIMES = r'\*'
    t_DIVIDE = r'/'
    t_LPAREN = r'\('
    t_RPAREN = r'\)'
    
    def t_IDENTIFIER(self,t):
        r'[a-zA-Z_][_A-Za-z0-9]*'
        return t

    def t_FLOAT(self,t):
        r'[0-9]+\.[0-9]*'
        #t.value = float(t.value)
        return t
    
    def t_INTEGER(self,t):
        r'[0-9]+'
        #t.value = int(t.value)
        return t

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

    
class ExpParser:
    
    tokens = ExpLexer().tokens
    
    precedence = (
    ('left','PLUS','MINUS'),
    ('left','TIMES','DIVIDE'),
    )
    
    def p_expr(self,p):
        '''expr : expr PLUS term
                | expr MINUS term
                | term'''
        if (len(p) == 4):
            p[0] = Node("Expression",[p[1],p[3]],p[2])
        elif (len(p) == 2):
            p[0] = p[1]
    
    def p_term(self,p):
        '''term : term TIMES factor
                | term DIVIDE factor
                | factor'''
        if (len(p) == 4):
            p[0] = Node("Expression",[p[1],p[3]],p[2])
        elif (len(p) == 2):
            p[0] = p[1]
    
    def p_factor(self,p):
        '''factor : IDENTIFIER
                  | MINUS IDENTIFIER
                  | MINUS FLOAT
                  | MINUS INTEGER
                  | FLOAT
                  | INTEGER
                  | LPAREN expr RPAREN'''
        if (len(p) == 2):
            p[0] = Node("Identifier",None,p[1])
        elif (len(p) == 4):
            p[0] = Node("Expression",[p[2]],(p[1],p[3]))
            p[0] = p[2]
        elif (len(p) == 3):
            p[0] = Node("Negative",[p[2]],'-')
    
    def p_error(self,p):
        print "Syntax Error in GuardParse at " + str(p)
        yacc.errok()
    
    def build_and_parse(self,data,**kwargs):
        parser = yacc.yacc(module=self)
        result = parser.parse(data,**kwargs)
        return result

def parse_function(data):
    
    expressionLexer = ExpLexer().build()
    result = ExpParser().build_and_parse(data, lexer=expressionLexer)
    #result.prints()
    
    return result
    

def parse_guardExp(data):
    
    rel = re.search(r'>=|==|<=|!=',data)
    if rel:
        sym = rel.group(0)
        pos = data.rfind(sym)
    else:
        rel = re.search(r'>|<',data)
        sym = rel.group(0)
        pos = data.rfind(sym)
    
    new_data1 = data[0:pos]
    new_data2 = data[pos+len(sym):len(data)]

    #print "--this is getting parsed for guards -- "+new_data1+"\n"
    #print "--this is getting parsed for guards -- "+new_data2+"\n"
    
    ndp1 = parse_function(new_data1)
    ndp2 = parse_function(new_data2)
        
    ndp = Node('Relational',[ndp1,ndp2],sym)
    #ndp.prints()
    
    return ndp

#N = parse_guardExp("x<=1")
#N.prints()
#parse_guardLogicalExp("x<=1 or vx>=0")
