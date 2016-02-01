import ply.yacc as yacc
import ply.lex as lex
from tree import print_tree, show_tree
from node import Node
        
class LSLexer:
        tokens = ('ENTRY',
                  'DURING',
                  'EXIT',
                  'COLON',
                  'SEMICOLON',
                  'STRING',
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
        t_COLON = r":"
        t_SEMICOLON = r';'
        
        def t_STRING(self,t):
            r'[a-zA-Z0-9+\-*/=_\. ]+'
            if (t.value=="entry" or t.value=="en"):
                t.type = 'ENTRY'
                return t
            elif (t.value=="exit" or t.value=="ex"):
                t.type = 'EXIT'
                return t
            elif (t.value=="during" or t.value=="du"):
                t.type = 'DURING'
                return t
            else:
                return t
            
        def t_error(self,t):
            t.lexer.skip(1)
            
        def build(self,**kwargs):
            self.lexer = lex.lex(module=self, **kwargs)
            return self.lexer
        
class LSParser:
    
    tokens = LSLexer().tokens
    
    def p_mdais(self,p):
        '''mdais : STRING entry during exit'''
        p[0] = Node("ModeDAIs",[p[2]]+[p[3]]+[p[4]])
        
    def p_empty(self,p):
        'empty :'
        pass
    
    def p_entry(self,p):
        '''entry : ENTRY COLON dais
                | empty'''
        if (len(p)==4):
            p[0] = Node("EntryDAIs",p[3])
        else:
            p[0] = Node("EntryDAIs")
                
    def p_during(self,p):
        '''during : DURING COLON dais
                | empty'''
        if (len(p)==4):
            p[0] = Node("DuringDAIs",p[3])
        else:
            p[0] = Node("DuringDAIs")

    def p_exit(self,p):
        '''exit : EXIT COLON dais
                | empty'''
        if (len(p)==4):
            p[0] = Node("ExitDAIs",p[3])
        else:
            p[0] = Node("ExitDAIs")
    
    def p_dais(self,p):
        '''dais : STRING SEMICOLON dais
                | empty'''
        if (len(p)==4):
            p[0] = [Node("DAI",None,p[1])]+p[3]
        else:
            p[0] = []
            
    def p_error(self,p):
        print "Syntax error in file parse_modes at token"
        print p
        yacc.errok()
        
    def build_and_parse(self,data,**kwargs):
        parser = yacc.yacc(module=self)
        result = parser.parse(data,**kwargs)
        return result


def extract_dais(data):
    lxr = LSLexer().build()
    result = LSParser().build_and_parse(data, lexer=lxr)
    return result

#data = "mode2\\ndu:\\nx_dot=2;\\ny_dot=1;\\nz=2*x;"
#show_tree(extract_dais(data))