import pygtk
pygtk.require('2.0')
import gtk,math, itertools, copy, re, os
from lxml import etree
from parse_modes import extract_dais
from tree import show_tree, clear_tree
from EdgeParse import lstring3
from ModeParse import parse_action
from guardParse import parse_guardExp
from guardLogicalParse import parse_guardLogicalExp
from fractions import Fraction
from node import Node
from automaton import *
from propDialog import *
from jacobiancalc import jacobian
from jacobiancalc import createCDFfunction
from collections import defaultdict

class HyIR:
    '''Hybrid Intermediate Representation.  Automaton is held as lists of 
    Modes, Transitions, and global variables'''
    def __init__(self, name="hybrid_system", vars=None,file_name=""):
        self.name = name
        if vars == None:
            self.vars = []
        else:
            self.vars = vars        
        self.automata = []
        self.annotations = ""
        self.annotationsRaw = []
        self.annotationsParsed = []
        self.treestore = gtk.TreeStore(str)
        self.file_name = file_name
        
        
    def add_automaton(self, automaton):
        self.automata.append(automaton)
        
    def add_var(self,var):
        self.vars.append(var)
        
    def print_vars(self):
        print "--- Variables ---"
        for i in self.vars:
            print i.name+" "+i.type+" "+i.scope
            
    def print_all(self):
        print "%s:" % self.name
        self.print_vars()
        for automaton in self.automata:
            automaton.print_all()

    def modesnumber(self):
        return len(self.automata[0].modes)

    def display_system(self,system_name):
        varList=[]
        for i in self.vars:
          if not i.scope=="OUTPUT_DATA":
            varList.append(i.name)
        var=self.treestore.append(None,["Variables"])
        self.treestore.append(var,[", ".join(varList)])

        modeList=[]
        modes=self.treestore.append(None,["Modes"])
        for i in self.automata[0].modes:
          modeList.append(i.name)
          m=self.treestore.append(modes,[i.name])      
          flows=self.treestore.append(m,["Flows"])
          for j in i.dais:
            if not(len(j.raw)>4 and "_out" in j.raw):
              self.treestore.append(flows,[j.raw])
          inv=self.treestore.append(m,["Invariants"])
          for j in i.invs:
            self.treestore.append(inv,[j.raw])
          #ann=self.treestore.append(m,["Annotations"])
          #For now these are just placeholders
          #found = 0
          #for annots in self.annotationsParsed:
              #if i.id == annots[0]-1:
                  #found = 1
                  #beta=self.treestore.append(ann,["K="+str(annots[1])])
                  #gamma=self.treestore.append(ann,["gamma="+str(annots[2])])

          #if found == 0:
              #beta=self.treestore.append(ann,["K="])
              #gamma=self.treestore.append(ann,["gamma="])

        trans=self.treestore.append(None,["Transitions"])
        for i in self.automata[0].trans:
          t=self.treestore.append(trans,[i.id])
          self.treestore.append(t,["Source: "+str(i.src)])
          self.treestore.append(t,["Destination: "+str(i.dest)])
          self.treestore.append(t,["Guards: "+i.guard.raw])
          acts=self.treestore.append(t,["Actions"])
          for j in i.actions:
            self.treestore.append(acts,[j.raw])

        swindow = gtk.ScrolledWindow()
        swindow.set_policy(gtk.POLICY_AUTOMATIC,gtk.POLICY_AUTOMATIC)
    
        modelTree=gtk.TreeView()
        modelColumn=gtk.TreeViewColumn()
        modelColumn.set_title(os.path.splitext(os.path.basename(self.file_name))[0])

        cell = gtk.CellRendererText()
        cell.set_property("editable",False)
        #cell.connect("edited",self.editRowCallback)
        modelColumn.pack_start(cell,True)
        modelColumn.add_attribute(cell,"text",0)

        modelTree.append_column(modelColumn)
        modelTree.set_model(self.treestore)
        modelTree.connect("row_expanded",self.treeExpandCallback)
        modelTree.expand_row(self.treestore.get_path(var),False)
        modelTree.expand_row(self.treestore.get_path(modes),False)

        swindow.add(modelTree)
        return swindow,varList,modeList

    def printFile(self, varList, invariants, guardResets):
        print invariants
        print guardResets
        file_name = "bloatedSimGI.cpp"
        checkFile = open(file_name, "w")
        codeString ="#include <ppl.hh>\n"
        codeString+="#include <iostream>\n"
        codeString+="#include <utility>\n"
        codeString+="#include <vector>\n"
        codeString+="#include <fstream>\n"
        codeString+="#include <typeinfo>\n\n"
        codeString+="using namespace std;\n\n"
        codeString+=self.printPoly()
        codeString+=self.constructBox(varList)
        checkFile.write(codeString)
        checkFile.close()

        self.printInvariants(file_name, varList, invariants)
        self.printGuardResets(file_name, varList, guardResets)

    def printInvariants(self, file_name, varList, invariants):
        checkFile = open(file_name, "a")
        codeString="extern \"C\" bool invariantSatisfied(int curMode, double *ptLower, double *ptUpper){\n"
        varList = ["Simu_time"]+varList
        for i,var in enumerate(varList):
            codeString+="  Variable "+var+"("+str(i)+");\n"
        codeString+="\n"

        codeString+="  Pointset_Powerset<NNC_Polyhedron> box(constructBox(ptLower, ptUpper));\n"
        codeString+="  Pointset_Powerset<NNC_Polyhedron> invariant("+str(len(varList))+",UNIVERSE);\n"
        for mode in invariants:
            codeString+="  if(curMode=="+str(mode+1)+"){\n"
            codeString+="    Pointset_Powerset<NNC_Polyhedron> curInv;\n"
            codeString+="    NNC_Polyhedron curPoly;\n"
            codeString+="    Constraint_System cs;\n"
            for eq in invariants[mode]:
                codeString+="    curInv = Pointset_Powerset<NNC_Polyhedron>("+str(len(varList))+",EMPTY);\n\n"
                for i,disjunct in enumerate(eq):
                    codeString+="    cs.set_space_dimension("+str(len(varList))+");\n"
                    codeString+="    cs.insert("+disjunct+");\n"
                    codeString+="    curPoly = NNC_Polyhedron(cs);\n"
                    codeString+="    curInv.add_disjunct(curPoly);\n"
                    codeString+="    cs.clear();\n\n"
                codeString+="    invariant.intersection_assign(curInv);\n\n"
            codeString+="    return !(invariant.is_disjoint_from(box));\n"
            codeString+="  }\n"
        codeString+="  return true;\n"
        codeString+="}\n\n"
        checkFile.write(codeString)
        checkFile.close()

    def printGuardResets(self, file_name, varList, guardResets):
        resetVarList = [var+"_new" for var in varList]
        varList = ["Simu_time"]+varList
        checkFile = open(file_name, "a")
        codeString="extern \"C\" vector<pair<NNC_Polyhedron, int> > hitsGuard(int curMode, double *ptLower, double *ptUpper){\n"
        codeString+="  vector<pair<NNC_Polyhedron, int> > toRet;\n"
        codeString+="  NNC_Polyhedron box = constructBox(ptLower, ptUpper);\n"

        for i,var in enumerate(varList):
            codeString+="  Variable "+var+"("+str(i)+");\n"
        codeString+="\n"

        # for i,var in enumerate(resetVarList):
        #     codeString+="  Variable "+var+"("+str(i+len(varList))+");\n"
        # codeString+="  //Variables to remove\n"
        # codeString+="  Variables_Set vars;\n"
        # for var in varList:
        #     codeString+="  vars.insert("+var+");\n"
        # codeString+="\n"
        for key in guardResets:
            init = str(key[0]+1)
            dest = str(key[1]+1)
            for a,b in guardResets[key]:
                codeString+="  if(curMode=="+init+"){\n"
                codeString+="    Constraint_System cs;\n"
                codeString+="    cs.set_space_dimension("+str(len(varList))+");\n"  
                for guard_eq in a:
                    codeString+="    cs.insert("+guard_eq+");\n"
                codeString+="    NNC_Polyhedron guard(cs);\n"
                if not b:
                    codeString+="    if(!guard.is_disjoint_from(box)){\n"
                    codeString+="      guard.intersection_assign(box);\n"
                    codeString+="      toRet.push_back(make_pair(guard,"+dest+"));\n"
                    codeString+="    }\n"
                else:
                    codeString+="HI\n"
                codeString+="  }\n"

        codeString+="  return toRet;\n"
        codeString+="}\n\n"
        checkFile.write(codeString)
        checkFile.close()


    def constructBox(self, varList):
        codeString="NNC_Polyhedron constructBox(double *ptLower, double *ptUpper){\n"
        varList = ["Simu_time"]+varList
        for i,var in enumerate(varList):
            codeString+="  Variable "+var+"("+str(i)+");\n"
        codeString+="\n"

        codeString+="  char buffer[100];\n\n"
        codeString+="  int multiplier=0, tmp_mul, str_len;\n"
        codeString+="  char *dot_loc;\n\n"
        mulLoop="  for(int i=0; i<"+str(len(varList))+"; i++){\n"
        mulLoop+="    sprintf(buffer, \"%lf\", ptLower[i]);\n"
        mulLoop+="    str_len = strlen(buffer);\n"
        mulLoop+="    dot_loc = strchr(buffer,'.');\n"
        mulLoop+="    if(dot_loc){\n"
        mulLoop+="      tmp_mul = (str_len-1)-(dot_loc-buffer);\n"
        mulLoop+="      if(tmp_mul>multiplier){\n"
        mulLoop+="        multiplier=tmp_mul;\n"
        mulLoop+="      }\n"
        mulLoop+="    }\n"
        mulLoop+="  }\n\n"
        codeString+=mulLoop
        mulLoop=mulLoop.replace('ptLower','ptUpper')
        codeString+=mulLoop

        codeString+="  double mult_factor=pow(10,multiplier);\n"
        codeString+="  Constraint_System cs_box;\n"
        for i,var in enumerate(varList):
            codeString+="  if(ptLower["+str(i)+"]<ptUpper["+str(i)+"]){\n"
            codeString+="    cs_box.insert(mult_factor*"+var+">=mult_factor*ptLower["+str(i)+"]);\n"
            codeString+="    cs_box.insert(mult_factor*"+var+"<=mult_factor*ptUpper["+str(i)+"]);\n"
            codeString+="  }\n"
            codeString+="  else{\n"
            codeString+="    cs_box.insert(mult_factor*"+var+"<=mult_factor*ptLower["+str(i)+"]);\n"
            codeString+="    cs_box.insert(mult_factor*"+var+">=mult_factor*ptUpper["+str(i)+"]);\n"
            codeString+="  }\n\n"
        codeString+="  return NNC_Polyhedron(cs_box);\n"
        codeString+="}\n\n"
        return codeString

    def printPoly(self):
        codeString="void print_box(NNC_Polyhedron poly){\n"
        codeString+="  Generator_System gs=poly.minimized_generators();\n"
        codeString+="  Generator_System::const_iterator i;\n"
        codeString+="  double divisor, dividend;\n"
        codeString+="  int dim;\n"
        codeString+="  cout << \"POLY: \" << endl;\n"
        codeString+="  for(i=gs.begin();i!=gs.end();++i){\n"
        codeString+="    if(i->is_point()){\n"
        codeString+="      divisor=mpz_get_d(i->divisor().get_mpz_t());\n"
        codeString+="      dim=int(i->space_dimension());\n"
        codeString+="      cout << \"POINT: \";\n"
        codeString+="      for(int j=0;j<dim;j++){\n"
        codeString+="        dividend=mpz_get_d(i->coefficient(Variable(j)).get_mpz_t());\n"
        codeString+="        cout<<dividend/divisor<<\" \";\n"
        codeString+="      }\n"
        codeString+="      cout<<endl;\n"
        codeString+="    }\n"
        codeString+="  }\n"
        codeString+="  cout << endl;\n"
        codeString+="}\n\n"
        return codeString

    def printHybridSimGuardsInvariants(self):
        file_name = "hybridSimGI.cpp"
        hybridSimFile = open(file_name,"w")
        declarationsReqd = '''#include <iostream> \n#include <stdio.h> \n#include <vector> \n#include <utility> \nusing namespace std; \n\n'''
        hybridSimFile.write(declarationsReqd)
        hybridSimFile.close()

        self.printHybridSimInvariants(file_name);
        self.printHybridSimGuardsResets(file_name);
    
    def printBloatedSimGuardsInvariants(self):
        file_name = "bloatedSimGI.cpp"
        bloatedSimFile = open(file_name,"w")
        declarationsReqd = '''#include <iostream> \n#include <stdio.h> \n#include <vector> \n#include <utility> \nusing namespace std; \n\n'''
        bloatedSimFile.write(declarationsReqd)
        bloatedSimFile.close()

        self.printBloatedSimInvariants(file_name);
        self.printBloatedSimGuardsResets(file_name);

    def printBloatedSimInvariants(self, file_name):
        self.printHybridSimInvariants(file_name);
        
    def printHybridSimInvariants(self, file_name):
        
        # Implements the function that generates the C++ file that
        # checks for Invariants for a given hyper-rectangle. 
        # The first thing it parses is the current mode and then 
        # checks for the invariant condition for each of the rectangles
        # for the current mode
        # If invariant is satisfied, then the rectangle is printed into 
        # invariant.dat file
        
        
        numVars=0;
        for j in self.vars:
            if not j.scope == "OUTPUT_DATA":
                numVars = numVars+1
        
        invariantFile = open(file_name,"a")

        # declarationsReqd = '''#include <iostream> \n#include <stdio.h> \nusing namespace std; \n'''
        # invariantFile.write(declarationsReqd)
        
        declaration = '''extern \"C\" bool invariantSatisfied(int curMode, double* ptLower, double *ptUpper){ \n'''
        invariantFile.write(declaration)
        
        currentMode = 0;
        for mode in self.automata[0].modes:
            
