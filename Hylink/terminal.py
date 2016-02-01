import pygtk
pygtk.require('2.0')
import gtk,gobject,re,os,subprocess,string,time,psutil
import logging
from plotWindow import PlotWindow
from hierarchy import IsHierarchical,RemoveHierarchy
from hyir import hyirMdl,hyirXML
from propDialog import PropDialog,PropertyDatum
from SFParse import extract_sf
from propDialog import *
import sys
import glpk
import ply.yacc as yacc
import ply.lex as lex
import numpy as np
import numpy.linalg as la
Lineararg = 0

def printprop(prop):
	print(prop.index)
	print("name:"+prop.name)
	print("initialSet:"+prop.initialSetStr)
	print("unsafeSet:"+prop.unsafeSetStr)

def loadfile(fileChoosen):
	print("-----------------compile warnning--------------------")
	fileName,fileExtension=os.path.splitext(os.path.basename(fileChoosen))
	propList=[]
	typeInput=0
	paramList=[]
	if fileExtension==".hyxml":
		hybridRep,propList=hyirXML(fileChoosen)
		typeInput = 1
		for prop in propList:
			if prop.paramData[0]!=0 and prop.paramData[1]!=0 and prop.paramData[2]!=0 and prop.paramData[3]!=0:
				paramList = [str(prop.paramData[0]),str(prop.paramData[1]),str(prop.paramData[2]),str(prop.paramData[3])]
	elif fileExtension==".mdl":
		typeInput=2
		model=open(fileChoosen,"r")
		rawModel=model.read()
		x=re.search(r"Stateflow {",rawModel)
		sf_data=rawModel[x.start():]
		sf_tree=extract_sf(sf_data)
		#sf_tree.bolunprints()
		if IsHierarchical(sf_tree):
			sf_tree=RemoveHierarchy(sf_tree)
		hybridRep=hyirMdl(sf_tree,fileChoosen)
		for annotString in hybridRep.annotationsRaw:
			annotMode=0;K=1.1;gamma=0;annotType=1;
			splits=string.split(annotString,';')
			for substrings in splits:
				match = re.search('mode=.*',substrings)
				if not match == None:
					matchedString = match.group(0)
					modeString=matchedString[5:]
					modeIndex=0
					for mode in hybridRep.automata[0].modes:
						modeIndex+=1
						if mode.name == modeString:
							annotMode=modeIndex
				match = re.search('k=.*',substrings)
				if not match == None:
					Kstring = match.group(0)[2:]
					K = float(Kstring)
				match = re.search('gamma=.*',substrings)
				if not match == None:
					GammaString = match.group(0)[6:]
					gamma = float(GammaString)
				match = re.search('type=.*',substrings)
				if not match == None:
					TypeString = match.group(0)[5:]
					if TypeString == "exponential":
						annotType = 1
					if TypeString == "linear":
						annotType = 2
					if TypeString == "contraction":
						annotType = 3
			newAnnot = [annotMode,K,gamma,annotType]
			hybridRep.annotationsParsed += [newAnnot]
	else:
		print("unknow file type")
		sys.exit()

	dupHybridRep = hybridRep
	dupHybridRep.printGuardsResets()
	print("guard file Generated")
	dupHybridRep.printInvariants()
	print("invariant file Generated")
	dupHybridRep.convertToCAPD("simulator")
	print("simulator file Generated")

	parseTree,vList,mList=hybridRep.display_system("System")

	arg1 = ['mv','guardGen.cpp','Invcheck.cpp','simulator.cpp','../wd/']
	subp1 = subprocess.Popen(arg1)
	arg = ['sh','./compileAndExecute']
	subp2 = subprocess.Popen(arg,cwd="../wd/")
	while subp2.poll() == None:
		if not subp2.poll()==None:
			break
	print("---------------model load success-------------------")

	return vList,mList,hybridRep,propList,paramList,typeInput

