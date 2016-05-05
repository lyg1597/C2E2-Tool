import pygtk
pygtk.require('2.0')
import gtk,gobject,re,sys,glpk
import ply.yacc as yacc
import ply.lex as lex
import logging
from symEq import SymEq

propertyLog = logging.getLogger('c2e2VerificationLog')


"""
  PropDialog
  Description: dialog where the user can create/modify/remove properties to be verified
"""
class PropDialog(gtk.Dialog):
  """
    __init__
    Description: initializes the contents of the dialog along with any data
    Inputs: parent - the application window
            butType - button that opened the dialog, add/edit
            propertyList - list of properties and associated data
            propertyListLen - length of the property list
            varList - list of the variables in the model
            modeList - list of the modes in the model
    Outputs: removePropList - list of the properties whose tabs in the notebook have to be removed
             editPropList - list of the properties whose data was edditted whose tab have to be 
                            renamed
    Return: none
  """
  def __init__(self,parent,butType,propertyList,propertyListLen,varList,modeList,removePropList,
               editPropList):
    gtk.Dialog.__init__(self)
    self.butType=butType
    self.propertyList=propertyList
    self.propertyListLen=propertyListLen
    self.varList=varList
    self.removePropList=removePropList
    self.editPropList=editPropList
    self.modeList=modeList
    self.existingPropNames=[]
    self.newPropNames=[]
    self.exprParser=ExprParser()

    print(propertyListLen)

    mode="(\s*[a-zA-Z_][_a-zA-Z0-9]*:\s*)"
    term="((\-?\d*\.?\d+\*?)|(\-?\(\-?\d+/\-?\d+\)\*?))"
    var="([_a-zA-Z][_a-zA-Z0-9]*)"
    equality="(\s*((<=?)|(>=?)|(==))\s*)"
    expression="("+"("+term+")*"+var+"(\s*((\-)|(\+))\s*(("+"("+term+")*"+var+")|("+term+")))*)"
    equation="("+expression+equality+term+")"

    self.modePattern=re.compile(mode)
    self.varPattern=re.compile(var) 
    pattern="("+equation+"(\s*&&\s*"+equation+")*)$"
    self.unsafePattern=re.compile(pattern)
    pattern="("+mode+equation+"(\s*&&\s*"+equation+")*)$"
    self.initialPattern=re.compile(pattern)

    if butType=="Add":
      self.propTitle=self.butType+" Property"
      self.propertyList.append([PropertyDatum(self.propertyListLen)])
      self.newPropNames.append(["",0,0,0,self.propertyListLen])
      for prop in self.propertyList:
        self.existingPropNames.append(prop[0].name)
    else:
      for prop in self.propertyList:
        prop=prop[0]
        if prop.checked:
          self.newPropNames.append([prop.name,1,1,1,prop.index])
        else:
          self.existingPropNames.append(prop.name)
      if len(self.newPropNames)>1:
        self.propTitle=self.butType+" Properties"
      else:
        self.propTitle=self.butType+" Property"

    self.set_title(self.propTitle)
    self.set_size_request(320,420)
    self.set_transient_for(parent)
    self.set_position(gtk.WIN_POS_CENTER_ON_PARENT)

    dialogSW=gtk.ScrolledWindow()
    dialogSW.set_policy(gtk.POLICY_NEVER,gtk.POLICY_AUTOMATIC)
    dialogVBox=gtk.VBox(False,0)

    self.widgets={}
    for index,prop in enumerate(self.newPropNames): 
      if not index==0:
        dialogVBox.pack_start(gtk.HSeparator(),False,False,5)

      prop=self.propertyList[prop[4]][0]
      table=gtk.Table(12,1,True)

      self.widgets[index]={}
      self.widgets[index]["nameEntry"]=NameEntry()
      self.widgets[index]["radioSafe"]=gtk.RadioButton(None,"Safety")
      self.widgets[index]["initialSet"]=gtk.TextView()
      self.widgets[index]["initialSet"].set_editable(True)
      self.widgets[index]["initialSet"].set_cursor_visible(True)
      self.widgets[index]["initialSet"].set_wrap_mode(gtk.WRAP_CHAR)
      self.widgets[index]["unsafeSet"]=gtk.TextView()
      self.widgets[index]["unsafeSet"].set_editable(True)
      self.widgets[index]["unsafeSet"].set_cursor_visible(True)
      self.widgets[index]["unsafeSet"].set_wrap_mode(gtk.WRAP_CHAR)

      row1=gtk.HBox(False,0)
      row1.pack_start(gtk.Label("Property name: "),False,False,0)
      row1.pack_start(self.widgets[index]["nameEntry"],True,True,0)
      self.widgets[index]["nameEntry"].set_text(prop.name)
      checkImage=gtk.Image()
      row1.pack_start(checkImage,False,False,5)
      if self.newPropNames[index][1]:
        checkImage.set_from_stock(gtk.STOCK_APPLY,gtk.ICON_SIZE_MENU)
      else:
        checkImage.set_from_stock(gtk.STOCK_CANCEL,gtk.ICON_SIZE_MENU)
      self.widgets[index]["nameEntry"].connect("changed",self.checkPropName,row1.get_children()[2],
                                               index)
      table.attach(row1,0,1,0,1)

      row2=gtk.HBox(False,0)
      row2.pack_start(self.widgets[index]["radioSafe"],False,False,0)
      table.attach(row2,0,1,1,2)

      row3=gtk.HBox(False,0)
      row3.pack_start(gtk.Label("Initial set: "),True,True,0)
      row3.pack_start(gtk.Label(),False,False,0)
      row3.get_children()[0].set_alignment(0,.5)
      checkImage=gtk.Image()
      row3.pack_start(checkImage,False,False,5)
      if self.newPropNames[index][1]:
        checkImage.set_from_stock(gtk.STOCK_APPLY,gtk.ICON_SIZE_MENU)
      else:
        checkImage.set_from_stock(gtk.STOCK_CANCEL,gtk.ICON_SIZE_MENU)
      tempBuff=self.widgets[index]["initialSet"].get_buffer()
      tempBuff.set_text(prop.initialSetStr)
      self.widgets[index]["initialSet"].set_buffer(tempBuff)
      tempBuff.connect("changed",self.checkInitialSet,row3.get_children()[1],
                       row3.get_children()[2],index)
      table.attach(row3,0,1,2,3)

      sw=gtk.ScrolledWindow()
      sw.set_policy(gtk.POLICY_NEVER,gtk.POLICY_ALWAYS)
      sw.add(self.widgets[index]["initialSet"])
      table.attach(sw,0,1,3,7)

      row4=gtk.HBox(False,0)
      row4.pack_start(gtk.Label("Unsafe set: "),True,True,0)
      row4.pack_start(gtk.Label(),False,False,0)
      row4.get_children()[0].set_alignment(0,.5)
      checkImage=gtk.Image()
      row4.pack_start(checkImage,False,False,5)
      if self.newPropNames[index][1]:
        checkImage.set_from_stock(gtk.STOCK_APPLY,gtk.ICON_SIZE_MENU)
      else:
        checkImage.set_from_stock(gtk.STOCK_CANCEL,gtk.ICON_SIZE_MENU)
      tempBuff=self.widgets[index]["unsafeSet"].get_buffer()
      tempBuff.set_text(prop.unsafeSetStr)
      self.widgets[index]["unsafeSet"].set_buffer(tempBuff)
      tempBuff.connect("changed",self.checkUnsafeSet,row4.get_children()[1],
                       row4.get_children()[2],index)
      table.attach(row4,0,1,7,8)

      sw=gtk.ScrolledWindow()
      sw.set_policy(gtk.POLICY_NEVER,gtk.POLICY_ALWAYS)
      sw.add(self.widgets[index]["unsafeSet"])
      table.attach(sw,0,1,8,12)

      dialogVBox.pack_start(table,True,True,0)

    saveBtn=gtk.Button("Save")
    saveBtn.connect("clicked",self.saveBtnCallback)
    cancelBtn=gtk.Button("Cancel")
    cancelBtn.connect("clicked",self.cancelBtnCallback)
      
    dialogSW.add_with_viewport(dialogVBox)
    dialogSW.get_child().set_shadow_type(gtk.SHADOW_NONE)
    self.vbox.pack_start(dialogSW,True,True,0)
    self.action_area.pack_start(cancelBtn)
    self.action_area.pack_start(saveBtn)
    self.connect("delete-event",self.closeDialog)
    self.show_all()

  """
    closeDialog
    Description: handles the closing of the dialog by sending a cancel repsonse
    Inputs: widget - widget to be closed
            event - event triggering the closing
    Outputs: none
    Return: none
  """
  def closeDialog(self,widget,event):
    if self.butType=="Add":
      self.propertyList.remove(self.propertyList.iter_nth_child(None,self.propertyListLen))
    self.response(gtk.RESPONSE_CANCEL)

  """
    checkPropNames
    Description: checks that all the properties names differ from one another
    Inputs: entry - name entry currently being editted
            image - image next to the name entry
            index - the index of the property whose name is currently being editted
    Outputs: none
    Return: none
  """
  def checkPropName(self,entry,image,index):
    newName=entry.get_text()
    namePattern=re.compile("([_a-zA-Z][_a-zA-Z0-9]*)$")
    match=namePattern.match(newName)

    if match==None:
      self.newPropNames[index][0]=newName
      self.newPropNames[index][1]=0
      image.set_from_stock(gtk.STOCK_CANCEL,gtk.ICON_SIZE_MENU)

    edittedPropNames=[name[0] for idx,name in enumerate(self.newPropNames) if not idx==index]
    if not newName=="" and not newName in self.existingPropNames and not newName in edittedPropNames:
      self.newPropNames[index][0]=newName
      self.newPropNames[index][1]=1
      image.set_from_stock(gtk.STOCK_APPLY,gtk.ICON_SIZE_MENU)
    else:
      self.newPropNames[index][0]=newName
      self.newPropNames[index][1]=0
      image.set_from_stock(gtk.STOCK_CANCEL,gtk.ICON_SIZE_MENU)

  """
    checkInitialSet
    Description: checks that the initial set is valid and bounded
    Inputs: textBuffer - the text view that is holding the initial set
            errorLabel - label that displays error why set is incorrect
            image - image that displays validity
            index - index of the property whose initial set is being editted
    Outputs: none
    Return: none
  """
  def checkInitialSet(self,textBuffer,errorLabel,image,index):
    start,end=textBuffer.get_bounds()
    inputText=textBuffer.get_text(start,end)
    match=self.initialPattern.match(inputText)

    if match==None:
      self.newPropNames[index][2]=0
      image.set_from_stock(gtk.STOCK_CANCEL,gtk.ICON_SIZE_MENU)
      if not inputText=="":
        errorLabel.set_text("Incorrect Syntax")
      else:
        errorLabel.set_text("")
    else:
      matchMode=self.modePattern.search(inputText).group(0).rstrip()[:-1]
      matchVars=self.varPattern.findall(inputText)[1:]
      if len(list(set(matchVars)|set(self.varList)))>len(self.varList) or \
         matchMode not in self.modeList:
        self.newPropNames[index][2]=0
        image.set_from_stock(gtk.STOCK_CANCEL,gtk.ICON_SIZE_MENU)
        errorLabel.set_text("Incorrect Mode/Variables")
      else:
        # initialSet=self.exprParser.parseSet(self.varList,inputText,0)
        initialSetSplit = inputText.split(':')
        initialSet = [initialSetSplit[0]] + SymEq.get_eqn_matrix(initialSetSplit[1], self.varList)
        # print 'initialSet: ' + str(initialSet)
        # print 'initialSet1: ' + str(initialSet1)
        if self.checkBoundness(initialSet):
          self.newPropNames[index][2]=1
          image.set_from_stock(gtk.STOCK_APPLY,gtk.ICON_SIZE_MENU)
          errorLabel.set_text("")
        else:
          self.newPropNames[index][2]=0
          image.set_from_stock(gtk.STOCK_CANCEL,gtk.ICON_SIZE_MENU)
          errorLabel.set_text("Set Unbounded")

  """
    checkUnsafeSet
    Description: checks that the unsafe set is valid
    Inputs: textBuffer - the text view that is holding the unsafe set
            errorLabel - label that displays error why set is incorrect
            image - image that displays validity
            index - index of the property whose unsafe set is being editted
    Outputs: none
    Return: none
  """
  def checkUnsafeSet(self,textBuffer,errorLabel,image,index):
    start,end=textBuffer.get_bounds()
    inputText=textBuffer.get_text(start,end)
    matchSet=self.unsafePattern.match(inputText)

    if matchSet==None:
      self.newPropNames[index][3]=0
      image.set_from_stock(gtk.STOCK_CANCEL,gtk.ICON_SIZE_MENU)
      if not inputText=="":
        errorLabel.set_text("Incorrect Syntax")
      else:
        errorLabel.set_text("")
    else:
      matchVars=self.varPattern.findall(inputText) 
      if len(list(set(matchVars)|set(self.varList)))>len(self.varList):
        self.newPropNames[index][3]=0
        image.set_from_stock(gtk.STOCK_CANCEL,gtk.ICON_SIZE_MENU)
        errorLabel.set_text("Incorrect Variables")
      else:
        self.newPropNames[index][3]=1
        image.set_from_stock(gtk.STOCK_APPLY,gtk.ICON_SIZE_MENU)
        errorLabel.set_text("")

  """
    saveBtnCallback
    Description: checks that all the entries are valid and save the changed to the property list
    Inputs: btn - button that was clicked
    Outputs: none
    Return: none
  """
  def saveBtnCallback(self,btn):
    for new in self.newPropNames:
      if new[1]==0 or new[2]==0 or new[3]==0:
        return

    for index,new in enumerate(self.newPropNames):
      prop=self.propertyList[new[4]][0]
      tempBuf=self.widgets[index]["initialSet"].get_buffer()
      start,end=tempBuf.get_bounds()
      initial=tempBuf.get_text(start,end)
      tempBuf=self.widgets[index]["unsafeSet"].get_buffer()
      start,end=tempBuf.get_bounds()
      unsafe=tempBuf.get_text(start,end)

      if not prop.status=="Not verified" and (not prop.initialSetStr==initial or \
         not prop.unsafeSetStr==unsafe):
        prop.status="Not verified"
        prop.verifResult=""
        prop.newTabPixbuf=None
        self.removePropList.append(prop.index)
      elif not prop.status=="Not verified" and not prop.name==new[0]:
        self.editPropList.append((new[0],prop.index))

      # initialSet=self.exprParser.parseSet(self.varList,initial,0)
      initialSetSplit = initial.split(':')
      initialSet = [initialSetSplit[0]] + SymEq.get_eqn_matrix(initialSetSplit[1], self.varList)
      # print 'initialSet: ' + str(initialSet)
      # print 'initialSet1: ' + str(initialSet1)

      print "This is before calling the parse set of the unsafe set"
      # unsafeSet=self.exprParser.parseSet(self.varList,unsafe,1)
      unsafeSet = SymEq.get_eqn_matrix(unsafe, self.varList)
      # print 'unsafeSet: ' + str(unsafeSet)
      # print 'unsafeSet1: ' + str(unsafeSet1)

      prop.name=new[0]
      prop.initialSetStr=initial
      prop.initialSetParsed=initialSet
      prop.unsafeSetStr=unsafe
      prop.unsafeSetParsed=unsafeSet
      
      propertyLog.info(' Property Added - Initial Set - ' + initial + ' - Unsafe Set - ' + unsafe + ' - ')

      prop.printData()

    self.response(gtk.RESPONSE_OK)

  """
    cancelBtnCallback
    Description: cancel the addition/editting of the properties and close the dialog
    Inputs: btn - cancel btn that was pressed
    Outputs: none
    Return: none
  """
  def cancelBtnCallback(self,btn):
    if self.butType=="Add":
      self.propertyList.remove(self.propertyList.iter_nth_child(None,self.propertyListLen))
    self.response(gtk.RESPONSE_CANCEL)

  """
    checkBoundness
    Description: checks that the initial set is bounded
    Inputs: inputSet - the initial set written by the user
    Outputs: none
    Return: 0 on if the set is unbounded and 1 if it is bounded
  """
  def checkBoundness(self,inputSet):
    aMatrix=inputSet[1]
    bMatrix=inputSet[2]
    eqMatrix=inputSet[3]

    lp=glpk.LPX()
    lp.rows.add(len(aMatrix))
    for r in xrange(len(aMatrix)):
      if eqMatrix[r][0]==">=":
        lp.rows[r].bounds=(bMatrix[r][0],None)
      else:
        lp.rows[r].bounds=(None,bMatrix[r][0])
    lp.cols.add(len(self.varList))
    for c in lp.cols:
      c.bounds=None,None
    lp.matrix=[r for row in aMatrix for r in row]

    lp.obj[:]=[0.0 for i in xrange(len(self.varList))]
    for v in xrange(len(self.varList)):
      lp.obj[v]=1.0
      for i in  xrange(2):
        if i%2==0:
          lp.obj.maximize=False
        else:
          lp.obj.maximize=True
        lp.simplex()
        res=lp.status
        if res=="opt":
          continue
        else:
          return 0
      lp.obj[v]=0
    return 1