#            for j in mode.invs:
#                print j.raw
#                print self.generateCAPDInvCode(j.parsed, 2)        

            currentMode += 1;
            numAnds = 0
            numOrs = 0
            numMaxOrs = -1;
            for j in mode.invs:
                numAnds = numAnds + 1
                numOrs = numberOfLogicalOper(j.parsed)
                if numMaxOrs < numOrs:
                    numMaxOrs = numOrs
                    
            
            codeString = "  if(curMode == "+str(currentMode)+"){\n"
            
            '''Here, add the folowing things, the number of stuff you define is max of number of ors in
            each of the logical operations '''
            
            for j in range(1,numMaxOrs+1):
                codeString+= "    double RHSGE"+str(j)+";\n"
                codeString+= "    double LHSGE"+str(j)+";\n"
                codeString+= "    double evalE"+str(j)+";\n"
                codeString+= "    bool SATE"+str(j)+";\n"
            
            for j in range(1,numAnds+1):
                codeString+= "    bool SATEAND"+str(j)+";\n"
                
            currIndex = 0;
            numberOfOrs = 0;
            for j in mode.invs:
                numberOfOrs = numberOfLogicalOper(j.parsed)
                currIndex+= 1
                codeString+= self.generateCAPDInvCode(j.parsed, numberOfOrs)
                codeString+= "    SATEAND"+str(currIndex)+" = "
                for j in range(1,numberOfOrs+1):
                    codeString+= "SATE"+str(j)+" || "
                codeString+= "false;\n"

            codeString+= "    if("            
            for j in range(1,numAnds+1):
                codeString+= " SATEAND"+str(j)+" &&"
            codeString+= " true){\n"            
            codeString+= "      return true;\n"
            codeString+= "    }\n"
            codeString+= "    else{\n"
            codeString+= "      return false;\n"    
            codeString+= "    }\n"
            codeString+= "  }\n"
            invariantFile.write(codeString)

        codeString+= "  return false;\n"
        invariantFile.write(codeString)
        
        endmain = '''}\n\n'''
        invariantFile.write(endmain)
        invariantFile.close()


    def printBloatedSimGuardsResets(self, file_name):
        # pass
        self.printHybridSimGuardsResets(file_name);

        # Implementation of translation from guards in parsed format
        # to C++ CAPD file.
        # Design decisions: expressions > and < are considered as invariants
        # whereas expressions >= and <= are considered as urgent
        # i.e. if the expression is x + y >= 5, then the guard will be only
        # be enabled when the expression x + y for the reachable set has non
        # empty intersection with 5, rest all cases, its not enabled
        # The cases == and != are also implemented as urgent.
        
#         numVars=0;
#         for j in self.vars:
#             if not j.scope == "OUTPUT_DATA":
#                 numVars = numVars+1
        
#         guardFile = open(file_name,"a")
        
#         # declarationsReqd = '''#include <iostream> \n#include <stdio.h> \n#include <vector> \n#include <utility> \nusing namespace std; \n'''
#         # guardFile.write(declarationsReqd)
        
#         declaration = '''extern \"C\" vector<pair<int, vector<pair<int, double> > > > hitsGuard(int curMode, double *ptLower, double *ptUpper){ \n'''
#         guardFile.write(declaration)
        
#         codeString = "  vector<pair<int, vector<pair<int, double> > > > toRet;\n"

#         guardFile.write(codeString)

#         for i in self.automata[0].trans:
#             guardNode = i.guard.parsed
#             #guardNode[1].prints()
#             numAnds = numberOfLogicalOper(guardNode)
            
#             codeString = "  if(curMode == "+str(i.src+1)+"){\n"
#             for j in range(1,numAnds+1):
#                 codeString+= "    double RHSGE"+str(j)+";\n"
#                 codeString+= "    double LHSGE"+str(j)+";\n"
#                 codeString+= "    double evalE"+str(j)+";\n"
#                 codeString+= "    bool SATE"+str(j)+";\n"
            
#             codeString+= self.generateCAPDExpCode(guardNode,numAnds)
#             codeString+= "    if("
#             for j in range(1,numAnds+1):
#                 codeString+= " SATE"+str(j)+" &&"
#             codeString+= " true){\n"
            
#             #Code for printing resets in this part
            
#             codeString+= "      vector<pair<int, double> > resets;\n"

#             resetNode = i.actions
#             for listResetElem in resetNode :
#                 parsedVal = listResetElem.parsed
#                 #parsedVal = parsedVal[0]
#                 #parsedVal[0].prints()
                
#                 #print "reset node "
#                 l = 1
#                 for j in self.vars :
#                     if not j.scope=="OUTPUT_DATA":
# #                         if j.name+"'" == parsedVal.children[0].value :
# #                             resetString = "        Range["+str(l)+"] = "
#                         if j.name == parsedVal.children[0].value :
#                             resetString = "      resets.push_back(make_pair("+str(l)+","
#                         l = l+1

#                 #parsedVal.children[0].prints()
#                 #parsedVal.children[1].prints()
#                 resetString+=self.generateCAPDExpCode(parsedVal.children[1], 0)
#                 resetString+="));\n"
#                 #print resetString
#                 codeString+= resetString
            
            
#             codeString+= "      toRet.push_back(make_pair(" + str(i.dest+1) + ", resets));\n"
#             codeString+= "    }\n"
#             codeString+= "  }\n"
#             guardFile.write(codeString)
        
#         codeString = "  return toRet;\n"
#         guardFile.write(codeString)
        
#         codeString = "}\n\n"
#         guardFile.write(codeString)
        
#         guardFile.close()
        
    def printHybridSimGuardsResets(self, file_name):
        
        # Implementation of translation from guards in parsed format
        # to C++ CAPD file.
        # Design decisions: expressions > and < are considered as invariants
        # whereas expressions >= and <= are considered as urgent
        # i.e. if the expression is x + y >= 5, then the guard will be only
        # be enabled when the expression x + y for the reachable set has non
        # empty intersection with 5, rest all cases, its not enabled
        # The cases == and != are also implemented as urgent.
        
        numVars=0;
        for j in self.vars:
            if not j.scope == "OUTPUT_DATA":
                numVars = numVars+1
        
        guardFile = open(file_name,"a")
        
        # declarationsReqd = '''#include <iostream> \n#include <stdio.h> \n#include <vector> \n#include <utility> \nusing namespace std; \n'''
        # guardFile.write(declarationsReqd)
        
        declaration = '''extern \"C\" vector<pair<int, double *> > hitsGuard(int curMode, double *ptLower, double *ptUpper){ \n'''
        guardFile.write(declaration)
        
        codeString = "  vector<pair<int, double*> > toRet;\n"

        guardFile.write(codeString)

        for i in self.automata[0].trans:
            guardNode = i.guard.parsed
            #guardNode[1].prints()
            numAnds = numberOfLogicalOper(guardNode)
            
            codeString = "  if(curMode == "+str(i.src+1)+"){\n"
            for j in range(1,numAnds+1):
                codeString+= "    double RHSGE"+str(j)+";\n"
                codeString+= "    double LHSGE"+str(j)+";\n"
                codeString+= "    double evalE"+str(j)+";\n"
                codeString+= "    bool SATE"+str(j)+";\n"
            
            codeString+= self.generateCAPDExpCode(guardNode,numAnds)
            codeString+= "    if("
            for j in range(1,numAnds+1):
                codeString+= " SATE"+str(j)+" &&"
            codeString+= " true){\n"
            
            #Code for printing resets in this part
            
            resetNode = i.actions
            for listResetElem in resetNode :
                parsedVal = listResetElem.parsed
                #parsedVal = parsedVal[0]
                #parsedVal[0].prints()
                
                #print "reset node "
                l = 1
                for j in self.vars :
                    if not j.scope=="OUTPUT_DATA":
#                         if j.name+"'" == parsedVal.children[0].value :
#                             resetString = "        Range["+str(l)+"] = "
                        if j.name == parsedVal.children[0].value :
                            resetString = "      ptUpper["+str(l)+"] = "
                        l = l+1

                #parsedVal.children[0].prints()
                #parsedVal.children[1].prints()
                resetString+=self.generateCAPDExpCode(parsedVal.children[1], 0)
                resetString+=";\n"
                #print resetString
                codeString+= resetString
            
            
            codeString+= "      toRet.push_back(make_pair(" + str(i.dest+1) + ", ptUpper));\n"
            codeString+= "    }\n"
            codeString+= "  }\n"
            guardFile.write(codeString)
        
        codeString = "  return toRet;\n"
        guardFile.write(codeString)
        
        codeString = "}\n\n"
        guardFile.write(codeString)
        
        guardFile.close()

    def generateCAPDExpCode(self,guardNode,numberOfAnds):
        #print "-----------generateCAPDExpCode----------------"
        #print guardNode.value
        #print numberOfAnds
        #print guardNode.type
        #print guardNode.children[0].type,guardNode.children[0].value
        if guardNode.type == 'Logical':
            #print len(guardNode.children)
            if len(guardNode.children) == 2:
                child1 = guardNode.children[0]
                child2 = guardNode.children[1]
                if (child1.type != 'Logical') & (child2.type == 'Logical'):
                    str1 = self.generateExpressionCode(child1,numberOfAnds)
                    str2 = self.generateCAPDExpCode(child2, numberOfAnds-1)
                    return str1 + "\n" + str2
                    
                if (child1.type != 'Logical') & (child2.type != 'Logical'):
                    str1 = self.generateExpressionCode(child1,numberOfAnds)
                    str2 = self.generateExpressionCode(child2,numberOfAnds-1)
                    return str1 + "\n" + str2
                
        if guardNode.type != 'Logical':
            return self.generateExpressionCode(guardNode,numberOfAnds)
    

    def generateExpressionCode(self,ExpressionNode,numberOfAnds):
        
        if ExpressionNode.type == 'Relational':
            child1 = ExpressionNode.children[0]
            child2 = ExpressionNode.children[1]
            
            str1 = "    LHSGE"+str(numberOfAnds)+" = "+self.generateExpressionCode(child1, 0)+";\n"
            str2 = "    RHSGE"+str(numberOfAnds)+" = "+self.generateExpressionCode(child2, 0)+";\n"
            str3 = "    evalE"+str(numberOfAnds)+" = LHSGE"+str(numberOfAnds)+" - RHSGE"+str(numberOfAnds)+";\n"
            
            #if (ExpressionNode.value == '>=') | (ExpressionNode.value == '<=') | (ExpressionNode.value == '==') | (ExpressionNode.value == '!=') :
            #    str4 = "      SATE"+str(numberOfAnds)+" = (evalE"+str(numberOfAnds)+".contains(0));\n"
            #else :
            #    str4 = "      SATE"+str(numberOfAnds)+" = (evalE"+str(numberOfAnds)+" "+ExpressionNode.value+" 0);\n"
            str4 = "    SATE"+str(numberOfAnds)+" = (evalE"+str(numberOfAnds)+" "+ExpressionNode.value+" 0);\n"
                
            return str1 + str2 + str3 + str4;
        
        if ExpressionNode.type == 'Identifier' :
            l = 1
            for j in self.vars :
                if not j.scope=="OUTPUT_DATA":
                    if j.name == ExpressionNode.value :
                        return "ptUpper["+str(l)+"]"
                    l = l+1
            if ExpressionNode.value == "Simu_time":
                return "ptUpper[0]"
                    
            return str(ExpressionNode.value)
            
        if ExpressionNode.type == 'Negative' :
            l = 1
            for j in self.vars :
                if not j.scope =="OUTPUT_DATA":
                    if j.name == ExpressionNode.children[0].value :
                        return "-ptUpper["+str(l)+"]"
                    l = l + 1
                
            return "-"+str(ExpressionNode.children[0].value)
        
        if ExpressionNode.type == 'Expression' :
            
            child1 = ExpressionNode.children[0]
            child2 = ExpressionNode.children[1]
            
            str1 = self.generateExpressionCode(child1, 0)
            str2 = self.generateExpressionCode(child2, 0)
            #print "============================================================"
            #print str1
            return str1 + " " + ExpressionNode.value + " " + str2                

        
    def generateCAPDInvCode(self,guardNode,numberOfAnds):
        
        if guardNode.type == 'Logical':
            if len(guardNode.children) == 2:
                child1 = guardNode.children[0]
                child2 = guardNode.children[1]
                if (child1.type != 'Logical') & (child2.type == 'Logical'):
                    str1 = self.generateInvariantCode(child1,numberOfAnds)
                    str2 = self.generateCAPDInvCode(child2, numberOfAnds-1)
                    return str1 + "\n" + str2
                    
                if (child1.type != 'Logical') & (child2.type != 'Logical'):
                    str1 = self.generateInvariantCode(child1,numberOfAnds)
                    str2 = self.generateInvariantCode(child2,numberOfAnds-1)
                    return str1 + "\n" + str2
                
        if guardNode.type != 'Logical':
            return self.generateInvariantCode(guardNode,numberOfAnds)
    

    def generateInvariantCode(self,ExpressionNode,numberOfAnds):
        
        if ExpressionNode.type == 'Relational':
            child1 = ExpressionNode.children[0]
            child2 = ExpressionNode.children[1]
            
            str1 = "    LHSGE"+str(numberOfAnds)+" = "+self.generateInvariantCode(child1, 0)+";\n"
            str2 = "    RHSGE"+str(numberOfAnds)+" = "+self.generateInvariantCode(child2, 0)+";\n"
            str3 = "    evalE"+str(numberOfAnds)+" = LHSGE"+str(numberOfAnds)+" - RHSGE"+str(numberOfAnds)+";\n"
            
            if (ExpressionNode.value == '>=') | (ExpressionNode.value == '>') :
                str4 = "    SATE"+str(numberOfAnds)+" = not (evalE"+str(numberOfAnds)+" < 0);\n"
            else :
                str4 = "    SATE"+str(numberOfAnds)+" = not (evalE"+str(numberOfAnds)+" > 0);\n"