def verification(hybridRep,paramData,modeList,varList,propList):
	print("----------------generating test file----------------")
	for prop in propList:
		printprop(prop)
		#prop.printData()
		pathString="../wd/ReachSet"+prop.name
		prop.reachSetPath = pathString
		c2e2String=""
		c2e2String+= "dimensions=\""+str(len(varList))+"\"\n"
		c2e2String+= "modes=\""+str(len(modeList))+"\"\n"
		c2e2String+= "simulator=\"simu\"\n"
		initialSet = prop.initialSetParsed
		unsafeSet = prop.unsafeSetParsed

		initialModeString = initialSet[0]
		initialMatrix = initialSet[1]
		initialB = initialSet[2]
		initialIneqs = initialSet[3]

		unsafeMatrix = unsafeSet[0]
		unsafeB = unsafeSet[1]
		unsafeIneqs = unsafeSet[2]
		initModeC2E2Rep = -1
		modesList = hybridRep.automata[0].modes
		modeI = 0
		for modeIndex in modesList:
			modeI += 1
			if (initialModeString == modeIndex.name):
				initModeC2E2Rep = modeI;

		c2e2String+= "init-mode=\""+str(initModeC2E2Rep)+"\"\n"
		taylor = paramData[3][1]
		relerr = "0.0001"
		abserr = "0.00001"
		thoriz = paramData[2][1]
		tstep = paramData[1][1]
		delta = paramData[0][1]
		#print(delta)
		prop.paramData[0]=float(delta)
		prop.paramData[1]=float(tstep)
		prop.paramData[2]=float(thoriz)
		prop.paramData[3]=float(taylor)
		c2e2String+= "delta=\""+delta+"\"\n"
		c2e2String+= "time-step=\""+tstep+"\"\n"
		c2e2String+= "abs-error=\""+abserr+"\"\n"
		c2e2String+= "rel-error=\""+relerr+"\"\n"
		c2e2String+= "time-horizon=\""+thoriz+"\"\n"

		numInitEqns = len(initialIneqs)
		c2e2String+= "init-eqns=\""+str(numInitEqns)+"\"\n"
		c2e2String+= "init-matrix=["
		commaCount = 0;
		for j in range(0,numInitEqns):
			multiplicity = 1;
			rowMatrix = initialMatrix[j]
			rowDir = initialIneqs[j][0]

			if rowDir == '>=':
				multiplicity = -1

			for number in rowMatrix:
				if commaCount == 0:
					c2e2String += str(multiplicity*number)
					commaCount+=1
				else:
					c2e2String += ","+str(multiplicity*number)
		c2e2String += "]\n"
		c2e2String += "init-b=["
		commaCount = 0
		for j in range(0,numInitEqns):
			multiplicity = 1;
			number = initialB[j][0]
			rowDir = initialIneqs[j][0]

			if rowDir == '>=':
				multiplicity = -1

			if commaCount == 0:
				c2e2String += str(multiplicity*number)
				commaCount+=1
			else:
				c2e2String += ","+str(multiplicity*number)
		
		c2e2String += "]\n"        
		  
		numUnsafeEqns = len(unsafeIneqs)
		c2e2String+= "unsafe-eqns=\""+str(numUnsafeEqns)+"\"\n"
		c2e2String+= "unsafe-matrix=["
		commaCount = 0;
		for j in range(0,numUnsafeEqns):
			multiplicity = 1;
			rowMatrix = unsafeMatrix[j]
			rowDir = unsafeIneqs[j][0]

			if rowDir == '>=':
				multiplicity = -1

			for number in rowMatrix:
				if commaCount == 0:
					c2e2String += str(multiplicity*number)
					commaCount+=1
				else:
					c2e2String += ","+str(multiplicity*number)

		c2e2String += "]\n"
		c2e2String += "unsafe-b=["
		commaCount = 0
		for j in range(0,numUnsafeEqns):
			multiplicity = 1;
			number = unsafeB[j][0]
			rowDir = unsafeIneqs[j][0]

			if rowDir == '>=':
				multiplicity = -1
			if commaCount == 0:
				c2e2String += str(multiplicity*number)
				commaCount+=1
			else:
				c2e2String += ","+str(multiplicity*number)

		c2e2String += "]\n"
		mode_num  = 1
		if len(hybridRep.annotationsParsed) == len(modeList) :
			for array in hybridRep.annotationsParsed :
				c2e2String+= "annotation-mode=\""+str(array[0])+"\"\n"
				if array[3] == 1 or array[3] == 3 :
					c2e2String+= "annotation-type=\"contraction\"\n"
				if array[3] == 2 :
					c2e2String+= "annotation-type=\"linear\"\n"
				c2e2String+= "annotation=\'dx1^2 + dx2^2\'\n"
				c2e2String+= "beta=\'dx1^2 + dx2^2\'\n"
				if (Lineararg):
					filename = ""
					filename +="../wd/jacobiannature"
					filename += str(mode_num)
					filename +=".txt"
					fid = open(filename,'r').read().split('\n')
					numofvar = int(fid[0])
					#print numofvar
					listofvar = []
					for i in range (numofvar):
						listofvar.append(fid[i+1])
					#rint listofvar
					equationpos = 1+numofvar
					numofeq = int(fid[equationpos])
					equationpos+=1
					listofeq = []
					for i in range (numofeq):
						listofeq.append(fid[equationpos+i])
					#print listofeq
					codestring = "def jcalc("
					codestring += "listofvar, "
					codestring += "listvalue"
					codestring+='):\n'
					codestring+=" for i in range (len(listofvar)):\n"
					codestring+="   temp = listofvar[i]\n"
					codestring+="   rightside = '='+str(listvalue[i])\n"
					codestring+="   exec(temp+rightside)\n"
					codestring+=" ret = []\n"
					for i in range (numofeq):
						codestring+=" "
						codestring+=listofeq[i]
						codestring+='\n'
						codestring+=' ret.append(Entry)\n'
					codestring+=' return ret'
					#print(codestring)
					exec(codestring)
					Constant_Jacobian = jcalc(listofvar,np.ones((1,numofvar))[0])
					Constant_Jacobian = np.reshape(Constant_Jacobian,(numofvar,numofvar))
					gamma_rate = la.eigvals(Constant_Jacobian).real
					array[2] = max(gamma_rate)
					array[1] = la.norm(Constant_Jacobian)
				c2e2String+= "k=\""+str(array[1])+"\"\n"
				c2e2String+= "gamma=\""+str(array[2])+"\"\n"
				c2e2String+= "is_linear=\""+str(Lineararg)+"\"\n"
				mode_num = mode_num+1

		else:
			i = 0
			while i < len(modeList) :
				c2e2String+= "annotation-mode=\""+str(i+1)+"\"\n"
				c2e2String+= "annotation-type=\"contraction\"\n"
				c2e2String+= "annotation=\'dx1^2 + dx2^2\'\n"
				c2e2String+= "beta=\'dx1^2 + dx2^2\'\n"
				c2e2String+= "k=\"1.1\"\n"
				c2e2String+= "gamma=\"0.0\"\n"
				i+=1   



		c2e2String+= "visualize all to ReachSet"+prop.name+"\n"

		writer = open("Configuration-C2E2","w")
		writer.write(c2e2String)
		writer.close()

		arguments1 = ['mv', 'Configuration-C2E2', '../wd/']
		subp1 = subprocess.Popen(arguments1); 

		print("----------------start to verification---------------")
		arguments = ['sh','./ExecuteC2E2']
		subp = subprocess.Popen(arguments,cwd="../wd/",preexec_fn=os.setsid)

		while subp.poll() == None:
			if not subp.poll() == None:
				break
		print("----------------Verification Done-------------------")
		fileHandle = open ( '../wd/log',"r" )
		lineList = fileHandle.readlines()
		fileHandle.close()
		print lineList[-1]

