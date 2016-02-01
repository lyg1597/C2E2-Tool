import ply.yacc as yacc
import ply.lex as lex
from tree import print_tree, show_tree
from node import Node
import re
import logging
import time
from lxml import etree


logging.basicConfig(
    level = logging.DEBUG,
    filename = "parselog.txt",
    filemode = "w",
    format = "%(filename)10s:%(lineno)4d:%(message)s"
)
log = logging.getLogger()


class LSLexer:
    tokens = ('IDENTIFIER',
          'EQUALS',
          'INTEGER',
          'FLOAT',
          'SEMICOLON',
          'PLUS',
          'MINUS',
          'TIMES',
          'DIVIDE',
          'LPAREN',
          'RPAREN',
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
    t_PLUS = r'\+'
    t_MINUS = r'-'
    t_TIMES = r'\*'
    t_DIVIDE = r'/'
    t_LPAREN = r'\('
    t_RPAREN = r'\)'
    #t_COS = r'cos'
    
    def t_IDENTIFIER(self,t):
        r'[a-zA-Z_][_A-Za-z0-9]*'
        if (t.value=="cos" or t.value=="COS" or t.value=='sin' or t.value=='SIN' or t.value=='ln' or t.value=='LN'):
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

    def p_Assgn(self,p):
        '''Assgn  : IDENTIFIER EQUALS expr SEMICOLON
                  | IDENTIFIER EQUALS expr'''
        p[0] = Node("Assignment",[Node('Identifier',None,p[1]),p[3]],p[2])
    
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
                | term TIMES TIMES factor
                | factor'''
        if (len(p) == 4):
            p[0] = Node("Expression",[p[1],p[3]],p[2])
        elif (len(p) == 5):
        	p[0] = Node("Expression",[p[1],p[4]],'POW')
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
        	var = 0
        	try:
        		float(p[1])
        	except ValueError:
        		var = 1
        	if var == 1:
        		p[0] = Node("Variable",None,p[1])
        	else:
        		p[0] = Node("Constant",None,p[1])
        	#p[0] = Node("Identifier",None,p[1])
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
        	elif p[1] == 'ln':
        		p[0] = Node("Function",[p[3]],'LN')
        	elif p[1] == 'LN':
        		p[0] = Node("Function",[p[3]],'LN')

    
    def p_error(self,p):
        print "Syntax Error in ModeParse at " + str(p)
        yacc.errok()
    
    def build_and_parse(self,data,**kwargs):
        log = logging.getLogger()
        parser = yacc.yacc(module=self)
        #result = parser.parse(data,debug=log)
        result = parser.parse(data,debug=log,**kwargs)
        return result


def parse(data):
    
    if not data[0] == '@':
        lxr = LSLexer().build()
        result = LSParser().build_and_parse(data, lexer=lxr)
        #result.bolunprints()
        return result
    else:
        return data
'''

def derivative(respect,function,multi=0,sibling=None,parenthesis=0,div=0,power=0):
	#print(debug)
	if power == 1:
		value = str(float(sibling.value)-1)
		if function.value == '()':
			return Node('Expression',[Node('Expression',[function,Node('children',None,value)],'^'),derivative(respect,function.children[0],0,None,1)],'*')
		else:
			return Node('Expression',[Node('Expression',[function,Node('children',None,value)],'^'),derivative(respect,function)],'*')


	if parenthesis==1:
		if div == 1:
			return Node('parenthesis',[derivative(respect,function,0,sibling,0,1)],'()')
		else:
			return Node('parenthesis',[derivative(respect,function)],'()')
	if div==1 and parenthesis==0:
		return Node('Expression',[derivative(respect,function,1,sibling),derivative(respect,sibling,1,function)],'-')
	if multi==1:
		if function.value == '()':
			return Node('Expression',[derivative(respect,function.children[0],0,None,1),sibling],'*')
		else:
			return Node('Expression',[derivative(respect,function),sibling],'*')
	if function.value == '+':
		return Node('Expression',[derivative(respect,function.children[0]),derivative(respect,function.children[1])],'+')
	elif function.value == '*':
		return Node('Expression',[derivative(respect,function.children[0],1,function.children[1]),derivative(respect,function.children[1],1,function.children[0])],'+')
	elif function.value == '-' and function.type =='Expression':
		return Node('Expression',[derivative(respect,function.children[0]),derivative(respect,function.children[1])],'-')
	elif function.value == 'COS':
		return Node('Expression',[Node('Function',[function.children[0]],'-SIN'),derivative(respect,function.children[0],0,None,1)],'*')
	elif function.value == 'SIN':
		return Node('Expression',[Node('Function',[function.children[0]],'COS'),derivative(respect,function.children[0],0,None,1)],'*')
	elif function.value == 'LN':
		return Node('Expression',[Node('Expression',[Node('Constant',None,'1'),function.children[0]],'/'),derivative(respect,function.children[0])],'*')
	elif function.value == '-' and function.type =='Negative':
		return Node('Negative',[derivative(respect,function.children[0])],'-()')
	elif function.value == '/':
		return Node('Expression',[derivative(respect,function.children[0],1,function.children[1],0,1),Node('Expression',[function.children[1],Node('Constant',None,'2')],'^')],'/')
	elif function.value == '^'and power ==0:
		return Node('Expression',[Node('Constant',None,function.children[1].value),derivative(respect,function.children[0],0,function.children[1],0,0,1)],'*')
	elif function.children == []:
		if function.type == 'Variable' and function.value == respect:
			return Node('Constant',None,'1')
		if function.type == 'Constant':
			return Node('Constant',None,'0')
		else:
			return Node('Constant',None,'0')





def simplify(node):
	if node.children != []:
		if len(node.children) == 2:
			simplify(node.children[0])
			simplify(node.children[1])
		else:
			simplify(node.children[0])

		if node.value == '^' and node.children[1].value =='1.0':
			node.type = node.children[0].type
			node.value = node.children[0].value
			node.children = node.children[0].children

		if node.value == '^' and node.children[1].value =='1':
			node.type = node.children[0].type
			node.value = node.children[0].value
			node.children = node.children[0].children

		if node.value == '+' and node.children[0].value =='0':
			node.type = node.children[1].type
			node.value = node.children[1].value
			node.children = node.children[1].children
			return	
		if node.value == '+' and node.children[1].value =='0':
			node.type = node.children[0].type
			node.value = node.children[0].value
			node.children = node.children[0].children
			return
		if node.value == '*' and node.children[1].value =='1':
			node.type = node.children[0].type
			node.value = node.children[0].value
			node.children = node.children[0].children
			return
		if node.value == '*' and node.children[0].value =='1':
			node.type = node.children[1].type
			node.value = node.children[1].value
			node.children = node.children[1].children
			return	
		if node.value == '*' and node.children[0].value =='0':
			node.type = 'Constant'
			node.value = '0'
			node.children =[]
		if node.value == '*' and node.children[1].value =='0':
			node.type = 'Constant'
			node.value = '0'
			node.children =[]
		if node.value == '+' and node.children[1].type == 'Constant' and node.children[0].type == 'Constant':
			node.type = 'Constant'
			node.value = str(int(node.children[0].value)+int(node.children[1].value))
			node.children=[]
		if node.value == '*' and node.children[1].type == 'Constant' and node.children[0].type == 'Constant':
			node.type = 'Constant'
			node.value = str(int(node.children[0].value)*int(node.children[1].value))
			node.children=[]

		if len(node.children) == 2:

			if node.children[0].value=='()':
				if node.value == '+' and node.children[0].children[0].value =='0':
					node.type = node.children[1].type
					node.value = node.children[1].value
					node.children = node.children[1].children
				if node.value =='+' and node.children[1].value =='0':
					node.type = node.children[0].children[0].type
					node.value = node.children[0].children[0].value
					node.children = node.children[0].children[0].children
				if node.value == '*' and node.children[0].children[0].value == '1':
					node.type = node.children[1].type
					node.value = node.children[1].value
					node.children = node.children[1].children
				if node.value == '*' and node.children[1].value == '1':
					node.type = node.children[0].children[0].type
					node.value = node.children[0].children[0].value
					node.children = node.children[0].children[0].children
				if node.value == '*' and node.children[0].children[0].value == '0':
					node.type = 'Constant'
					node.value = '0'
					node.children = []
				if node.value == '*' and node.children[1].value == '0':
					node.type = 'Constant'
					node.value = '0'
					node.children = []
				if node.value == '*' and node.children[1].type == 'Constant' and node.children[0].children[0].type =='Constant':
					node.type = 'Constant'
					node.value = str(int(node.children[0].children[0].value)*int(node.children[1].value))
					node.children=[]
				if node.value == '+' and node.children[1].type == 'Constant' and node.children[0].children[0].type =='Constant':
					node.type = 'Constant'
					node.value = str(int(node.children[0].children[0].value)+int(node.children[1].value))
					node.children=[]
			elif node.children[1].value=='()':
				if node.value == '+' and node.children[1].children[0].value =='0':
					node.type = node.children[0].type
					node.value = node.children[0].value
					node.children = node.children[0].children
				if node.value =='+' and node.children[0].value =='0':
					node.type = node.children[1].children[0].type
					node.value = node.children[1].children[0].value
					node.children = node.children[1].children[0].children
				if node.value == '*' and node.children[1].children[0].value == '1':
					node.type = node.children[0].type
					node.value = node.children[0].value
					node.children = node.children[0].children
				if node.value == '*' and node.children[0].value == '1':
					node.type = node.children[1].children[0].type
					node.value = node.children[1].children[0].value
					node.children = node.children[1].children[0].children
				if node.value == '*' and node.children[1].children[0].value == '0':
					node.type = 'Constant'
					node.value = '0'
					node.children = []
				if node.value == '*' and node.children[0].value == '0':
					node.type = 'Constant'
					node.value = '0'
					node.children = []
				if node.value == '*' and node.children[0].type == 'Constant' and node.children[1].children[0].type =='Constant':
					node.type = 'Constant'
					node.value = str(int(node.children[1].children[1].value)*int(node.children[0].value))
					node.children=[]
				if node.value == '+' and node.children[0].type == 'Constant' and node.children[1].children[0].type =='Constant':
					node.type = 'Constant'
					node.value = str(int(node.children[1].children[0].value)+int(node.children[0].value))
					node.children=[]
			if node.value == '+' and node.children[0].value == '()':
				node.children[0].value = node.children[0].children[0].value
				node.children[0].type = node.children[0].children[0].type
				node.children[0].children = node.children[0].children[0].children
			if node.value == '+' and node.children[1].value == '()':
				node.children[0].value = node.children[1].children[0].value
				node.children[0].type = node.children[1].children[0].type
				node.children[0].children = node.children[1].children[0].children

	else:
		return
'''
def recursion_to_hyxml(node,parent):

	child = etree.SubElement(parent,str(node.type),{'value':str(node.value)})
	if node.children==[]:
		return
	if len(node.children)==2:
		recursion_to_hyxml(node.children[0],child)
		recursion_to_hyxml(node.children[1],child)
	else:
		recursion_to_hyxml(node.children[0],child)

def converthyxml(nodelist,dvl):
	#print('HAHA')
	print(dvl)


	hyxml =etree.Element("hyxml")
	order =etree.SubElement(hyxml,'Ordering')
	for i in range (len(dvl)):
		etree.SubElement(order,'variable',{'value':dvl[i]})

	for node in nodelist:
		recursion_to_hyxml(node,hyxml)
	strvalue = etree.tostring(hyxml,pretty_print=True,xml_declaration=True,encoding="utf-8",doctype="<!DOCTYPE hyxml>")
	#print(strvalue)

	saveFile = open("../wd/Jacobiantree.hyxml",'w')
	saveFile.write(strvalue)
	saveFile.close()
	