#            if (ExpressionNode.value == '>=') | (ExpressionNode.value == '<=') | (ExpressionNode.value == '==') | (ExpressionNode.value == '!=') :
#                str4 = "      SATE"+str(numberOfAnds)+" = (evalE"+str(numberOfAnds)+".contains(0));\n"
#            else :
#                str4 = "      SATE"+str(numberOfAnds)+" = (evalE"+str(numberOfAnds)+" "+ExpressionNode.value+" 0);\n"
                
            return str1 + str2 + str3 + str4;
        
        if ExpressionNode.type == 'Identifier' :
            l = 1
            for j in self.vars :
                if not j.scope=="OUTPUT_DATA":
                    if j.name == ExpressionNode.value :
                        return "ptUpper["+str(l)+"]"
                    l = l+1
            if ExpressionNode.value == "Simu_time":
                return "ptUpper[0]"
                    
            return ExpressionNode.value
            
        if ExpressionNode.type == 'Negative' :
            l = 1
            for j in self.vars :
                if not j.scope =="OUTPUT_DATA":
                    if j.name == ExpressionNode.children[0].value :
                        return "-ptUpper["+str(l)+"]"
                    l = l + 1
                
            return "-"+ExpressionNode.children[0].value
        
        if ExpressionNode.type == 'Expression' :
            
            child1 = ExpressionNode.children[0]
            child2 = ExpressionNode.children[1]
            
            str1 = self.generateExpressionCode(child1, 0)
            str2 = self.generateExpressionCode(child2, 0)
            
            return str1 + " " + ExpressionNode.value + " " + str2                
        
        
    
    #The callback to get the modes to auto expand
    def treeExpandCallback(self,treeview,it,path):
      parIter=self.treestore.iter_parent(it)
      if not parIter==None:
        parValue=self.treestore.get_value(parIter,0)
        if parValue=="Modes" or parValue=="Transitions":
          treeview.expand_row(path,True)

    def editRowCallback(self,cell,path,newText):
      self.treestore[path][0]=newText

    def capd_callback(self,widget):
        try:
            #file = open(self.file_name+".cpp", "w")
            try:
                self.convertToCAPD(self.file_name)
                dialog = gtk.MessageDialog(None,0,gtk.MESSAGE_INFO, gtk.BUTTONS_CLOSE,"CAPD C++ file has been created")
                dialog.set_title("Success")
                dialog.run()
                dialog.destroy()
            finally:
                #file.close()
                something = 1
        except IOError:
            dialog = gtk.MessageDialog(None,0,gtk.MESSAGE_ERROR,gtk.BUTTONS_CLOSE,"CAPD C++ file failed to create")
            dialog.set_title("Error")
            dialog.run()
            dialog.destroy()
        
    
    def vnode_callback(self,widget):
        try:
            file = open(self.file_name+".cc", "w")
            try:
                self.convertToVnodeLP(file)
                dialog = gtk.MessageDialog(None,0,gtk.MESSAGE_INFO, gtk.BUTTONS_CLOSE,"C++ file has been created")
                dialog.set_title("Success")
                dialog.run()
                dialog.destroy()
            finally:
                file.close()
        except IOError:
            dialog = gtk.MessageDialog(None,0,gtk.MESSAGE_ERROR,gtk.BUTTONS_CLOSE,"C++ file failed to create")
            dialog.set_title("Error")
            dialog.run()
            dialog.destroy()

    
    def hytech_callback(self,widget):
        try:
            file = open(self.file_name+".hy", "w")
            try:
                self.convertToHyTech(file)
                dialog = gtk.MessageDialog(None,0,gtk.MESSAGE_INFO, gtk.BUTTONS_CLOSE,"Hytech file has been created")
                dialog.set_title("Success")
                dialog.run()
                dialog.destroy()
            finally:
                file.close()
        except IOError:
            dialog = gtk.MessageDialog(None,0,gtk.MESSAGE_ERROR,gtk.BUTTONS_CLOSE,"Hytech file failed to create")
            dialog.set_title("Error")
            dialog.run()
            dialog.destroy()
    
    def uppaal_callback(self,widget):
        try:
            file = open(self.file_name+"_uppaal.xml", "w")
            try:
                self.convertToUPPAAL(file)
                dialog = gtk.MessageDialog(None,0,gtk.MESSAGE_INFO, gtk.BUTTONS_CLOSE,"UPPAAL XML file has been created")
                dialog.set_title("Success")
                dialog.run()
                dialog.destroy()
            finally:
                file.close()
        except IOError:
            dialog = gtk.MessageDialog(None,0,gtk.MESSAGE_ERROR,gtk.BUTTONS_CLOSE,"UPPAAL XML file failed to create")
            dialog.set_title("Error")
            dialog.run()
            dialog.destroy()
                  
    def hyxml_callback(self,widget):
        try:
            file = open(self.file_name+"_hyxml.xml", "w")
            try:
                self.convertToXML(file)
                dialog = gtk.MessageDialog(None,0,gtk.MESSAGE_INFO, gtk.BUTTONS_CLOSE,"HyXML file has been created")
                dialog.set_title("Success")
                dialog.run()
                dialog.destroy()
            finally:
                file.close()
        except IOError:
            dialog = gtk.MessageDialog(None,0,gtk.MESSAGE_ERROR,gtk.BUTTONS_CLOSE,"HyXML file failed to create")
            dialog.set_title("Error")
            dialog.run()
            dialog.destroy()
    
    def delete_event(self,widget,event,data=None):
        print "Window Closed"
        return False
    
    def destroy(self,widget, data=None):
        gtk.main_quit()
    
    def build_tree(self,node,parent):
        if (node.value != None):
            if (node.value != ''):
                curr = self.treestore.append(parent,[(node.type,node.value)])
        else:
            curr = self.treestore.append(parent,[node.type])
        for i in node.children:
            self.build_tree(i,curr)
    
    def remove_nd(self):
        for i in self.automata[0].modes:
            i.invs = filter(lambda j:j.raw.find("choice")==-1, i.invs)
        
        for i in self.automata[0].trans:
            i.guard.parsed = check_and_remove_nd(i.guard.parsed, "choice")
            i.guard.parsed = check_and_remove_nd(i.guard.parsed, "choice2")
            i.guard.raw = collapse2(i.guard.parsed)
        
    def convertToCAPD(self, filename):
        print("CAPD CONVERTING START")
        
        delete_element_list = []
        annotfile = open(filename+"annot", "w")
        bufferString2 = self.annotations.replace("\\n", '\n')        
        
        #for traverseIndex in range(0,len(bufferString2)):
        #    if bufferString2[traverseIndex] == 'i' && bufferString2[traverseIndex+1] == 'n' && bufferString2[traverseIndex+2] == 'i' && bufferString2[traverseIndex+3] == 'i' 
        
        file = open(filename+".cpp","w")
        ''' Creates a C++ file which uses CAPD classes and gives a simulator after compiling \n '''
        infoFile = '''/* CAPD C++ file generated Automatically from HyLink */\n'''
        file.write(infoFile)
        declarationsReqd = ''' #include <iostream> \n #include "capd/capdlib.h" \n using namespace std; \n using namespace capd; \n '''
        file.write(declarationsReqd)
        curAut = self.automata[0]
        
        declarationsReqd = "\nint getNextMode(int curMode, interval curModeTime);\n"
        file.write(declarationsReqd)
        
        #print " Have to give definition of automata and all the modes "
        out_vars = []
        countVars = 0
        cont_vars = []
        
        mainDeclaration = '''main(){ \n \n  cout.precision(10);\n  try{ \n'''
        file.write(mainDeclaration)

        for vars in self.vars:
            #print vars.name + " this is a variable \n"
            if not vars.scope == 'OUTPUT_DATA' and vars.name.find("dot") == -1 and vars.name.find("clock") == -1:
                countVars = countVars+1
                cont_vars+=[vars.name]
                #print " yes, this variable is regular and not dot or output" 
            if vars.scope == 'OUTPUT_DATA':
                out_vars+= [vars.name]        
        
        varCount = 0
        for varnames in cont_vars:
            bufferString2 = bufferString2.replace("init="+varnames, "init=x"+str(varCount+1))
            bufferString2 = bufferString2.replace("forbidden="+varnames, "forbidden=x"+str(varCount+1))
            bufferString2 = bufferString2.replace(","+varnames, ",x"+str(varCount+1))
            bufferString2 = bufferString2.replace(varnames+",", "x"+str(varCount+1)+",")
            varCount+=1

        bufferString2 = "dimensions="+str(varCount)+"\n"+"simulator=capdsim"+"\n"+bufferString2
        annotfile.write(bufferString2)
        annotfile.close()
                
        numModes = 0;
        initialMode = curAut.initial_mode_id;
        initialMode += 1;
        temp = ""
        loop = 0
        #print " Initial mode is " + str(initialMode) + "\n"
        for curMode in curAut.modes:
            numModes = numModes + 1
            #if(curMode.)
            #print " Now generate diff eqns for each mode \n"
            newDiff = ''' /* Differential equation for mode ''' + curMode.name + ''' Testing */ \n'''
            file.write(newDiff)
            varstring = "var:"
            difvarstring=''
            diffunstring=''
            funstring = "fun:"
            lenVars = cont_vars.__len__();
            #print "length is -- " + str(lenVars) + "\n"
            index=0;
            
            for variable in cont_vars:
                #print(variable)
                
                index = index+1
                varstring = varstring + variable
                difvarstring = difvarstring + variable
                if index == lenVars :
                    varstring = varstring + ";"
                else:
                    varstring = varstring + ","
                    difvarstring = difvarstring + ","
                for dai in curMode.dais:
                    flag = 0
                    for v in out_vars:
                        if dai.raw.count(v) > 0:
                            flag = 1
                    if flag == 0:
                        if collapse(dai.parsed.children[0]) == variable+"_dot":
                            #dai.parsed.children[1].bolunprints()
                            funstring = funstring + collapse(dai.parsed.children[1])
                            diffunstring = diffunstring + collapse(dai.parsed.children[1])
                            if index == lenVars:
                                funstring = funstring + ";"
                            else:
                                funstring = funstring + ","
                                diffunstring = diffunstring +","
            
            modeString = "    IMap mode"+str(numModes)+"(\""+varstring+funstring+"\");\n"
            #print(difvarstring)
            #print(diffunstring)
            delete_element =jacobian(difvarstring,diffunstring,loop)
            delete_element_list.append(delete_element)
            loop+=1
    
            jff = "mode"+str(numModes)+" (\""+varstring+funstring+"\");\n"
            temp= temp+jff
            file.write(modeString)
            
        taylorString = "    ITaylor* solvers[] = {\n"
        i = 0
        for i in range(0,numModes):
            taylorString = taylorString + "     new ITaylor(mode"+str(i+1)+",5),\n"
        """
        if not i == 0:
            i = i+1
        taylorString = taylorString + "     new ITaylor(mode"+str(i+1)+",5)\n"
        """
        taylorString = taylorString + "    };\n"
        file.write(taylorString)
        
        declarationString = "    double initialState["+str(countVars)+"];\n"
        declarationString+= "    IVector IState("+str(countVars)+");\n"
        declarationString+= "    int i;\n    double absErr, relErr;\n    double tstep,Gt;\n"
        declarationString+= "    double curTime;\n    int curMode, nextMode;\n    std::cin >> curTime;\n"
        declarationString+= "    for(i=0;i<"+str(countVars)+";i++){\n      std::cin >> initialState[i]; IState[i] = initialState[i];\n    }\n"
        declarationString+= "    std::cin >> absErr >> relErr >> tstep >> Gt;\n"
        declarationString+= "    std::cin >> curMode;\n    nextMode = curMode;\n"
        declarationString+= "    IVector SimState("+str(countVars)+");\n"
        declarationString+= "    IVector PrevSimState("+str(countVars)+");\n"
        declarationString+= "    PrevSimState = IState;\n"
        declarationString+= "    interval tstepi(tstep);\n"
        declarationString+= "    interval Gti(Gt);\n"
        declarationString+= "    for(i=0;i<"+str(numModes)+";i++)\n      solvers[i]->setStep(tstepi);\n"
        declarationString+= "    interval currTime(curTime), currModeTime(0.0);\n"
        #declarationString+= "    int curMode = "+ str(initialMode)+", nextMode = "+ str(initialMode)+";\n"
        declarationString+= "    C0HORect2Set SimSet(IState);\n"
        file.write(declarationString)
        
        # New way for generating simulations
        declarationString = "    ITimeMap timeMap(*solvers[curMode-1]);\n"
        declarationString+= "    timeMap(Gt,SimSet);\n"
        declarationString+= "    const ITaylor::SolutionCurve& curve = solvers[curMode-1]->getCurve();\n"
        #file.write(declarationString)
        
        solverString = "    while(currTime < Gti){\n"
        solverString+= "      interval integInterval(0);\n"
        solverString+= "      integInterval(0);\n"
        solverString+= "      std::cout << \" \" << currTime.leftBound();\n"
        solverString+= "      for(i=0;i<"+str(countVars)+";i++){\n"
        solverString+= "        std::cout << \" \" << PrevSimState[i].leftBound();\n"
        solverString+= "      }\n"
        solverString+= "      std::cout << endl;\n"
        solverString+= "      SimSet.move(*solvers[curMode-1]);\n"
        solverString+= "      SimState = (IVector)SimSet;\n"
        solverString+= "      currTime+=tstepi;currModeTime+=tstepi;\n"
        solverString+= "      std::cout << \" \" << currTime.leftBound();\n"
        solverString+= "      for(i=0;i<"+str(countVars)+";i++){\n"
        solverString+= "        std::cout << \" \" << SimState[i].leftBound();\n"
        solverString+= "      }\n"
        solverString+= "      std::cout << endl;\n"
        solverString+= "      PrevSimState = SimState;\n"
        solverString+= "      nextMode = getNextMode(curMode,currModeTime);\n"
        solverString+= "      if(nextMode != curMode){\n"
        solverString+= "        curMode = nextMode; currModeTime = 0.0;\n"
        solverString+= "      }\n"
        solverString+= "    }\n"
        #file.write(solverString)

        
        # Old Technique for generating Simulation.
        # Now using rigorous enclosures
        solverString = "    while(currTime < Gti){\n"
        solverString+= "      std::cout << \" \" << currTime.leftBound();\n"
        solverString+= "      for(i=0;i<"+str(countVars)+";i++){\n"
        solverString+= "        std::cout << \" \" << PrevSimState[i].leftBound();\n"
        solverString+= "      }\n"
        solverString+= "      std::cout << endl;\n"
        solverString+= "      SimSet.move(*solvers[curMode-1]);\n"
        solverString+= "      SimState = (IVector)SimSet;\n"
        solverString+= "      currTime+=tstepi;currModeTime+=tstepi;\n"
        solverString+= "      std::cout << \" \" << currTime.leftBound();\n"
        solverString+= "      for(i=0;i<"+str(countVars)+";i++){\n"
        solverString+= "        std::cout << \" \" << SimState[i].leftBound();\n"
        solverString+= "      }\n"
        solverString+= "      std::cout << endl;\n"
        solverString+= "      PrevSimState = SimState;\n"
        solverString+= "      nextMode = getNextMode(curMode,currModeTime);\n"
        solverString+= "      if(nextMode != curMode){\n"
        solverString+= "        curMode = nextMode; currModeTime = 0.0;\n"
        solverString+= "      }\n"
        solverString+= "    }\n"
        file.write(solverString)
            
        closeString = "  }catch(exception& e){\n    cout << \"Exception caught!\" << e.what() << endl << endl;\n  }\n}\n\n"
        file.write(closeString)
        createCDFfunction(delete_element_list)
        switchingString = "int getNextMode(int curMode, interval curModeTime){\n"
        