def checkBound(inputSet,varList):
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
	lp.cols.add(len(varList))
	for c in lp.cols:
		c.bounds=None,None
	lp.matrix=[r for row in aMatrix for r in row]
	lp.obj[:]=[0.0 for i in xrange(len(varList))]
	for v in xrange(len(varList)):
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

def addprop(vList,mLfist,propList,proplen):
	mode="(\s*[a-zA-Z_][_a-zA-Z0-9]*:\s*)"
	term="((\-?\d*\.?\d+\*?)|(\-?\(\-?\d+/\-?\d+\)\*?))"
	var="([_a-zA-Z][_a-zA-Z0-9]*)"
	equality="(\s*((<=?)|(>=?)|(==))\s*)"
	expression="("+"("+term+")*"+var+"(\s*((\-)|(\+))\s*(("+"("+term+")*"+var+")|("+term+")))*)"
	equation="("+expression+equality+term+")"

	modePattern=re.compile(mode)
	varPattern=re.compile(var) 
	pattern="("+equation+"(\s*&&\s*"+equation+")*)$"
	unsafePattern=re.compile(pattern)
	pattern="("+mode+equation+"(\s*&&\s*"+equation+")*)$"
	initialPattern=re.compile(pattern)
	exprParser = ExprParser()
	newprop = PropertyDatum(proplen)
	namepass = 0
	nametemp = 0
	while namepass!=1:
		newpropname = raw_input("Enter your Property name:")
		for prop in propList:
			if newpropname == prop.name:
				nametemp = 1
				print("Name exist in property list, please enter another name!")
				break
		if nametemp==1:
			namepass=0
		else:
			namepass=1

	initpass = 0
	while initpass!=1:
		newpropinitialset = raw_input("Enter your initial set:")
		print("----------------check initial set------------------")
		match = initialPattern.match(newpropinitialset)
		if match==None:
			if not newpropinitialset=="":
				print("----------------------result-------------------------")
				print("Incorrect Syntax")
			else:
				print("----------------------result-------------------------")
				print("No Input")
		else:
			matchMode = modePattern.search(newpropinitialset).group(0).rstrip()[:-1]
			matchVars = varPattern.findall(newpropinitialset)[1:]
			if len(list(set(matchVars)|set(vList)))>len(vList) or matchMode not in mList:
				print("----------------------result-------------------------")
				print("Incorrect Mode/Variables")
			else:
				initialSet = exprParser.parseSet(vList,newpropinitialset,0)
				if checkBound(initialSet,vList):
					print("----------------------result-------------------------")
					print("Valid Initial Set")
					initpass = 1
				else:
					print("----------------------result-------------------------")
					print("Set Unbounded")
	unsafepass = 0
	while unsafepass!=1:
		newpropunsafeset = raw_input("Enter your unsafe set:")
		print("---------------check unsafe set--------------------")
		matchSet = unsafePattern.match(newpropunsafeset)
		if matchSet==None:
			if not newpropunsafeset=="":
				print("----------------------result-------------------------")
				print("Incorrect Syntax")
			else:
				print("----------------------result-------------------------")
				print("No Input")
		else:
			matchVars = varPattern.findall(newpropunsafeset)
			if len(list(set(matchVars)|set(vList)))>len(vList):
				print("----------------------result-------------------------")
				print("Incorrect Variables")
			else:
				print("Valid Unsafe Set")
				unsafepass=1
	newprop.status = "Not verified"
	newprop.verifResult=""
	newprop.newTabPixbuf=None
	newprop.name=newpropname
	newprop.initialSetStr=newpropinitialset
	newprop.initialSetParsed=initialSet
	newprop.unsafeSetStr=newpropunsafeset
	newprop.unsafeSetParsed=exprParser.parseSet(vList,newpropunsafeset,1)
	propList.append(newprop)
	


