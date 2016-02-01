import ply.yacc as yacc
import ply.lex as lex
import re,sys
#Will be using the variables from the FourGridNavigation.mdl file: x,y,vx,vy
#The sets will be of the form a1*x+a2*y+a3*vx+a4*vy+b<=c where any of the constants may be 0

#May try to parse data such as 2x or 2.5x correctly


class ExprLexer:
  def __init__(self):
    self.error=0

  tokens=(
    "VARIABLE",
    "NUMBER",
    "PLUS",
    "MINUS",
    "TIMES",
    "DIVIDE",
    "LPAREN",
    "RPAREN",
  )

  t_PLUS=r'\+'
  t_MINUS=r'-'
  t_TIMES=r'\*'
  t_DIVIDE=r'/'
  t_LPAREN=r'\('
  t_RPAREN=r'\)'
  t_ignore=' \t\n'

  def t_VARIABLE(self,t):
    r'[a-zA-Z_][a-zA-Z_0-9]*'
    return t

  def t_NUMBER(self,t):
    r'\d*\.?\d+'
    return t

  def t_error(self,t):
    self.error=-1
    t.lexer.skip(1)

  def build(self,**kwargs):
    self.lexer=lex.lex(module=self,**kwargs)
    return self.lexer

class ExprParser:
  def __init__(self): 
    self.term=[] #(coefficient,variable)
    self.constant=0
    self.negative=1
    self.exprLex=ExprLexer()
    self.lexer=self.exprLex.build()
    self.parser=yacc.yacc(module=self)
    self.error=0
    self.multiplier=0
    self.divisorList=set()

  tokens=ExprLexer().tokens

  precedence=(
    ("left","PLUS","MINUS"),
    ("left","TIMES","DIVIDE"),
  )

  def p_expression(self,p):
    """expression : expression PLUS expression
                  | expression MINUS expression
                  | term"""
    if len(p)==4:
      p[0]=Node(p[2],[p[1],p[3]])
    elif len(p)==2:
      p[0]=p[1]

  def p_term(self,p):
    """term : factor TIMES factor
            | factor DIVIDE factor
            | factor"""
    if len(p)==4:
      p[0]=Node(p[2],[p[1],p[3]])
    elif len(p)==2:
      p[0]=p[1]

  def p_factor(self,p):
    """factor : MINUS LPAREN term RPAREN VARIABLE
              | LPAREN term RPAREN VARIABLE
              | MINUS LPAREN term RPAREN 
              | LPAREN term RPAREN 
              | MINUS NUMBER VARIABLE
              | NUMBER VARIABLE
              | MINUS NUMBER
              | MINUS VARIABLE
              | NUMBER
              | VARIABLE"""

    if len(p)==2:
      if self.isNumber(p[1]):
        p[0]=Node(float(p[1]),0)
      else:
        p[0]=Node(p[1],0)
    elif len(p)==3:
      if p[1]=='-':
        if self.isNumber(p[2]):
          p[0]=Node(-float(p[2]),0)
        else:
          p[0]=Node('*',[Node(float(-1),0),Node(p[2],0)])
      else:
        p[0]=Node('*',[Node(float(p[1]),0),Node(p[2],0)])
    elif len(p)==4:
      if p[1]=='(':
        p[0]=p[2]
      else:
        p[0]=Node('*',[Node(-float(p[2]),0),Node(p[3],0)])
    elif len(p)==5:
      if p[1]=='-':
        p[0]=Node('-',[p[3]])
      else:
        p[0]=Node('*',[p[2],Node(p[4],0)])
    elif len(p)==6:
      p[0]=Node('*',[Node('-',[p[3]]),Node(p[5],0)])

  def p_error(self,p):
    yacc.errok()

  def isNumber(self,value):
    try:
      float(value)
    except ValueError:
      return False
    return True

  def containDecimals(self,value):
    decimals=str(value).split(".")[1]
    if decimals=="0":
      return
    if self.multiplier<len(decimals):
      self.multiplier=len(decimals)
      print(self.multiplier)

  def parseTree(self,node,neg):
    if node.value=="+" or node.value=="-":
      self.parseTree(node.children[0],1)
      if neg==-1 or node.value=="-":
        self.parseTree(node.children[1],-1)
      else:
        self.parseTree(node.children[1],1)
    elif node.value=="*":
      if node.children[0].value=='-':
        div=node.children[0].children[0]
        self.divisorList.add(div.children[1].value)
        val=-div.children[0].value/div.children[1].value
        self.term.append((neg*val,node.children[1].value))
      elif node.children[0].value=='/':
        div=node.children[0]
        self.divisorList.add(div.children[1].value)
        val=div.children[0].value/div.children[1].value
        self.term.append((neg*val,node.children[1].value))
      else:
        self.containDecimals(node.children[0].value)
        self.term.append((neg*node.children[0].value,node.children[1].value))
    elif self.isNumber(node.value):
      self.constant+=neg*node.value
    else:
      self.term.append((neg*1.0,node.value))

  def printNode(self,node):
    print("Value: "+str(node.value)+" children: "+str(len(node.children)))
    for n in node.children:
      self.printNode(n)

  #Fix this so it doesn't use any global vars
  def parse(self,data):
    self.term=[]
    self.constant=0
    result=self.parser.parse(data)
    self.parseTree(result,1)
    return self.term,self.constant

class Node:
  def __init__(self,value,children=None):
    self.value=value
    if children:
      self.children=children
    else:
      self.children=[]

def generateMatrix(varList,modeList,dataSet):
  expr=ExprParser()
  dataList=[]

  parseList=("".join(dataSet.split())).split(':')

  mode=parseList[0]

  inequal=parseList[1].split("&&")

  addIneq=[]
  remIneq=[]
  for j,ineq in enumerate(inequal):
    equal=re.search(r'==',ineq)
    if not equal==None:
      remIneq.append(j)
      addIneq.append(ineq.replace("==","<="))
      addIneq.append(ineq.replace("==",">="))

  for rem in remIneq:
    inequal.pop(rem)
  inequal+=addIneq

  aMatrix=[[0 for v in varList] for s in inequal]
  bMatrix=[0 for x in inequal]
  equality=[]
    
  for j,ineq in enumerate(inequal):
    row=aMatrix[j]
    match=re.search(r'>=?|<=?',ineq).group(0)
    pos=ineq.rfind(match)
    expression=ineq[0:pos]
    constant=ineq[pos+len(match):len(ineq)]
    expression+="-"+constant
    data,term=expr.parse(expression)
    for d in data:
      print(d)
      row[varList.index(d[1])]+=d[0]
      bMatrix[j]=-term
      if bMatrix[j]==0:
        bMatrix[j]=0
    equality.append(match)

  dataList.append([mode,aMatrix,bMatrix,equality])
  return dataList

#This will be from c2e2.py after a file is opened
varList=["x","y","vx","vy"]
modeList=["Mode1","Mode2"]
initSet="Mode1: (2/3)*x+(5/3)y==3 && (4/2)*vx+5vy+4<=4"
print(initSet)
print(generateMatrix(varList,modeList,initSet))
#generateMatrix(varList,initSet)