#        for transitions in curAut.trans:
#            source = transitions.src
#            destination = transitions.dest
#            guardString = transitions.guard.raw
#            print "the guard string is " + guardString + "\n"
#            valid = re.search(r"(\d+)\.(\d+)",guardString)
#            if valid:
#                print " this is valid value \n"
#                for matchedStrings in valid.groups():
#                    print "match is " + matchedStrings + "\n"
#                    clockVal = float(matchedStrings)
#                    print " clock val is " + str(clockVal) + "\n"
#                switchingString+= "  if(curMode == "+str(source+1)+" && ( curModeTime >= " + str(clockVal) + " || curModeTime.contains(" + str(clockVal) + ") ) )\n"
#                switchingString+= "    return "+str(destination+1)+";\n"            
#            valid = re.search(r"(\d+)",guardString)
#            if valid:
#                print " this is valid value \n"
#                for matchedStrings in valid.groups():
#                    clockVal = int(matchedStrings)
#                    print " clock val is " + str(clockVal) + "\n"
#                switchingString+= "  if(curMode == "+str(source+1)+" && ( curModeTime >= " + str(clockVal) + " || curModeTime.contains(" + str(clockVal) + ") ) )\n"
#                switchingString+= "    return "+str(destination+1)+";\n"
                            
        switchingString+= "  return curMode;\n"
        switchingString+= "}\n"
        file.write(switchingString)
        file.close()



        
    def convertToVnodeLP(self, file):
        ''' Creates a VNODE-LP file that gives a simulator after compiling it with VNODE-LP package.'''
        infoFile = '''/* VNODE-LP C++ file generated Automatically from HyLink */\n'''
        file.write(infoFile)
        declarationsReqd = ''' #include <ostream> \n #include "vnode.h" \n using namespace std; \n using namespace vnodelp; \n '''
        file.write(declarationsReqd)
        curAut = self.automata[0]
        print " Have to give definition of automata and all the modes "

        out_vars = []
        countVars = 0
        cont_vars = []

        for vars in self.vars:
            print vars.name + " this is a variable \n"
            if not vars.scope == 'OUTPUT_DATA' and vars.name.find("dot") == -1:
                countVars = countVars+1
                cont_vars+=[vars.name]
                print " yes, this variable is regular and not dot or output" 
            if vars.scope == 'OUTPUT_DATA':
                out_vars+= [vars.name]
        
        for curMode in curAut.modes:
            print " Have to generate differential euqations for each mode "
            newDiff = ''' /* Differential equation for mode ''' + curMode.name + ''' Testing */ \n'''
            file.write(newDiff)
            
            newDiff = '''template<typename var_type> \n'''
            newDiff += '''void '''+curMode.name.replace(".","") + '''(int n, var_type* RHSDE, const var_type* STATE, var_type t, void* param){\n'''
            file.write(newDiff)
            constintervals = []
            countConstants=0
            allDiffEqns = ""
            for dai in curMode.dais:
                    flag = 0
                    for v in out_vars:
                        if dai.raw.count(v) > 0:
                            flag = 1
                    if flag==0:
                        newDiff = dai.raw
                        for i in range(1,10):
                            valid = re.search(r"(\d+)\.?(\d+)?",newDiff)
                            if not valid:
                                break
                            else:
                                countConstants+=1;
                                constintervals += [newDiff[valid.start():valid.end()]]
                                newDiff = newDiff[:valid.start()] + "IntervalConst" + genStringNumber(countConstants) + newDiff[valid.end():]
                        valid = re.search(r"(\d+)",newDiff)
                        if valid:
                            print "this is searching for digits and trying to do it."
                            print newDiff[:valid.start()] + newDiff[valid.end():]
#                            for matchedstrings in valid.groups():
                        valid = re.search(r"(\d+)\.(\d+)",newDiff)
                        if valid:
                            print "this is searching for digits with decimals and trying to do it."
                            for matchedstrings in valid.groups():
                                print matchedstrings                                                        
                        countContVars = 0
                        for eachVar in cont_vars:
                            newDiff = newDiff.replace(eachVar+"_dot", "RHSDE["+str(countContVars)+"]")
                            newDiff = newDiff.replace(eachVar,"STATE["+str(countContVars)+"]")
                            countContVars+=1
                        countContVars = 0
                        for eachVar in cont_vars:
                            print eachVar + "this is each var \n"
                            newDiff = newDiff.replace(eachVar,"STATE["+str(countContVars)+"]")
                            countContVars+=1
                        newDiff += ";\n"
                        allDiffEqns += newDiff
            if len(constintervals) > 0:
                printString = "interval "
                file.write(printString)
                countConstIntervals=0;
                for number in constintervals:
                    countConstIntervals+=1
                    printString = "IntervalConst"+genStringNumber(countConstIntervals)+'''(''' + number + '''), '''
                    file.write(printString)
                file.write("Zero(0);\n")
            file.write(allDiffEqns)
#                        file.write(newDiff)
#                        file.write("\n\t\t<dai equn='"+dai.raw.replace(">","&gt;").replace("<","&lt;")+"'/>")
            newDiff = '''}\n'''
            file.write(newDiff)
        mainFunction = ''' int main() { \n'''
        file.write(mainFunction);
        declarationString = ''' const int dim = ''' + str(countVars) + ''';\n interval t=0, tend, tstep;\n interval absErr, relErr; \n'''
        file.write(declarationString)
        declarationString = ''' double absErrd, relErrd, timeStep, timeEnd;\n int modeID;\n'''
        file.write(declarationString)
        declarationString = ''' iVector state(dim);\n double stated[dim];\n'''
        file.write(declarationString)
        
        simulatorCount = 1
        for curMode in curAut.modes:
            print "Generating the solver for each differential equation \n"
            generatorString = ''' AD* ad'''+str(simulatorCount)+''' = new FADBAD_AD(dim,'''+curMode.name+''','''+curMode.name+''');\n'''
            file.write(generatorString)
            generatorString = ''' VNODE* Solver'''+str(simulatorCount)+''' = new VNODE(ad'''+str(simulatorCount)+''');\n'''
            file.write(generatorString)
            simulatorCount = simulatorCount+1
        readString = ''' for(int j=0;j<dim;j++){\n'''
        readString+= '''    cin >> stated[j];\n'''
        readString+= '''    state[j] = stated[j];\n'''
        readString+= ''' }\n'''
        file.write(readString)
        readString = ''' cin >> absErrd >> relErrd >> timeStep;\n absErr = absErrd;\n relErr = relErrd;\n tstep = timeStep;\n'''
        file.write(readString)
        #readString = ''' cin >> modeID >> timeEnd;\n tend = timeEnd;\n'''
        readString = ''' cin >> timeEnd;\n tend = timeEnd;\n'''
        file.write(readString)
        
        configString = '''     while(midpoint(t) < midpoint(tend)){\n'''
        configString+= '''       Solver1->integrate(t,state,t+tstep);\n'''
        configString+= '''       if(!Solver1->successful())\n'''
        configString+= '''         cout << "VNODE-LP could not reach the t = "<< t+tstep<<endl;\n'''
        configString+= '''       else{\n'''
        configString+= '''         int i;\n'''        
        configString+= '''         cout << " " << midpoint(t);\n'''
        configString+= '''         for(i=0;i<dim;i++)\n'''
        configString+= '''           cout << " " << midpoint(state[i]);\n'''
        configString+= '''         cout << " ''' + "\\"+"n" + '''";\n'''
        #configString+= '''         printVector(state);\n'''
        configString+= '''       }\n'''
        configString+= '''     }\n'''
        file.write(configString)

        readString = ''' return 0;\n }'''
        file.write(readString) 

        
        