"""
  PropertyDatum
  Description: a class which contains all the necessary information reagarding a specific property
"""
class PropertyDatum():
  def __init__(self,propertyListLen):
    self.checked=False
    self.name=""
    self.status="Not verified"
    self.verifResult=""
    self.newTabPixbuf=None
    self.type=0
    self.initialSetStr=""
    self.initialSetParsed=None
    self.unsafeSetStr=""
    self.unsafeSetParsed=None
    self.index=propertyListLen
    self.tabChild=None
    self.reachSetPath=""
    self.paramData=[0,0,0,0]
    self.simulator=-1
    self.simulation = -1

  def copyDatum(self,propDatum,propertyListNames):
    copyNum=0
    self.name=propDatum.name+"Copy%s"%(copyNum)
    while self.name in propertyListNames:
      copyNum+=1
      self.name=propDatum.name+"Copy%s"%(copyNum)

    self.type=propDatum.type
    self.initialSetStr=propDatum.initialSetStr
    self.initialSetParsed=propDatum.initialSetParsed
    self.unsafeSetStr=propDatum.unsafeSetStr
    propertyLog.info(' Copying a property - name - ' + self.name + ' - initial set - ' + self.initialSetStr + ' - unsafe set - ' + self.unsafeSetStr + ' - ')
    self.unsafeSetParsed=propDatum.unsafeSetParsed

  def printData(self):
    print(self.checked)
    print(self.name)
    print(self.status)
    print(self.verifResult)
    print(self.newTabPixbuf)
    print(self.type)
    print(self.initialSetStr)
    print(self.initialSetParsed)
    print(self.unsafeSetStr)
    print(self.unsafeSetParsed)
    print(self.index)
    print(self.tabChild)
    print(self.reachSetPath)
    print(self.paramData)