def changepara(paramData):
	partpass=0
	timesteppass=0
	timehorizonpass=0
	taylorpass=0
	while partpass==0:
		Partitioning = raw_input('Please enter Partitioning:')
		try:
			partpass=1
			float(Partitioning)
		except ValueError:
			partpass=0
			print("Invaild Partitioning value")
	while timesteppass==0:	
		Time_step= raw_input('Please enter Time-step:')
		try:
			timesteppass=1
			float(Time_step)
		except ValueError:
			timesteppass=0
			print("Invaild Time-step value")
	while timehorizonpass==0:	
		Time_horizon=raw_input('Please enter Time-horizon:')
		try:
			timehorizonpass=1
			float(Time_horizon)
		except ValueError:
			timehorizonpass=0
			print("Invaild Time-horizon value")
	while taylorpass==0:	
		Taylor = raw_input('Please enter Taylor model order(integer):')
		try:
			taylorpass=1
			float(Taylor)
		except ValueError:
			taylorpass=0
			print("Invaild Time-horizon value")
	return Partitioning, Time_step,Time_horizon,Taylor			
	

def printallprop(propList):
	for prop in propList:
		print("-------------------------------------------------")
		printprop(prop)

def helper():
	print("-----------------------------------------------")
	print("printprop ----- print all property")
	print("addprop   ----- add new property")
	print("verify    ----- verify all property")
	print("parameter ----- change parameter")
	print("save      ----- save property as hyxml file")
	print("delete    ----- delete property in proplist")
	print("help      ----- command helper")
	print("quit      ----- end C2E2")
	print("-----------------------------------------------")

def save(propList,hybridRep):
	filename = raw_input("Please enter your file name(with .hyxml):")
	saveFile = open("../Examples/"+filename,'w')
	plist=[]
	for prop in propList:
		plist.append([prop])
	savedModeString = hybridRep.convertToXML(plist)
	saveFile.write(savedModeString)
	saveFile.close()

def delete(propList,propLen):
	printallprop(propList)
	numpass = 0
	numrange = 0
	while numpass==0 or numrange ==0:
		numdele = raw_input('Please enter index of property you want to delete:')
		try:
			numpass=1
			int(numdele)
		except ValueError:
			numpass=0
			print("Invaild input")
		if int(numdele)<propLen:
			numrange=1
		else:
			print("input number too large, so such property") 
			numrange=0

	propList.pop(int(numdele))