#        configString = ''' while( modeID!= -1 ){\n'''
#        file.write(configString)
#        simulatorCount = 1;
#        for curMode in curAut.modes:
#            configString = '''   if(modeID =='''+str(simulatorCount)+''' ){\n'''
#            configString+= '''     while(midpoint(t) < midpoint(tend)){\n'''
#            configString+= '''       Solver'''+str(simulatorCount)+'''->integrate(t,state,t+tstep);\n'''
#            configString+= '''       if(!Solver'''+str(simulatorCount)+'''->successful())\n'''
#            configString+= '''         cout << "VNODE-LP could not reach the t = "<< t+tstep<<endl;\n'''
#            configString+= '''       else\n'''
#            configString+= '''         printVector(state);\n'''
#            configString+= '''     }\n'''
#            configString+= '''   }\n'''
#            file.write(configString)
#            simulatorCount+=1        
#        readString = ''' cin >> modeID >> timeEnd;\n tend = timeEnd;\n'''
#        file.write(readString)
#        readString = ''' }\n'''
#        file.write(readString)
#        readString = ''' return 0;\n }'''
#        file.write(readString) 

        
    def convertToUPPAAL(self, file):
        '''creates an XML file which can be used for verification in UPPAAL. This does not create any UPPAAL query files
        and the safety queries have to be written separately by the user'''
        xml_dec = '''<?xml version="1.0" encoding="utf-8"?><!DOCTYPE nta PUBLIC '-//Uppaal Team//DTD Flat System 1.1//EN' 'http://www.it.uu.se/research/group/darts/uppaal/flat-1_1.dtd'><nta>\n'''
        file.write(xml_dec)
    
        global_dec = '''<declaration></declaration>\n'''
        file.write(global_dec)
        
        hy = self.automata[0]
        t = '''<template>\n<name x="5" y="5">'''
        t+=hy.name
        t+= '''</name>\n<declaration>'''
        
        for x in self.vars:
            if not x.scope == 'OUTPUT_DATA':
                if x.update_type == 'Continuous':
                    t+="clock "+x.name+";\n"
                elif x.update_type == 'Discrete':
                    t+="int "+x.name+";\n"
        t+='''</declaration>\n\n'''
        
        for i in hy.modes:
            if i.initial == True:
                iref = i.id
            strng = '''\n<location id="id'''+str(i.id)+'''" x="'''+str(i.xpos)+'''" y="'''+str(i.ypos)+'''">
<name x="'''+str(i.xpos)+'''" y="'''+str(i.ypos+15)+'''">'''+i.name+'''</name>
<label kind="invariant" x="'''+str(i.xpos)+'''" y="'''+str(i.ypos+30)+'''">'''
            for j in range(0,len(i.invs)):
                if j == 0:
                    strng+= (i.invs[j].raw.replace(">","&gt;")).replace("<","&lt;")
                else:
                    strng+= " and " + (i.invs[j].raw.replace(">","&gt;")).replace("<","&lt;")
            strng+='''</label>
</location>\n\n'''      
            t+=strng
        t+='''<init ref="id'''+str(iref)+'''"/>'''
        file.write(t)
        
        t = ""
        
        for i in hy.trans:
            if i.src != i.dest:
                for j in hy.modes:
                    if j.id == i.src:
                        x1 = j.xpos
                        y1 = j.ypos
                    if j.id == i.dest:
                        x2 = j.xpos
                        y2 = j.ypos
                print str(x1)+","+str(y1)+" --> "+str(x2)+","+str(y2)
                theta = math.atan((y2-y1)/(x2-x1+1))
                nailx = int(((x1+x2)/2) + (i.fac*-50*math.sin(theta))) 
                naily = int(((y1+y2)/2) + (i.fac*50*math.cos(theta)))
                if (len(i.actions) > 0):
                    resets = i.actions[0].raw.replace("'","")
                    for k in i.actions[1:]:
                        resets += ', '+k.raw.replace("'","")
                    strng = '''\n<transition>
    <source ref="id'''+str(i.src)+'''"/>
    <target ref="id'''+str(i.dest)+'''"/>
    <label kind="guard" x="'''+str(nailx)+'''" y="'''+str(naily)+'''">'''+(i.guard.raw.replace(">","&gt;")).replace("<","&lt;")+'''</label>
    <label kind="assignment" x="'''+str(nailx)+'''" y="'''+str(naily+15)+'''">'''+(resets.replace("~","!")).replace(">","&gt;").replace("<","&lt;")+'''</label>
    <nail x="'''+str(nailx)+'''" y="'''+str(naily)+'''"/>
</transition>\n\n
    '''
                else:
                    strng = '''\n<transition>
    <source ref="id'''+str(i.src)+'''"/>
    <target ref="id'''+str(i.dest)+'''"/>
    <label kind="guard" x="'''+str(nailx)+'''" y="'''+str(naily)+'''">'''+(i.guard.raw.replace(">","&gt;")).replace("<","&lt;")+'''</label>
    <nail x="'''+str(nailx)+'''" y="'''+str(naily)+'''"/>
</transition>\n\n
    '''                    
                t+=strng
            else:
                for j in hy.modes:
                    if j.id == i.src:
                        x = j.xpos
                        y = j.ypos
                if (len(i.actions) > 0):
                    resets = i.actions[0].raw
                    for k in i.actions[1:]:
                        resets += ', '+k.raw
                    strng = '''\n<transition>
    <source ref="id'''+str(i.src)+'''"/>
    <target ref="id'''+str(i.dest)+'''"/>
    <label kind="guard" x="'''+str(x+100)+'''" y="'''+str(y)+'''">'''+(i.guard.raw.replace(">","&gt;")).replace("<","&lt;")+'''</label>
    <label kind="assignment" x="'''+str(x+100)+'''" y="'''+str(y+15)+'''">'''+(resets.replace(">","&gt;")).replace("<","&lt;")+'''</label>
    <nail x="'''+str(x+100)+'''" y="'''+str(y-50)+'''"/>
    <nail x="'''+str(x+100)+'''" y="'''+str(y+50)+'''"/>
    </transition>\n\n
    '''
                else:
                    strng = '''\n<transition>
    <source ref="id'''+str(i.src)+'''"/>
    <target ref="id'''+str(i.dest)+'''"/>
    <label kind="guard" x="'''+str(x+100)+'''" y="'''+str(y)+'''">'''+(i.guard.raw.replace(">","&gt;")).replace("<","&lt;")+'''</label>
    <nail x="'''+str(x+100)+'''" y="'''+str(y-50)+'''"/>
    <nail x="'''+str(x+100)+'''" y="'''+str(y+50)+'''"/>
    </transition>\n\n
    '''
                t+=strng
        
        file.write(t)
        
        t = '''</template><system>system '''+hy.name+''';</system></nta>'''
        file.write(t)
        
    def convertToXML(self,propertyList):
      hyxml=etree.Element("hyxml")
      for var in self.vars:
        if not var.scope=="OUTPUT_DATA":
          etree.SubElement(hyxml,"variable",{"name":var.name,"scope":var.scope,"type":var.type})

      auto=etree.SubElement(hyxml,"automaton",{"name":self.automata[0].name})
      modeIndex = 0
      for mode in self.automata[0].modes:
        modeIndex+=1
        annotationFound=0
        m=etree.SubElement(auto,"mode",{"id":str(mode.id),"initial":str(mode.initial),"name":mode.name})
        for dai in mode.dais:
          etree.SubElement(m,"dai",{"equation":dai.raw})
        for inv in mode.invs:
          etree.SubElement(m,"invariant",{"equation":inv.raw})
        for elements in self.annotationsParsed :
          if modeIndex == elements[0] :
            annotationFound=1  
            annotTree = etree.SubElement(m,"annotation",{"mode":mode.name})
            etree.SubElement(annotTree,"K",{"value":str(elements[1])})
            etree.SubElement(annotTree,"gamma",{"value":str(elements[2])})
            if elements[3] == 1 :
                annotType = "exponential"
            if elements[3] == 2 :
                annotType = "linear"
            if elements[3] >= 3 :
                annotType = "contraction"
            etree.SubElement(annotTree,"type",{"string":annotType,"value":str(elements[3])})
            #etree.SubElement(annotTree,"linear",{"value":str(elements[4])})

            
        if annotationFound == 0 :
          annotTree=etree.SubElement(m,"annotation",{"mode":mode.name})
          etree.SubElement(annotTree,"K",{"value":str(1.1)})
          etree.SubElement(annotTree,"gamma",{"value":str(0.0)})
          etree.SubElement(annotTree,"type",{"string":"exponential", "value":"1"})



      for tran in self.automata[0].trans:
        t=etree.SubElement(auto,"transition",{"id":str(tran.id),"destination":str(tran.dest),"source":str(tran.src)})
        etree.SubElement(t,"guard",{"equation":tran.guard.raw})
        for act in tran.actions:
          etree.SubElement(t,"action",{"equation":act.raw})

      for prop in propertyList:
        prop=prop[0]
        pt1 = etree.SubElement(hyxml,"property",{"name":prop.name,"type":str(prop.type),
                         "initialSet":prop.initialSetStr,"unsafeSet":prop.unsafeSetStr})
        if prop.status == "Verified" or prop.status == "Verified*" or (prop.paramData[0] != 0 and prop.paramData[1] != 0 and prop.paramData[2] != 0 and prop.paramData[3] != 0) :
            etree.SubElement(pt1,"parameters",{"delta":str(prop.paramData[0]), "timestep":str(prop.paramData[1]),
                        "timehorizon":str(prop.paramData[2]), "taylororder":str(prop.paramData[3])})
      
      return etree.tostring(hyxml,pretty_print=True,xml_declaration=True,encoding="utf-8",doctype="<!DOCTYPE hyxml>")
    
    def convertToHyTech(self, file):
        '''writes a hytech file based on the values of the hytech object'''
        lines = []
        continuousVars = []
        discreteVars = []
        initialLocations = []
        initialConditions = []
        lines.append('--automatically generated hytech file from Stateflow/Simulink model\n')
        
        lines.append('var')
        for var in self.vars:
            if var.update_type == "Continuous":
                continuousVars.append(var.name)
            elif var.update_type == "Discrete":
                discreteVars.append(var.name)
            else:
                print var.update_type + " " + var.name
        if continuousVars:
            lines.append("\t" + ",\n\t".join(continuousVars) + ": analog;")
        if discreteVars:
            lines.append("\t" + ",\n\t".join(discreteVars) + ": discrete;")        
        lines.append("\n-- --------------------------------------------------")

        for automaton in self.automata:

            conj = '' #indicates whether the initial mode has initial conditions
            
            #called because of the way hytech's inability to handle deterministic transitions
            reconstructGuardsInvForHytech(automaton)
            makeInitialized(automaton)
            
            lines.append("\nautomaton %s" % (automaton.name))
            lines.append("synclabs:;")
            index = len(lines)
            for mode in automaton.modes:
                if mode.initial: #potential for bug if more than one initial mode
                    if mode.initialConditions:
                        conj = " & "
                        for cond in mode.initialConditions:
                            for oldCond in initialConditions[:]:
                                if oldCond == cond:
                                    initialConditions.remove(oldCond)
                                    break
                            initialConditions.append(cond)
                    lines.insert(index, "initially %s%s%s;" % (mode.name, 
                                    conj, " & ".join(mode.initialConditions)))
                    initialLocations.append("loc[%s] = %s" % (automaton.name, mode.name))
                
                #if no invariants than invariant is True
                if mode.invs:
                    invStr = " & ".join(inv.raw for inv in mode.invs)
                else:
                    invStr = "True" 
                lines.append("\nloc %s: while %s wait {%s}" % (mode.name, invStr,
                            ", ".join("d%s" % (re.sub(r"_dot", lambda x: '', dai.raw)) for dai in mode.dais)))
                for tran in automaton.trans[:]:
                    #WARNING: after the transitions are used they are removed from the list
                    if tran.src == mode.id:
                        if tran.actions:
                            actions = "do {%s}" % ", ".join(action.raw for action in tran.actions)
                        else:
                            actions = "\t"
                        if tran.extraG is None:
                            extraG = ""
                        else:
                            extraG = "& %s " % tran.extraG.raw
                            
                        lines.append("  when %s %s%s goto %s;" % (
                        re.sub(r' and ', lambda x: ' & ', tran.guard.raw), 
                        extraG, actions, automaton.modes[tran.dest].name))
                        automaton.trans.remove(tran)
                        
            lines.append("\nend\n")
            lines.append("-- --------------------------------------------------\n")
        
        #START VERIFICATION PORTION
        lines.append("var")
        lines.append("   init_reg,")
        lines.append("   unsafe_reg,")
        lines.append("   old,")
        lines.append("   reached_reg: region;")
        
        lines.append("\nunsafe_reg := %s;" % "BaseC>1")
        conj = ''
        if initialConditions:
            conj = ' & '
        lines.append("init_reg := %s%s%s;" % 
                    (" & ".join(initialLocations), conj, " & ".join(initialConditions)))
        lines.append("\nreached_reg := reach forward from init_reg endreach;")