"""
  ExprLexer
  Description: a lexer that assigns tokens to various parts of the user equation
"""
class ExprLexer:
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
    t.lexer.skip(1)

  """
    build
    Description: builds the lexer
    Inputs: other possible user inputs
    Outputs: none
    Return: returns the built lexer
  """
  def build(self,**kwargs):
    self.lexer=lex.lex(module=self,**kwargs)
    return self.lexer

  def test(self,data):
    self.lexer.input(data)
    while True:
      tok=self.lexer.token()
      if not tok:
        break
      print tok

"""
  ExprParser
  Description: parses the tokens created by the lexer and returns the initial/unsafe set as matrices
"""
class ExprParser:
  """
    __init__
    Description: initializes variables and builds the lexer
    Inputs: none
    Outputs: none
    Return: none
  """
  def __init__(self):
    self.termList=[] #(coefficient,variable)
    self.constant=0
    self.negative=1
    self.exprLexer=ExprLexer()
    self.lexer=self.exprLexer.build()  
    self.parser=yacc.yacc(module=self)
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

  """
    isNumber
    Description: checks whether the input is a float or not
    Inputs: value - the input to be checked
    Outputs: none
    Return: returns True on success and False on failure
  """
  def isNumber(self,value):
    try:
      float(value)
    except ValueError:
      return False
    return True

  """
    checkDecimals
    Description: checks if the input contains any decimals and gets the number of decimals
    Inputs: value - input to be checked for decimals
    Outputs: none
    Return: none
  """
  def checkDecimals(self,value):
    if str(value).find('.') == -1 :
      return  
    decimals=str(value).split(".")[1]
    print "The decimals are " + decimals
    if decimals=="0":
      return
    numDec=len(decimals)
    if self.multiplier<numDec:
      self.multiplier=numDec

  """
    parseTree
    Decription: recursively parses the tree created by the parser into coefficient, variable tuples
    Inputs: node - the node of the parse tree 
            neg - informs lower children nodes about the existence of a minus/negative
    Outputs: none
    Return: none
  """
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
        self.termList.append((neg*val,node.children[1].value))
      elif node.children[0].value=='/':
        div=node.children[0]
        self.divisorList.add(div.children[1].value)
        val=div.children[0].value/div.children[1].value
        self.termList.append((neg*val,node.children[1].value))
      else:
        self.checkDecimals(node.children[0].value)
        self.termList.append((neg*node.children[0].value,node.children[1].value))
    elif node.value=="/":
      self.divisorList.add(node.children[1].value)
      val=node.children[0].value/node.children[1].value
      self.constant+=neg*val
    elif self.isNumber(node.value):
      self.checkDecimals(neg*node.value)
      print "The constant value here " + str(neg*node.value)  
      self.constant+=neg*node.value
    else:
      self.checkDecimals(node.value)
      self.termList.append((neg*1.0,node.value))

  """
    printNode
    Description: recursively prints out the tree generated by the parse
    Inputs: node - node of the tree being printed
    Outputs: none
    Return: none
  """
  def printNode(self,node):
    print("Value: "+str(node.value)+", Children: "+str(len(node.children)))
    for n in node.children:
      self.printNode(n)

  """
    parse
    Description: initiates the call to the parseTre function
    Inputs: user input set that has to be parsed
    Outputs: none
    Return: termList - list of coefficient,variables tuples
            constant - summation of the constants in the set
            divisorList - set of the divisors in the set
            multiplier - length of largest decimals value in the set
    Note: the variables are class accessible since modified by recursive function
  """
  def parse(self,data):
    self.termList=[]
    self.divisorList.clear()
    self.multiplier=0
    self.constant=0

    result=self.parser.parse(data,lexer=self.lexer)
    # print(result)
    self.parseTree(result,1)
    return self.termList,self.constant,self.divisorList,self.multiplier

  """
    parseSet
    Description: takes in the set written by the user, splits them up into smaller chunks that
                 are fed into the other parsing functions 
    Inputs: varList - list of variables in the mode
            inputText - the set typed out by the user
            setType - which set is currently being parsed, initial or unsafe
    Outputs: none
    Return: none
  """
  def parseSet(self,varList,inputText,setType):
    parsedInput=[]
    equations=[]
    print(varList,inputText)
    if setType==0:
      parsedInput=(''.join(inputText.split())).split(':')
      equations=parsedInput[1]
    else:
      equations=''.join(inputText.split())

    #setType 2 is for invariants
    if setType==2:
      inequalities=equations.split("||")
    else:
      inequalities=equations.split("&&")
    
    addInequalities=[]
    remInequalities=[]
    for j,ineq in enumerate(inequalities):
      equal=re.search(r'==',ineq)
      if not equal==None:
        remInequalities.append(j)
        addInequalities.append(ineq.replace("==","<="))
        addInequalities.append(ineq.replace("==",">="))

    print(remInequalities,inequalities)
    for rem in reversed(remInequalities):
      inequalities.pop(rem)
    inequalities+=addInequalities

    aMatrix=[[0 for v in varList] for s in inequalities]
    bMatrix=[0 for x in inequalities]
    eqMatrix=[]

    for j,ineq in enumerate(inequalities):
      row=aMatrix[j]
      match=re.search(r'>=?|<=?',ineq).group(0)
      pos=ineq.rfind(match)
      expression=ineq[0:pos]
      constant=ineq[pos+len(match):len(ineq)]
      expression+="-"+constant
      print(expression)
      data,term,divisorList,multiplier=self.parse(expression)
      # print data
      # print term
      # print divisorList
      # print multiplier
      # print "for unsafe set, what is the expression and other things"
      
      print "Multiplier is " + str(multiplier)
      
      multiply=1*(10**multiplier)
      for d in divisorList:
        multiply*=d

      for d in data:
        row[varList.index(d[1])]+=d[0]*multiply
        bMatrix[j]=[(-term)*multiply]
        if bMatrix[j][0]==-0.0:
          bMatrix[j][0]=0.0

      if match==">":
        match=">="
      elif match=="<":
        match="<="
      eqMatrix.append([match])
    
    if setType==0:
      return[parsedInput[0],aMatrix,bMatrix,eqMatrix]
    else:
      return[aMatrix,bMatrix,eqMatrix]

"""
  Node
  Description: nodes in the tree that the parser uses to store information
"""
class Node:
  def __init__(self,value,children=None):
    self.value=value
    if children:
      self.children=children
    else:
      self.children=[]

"""
  NameEntry
  Description: a subclass of the gtk.Entry that limits a user to only input valid names
"""
class NameEntry(gtk.Entry):
  """
    __init__
    Description: initializes the entry and the regex pattern
    Inputs: none
    Outputs: none
    Return: none
  """
  def __init__(self):
    gtk.Entry.__init__(self)
    self.connect("changed",self.validateInput)
    self.namePattern=re.compile("(?:[_a-zA-Z])(?:[_a-zA-Z0-9])*")

  """
    validateInput
    Description: checks to see if the input is valid and keep the changes if it is
    Inputs: entry - entry being editted
    Outputs: none
    Return: none
  """
  def validateInput(self,entry):
    text=entry.get_text()
    validInput=self.namePattern.findall(text)
    if len(validInput):
      entry.set_text(validInput[0])
    else:
      entry.set_text("")
gobject.type_register(NameEntry)
