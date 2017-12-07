# FIXME try to remove all I/O files required for the execution and use boost::python instead

import math, itertools, copy, re, os
#from lxml import etree as etre
from frontend.mod.automaton import *

# FIXME these don't have to be written to a file anymore b/c of transition to boost::python
from frontend.mod.jacobiancalc import *

from collections import defaultdict

class HyIR:
    def __init__(self, name="hybrid_system", file_name=""):
        self.name = name
        self.file_name = file_name
        
        self.vars = []
        self.variables = Variables()
        self.varList = []

        self.thinvars = []
        self.thinvarList = []
        self.thinvariables = ThinVariables()

        self.automata = Automaton()
        self.annotations = ""
        self.annotationsRaw = []
        ###self.treestore = gtk.TreeStore(str)
    
    @staticmethod
    def compose(hyir1, hyir2):
        composed = HyIR()
        m1_len = len(hyir1.automata.modes)
        m2_len = len(hyir2.automata.modes)
        #Construct Cartesian product of modes
        for m1 in hyir1.automata.modes:
            for m2 in hyir2.automata.modes:
                m_name = m1.name + '_' + m2.name
                m_id = m1.id*m2_len + m2.id
                m_initial = m1.initial and m2.initial
                cross_mode = Mode(name=m_name,id=m_id,initial=m_initial)

                # Replace input variables with corresponding output variables
                dai_dict = {}
                HyIR.construct_output_dict(cross_mode, m1.dais, dai_dict)
                HyIR.construct_output_dict(cross_mode, m2.dais, dai_dict)
                HyIR.replace_dais(cross_mode, dai_dict)

                # Resulting invariant is a conjunction
                for inv1 in m1.invs:
                    cross_mode.add_inv(inv1)
                for inv2 in m2.invs:
                    cross_mode.add_inv(inv2)
                composed.automata.add_mode(cross_mode)

        # Construct guards of composed automata. (a,b)->(a',b) and (a,b)->(a,b')
        # for all transitions a->a' and b->b'. Note there is no (a,b)->(a',b')
        trans_id = 0
        for t1 in hyir1.automata.trans:
            t_guard = t1.guard
            t_actions = t1.actions
          
            for i in range(m2_len):    
                t_src = t1.src*m2_len+i
                t_dest = t1.dest*m2_len+i
                cross_trans = Transition(guard=t_guard,actions=t_actions,src=t_src,dest=t_dest,id=trans_id)
                composed.automata.add_trans(cross_trans)
                trans_id+=1

        for t2 in hyir2.automata.trans:
            t_guard = t2.guard
            t_actions = t2.actions
         
            for i in range(m1_len):    
                t_src = i*m2_len+t2.src
                t_dest = i*m2_len+t2.dest
                cross_trans = Transition(guard=t_guard,actions=t_actions,src=t_src,dest=t_dest,id=trans_id)
                composed.automata.add_trans(cross_trans)
                trans_id+=1

        # Construct composed variables
        composed.variables.local = hyir1.variables.local+hyir2.variables.local
        composed.variables.output = hyir1.variables.output+hyir2.variables.output
        composed.variables.input = hyir1.variables.input+hyir2.variables.input
        composed.variables.input = [var for var in composed.variables.input if var not in composed.variables.output]
        composed.vars = composed.variables.local+composed.variables.output+composed.variables.input
        composed.varList = [v.name for v in composed.variables.local]

        return composed
        
    def print_vars(self):
        print("--- Variables ---")
        for i in self.vars:
            print(i.name+" "+i.type+" "+i.scope)
    
    # Construct dictionary mapping output variables to their RHS expressions
    @staticmethod
    def construct_output_dict(mode, dais, dai_dict):
        for dai in dais:
            lhs = str(dai.expr.lhs)
            rhs = str(dai.expr.rhs)
            if not lhs.endswith('_dot'):
                dai_dict[lhs] = rhs
            mode.add_dai(dai)

    # Replace input variables with corresponding output variables
    @staticmethod
    def replace_dais(mode, dai_dict):
        for dai in mode.dais:
            lhs = str(dai.expr.lhs)
            if lhs.endswith('_dot'):
                free_syms = dai.expr.rhs.free_symbols
                for var in free_syms:
                    var_name = str(var)
                    if var_name in dai_dict:
                        dai.expr = dai.expr.func(dai.expr.lhs, dai.expr.rhs.subs(var, dai_dict[var_name]))
                dai.raw = str(dai.expr)

    # Populate the guard and invariant dictionaries we use to generate PPL files
    def populateInvGuards(self):
        guardResets = defaultdict(list)
        invariants = defaultdict(list)
        varList = self.varList

        for m in self.automata[0].modes:
          inv_eqs = []
          inv_vars = set()
          for inv in m.invs:
            inv_eqs.append(inv.expr)
            inv_vars = inv_vars.union(SymEq.vars_used(inv.expr))
          invariants[m.id] = (inv_eqs, inv_vars)

        for t in self.automata[0].trans:
          g_eqs = t.guard.expr
          g_vars = SymEq.vars_used(t.guard.expr)
          print (g_vars)
          action_eqs = []
          for act in t.actions:
            action_eqs.extend(act.expr)
          guardResets[(t.src,t.dest)].append((g_eqs, action_eqs, g_vars))

        self.guardResets = dict(guardResets)
        self.invariants = dict(invariants)

    def is_valid(self):
        if not self.variables.input==[]:
            return False
        output = set([v.name for v in self.variables.output])
        local = set([v.name for v in self.variables.local])
        if len(local.intersection(output))!=0:
            return False
        return True

    def add_var(self, v):
        self.vars.append(v)
        self.variables.add_var(v)
        if v.scope=='LOCAL_DATA':
            self.varList.append(v.name)

    def add_thin_var(self, v):
        self.thinvars.append(v)
        self.thinvariables.add_thin_var(v)
        if v.scope=='LOCAL_DATA':
            self.thinvarList.append(v.name)
        

    def print_all(self):
        print("%s:" % self.name)
        self.print_vars()
        for automaton in self.automata:
            automaton.print_all()

    def modesnumber(self):
        return len(self.automata[0].modes)
    
    def printHybridSimGuardsInvariants(self):
        self.printGuardsInvariants("../work-dir/hybridSimGI.cpp", True)

    def printBloatedSimGuardsInvariants(self):
        self.printGuardsInvariants("../work-dir/bloatedSimGI.cpp", False)

    def printGuardsInvariants(self, file_name, is_hybrid):
        checkFile = open(file_name, "w")
        codeString ="#include <ppl.hh>\n"
        codeString+="#include <iostream>\n"
        codeString+="#include <utility>\n"
        codeString+="#include <vector>\n"
        codeString+="#include <fstream>\n"
        codeString+="#include <typeinfo>\n\n"
        codeString+="using namespace std;\n\n"
        codeString+=self.printPoly()
        codeString+=self.getMultFactorPt()
        codeString+=self.getMultFactor()
        # codeString+=self.constructBox()
        checkFile.write(codeString)
        checkFile.close()

        self.printInvariants(file_name, is_hybrid)
        self.printGuardResets(file_name, is_hybrid)

    # Generate PPL code to check invariants. 
    # Only check against those variables in the invariant
    def printInvariants(self, file_name, is_hybrid):
        checkFile = open(file_name, "a")
        codeString="extern \"C\" bool invariantSatisfied(int curMode, double *ptLower, double *ptUpper){\n"
        codeString+="  NNC_Polyhedron box_poly;\n"
        codeString+="  double mult_factor = getMultFactor(" + ("ptUpper" if is_hybrid else "ptLower, ptUpper") + ");\n"
        for mode in self.invariants:
            eqs, varsUsed = self.invariants[mode]
            if not eqs:
                continue
            codeString+="  if(curMode=="+str(mode+1)+"){\n"
            codeString+= self.constructBoxHelper(varsUsed, "", 4, is_hybrid)
            codeString+="    Pointset_Powerset<NNC_Polyhedron> box(box_poly);\n"
            codeString+="    Pointset_Powerset<NNC_Polyhedron> invariant("+str(len(varsUsed))+",UNIVERSE);\n"
            codeString+="    Pointset_Powerset<NNC_Polyhedron> curInv;\n"
            codeString+="    NNC_Polyhedron curPoly;\n"
            codeString+="    Constraint_System cs;\n"
            for eq in eqs:
                codeString+="    curInv = Pointset_Powerset<NNC_Polyhedron>("+str(len(varsUsed))+",EMPTY);\n\n"
                for i,disjunct in enumerate(eq):
                    codeString+="    cs.set_space_dimension("+str(len(varsUsed))+");\n"
                    codeString+="    cs.insert("+str(disjunct)+");\n"
                    codeString+="    curPoly = NNC_Polyhedron(cs);\n"
                    codeString+="    curInv.add_disjunct(curPoly);\n"
                    codeString+="    cs.clear();\n\n"
                codeString+="    invariant.intersection_assign(curInv);\n\n"
            if is_hybrid: codeString+="    return invariant.contains(box);\n"
            else: codeString+="    return !(invariant.is_disjoint_from(box));\n"
            codeString+="  }\n"
        codeString+="  return true;\n"
        codeString+="}\n\n"
        checkFile.write(codeString)
        checkFile.close()

    # Generates PPL code to check guards and create transitions.
    # Only check against those variables in the guard. If reset exists, create 2*n dimensional space.
    def printGuardResets(self, file_name, is_hybrid):
        varList = ["Simu_time"]+self.varList
        resetVarList = [var+"_new" for var in varList]
        tempVarList = [var+"_temp" for var in varList]
        checkFile = open(file_name, "a")
        codeString="extern \"C\" vector<pair<NNC_Polyhedron, int> > hitsGuard(int curMode, double *ptLower, double *ptUpper){\n"
        codeString+="  vector<pair<NNC_Polyhedron, int> > toRet;\n"
        codeString+="  NNC_Polyhedron box_poly;\n"
        codeString+="  double mult_factor = getMultFactor(" + ("ptUpper" if is_hybrid else "ptLower, ptUpper") + ");\n"
        
        for key in self.guardResets:
            init = str(key[0]+1)
            dest = str(key[1]+1)
            for a,b,varsUsed in self.guardResets[key]:
                codeString+="  if(curMode=="+init+"){\n"
                codeString+= self.constructBoxHelper(varsUsed, "", 4, is_hybrid)
                thin_prop = -1
                for var in varsUsed:
                    if var in self.thinvarList:
                        if thin_prop == 0:
                            thin_prop = -1
                            break
                        else:
                            thin_prop = 1
                    else:
                        if thin_prop == 1:
                            thin_prop = -1
                            break
                        else:
                            thin_prop = 0


                codeString+="    Constraint_System cs;\n"
                codeString+="    cs.set_space_dimension("+str(len(varsUsed))+");\n"    
    
                for guard_eq in a:
                    codeString+="    cs.insert("+str(guard_eq)+");\n"
                    
                codeString+="    NNC_Polyhedron guard(cs);\n"
                if is_hybrid: codeString+="    if(guard.contains(box_poly)){\n"
                else: codeString+="    if(!guard.is_disjoint_from(box_poly)){\n"
                codeString+= self.constructBoxHelper(tempVarList, "_temp", 6, is_hybrid)

                if is_hybrid and not b:
                    #codeString+="      toRet.push_back(make_pair(make_pair(box_poly,"+dest+"),"+str(thin_prop)+"));\n"
                    codeString+="      toRet.push_back(make_pair(box_poly,"+dest+"));\n"
                    codeString+="    }\n"
                    codeString+="  }\n"
                    continue

                codeString+="      Constraint_System cs_gd;\n"
                space_dim = 2*len(varList) if b else len(varList)
                codeString+="      cs_gd.set_space_dimension("+str(space_dim)+");\n"
                
                if b:
                    remVars = set(varList)
                    for i,var in enumerate(resetVarList):
                        codeString+="      Variable "+var+"("+str(len(varList)+i)+");\n"
                    codeString+="      box_poly.add_space_dimensions_and_embed("+str(len(varList))+");\n"
                    for reset_eq in b:
                        lhs, rhs = reset_eq.lhs, reset_eq.rhs
                        free_vars = list(lhs.free_symbols)
                        assert len(free_vars)==1
                        var = str(free_vars[0])
                        lhs = lhs.subs(var, var+'_new')
                        for v in self.varList:
                            rhs = rhs.subs(v, v+'_temp')
                        codeString+="      cs_gd.insert("+str(lhs)+"=="+str(rhs)+");\n"
                        remVars.discard(var)
                    for var in remVars:
                        codeString+="      cs_gd.insert("+var+'_new'+'=='+var+"_temp);\n"
                
                if not is_hybrid:
                    for guard_eq in a:
                        for v in self.varList:
                            guard_eq = guard_eq.subs(v, v+'_temp')
                        codeString+="      cs_gd.insert("+str(guard_eq)+");\n"
                codeString+="      NNC_Polyhedron guard_reset(cs_gd);\n"
                codeString+="      guard_reset.intersection_assign(box_poly);\n"

                if b:
                    codeString+="      Variables_Set vars;\n"
                    for var in varList:
                        codeString+="      vars.insert("+var+"_temp);\n"
                    codeString+="      guard_reset.remove_space_dimensions(vars);\n"
                
                #codeString+="      toRet.push_back(make_pair(make_pair(guard_reset,"+dest+"),"+str(thin_prop)+"));\n"
                codeString+="      toRet.push_back(make_pair(guard_reset,"+dest+"));\n"
                codeString+="    }\n"
                codeString+="  }\n"

        codeString+="  return toRet;\n"
        codeString+="}\n\n"
        checkFile.write(codeString)
        checkFile.close()

    def constructBox(self, varList=None):
        if not varList:
            varList=["Simu_time"]+self.varList
        codeString="NNC_Polyhedron constructBox(double *ptLower, double *ptUpper){\n"
        codeString+="  NNC_Polyhedron box_poly;\n"
        codeString+="  double mult_factor = getMultFactor(ptLower, ptUpper);\n"
        codeString+=self.constructBoxHelper(varList, "", 2)
        codeString+="  return box_poly;\n"
        codeString+="}\n\n"
        return codeString

    def constructBoxHelper(self, varList, suffix, indent, is_point):
        indentation = " "*indent
        allVars = ["Simu_time"]+self.varList

        codeString=indentation+"Constraint_System cs_box;\n"        
        for i,var in enumerate(varList):
            codeString+=indentation+"Variable "+var+"("+str(i)+");\n"
        codeString+="\n"

        for i,v in enumerate(allVars):
            if v+suffix in varList:
                var = v+suffix
                if is_point:
                    codeString+=indentation+"cs_box.insert(mult_factor*"+var+"==mult_factor*ptUpper["+str(i)+"]);\n"
                else:
                    codeString+=indentation+"if(ptLower["+str(i)+"]<ptUpper["+str(i)+"]){\n"
                    codeString+=indentation+"  cs_box.insert(mult_factor*"+var+">=mult_factor*ptLower["+str(i)+"]);\n"
                    codeString+=indentation+"  cs_box.insert(mult_factor*"+var+"<=mult_factor*ptUpper["+str(i)+"]);\n"
                    codeString+=indentation+"}\n"
                    codeString+=indentation+"else{\n"
                    codeString+=indentation+"  cs_box.insert(mult_factor*"+var+"<=mult_factor*ptLower["+str(i)+"]);\n"
                    codeString+=indentation+"  cs_box.insert(mult_factor*"+var+">=mult_factor*ptUpper["+str(i)+"]);\n"
                    codeString+=indentation+"}\n\n"
        codeString+=indentation+"box_poly = NNC_Polyhedron(cs_box);\n"
        return codeString

    # Get factor to multiply doubles by because PPL only takes integers.
    def getMultFactorPt(self):
        codeString="double getMultFactor(double *pt){\n"
        codeString+="  int multiplier=0, tmp_mul, str_len;\n"
        codeString+="  char buffer[100];\n"
        codeString+="  char *dot_loc;\n\n"
        mulLoop="  for(int i=0; i<"+str(len(self.varList)+1)+"; i++){\n"
        mulLoop+="    sprintf(buffer, \"%lf\", pt[i]);\n"
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
        codeString+="  return pow(10, multiplier);\n"
        codeString+="}\n\n"
        return codeString

    def getMultFactor(self):
        codeString="double getMultFactor(double *ptLower, double *ptUpper){\n"
        codeString+="  int lowerMult = getMultFactor(ptLower);\n"
        codeString+="  int upperMult = getMultFactor(ptUpper);\n"
        codeString+="  int multiplier = lowerMult > upperMult ? lowerMult : upperMult;\n"
        codeString+="  return multiplier;\n"
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
       
    #FIXME remove these!!!
    #The callback to get the modes to auto expand
    def treeExpandCallback(self,treeview,it,path):
      parIter=self.treestore.iter_parent(it)
      if not parIter==None:
        parValue=self.treestore.get_value(parIter,0)
        if parValue=="Modes" or parValue=="Transitions":
          treeview.expand_row(path,True)

    def editRowCallback(self,cell,path,newText):
      self.treestore[path][0]=newText


    def convertToCAPD(self, filename):
        print("CAPD CONVERTING START")
        
        delete_element_list = []
        annotfile = open(filename+"annot", "w")
        bufferString2 = self.annotations.replace("\\n", '\n')        
        
        file = open("../work-dir/"+filename+".cpp","w")
        ''' Creates a C++ file which uses CAPD classes and gives a simulator after compiling \n '''
        infoFile = '''/* CAPD C++ file generated Automatically from HyLink */\n'''
        file.write(infoFile)
        declarationsReqd = ''' #include <iostream> \n #include "capd/capdlib.h" \n using namespace std; \n using namespace capd; \n '''
        file.write(declarationsReqd)
        curAut = self.automata[0]
        
        declarationsReqd = "\nint getNextMode(int curMode, interval curModeTime);\n"
        file.write(declarationsReqd)
        
        countVars = 0
        cont_vars = []
        
        mainDeclaration = '''main(){ \n \n  cout.precision(10);\n  try{ \n'''
        file.write(mainDeclaration)

        for vars in self.vars:
            if vars.scope == 'LOCAL_DATA' and vars.name.find("dot") == -1 and vars.name.find("clock") == -1:
                countVars = countVars+1
                cont_vars+=[vars.name]
        
        varCount = 0
        for varnames in cont_vars:
            bufferString2 = bufferString2.replace("init="+varnames, "init=x"+str(varCount+1))
            bufferString2 = bufferString2.replace("forbidden="+varnames, "forbidden=x"+str(varCount+1))
            bufferString2 = bufferString2.replace(","+varnames, ",x"+str(varCount+1))
            bufferString2 = bufferString2.replace(varnames+",", "x"+str(varCount+1)+",")
            varCount+=1

        #bufferString2 = "dimensions="+str(varCount)+"\n"+"simulator=capdsim"+"\n"+bufferString2
        #annotfile.write(bufferString2)
        #annotfile.close()
                
        numModes = 0;
        initialMode = curAut.initial_mode_id;
        initialMode += 1;
        temp = ""
        loop = 0
        for curMode in curAut.modes:
            numModes = numModes + 1
            newDiff = ''' /* Differential equation for mode ''' + curMode.name + ''' Testing */ \n'''
            file.write(newDiff)
            varstring = "var:"
            difvarstring=''
            diffunstring=''
            funstring = "fun:"
            lenVars = cont_vars.__len__();
            index=0;
            
            for variable in cont_vars:
                index = index+1
                varstring = varstring + variable
                difvarstring = difvarstring + variable
                if index == lenVars :
                    varstring = varstring + ";"
                else:
                    varstring = varstring + ","
                    difvarstring = difvarstring + ","
                for dai in curMode.dais:
                    if str(dai.expr.lhs) == variable+'_dot':
                        funstring += str(SymEq.convert_pow(dai.expr.rhs))
                        diffunstring += str(SymEq.convert_pow(dai.expr.rhs))

                        if index == lenVars:
                            funstring = funstring + ";"
                        else:
                            funstring = funstring + ","
                            diffunstring = diffunstring +","
            
            modeString = "    IMap mode"+str(numModes)+"(\""+varstring+funstring+"\");\n"

            # FIXME fix this stuff!
            diffunstring = diffunstring.split(',')
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
        declarationString+= "    C0HORect2Set SimSet(IState);\n"
        file.write(declarationString)
        
        # New way for generating simulations
        declarationString = "    ITimeMap timeMap(*solvers[curMode-1]);\n"
        declarationString+= "    timeMap(Gt,SimSet);\n"
        declarationString+= "    const ITaylor::SolutionCurve& curve = solvers[curMode-1]->getCurve();\n"
        #file.write(declarationString)
                
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

        # FIXME 
        createCDFfunction(delete_element_list)

        switchingString = "int getNextMode(int curMode, interval curModeTime){\n"
        switchingString+= "  return curMode;\n"
        switchingString+= "}\n"
        file.write(switchingString)
        file.close()
        

     
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
        
        print("NOW " + str(len(BASE_MODES_COPY)))
        for mode in BASE_MODES_COPY:
            print(str(mode.id) + " " + mode.name)
   
        #adds initial mode to each automaton and 
        #initializes the dictionary that maps ID for each automaton
        for count in range(0, len(hyir.automata)):
            startMode = copy.deepcopy(BASE_AUTOMATON.modes[hyir.automata[count].initial_mode_id])
            print(startMode.id)
            for mode in BASE_MODES_COPY[:]:
                if mode.id == startMode.id:
                    BASE_MODES_COPY.remove(mode)
                    break
            mapID_List.append({startMode.id:hyir.automata[count].new_mode_id()})
            startMode.id = mapID_List[count][startMode.id]
            hyir.automata[count].add_mode(startMode)
        print("NOW " + str(len(BASE_MODES_COPY)))
        for mode in BASE_MODES_COPY:
            print(str(mode.id) + " " + mode.name)
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