#        lines.append("print reached_reg;")
#        lines.append('prints "EXISTENTIALLY QUANTIFIED";')
#        lines.append("print omit all locations reached_reg;")
        
        ##DEBUGGING LINE##
#        lines.append('reached_reg := init_reg;\nold := init_reg;\nreached_reg := post(old); \
#            \nwhile not reached_reg <= old do\n    old := reached_reg; \
#            \n    prints "NEW REACHABLE STATE:"; \n    print reached_reg; \
#            \n    reached_reg := post(reached_reg); \
#            \nendwhile; \
#            \nprints "FINAL REACHABLE STATE:"; \nprint reached_reg;')
        
        lines.append("\nif empty(reached_reg & unsafe_reg)")
        lines.append("  then")
        lines.append('    prints "%s, system is safe";' % "BaseC<=1")
        lines.append("  else")     
        lines.append('    prints "%s, safety is violated";' % "BaseC>1")
        lines.append('    print trace to unsafe_reg using reached_reg;')
        lines.append('endif;')
        
        file.write("\n".join(lines))

def hyirMdl(sftree, file_name):
    '''From a given parse tree, returns the hybrid intermediate 
    representation as an hyir object, consisting of variables and 
    automata, which are comprised of modes and transitions.
    
    Maps id's used in the parse tree so that for a given
    automaton a mode's id corresponds to its location in the mode list
    
    This function also identifies states of the type AND_STATE as 
    separate automata and creates a corresponding automaton in hyir.automata
    by calling the function separateAutomata(hyir)
    '''
    hybrid = HyIR()
    hybrid.file_name = file_name
    automaton = Automaton()
    hybrid.automata = [automaton]
    #For initial parsing assumes that everything is part of a single automaton
    #hybrid.add_automaton(automaton)
    
    mapIDs = {} #maps ID's so that modes can have incremental id's
    f_tran_list = []
    for i in sftree.children[1].children:
        if i.type == "SFState":
            m = Mode()
            acceptableMode = True
            f_tran_id = 0
            for j in i.children:
                if j.type == "type" and j.value == "AND_STATE":
                    acceptableMode = False
                if j.type == "id":
                    m.id = automaton.new_mode_id()
                    mapIDs[j.value] = m.id
                if j.type == "labelString":
                    m.name = j.children[0].value
                    for k in j.children[2].children:
                        #print " While parsing the models "
                        #print "_" + k.value + "_"
                        rate = (str)(k.children[1].value)
                        if not re.search(r'^begin', rate) is None and \
                        not re.search(r'end$', rate) is None:
                            rate = rate[5:-3]
                            rate = re.sub('_',',',rate)
                            rate = re.sub('neg', lambda x: '-', rate)
                            k.children[1].value = "[%s]" % rate
                            k.value = " in "
                        m.add_dai(DAI(k,collapse(k)))
                        #print "DAI is "
                        #print collapse(k)
                    #print "children loop ended"
                if j.type == "firstTransition":
                    f_tran_id = j.value
                if j.type == "position":
                    tmp3 = j.value
                    xp = int((tmp3[0]+tmp3[2]))
                    yp = int((tmp3[1]+tmp3[3]))
                    m.xpos = xp
                    m.ypos = yp
            if acceptableMode:
                hybrid.automata[0].add_mode(m)
            else:
                hybrid.automata.append(Automaton(m.name))
                automaton.next_mode_id -= 1
                f_tran_list.append(f_tran_id)
                print "\t".join([str(item) for item in f_tran_list])

        if i.type == "SFAnnot":
            for j in i.children:
                if j.type == "labelString":
                    bufferString = j.value.replace("@","")
                    hybrid.annotationsRaw += [bufferString]
                    hybrid.annotations += bufferString + "\n"
#                    print hybrid.annotationsRaw
#                    print "This is the test string -- \n"
#                    print testString + "\n"
#                    print "Over and out \n"
            
        if i.type == "SFData":
            v = Variable()
            for j in i.children:
                if j.type == "name":
                    v.name = j.value
                if j.type == "scope":
                    v.scope = j.value
                if j.type == "props": 
                    if j.children[0].value == "SF_CONTINUOUS_TIME_DATA":
                        v.update_type = "Continuous"
                        v.type = "Real"
                    else:
                        v.update_type = "Discrete"
                        v.type = ""
            if v.scope != "INPUT_DATA":
                hybrid.add_var(v)
        if i.type == "SFTransition":
            actions = []
            initialFlag = False;
            for j in i.children:
                if j.type == "id":
                    tid = automaton.new_transition_id()
                    sl_id = j.value #used for determining initial modes
                elif j.type == "Source Block":
                    if j.children[0].value != 'None' and j.children[0].value !='':
                        srid = mapIDs[j.children[0].value]
                    else:
                        initialFlag = True
                elif j.type == "Dest Block":
                        dsid = mapIDs[j.children[0].value]                           
                elif j.type == "labelString":
                    for k in j.children:
                        if k.type == 'Logical' or k.type == 'Relational':
                            guard = Guard(k,collapse2(k))
                        elif k.type == 'Assignment':
                            actions.append(Action(k,collapseAction(k)))

            if not initialFlag: 
                hybrid.automata[0].modes[srid].add_inv(build_invariant(guard.parsed))
                t = Transition(guard,actions,tid,srid,dsid)
                hybrid.automata[0].add_trans(t)
            else:
                hybrid.automata[0].modes[dsid].initial = True
                for action in actions:
                    hybrid.automata[0].modes[dsid].initialConditions.append(re.sub(r"'", lambda x: '', action.raw)) 
                initialFlag = False
                
                #This tells the translator which modes are initial for each automata
                #Thus giving allowing further modes in the automata to be deduced
                for x in range(0, len(f_tran_list)):
                    if sl_id == f_tran_list[x]:
                        print dsid                   
                        hybrid.automata[x+1].initial_mode_id = dsid
   
    separateAutomata(hybrid)
    return hybrid

def hyirXML(fileName):
  exprParser=ExprParser()
  parser=etree.XMLParser(remove_blank_text=True)
  tree=etree.parse(fileName,parser)
  root=tree.getroot()

  hybrid=HyIR()
  hybrid.file_name=fileName
  automaton=Automaton()
  hybrid.automata=[automaton]
  varList=[]

  guardResets = defaultdict(list)
  invariants = defaultdict(list)

  # guardresets = defaultdict(list)
  # invariants = defaultdict(list)

  for var in root.iterfind("variable"):
    v=Variable()
    v.name=var.get("name")
    v.scope=var.get("scope")
    v.type=var.get("type")
    hybrid.add_var(v)
    varList.append(v.name)

  auto=root.find("automaton")
  hybrid.automata[0].name=auto.get("name")

  modeIndex=0
  for mode in auto.iterfind("mode"):
    modeIndex+=1  
    m=Mode()
    m.name=mode.get("name")
    m.id=int(mode.get("id"))
    m.initial=mode.get("initial")
    for dai in mode.iterfind("dai"):
      v1 = dai.get("equation")
#      print "DAI while parseing is " + v1
      m.add_dai(DAI(parse_action(v1)[0],v1))
      N1 = parse_action(v1)
#      print " Printing List"
#      for elems in N1:
#          print " element "
#          print collapse(elems)
#          #elems.prints()
#      print " ended list"    
    for inv in mode.iterfind("invariant"):
      i=inv.get("equation").replace("&lt;","<").replace("&gt;",">").replace("&amp;","&").replace("and","&&").replace("or","||")
      #.replace("and", "&").replace("or", "\|\|")
      # print " This is the invariant "
      # print i
      #m.add_inv(Invariant(lstring3(i),i))
      m.add_inv(Invariant(parse_guardLogicalExp(i)[0],i))
      # invariants[m.id].append(i);
      print 'invariant equation: ' + i
      aMatrix, bMatrix, eqMatrix = exprParser.parseSet(varList,i,2)
      # print 'successfully parsed\n'
      # convertMatrixToStrEqn(varList, aMatrix, bMatrix, eqMatrix)
      # print exprParser.parseSet(varList,i,2)
      invariants[m.id].append(convertMatrixToStrEqn(varList, aMatrix, bMatrix, eqMatrix))
      # print "added"
      # invariants[m.id].append(exprParser.parseSet(varList,i,1))
    annot=mode.find("annotation")
    K=annot.find("K")
    Gamma=annot.find("gamma")
    Type=annot.find("type")
    #Lin = annot.find("linear")
    annotTypeString = Type.get("string")
    annotTypeValue = Type.get("value")
    annotElem = []
    annotElem += [modeIndex]
    annotElem += [float(K.get("value"))]
    annotElem += [float(Gamma.get("value"))]
    annotElem += [int(annotTypeValue)]
    #annotElem += [int(Lin.get("value"))]
    
    
    hybrid.annotationsParsed+= [annotElem]
      
    hybrid.automata[0].add_mode(m)

  for tran in auto.iterfind("transition"):
    guard=tran.find("guard")
    g=guard.get("equation").replace("&lt","<").replace("&gt",">").replace("&amp;","&").replace("and","&&").replace("or","||")
    # print g
    print 'guard equation: ' + g
    aMatrix, bMatrix, eqMatrix = exprParser.parseSet(varList,g,1)
    # convertMatrixToStrEqn(varList, aMatrix, bMatrix, eqMatrix)
    # print exprParser.parseSet(varList,g,1)
    #.replace("and", "&").replace("or", "\|\|")
    #N = lstring3(g)
    #N.prints()
    actions=[]
    action_eqs = []
    for act in tran.iterfind("action"):
      a=act.get("equation").replace("&lt","<").replace("&gt",">").replace("&amp;","&").replace("and","&&").replace("or","||")
      # print a
      actions.append(Action(parse_action(a)[0],a))
      a = a.replace("=","==")
      print 'action equation: ' + a
      aMatrix, bMatrix, eqMatrix = exprParser.parseSet(varList,a,1)
      convertMatrixToStrEqn(varList, aMatrix, bMatrix, eqMatrix)
      # print exprParser.parseSet(varList,a,1)
      # action_eqs.append(exprParser.parseSet(varList,a,1))
      # action_eqs.append(a)
      action_eqs.extend(convertMatrixToStrEqn(varList, aMatrix, bMatrix, eqMatrix))
    t=Transition(Guard(parse_guardLogicalExp(g)[0],g),actions,int(tran.get("id")),int(tran.get("source")),int(tran.get("destination")))
    hybrid.automata[0].add_trans(t)
    guardResets[(int(tran.get("source")),int(tran.get("destination")))].append((convertMatrixToStrEqn(varList, aMatrix, bMatrix, eqMatrix), action_eqs))
      
  propertyList=[]
  for prop in root.iterfind("property"):
    p=PropertyDatum(len(propertyList))
    p.name=prop.get("name")
    p.type=int(prop.get("type"))
    p.initialSetStr=str(prop.get("initialSet").replace("&lt","<").replace("&gt",">").replace("&amp;","&"))
    p.unsafeSetStr=str(prop.get("unsafeSet")).replace("&lt","<").replace("&gt",">").replace("&amp;","&")
    p.initialSetParsed=exprParser.parseSet(varList,p.initialSetStr,0)
    p.unsafeSetParsed=exprParser.parseSet(varList,p.unsafeSetStr,1)
    for paramSubTree in prop.iterfind("parameters"):
        p.paramData[0] = float(paramSubTree.get("delta"))
        p.paramData[1] = float(paramSubTree.get("timestep"))
        p.paramData[2] = float(paramSubTree.get("timehorizon"))
        p.paramData[3] = float(paramSubTree.get("taylororder"))
    propertyList.append(p)

  return hybrid,propertyList,varList,dict(invariants),dict(guardResets)

def convertMatrixToStrEqn(varList, aMatrix, bMatrix, eqMatrix):
    eqs = []
    for a,b,eq in zip(aMatrix, bMatrix, eqMatrix):
        lhs = []
        for i,coeff in enumerate(a):
            if coeff==1:
                lhs.append(varList[i])
            elif coeff!=0:
                lhs.append(str(coeff)+'*'+varList[i])
        eqs.append('+'.join(lhs)+str(eq[0])+str(b[0]))
    return eqs