def autotest(model):
	inputfile = open ( '../Examples/'+model,"r" )
	lineList = inputfile.read().splitlines()
	inputfile.close()
	#print lineList
	if lineList[0]!= "C2E2 TEST FILE":
		print("Not a test script file, test end")
		sys.exit()
	else:
		vList,mList,hybridRep,propList,paramsData,typeInput = loadfile("../Examples/"+lineList[1])
		paramData=[["Partitioning:",lineList[2],1],["Time-step:",lineList[3],1],["Time horizon:",lineList[4],1],["Taylor model order:",lineList[5],1]]
		#print(len(lineList))
		propLen = (len(lineList)-6)/3
		exprParser = ExprParser()
		for i in range (propLen):
			newprop = PropertyDatum(i)
			newprop.status = "Not verified"
			newprop.verifResult=""
			newprop.newTabPixbuf=None
			newprop.name=lineList[6+3*i]
			newprop.initialSetStr=lineList[6+3*i+1]
			initialSet = exprParser.parseSet(vList,lineList[6+3*i+1],0)
			newprop.initialSetParsed=initialSet
			newprop.unsafeSetStr=lineList[6+3*i+2]
			newprop.unsafeSetParsed=exprParser.parseSet(vList,lineList[6+3*i+2],1)
			propList.append(newprop)
		verification(hybridRep,paramData,mList,vList,propList)
		sys.exit()

def autotest2(model):
	print(Lineararg)
	vList,mList,hybridRep,propList,paramsData,typeInput = loadfile("../Examples/"+model)
	paramData=[["Partitioning:",paramsData[0],1],["Time-step:",paramsData[1],1],["Time horizon:",paramsData[2],1],["Taylor model order:",paramsData[3],1]]
	verification(hybridRep,paramData,mList,vList,propList)
 	sys.exit()


'''
print("================================================================================")
print("Welcome to C2E2 Terminal Version")
print("This version support 3 kinds of files, hyxml, mdl and test script file")
print("This version only reads files in Examples directory within your C2E2 directory")
print("For hyxml and mdl file, you can do lots of things, enter help for command list")
print("For test script file, it will automatically start test")
print("Sample test script file is in Examples directory")
print("First, enter filename to get start")
print("================================================================================")
'''
filename = sys.argv[-1]
Lineararg = int(sys.argv[-2])
#print(filename)
if filename != "terminal.py":
	#autotest(filename)
	autotest2(filename)
'''
while raw_input("If you are ready to use, Enter Y:")!='Y':
	continue
'''
print("Current exist models in Examples directory are:")
print("---------------------------------------------------")
for file in os.listdir("../Examples"):
    print(file)
exist = 0
while exist!=1:
	print("---------------------------------------------------")
	model = raw_input('Please enter a model name with extension: ')
	for file in os.listdir("../Examples"):
		if str(file) == model:
			exist = 1
	if exist == 1:
		fileName,fileExtension=os.path.splitext(os.path.basename(model))
		if fileExtension == "":
			autotest(model)
		else:
			vList,mList,hybridRep,propList,paramsData,typeInput = loadfile("../Examples/"+model)
	else:
		print("file not exist")

print("your module have variable:")
print(vList)
print("your module have mode:")
print(mList)
proplen = 0
for prop in propList:
	proplen+=1

if paramsData == None or paramsData == []:
	Partitioning = "0.2"
	Time_step = "0.01"
	Time_horizon = "10.0"
	Taylor = "10"
	paramData=[["Partitioning:",Partitioning,1],["Time-step:",Time_step,1],["Time horizon:",Time_horizon,1],["Taylor model order:",Taylor,1]]
	print("----------------------------------------------------")
elif paramsData != None and paramsData != []:
	paramData=[["Partitioning:",paramsData[0],1],["Time-step:",paramsData[1],1],["Time horizon:",paramsData[2],1],["Taylor model order:",paramsData[3],1]]


while True:
	command = raw_input("Please type your command (type help if you need help):")
	if command == "printprop":
		printallprop(propList)
	elif command == "addprop":
		addprop(vList,mList,propList,proplen)
		proplen+=1
	elif command == "parameter":
		Partitioning, Time_step,Time_horizon,Taylor = changepara(paramData)
		#print(Partitioning)
		#print(Time_step)
		#print(Time_horizon)
		#print(Taylor)
		paramData=[["Partitioning:",Partitioning,1],["Time-step:",Time_step,1],["Time horizon:",Time_horizon,1],["Taylor model order:",Taylor,1]]
	elif command == "help":
		helper()
	elif command == "verify":
		if proplen == 0:
			print("no property in list, please add property first")
		else:
			verification(hybridRep,paramData,mList,vList,propList)
	elif command == "quit":
		sys.exit()
	elif command == "save":
		if proplen == 0:
			print("no property in list, please add property first")
		else:
			save(propList,hybridRep)

	elif command == "delete":
		if proplen == 0:
			print("no property in list, please add property first")
		else:
			delete(propList,proplen)
			proplen -=1
	else:
		print("No such command")




