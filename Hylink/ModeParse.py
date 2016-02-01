import ply.yacc as yacc
import ply.lex as lex
from tree import print_tree, show_tree
from node import Node
import re
import logging
logging.basicConfig(
    level = logging.DEBUG,
    filename = "parselog.txt",
    filemode = "w",
    format = "%(filename)10s:%(lineno)4d:%(message)s"
)
log = logging.getLogger()

ls_nodes = []
ls_values = []

class LSLexer:
    tokens = ('IDENTIFIER',
          'DURING',
          'ENTRY',
          'EXIT',
          'BIND',
          'EQUALS',
          'INTEGER',
          'FLOAT',
          'SEMICOLON',
          'COLON',
          'PLUS',
          'MINUS',
          'TIMES',
          'DIVIDE',
          'LPAREN',
          'RPAREN',
          'COMMA',
          'FUNC',
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
    t_EQUALS = r'='
    t_SEMICOLON = r';'
    t_COLON = r':'
    t_PLUS = r'\+'
    t_MINUS = r'-'
    t_TIMES = r'\*'
    t_DIVIDE = r'/'
    t_LPAREN = r'\('
    t_RPAREN = r'\)'
    t_COMMA = r','
    #t_COS = r'cos'
    
    def t_IDENTIFIER(self,t):
        r'[a-zA-Z_][_A-Za-z0-9]*'
        if (t.value=="entry" or t.value=="en"):
            t.type = 'ENTRY'
            return t
        elif (t.value=="exit" or t.value=="ex"):
            t.type = 'EXIT'
            return t
        elif (t.value=="during" or t.value=="du"):
            t.type = 'DURING'
            return t
        elif (t.value=="bind"):
            t.type = 'BIND'
            return t
        elif (t.value=="cos" or t.value=="COS" or t.value=='sin' or t.value=='SIN'):
            t.type = 'FUNC'
            return t
        else:
            return t

    def t_FLOAT(self,t):
        r'[0-9]+\.[0-9]*'
        t.value = float(t.value)
        return t
    
    def t_INTEGER(self,t):
        r'[0-9]+'
        t.value = int(t.value)
        return t

    def t_error(self,t):
        t.lexer.skip(1)
    
    def build(self,**kwargs):
        self.lexer = lex.lex(module=self, **kwargs)
        #self.lexer=lex.lex(module=self,debug=True,debuglog=log)
        return self.lexer
        
    def test(self,data):
        self.lexer.input(data)
        while True:
            tok = self.lexer.token()
            if not tok: break
            print tok

class LSParser:
    
    tokens = LSLexer().tokens
    
    def p_Action(self,p):
        'Action : IDENTIFIER entry during exit bind'
        p[0] = Node("SActions",[Node("Mode",None,p[1])]+[p[2]]+[p[3]]+[p[4]]+[p[5]])

    def p_empty(self,p):
        'empty :'
        pass

    def p_entry(self,p):
        '''entry : ENTRY COLON Exprs
                 | ENTRY COLON
                 | empty'''
        if (len(p)==4):
            p[0] = Node("Entry")
        else:
            p[0] = Node("Entry")
        
    def p_during(self,p):
        '''during : DURING COLON Exprs
                  | DURING COLON
                  | empty'''
        if (len(p)==4):
            p[0] = Node("During",p[3])
        else:
            p[0] = Node("During")
        
    def p_exit(self,p):
        '''exit : EXIT COLON Exprs
                | EXIT COLON
                | empty'''
        if (len(p)==4):
            p[0] = Node("Exit",p[3])
        else:
            p[0] = Node("Exit")
            
    def p_bind(self,p):
        '''bind : BIND COLON binds
                | BIND COLON
                | empty'''
        if (len(p)==4):
            p[0] = Node("Bind",p[3])
        else:
            p[0] = Node("Bind")
            
    def p_binds(self,p):
        '''binds : IDENTIFIER COMMA IDENTIFIER SEMICOLON binds
                 | IDENTIFIER COMMA IDENTIFIER COMMA binds
                 | IDENTIFIER COMMA IDENTIFIER binds
                 | IDENTIFIER COMMA IDENTIFIER'''
        if (len(p) == 6):
            p[0] = [Node("Binds",[Node(p[1]),Node(p[3])])]+p[5]
        elif (len(p) == 5):
            p[0] = [Node("Binds",[Node(p[1]),Node(p[3])])]+p[4]
        elif (len(p) == 4):
            p[0] = [Node("Binds",[Node(p[1]),Node(p[3])])]
        
    def p_Exprs(self,p):
        '''Exprs    : Assgn Exprs
                    | Assgn
                    | BCast Exprs
                    | BCast''' 
        if (len(p) == 2):
            p[0] = [p[1]]
        elif (len(p) == 3):
            p[0] = [p[1]]+p[2]
    
    def p_Assgn(self,p):
        '''Assgn  : IDENTIFIER EQUALS expr SEMICOLON
                  | IDENTIFIER EQUALS expr COMMA
                  | IDENTIFIER EQUALS expr'''
        p[0] = Node("Assignment",[Node('Identifier',None,p[1]),p[3]],p[2])
    
    def p_BCast(self,p):
        '''BCast  : IDENTIFIER SEMICOLON
                  | IDENTIFIER COMMA
                  | IDENTIFIER'''
        p[0] = Node("Broadcast",[Node('Identifier',None,p[1])])
    
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
                  | MINUS factor
                  | FLOAT
                  | INTEGER
                  | LPAREN expr RPAREN
                  | FUNC LPAREN expr RPAREN'''
        if (len(p) == 2):
            p[0] = Node("Identifier",None,p[1])
        elif (len(p) == 4):
#            p[0] = Node("Expression",[p[2]],(p[1],p[3]))
            #p[0] = p[2]
            p[0] = Node("parenthesis",[p[2]],'()')
        elif (len(p) == 3):
            p[0] = Node("Negative",[p[2]],'-')
        elif (len(p) == 5):
            #print(p[1])
            if p[1] == 'cos':
                p[0] = Node("Function",[p[3]],'COS')
            elif p[1] == 'COS':
                p[0] = Node("Function",[p[3]],'COS')
            elif p[1] == 'sin':
                p[0] = Node("Function",[p[3]],'SIN')
            elif p[1] == 'SIN':
                p[0] = Node("Function",[p[3]],'SIN')

    
    def p_error(self,p):
        print "Syntax Error in ModeParse at " + str(p)
        yacc.errok()
    
    def build_and_parse(self,data,**kwargs):
        #log = logging.getLogger()
        #parser = yacc.yacc(module=self,debug=True,debuglog=log)
        #result = parser.parse(data,debug=log,**kwargs)
        parser = yacc.yacc(module=self)
        result = parser.parse(data,**kwargs)
        return result

def build_lstring(node):
    if node.type == 'SFState':
        ls_nodes.append(node)
    for i in node.children:
        build_lstring(i)
        
def lstring(tree):
    build_lstring(tree)
    for i in ls_nodes:
        ls_values.append((i.children[0].value,i.children[1].value))
    print len(ls_values)
    for i in ls_values:
        print i[1]


def lstring2(data):
    
    if not data[0] == '@':
        lxr = LSLexer().build()
        result = LSParser().build_and_parse(data, lexer=lxr)
        #print('HAHAHAHAHAHAHAHAH')
        return result
    else:
        return data

def parse_action(data):
    tmp = "trans\\ndu:" + data
    tmp2 = lstring2(tmp)
    return tmp2.children[2].children

def parse_guard(data):
    rel = data.rfind('<=')
    if rel == -1:
        rel = re.search(r'>=|==|!=|>|<',data)
        rel = rel.group(0)
    else:
        rel = data[rel:rel+2]
    new_data = data.replace(rel,'=')
    ndp = parse_action(new_data)
    ndp[0].type = 'Relational'
    ndp[0].value = rel
    return ndp[0]
'''
inputtest = "Brussellator\ndu:\nx_dot = 1 + x*x*y - 2.5*x;\ny_dot = 1.5*x - x*x*y;\nx_out = x;\ny_out = y;"
inputtest2 = 'Brussellator\ndu:\nx_dot = 1 + x*x*(y - 2.5);'
inputtest3 = 'Brussellator\ndu:\nx_dot = 1 + cos(x+4)+sin(y*5*(3+x));'



lxr = LSLexer().build()
lxr.input(inputtest)
while True:
    tok = lxr.token()
    if not tok:
        break
    print(tok)



result = lstring2(inputtest)
result.bolunprints()
'''