def reconstructGuardsInvForHytech(automaton):
    '''For each transition in a given automaton ensures that each 
    guard has equalities to help ensure that the guard condition of
    each transition and its corresponding source mode share a common point'''
    for tran in automaton.trans:
        add_equalities(tran.guard.parsed)
        tran.guard.raw = collapse2(tran.guard.parsed)

    for mode in automaton.modes:
        for inv in mode.invs:
            add_equalities(inv.parsed)
            inv.raw = collapse(inv.parsed)
  
def makeInitialized(automaton):
    '''This function makes/ensures that a given automaton is initialized,
    i.e. each time the rate of change of a variable changes from mode to 
    mode there exists a corresponding reset action as part of the 
    transition between those modes
    
    If a designer explicitly creates a system that does not obey this 
    property then the function raises a UninitializedError.  If the 
    designer unknowingly creates an uninitialized system by not specifying 
    a variable's rate of change but not explicitly declaring that it
    is different, then the function assumes that the rate of change
    is suppose to remain the same, and the new rate of change is
    added to the mode'''
    varList = []
    completeParse = False
    while not completeParse:
        completeParse = True
        for tran in automaton.trans:
            srcM = automaton.modes[tran.src]
            destM = automaton.modes[tran.dest]
            for src_dai in srcM.dais:
                count = 0
                if src_dai.parsed.children[0].value not in varList:
                    varList.append(src_dai.parsed.children[0].value)
                    completeParse = False
                exists = False
                for dest_dai in destM.dais:
                    exists = exists or checkInitialized(src_dai, dest_dai, tran, destM)
                    if dest_dai.parsed.children[0].value in varList:
                        count += 1
                completeParse = completeParse and count == len(varList)
                #NOTE-->ASSUMPTION: if the dai is not present in the next mode
                #it is presumed to be the same as before
                if not exists:
                    new_dai = copy.deepcopy(src_dai)
                    destM.dais.append(new_dai)
                
            for dest_dai in destM.dais:
                count = 0
                if dest_dai.parsed.children[0].value not in varList:
                    varList.append(dest_dai.parsed.children[0].value)
                    completeParse = False
                exists = False
                for src_dai in srcM.dais:
                    exists = exists or checkInitialized(dest_dai, src_dai, tran, srcM) 
                    if src_dai.parsed.children[0].value in varList:
                        count += 1
                completeParse = completeParse and count == len(varList)
                if not exists:
                    new_dai = copy.deepcopy(dest_dai)   
                    srcM.dais.append(new_dai)
        
                          
def checkInitialized(src_dai, dest_dai, tran, destM): 
    '''returns true if there exists the same variable in both
    the dest mode and the source mode, false otherwise'''                       
    if src_dai.parsed.children[0].value == dest_dai.parsed.children[0].value:
        if src_dai.parsed.children[1].value != dest_dai.parsed.children[1].value:
            initialized = False
            for action in tran.actions:
                if action.parsed.children[0].value == src_dai.parsed.children[0].value:
                    initialized = True
            if not initialized:
                pass
#                raise UninitializedError("System is not initialized")
        return True
    return False                        
        
def separateAutomata(hyir):
    '''For a given hybrid intermediate representation (hyir), this function
    separates all modes into distinct automata whose composition represents
    the entire hybrid system
    
    This function assumes that this process has already been started during 
    the parsing state.  That is, the first automaton in the list 
    hyir.automata is meant to represent the entire system.  The rest of this
    list is supposed to be comprised of all potential automata in that
    system and already have the id of the initial mode saved in the 
    instance variable initial_mode_id
    
    This function ensures that mode id's in each automaton still 
    correspond to their location in automaton.modes'''
    BASE_AUTOMATON = hyir.automata[0]
    BASE_MODES_COPY = copy.deepcopy(BASE_AUTOMATON.modes)
    if len(hyir.automata) == 2:
        BASE_AUTOMATON.name = hyir.automata[1].name
        hyir.automata.pop()
    if len(hyir.automata) >= 3:
        mapID_List = [] #List of mapping dictionaries
        hyir.automata.remove(BASE_AUTOMATON)
        
        print "NOW " + str(len(BASE_MODES_COPY))
        for mode in BASE_MODES_COPY:
            print str(mode.id) + " " + mode.name
   
        #adds initial mode to each automaton and 
        #initializes the dictionary that maps ID for each automaton
        for count in range(0, len(hyir.automata)):
            startMode = copy.deepcopy(BASE_AUTOMATON.modes[hyir.automata[count].initial_mode_id])
            print startMode.id
            for mode in BASE_MODES_COPY[:]:
                if mode.id == startMode.id:
                    BASE_MODES_COPY.remove(mode)
                    break
            mapID_List.append({startMode.id:hyir.automata[count].new_mode_id()})
            startMode.id = mapID_List[count][startMode.id]
            hyir.automata[count].add_mode(startMode)
        print "NOW " + str(len(BASE_MODES_COPY))
        for mode in BASE_MODES_COPY:
            print str(mode.id) + " " + mode.name      
        needsDestChange = {}
        
        while BASE_AUTOMATON.trans:
            for tran in BASE_AUTOMATON.trans[:]:
                breakAgain = False
                for autoNum in range(0, len(mapID_List)):
                    if breakAgain:
                        break
                    for key in mapID_List[autoNum].keys():
                        if tran.src == key:
                            change = True
                            for x in mapID_List[autoNum].keys():
                                if tran.dest == x:
                                    tran.dest = mapID_List[autoNum][x]
                                    change = False
                            if change:
                                needsDestChange[tran] = autoNum
                            tran.src = mapID_List[autoNum][tran.src]
                            hyir.automata[autoNum].add_trans(tran)
                            BASE_AUTOMATON.remove_tran(tran)
                            breakAgain = True
                            break
            for mode in BASE_MODES_COPY[:]:
                os.system('cd /Users/danielgrier/Desktop/; echo "%s" > HyLink_error_log' % "blah")
                for tran in needsDestChange.keys():
                    if mode.id == tran.dest:
                        auto_idx = needsDestChange[tran]
                        mode.id = hyir.automata[auto_idx].new_mode_id()
                        
                        mapID_List[auto_idx][tran.dest] = mode.id
                        tran.dest = mode.id
    
                        hyir.automata[auto_idx].add_mode(mode)
                        
                        BASE_MODES_COPY.remove(mode)
                        del needsDestChange[tran]
                        break
        #Some transitions still might need converting
        for tran in needsDestChange.keys():
            tran.dest = mapID_List[needsDestChange[tran]][tran.dest]

def add_equalities(node):
    '''for a given node, which is presumably a parsed guard, 
    adds the equality for any inequality. (e.g. < becomes <=)
    In anticipation for use with HyTech, this function also converts
    == to =, which is the standard notation in HyTech for guard conditions'''
    if node.value == ">":
        node.value = ">="
    elif node.value == "<":
        node.value = "<="
    elif node.value == "==":
        node.value = "="
    elif node.type == "Logical":
        add_equalities(node.children[0])
        add_equalities(node.children[1])

def build_invariant(guard):
    '''creates a deep copy of a parsed guard (ie Node tree) and calls the 
    negate method on that copy and returns the result as an Invariant object'''
    x = copy.deepcopy(guard)
    negate(x)
    return Invariant(x, collapse2(x))

def gen_invs(automaton):
    '''First clears all invariances of all modes.  Then, for each
    deterministic transition, an invariant is built for the corresponding
    mode affected'''
    for m in automaton.modes:
        m.clear_inv()
        ##COMMENTED OUT FOR NEW TRANSFORMATION ALGORITHM
#    for i in hy.trans:
#        if i.nd == 0:
#            hy.modes[i.src].add_inv(build_invariant(i.guard.parsed))
#    return automaton  

def negate(node):
    '''assumes this method is being used to create deterministic transitions
    by altering mode invariances'''
    if node.value == 'or':
        node.value = 'and'
    elif node.value == 'and':
        node.value = 'or'
    elif node.value == '>':
        node.value = '<='
    elif node.value == '<':
        node.value = '>='
    elif node.value == '>=' or node.value == '=>':
        node.value = '<'
    elif node.value == '<=' or node.value == '=<':
        node.value = '>'
    elif node.value == '==':
        node.value = '!='
    if len(node.children) > 0:
        for i in node.children:
            negate(i)
    return node

def collapseAction(node):
    '''There exists an implicit assumption due to the parsing of the tree 
    and the necessity to differentiate between variable values before 
    and after a transition has occurred that the resultant value on the right 
    side of the equation is assigned to the left side'''
    
    if node.children == []:
        return str(node.value)
    elif len(node.children) == 2:
        if node.value == '=':
            #This is done for translation into hytech
            #hytech requires that variables that are reassigned a value must
            #be followed by a ' (e.g. x') to denote such
            leftNode = node.children[0]
            if leftNode.type == "Identifier" and not leftNode.value.isdigit():
                leftNode.value += ""
                #leftNode.value += "'"
            return collapseAction(leftNode) + ' ' + node.value + ' ' + collapseAction(node.children[1])
        else:
            return collapseAction(node.children[0]) + node.value + collapseAction(node.children[1])
    elif len(node.children) == 1:
        return node.value + collapseAction(node.children[0])

def collapse(node):
    if node.children == []:
        return str(node.value)
    elif len(node.children) == 2:
        if node.value == '=':
            return collapse(node.children[0]) + ' ' + node.value + ' ' + collapse(node.children[1])
        else:
            return collapse(node.children[0]) + node.value + collapse(node.children[1])
    elif len(node.children) == 1:
        if node.value == '()':
            return '(' + collapse(node.children[0]) + ')'
        elif node.value == 'COS':
            return 'cos(' + collapse(node.children[0]) + ')'
        elif node.value == 'SIN':
            return 'sin(' + collapse(node.children[0]) + ')'
        else:
            return node.value + collapse(node.children[0])

def collapse2(node):
    if node == None:
        return ''
    
    if node.children == []:
        return str(node.value)
    elif len(node.children) == 2:
        if node.type == 'Logical':
            return collapse2(node.children[0]) + ' ' + node.value + ' ' + collapse2(node.children[1])
        else:
            return collapse2(node.children[0]) + node.value + collapse2(node.children[1])
    elif len(node.children) == 1:
        return node.value + collapse2(node.children[0])

def gcd(a, b):
    while b:    
        a, b = b, a % b
    return a

def lcm(a, b):
    return a * b // gcd(a, b)

def lcmm(*args):
    """Return lcm of args."""   
    return reduce(lcm, args)

def removeDisjGuards(automaton, trans):
    '''recursively removes disjunctions from transition guard.  Simply 
    does this by adding an extra transition for each 'or' disjunction'''
    if trans.guard.parsed.type == "Logical" and trans.guard.parsed.value == "or":
        removeDisjGuards(automaton,Transition(Guard(trans.guard.parsed.children[1], 
        collapse2(trans.guard.parsed.children[1])),
        [copy.deepcopy(action) for action in trans.actions],
        automaton.new_transition_id(),trans.src,trans.dest))
        
        trans.guard = Guard(trans.guard.parsed.children[0], 
                            collapse2(trans.guard.parsed.children[0]))
        removeDisjGuards(automaton, trans)
def returnLinearConjuctions(parsedGuard, linearList):
    if parsedGuard.type == "Logical" and parsedGuard.value == "and":
        returnLinearConjuctions(parsedGuard.children[0], linearList)
        returnLinearConjuctions(parsedGuard.children[1], linearList)
    else:
        linearList.append(parsedGuard)
    return linearList
        
def remove_disjunc(automaton):
    '''removes disjunctions from the hybrid intermediate representation.  It
    first removes disjunctions in the guards, thus preserving the structure of 
    the automaton.  Then a second pass removes conjunctions over
    deterministic jumps (which would indicate an invariance disjunction).
    This function splits these guards into multiple transitions over newly 
    created modes to compensate for the inability of HyTech to deal with this
    class of hybrid automata
    
    CAUTION: THIS FUNCTION MUST BE CALLED AFTER GEN_INV() IS CALLED'''
    
    #REMOVES ALL DISJUNCTIONS
    for i in automaton.trans:
        removeDisjGuards(automaton, i)
    
    #REMOVES CONJUNCTIONS IF TRANSITION IS DETERMINISTIC
    needToParse = [dtran for dtran in automaton.trans if dtran.nd == 0]
    needDestChange = []

    while needToParse:
        i = needToParse[0]
        
        #Used in automatically creating transitions after translating deterministic
        #transitions of conjunctions
        preID = automaton.next_mode_id - 1
        
        #Holds guards in linear list without conjunctions
        guardList = returnLinearConjuctions(i.guard.parsed, [])

        outgoingTrans = []
        incomingTrans = []
        #adds all other outgoing transitions to outgoingTrans
        #adds all other incoming transitions to incomingTrans
        for j in automaton.trans:
            if j.src == i.src and j.id != i.id:
                outgoingTrans.append(j)
            elif j.dest == i.src and j.id != i.id:
                incomingTrans.append(j)

        if len(guardList) >= 2:
            for inTran in incomingTrans:
                for k in range(1, 2**len(guardList)-1):
                    startCond = genGuardSet(k, guardList)
                    newT = Transition(copy.deepcopy(inTran.guard),
                            copy.deepcopy(inTran.actions), automaton.new_transition_id(), inTran.src, 
                            preID+k, ap=True)
                    automaton.add_trans(newT)
        #When len(guardList)>=2 must create a new modes to represent invariants as conjunctions
        for j in range(1, 2**len(guardList) -1):
            newModeID = automaton.new_mode_id()
                
            #NOTE: generates the invariance for the mode
            parsedInv = genGuardSet(j, guardList)

            #preserves previous invariants
            inv_list = automaton.modes[i.src].invs[:] + [Invariant(inv, collapse2(inv)) for inv in returnLinearConjuctions(parsedInv, [])]
            
            newMode = Mode('%s_%d' % (automaton.modes[i.src].name, j), newModeID,False,
                           inv_list, automaton.modes[i.src].dais)
            automaton.add_mode(newMode)
            
            #adds a new transition for every possible change of variables that would affect guard conditions
            for k in range(0, len(guardList)):
                if isActiveBit(k, j):
                    guardPos = j-2**k
                    newGuard = genGuardSet(guardPos, guardList)
                    if guardPos == 0:
                        newT = Transition(Guard(newGuard, collapse2(newGuard)),[], automaton.new_transition_id(),
                                          newModeID, i.src, ap=True)
                    else:
                        newT = Transition(Guard(newGuard, collapse2(newGuard)),[], automaton.new_transition_id(),
                                          newModeID, newModeID-2**k, ap=True)
                else:
                    guardPos = j+2**k
                    newGuard = genGuardSet(guardPos, guardList)
                    #that is, if all guard conditions are satisfied
                    if guardPos == 2**len(guardList) - 1:
                        newT = Transition(Guard(newGuard, collapse2(newGuard)),i.actions, automaton.new_transition_id(),
                                      newModeID, i.dest, ap=True)
                    else:
                        newT = Transition(Guard(newGuard, collapse2(newGuard)),[], automaton.new_transition_id(),
                                      newModeID, newModeID+2**k, ap=True)
                automaton.add_trans(newT)

            #adds all other outgoing transitions from the original state to the new state
            #if the alreadyParsed flag is set then the transition is not added
            for k in outgoingTrans:
                newT = Transition(copy.deepcopy(k.guard),copy.deepcopy(k.actions), 
                                  automaton.new_transition_id(), newModeID,k.dest, ap=k.ap)
                if not k.ap:
                    needToParse.append(newT)
                automaton.add_trans(newT)

#        print str(i.id)+": "+str(i.src)+" "+str(i.dest)+" "+i.guard.raw \
#        +" "+ "Actions: " +", ".join(action.raw for action in i.actions) + " len: " +str(len(guardList))
        
        if len(guardList) >= 2:
            #That is, if the source mode does not have a unique identifier 
            #that will differentiate modes that are generated from this mode
            automaton.modes[i.src].name = "%s_0" % automaton.modes[i.src].name
                
            #Generate transitions from i.src
            for k in range(0, len(guardList)):
                newGuard = genGuardSet(2**k, guardList)
                newT = Transition(Guard(newGuard, collapse2(newGuard)),[], 
                     automaton.new_transition_id(), i.src, 
                     preID +2**k, ap=True)
                automaton.add_trans(newT)
            automaton.remove_tran(i)
                            
        #Generate invariance for i.src
        parsedInv = genGuardSet(0, guardList)
        inv_list = automaton.modes[i.src].invs[:] + [Invariant(inv, collapse2(inv)) for inv in returnLinearConjuctions(parsedInv, [])]
        automaton.modes[i.src].invs = inv_list  
        
        needToParse.remove(i)
def changeDestsToDummy(hyir, needDestChange):
    for tran in needDestChange:
        tran.dest = hyir.modes[tran.dest].dummyIdx

def changeInitialModeToDummy(hyir):
    for mode in hyir.modes:
        if mode.initial:
            hyir.modes[mode.dummyIdx].initial = True
            mode.initial = False
            break             

#def generateDummyMode(hyir, tran, location):
#    '''Generates a dummy mode based on a deterministic transition
#    location is either "src" or "dest" '''
#    dummyLoc = getattr(tran, location)
#    hyir.modes[dummyLoc].dummyIdx = new_mode_id(hyir)                        
#    daisList = genStoppedDAIs(hyir)
#    newMode = Mode('%s_Dummy' % hyir.modes[dummyLoc].name, 
#                   hyir.modes[dummyLoc].dummyIdx, dais=daisList)
#    hyir.add_mode(newMode)

#def makeTransitionsFromDummy(hyir, sourceTran):   
#    #This list is needed because a dummy mode might not have been created 
#    #for these destinations yet
#    needDestChange = []
#    transFromSource = []
#    willBeParsedLater = False
#    for tran in hyir.trans:
#        if tran.src == sourceTran.dest:
#            if tran.guard.parsed.value != "and":
#                transFromSource.append(tran)
#            else:
#                willBeParsedLater = True
#    if not willBeParsedLater:
#        for tran in transFromSource:
#            newT = copy.deepcopy(tran)
#            newT.src = hyir.modes[sourceTran.dest].dummyIdx
#            newT.ap = True
#            hyir.add_trans(newT)
#            needDestChange.append(newT)
#        
#        newT = Transition(Guard(Node("Identifier", [], "True"), "True"),[], 
#                          new_transition_id(hyir), hyir.modes[sourceTran.dest].dummyIdx, 
#                          sourceTran.dest, ap=True)    
#        hyir.add_trans(newT)    
#    return needDestChange

def genGuardSet(guardPos, guardList):
    '''Given a linear list of guards, guardList, and number that indicates which
    of these guards should be negated, guardPos, this function returns a node tree
    of the resultant guard connected with conjunctions'''
    if guardPos % 2 == 1:
        oldGuard = copy.deepcopy(guardList[0])
        guardPos -= 1
    else:
        oldGuard = negate(copy.deepcopy(guardList[0]))

    for idx in range(1,len(guardList)):
        if guardPos % 2**(idx+1) == 2**(idx):
            newGuard = copy.deepcopy(guardList[idx])
            guardPos -= 2**(idx)
        else: 
            newGuard = negate(copy.deepcopy(guardList[idx]))

        oldGuard = concatGuards(oldGuard, newGuard)

    return oldGuard

def isActiveBit(bitPos, num):
    '''if bitPos is 1 when num is binary then return True'''
    num %= 2**(bitPos+1)
    if num % 2**bitPos == num:
        return False
    return True

def retBaseNum(number):
    if(number==0):
        return "Zero"
    if(number==1):
        return "One"
    if(number==2):
        return "Two"
    if(number==3):
        return "Three"
    if(number==4):
        return "Four"
    if(number==5):
        return "Five"
    if(number==6):
        return "Six"
    if(number==7):
        return "Seven"
    if(number==8):
        return "Eight"
    if(number==9):
        return "Nine"

def genStringNumber(number):
    retString = ""
    temp = 0
    base = 10
    while number > 0:
        temp = number%base
        retString += retBaseNum(temp)
        number= (number-temp)/base
    return retString

def genStoppedDAIs(hyir):
    daiList = []
    for var in hyir.vars:
        node = Node("Assignment", [Node('Identifier',[],"%s_dot" % var.name), Node('Identifier',[],0)], "=")
        daiList.append(DAI(node, collapse(node)))
    return daiList

def concatGuards(guard1, guard2):
    return Node("Logical", [guard1,guard2], "and")

def remove_fractions(hyir):
    pass

#def identify_choice_nd(hyir):
#    for i in hyir.trans:

def identify_choice_nd(automaton):
    for i in automaton.trans:
        if identify_nd_src(i.guard.parsed, "choice"):
            i.nd = 1
            remove_nd_relation(i.guard.parsed, i)
            i.guard.raw = collapse2(i.guard.parsed)

def check_and_remove_nd(node, kind):
    '''Removes nodes of choice type from guards'''
    if node == None:
        return None
    elif node.children == []:
        return node
    elif node.children[0].children == [] or node.children[1].children==[]:
        if (node.children[0].type == 'Identifier' and node.children[0].value == kind) or (node.children[1].type == 'Identifier' and node.children[1].value == kind):
            return None
        else:
            return node
    
    #Check first child
    if node.children[0].children[0].children == []:
        if (node.children[0].children[0].type == 'Identifier' and node.children[0].children[0].value == kind) or (node.children[0].children[1].type == 'Identifier' and node.children[0].children[1].value == kind):
            firstChild = None
        else:
            firstChild = check_and_remove_nd(node.children[0],kind)
    else:
        firstChild = check_and_remove_nd(node.children[0],kind)
    
    #Check second child
    if node.children[1].children[0].children == []:
        if (node.children[1].children[0].type == 'Identifier' and node.children[1].children[0].value == kind) or (node.children[1].children[1].type == 'Identifier' and node.children[1].children[1].value == kind):
            secondChild = None
        else:
            secondChild = check_and_remove_nd(node.children[1],kind)
    else:
        secondChild = check_and_remove_nd(node.children[1],kind)
    
    if firstChild == None and secondChild == None:
        return None
    elif firstChild == None and not secondChild == None:
        return secondChild
    elif not firstChild == None and secondChild == None:
        return firstChild
    else:
        return Node(node.type,[firstChild, secondChild],node.value)
        

def identify_nd_src(node, nd_kind):
    '''Helper function for identify_tran_nd()'''
    isChoice = False
    if node.children:
        if re.search('_%s[0-9]*$' % nd_kind, str(node.children[0].value)) is not None:
            node.value = "_remove_"
            return True
        isChoice = identify_nd_src(node.children[0], nd_kind) or \
                   identify_nd_src(node.children[1], nd_kind)
    return isChoice

def remove_nd_relation(node, tran):
    '''Helper function for identify_tran_nd()'''
    if node.value == "_remove_":
        tran.guard.parsed = Node("Identifier", [], "True")
        return None
    
    if node.children:
        if node.children[0].type == "Logical":
            if node.children[0].children[0].value == "_remove_":
                node.children[0] = node.children[0].children[1]
                return None
            elif node.children[0].children[1].value == "_remove_":
                node.children[0] = node.children[0].children[0]
                return None
                
        if node.children[1].type == "Logical":
            if node.children[1].children[0].value == "_remove_":
                node.children[1] = node.children[1].children[1]
                return None
            elif node.children[1].children[1].value == "_remove_":
                node.children[1] = node.children[1].children[0]
                return None

        if node.children[0].value == "_remove_":
            tran.guard.parsed = node.children[1]
            return None
        if node.children[1].value == "_remove_":
            tran.guard.parsed = node.children[0]
            return None

        remove_nd_relation(node.children[0], tran)
        remove_nd_relation(node.children[1], tran)

def identify_timed_nd(automaton, hyir):    
    for i in automaton.trans[:]:
        #Removes transitions only used in stateflow to simulate timed non-determinism
        if len(i.actions) == 1 and \
        re.search("_t[0-9]*'$", str(i.actions[0].parsed.children[0].value)) is not None:
            automaton.remove_tran(i)
        #most likely transitions into this node will have a timed reset condition
        #this is no longer needed so it is removed
        if i.actions:
            for action in i.actions[:]:
                if re.search("_t[0-9]*'$", str(action.parsed.children[0].value)) is not None:
                    i.actions.remove(action)
        if identify_nd_src(i.guard.parsed, "t"):
            i.nd = 1        
            remove_nd_relation(i.guard.parsed, i)
            i.guard.raw = collapse2(i.guard.parsed)

    for i in hyir.vars[:]:
        if re.search("_t[0-9]*$", i.name) is not None:
            hyir.vars.remove(i)
        elif re.search("_c[0-9]*$", i.name) is not None:
            hyir.vars.remove(i)
    for mode in automaton.modes:
        for i in mode.dais[:]:
            if re.search("_t[0-9]*_dot$", str(i.parsed.children[0].value)) is not None:
                mode.dais.remove(i)

def numberOfLogicalOper(guardNode):
    
    if guardNode.type == 'Logical':
        if len(guardNode.children) == 2:
            child1 = guardNode.children[0]
            child2 = guardNode.children[1]
            if (child1.type != 'Logical') & (child2.type == 'Logical'):
                return 1 + numberOfLogicalOper(child2)
            if (child1.type != 'Logical') & (child2.type != 'Logical'):
                return 2
            
    if guardNode.type != 'Logical':
        return 1;

class UninitializedError(Exception):
    def __init__(self, value):
        self.value = value
    def __str__(self):
        return self.value





