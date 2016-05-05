import pygtk
pygtk.require('2.0')
import gtk, math, itertools, copy, re, os
from lxml import etree
# from parse_modes import extract_dais
# from tree import show_tree, clear_tree
# from EdgeParse import lstring3
# from ModeParse import parse_action
# from guardParse import parse_guardExp
# from guardLogicalParse import parse_guardLogicalExp
# from fractions import Fraction
# from node import Node
from automaton import *
from propDialog import *
from jacobiancalc import jacobian
from jacobiancalc import createCDFfunction
from collections import defaultdict

class HyIR:
    def __init__(self, name="hybrid_system", file_name=""):
        self.name = name
        self.file_name = file_name
        self.vars = []
        self.variables = Variables()
        self.varList = []
        self.automata = Automaton()
        self.annotations = ""
        self.annotationsRaw = []
        # self.annotationsParsed = []
        self.treestore = gtk.TreeStore(str)
    
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

                dai_dict = {}
                HyIR.construct_output_dict(cross_mode, m1.dais, dai_dict)
                HyIR.construct_output_dict(cross_mode, m2.dais, dai_dict)
                HyIR.replace_dais(cross_mode, dai_dict)

                for inv1 in m1.invs:
                    cross_mode.add_inv(inv1)
                for inv2 in m2.invs:
                    cross_mode.add_inv(inv2)
                composed.automata.add_mode(cross_mode)
                # annot = list(hyir2.annotationsParsed[m2.id])
                # annot[0] = m_id+1
                # composed.annotationsParsed.append(annot)

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

        composed.variables.local = hyir1.variables.local+hyir2.variables.local
        composed.variables.output = hyir1.variables.output+hyir2.variables.output
        composed.variables.input = hyir1.variables.input+hyir2.variables.input
        composed.variables.input = [var for var in composed.variables.input if var not in composed.variables.output]
        composed.vars = composed.variables.local+composed.variables.output+composed.variables.input
        composed.varList = [v.name for v in composed.variables.local]

        return composed
        
    def print_vars(self):
        print "--- Variables ---"
        for i in self.vars:
            print i.name+" "+i.type+" "+i.scope
    
    @staticmethod
    def construct_output_dict(mode, dais, dai_dict):
        for dai in dais:
            lhs = str(dai.expr.lhs)
            rhs = str(dai.expr.rhs)
            if not lhs.endswith('_dot'):
                dai_dict[lhs] = rhs
            mode.add_dai(dai)

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

    def populateInvGuards(self):
        guardResets = defaultdict(list)
        invariants = defaultdict(list)
        varList = self.varList

        for m in self.automata[0].modes:
          inv_eqs = []
          inv_vars = set()
          for inv in m.invs:
            print '\n' + 'invariant equation: ' + inv.raw
            inv_eqs.append(inv.expr)
            inv_vars = inv_vars.union(SymEq.vars_used(inv.expr))
            # print '\n' + 'invariant equation: ' + inv.raw
            # i = Invariant(raw=inv.raw)
            # inv_eqs.append(i.expr)
            # inv_vars = inv_vars.union(SymEq.vars_used(i.expr))
          invariants[m.id] = (inv_eqs, inv_vars)

        for t in self.automata[0].trans:
          print '\n' + 'guard equation: ' + t.guard.raw
          g_eqs = t.guard.expr
          g_vars = SymEq.vars_used(t.guard.expr)
          # g = Guard(raw=t.guard.raw)
          # g_eqs = g.expr
          # g_vars = SymEq.vars_used(g.expr)
          action_eqs = []
          for act in t.actions:
            # a_eq = act.raw.replace('=','==')
            print '\n' + 'action equation: ' + act.raw
            # a = Action(raw=act.raw)
            action_eqs.extend(act.expr)
          guardResets[(t.src,t.dest)].append((g_eqs, action_eqs, g_vars))

        self.guardResets = dict(guardResets)
        self.invariants = dict(invariants)

        print self.guardResets
        print self.invariants

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

    def print_all(self):
        print "%s:" % self.name
        self.print_vars()
        for automaton in self.automata:
            automaton.print_all()

    def modesnumber(self):
        return len(self.automata[0].modes)

    def display_system(self):
        varList=[]
        for i in self.vars:
          if i.scope=="LOCAL_DATA":
            varList.append(i.name)
        var=self.treestore.append(None,["Variables"])
        self.treestore.append(var,[", ".join(varList)])

        # modeList=[]
        mode_dict = {}
        modes=self.treestore.append(None,["Modes"])
        for i in self.automata[0].modes:
          mode_dict[i.id] = i.name
          # modeList.append(i.name)
          m=self.treestore.append(modes,[i.name+' ('+str(i.id)+')'])      
          flows=self.treestore.append(m,["Flows"])
          for j in i.dais:
            if "_dot" in j.raw:
              self.treestore.append(flows,[j.raw.replace('==','=')])
          inv=self.treestore.append(m,["Invariants"])
          for j in i.invs:
            self.treestore.append(inv,[j.raw])

        trans=self.treestore.append(None,["Transitions"])
        for i in self.automata[0].trans:
          src, dest = mode_dict[i.src], mode_dict[i.dest]
          t=self.treestore.append(trans,[src+' -> '+dest])
          self.treestore.append(t,["Source: "+src+' ('+str(i.src)+')'])
          self.treestore.append(t,["Destination: "+dest+' ('+str(i.dest)+')'])
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
        return swindow,varList
        # return swindow,varList,modeList

    def printHybridSimGuardsInvariants(self):
        self.printGuardsInvariants("../wd/hybridSimGI.cpp", True)

    def printBloatedSimGuardsInvariants(self):
        self.printGuardsInvariants("../wd/bloatedSimGI.cpp", False)

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
                codeString+="    Constraint_System cs;\n"
                codeString+="    cs.set_space_dimension("+str(len(varsUsed))+");\n"    
    
                for guard_eq in a:
                    codeString+="    cs.insert("+str(guard_eq)+");\n"
                    
                codeString+="    NNC_Polyhedron guard(cs);\n"
                if is_hybrid: codeString+="    if(guard.contains(box_poly)){\n"
                else: codeString+="    if(!guard.is_disjoint_from(box_poly)){\n"
                codeString+= self.constructBoxHelper(tempVarList, "_temp", 6, is_hybrid)

                if is_hybrid and not b:
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
                        codeString+="      cs_gd.insert("+str(reset_eq.func(lhs,rhs))+");\n"
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
            self.convertToCAPD(self.file_name)
            dialog = gtk.MessageDialog(None,0,gtk.MESSAGE_INFO, gtk.BUTTONS_CLOSE,"CAPD C++ file has been created")
            dialog.set_title("Success")
            dialog.run()
            dialog.destroy()
        except IOError:
            dialog = gtk.MessageDialog(None,0,gtk.MESSAGE_ERROR,gtk.BUTTONS_CLOSE,"CAPD C++ file failed to create")
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

    def convertToCAPD(self, filename):
        print("CAPD CONVERTING START")
        
        delete_element_list = []
        annotfile = open(filename+"annot", "w")
        bufferString2 = self.annotations.replace("\\n", '\n')        
        
        file = open(filename+".cpp","w")
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

        bufferString2 = "dimensions="+str(varCount)+"\n"+"simulator=capdsim"+"\n"+bufferString2
        annotfile.write(bufferString2)
        annotfile.close()
                
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
        createCDFfunction(delete_element_list)
        switchingString = "int getNextMode(int curMode, interval curModeTime){\n"
        switchingString+= "  return curMode;\n"
        switchingString+= "}\n"
        file.write(switchingString)
        file.close()
        
    def convertToXML(self, propertyList):
      hyxml=etree.Element("hyxml")

      auto=etree.SubElement(hyxml,"automaton",{"name":self.automata[0].name})
      for var in self.vars:
        etree.SubElement(auto,"variable",{"name":var.name,"scope":var.scope,"type":var.type})
      modeIndex = 0
      for mode in self.automata[0].modes:
        modeIndex+=1
        # annotationFound=0
        m=etree.SubElement(auto,"mode",{"id":str(mode.id),"initial":str(mode.initial),"name":mode.name})
        for dai in mode.dais:
          etree.SubElement(m,"dai",{"equation":dai.raw.replace('==','=')})
        for inv in mode.invs:
          etree.SubElement(m,"invariant",{"equation":inv.raw})

      for tran in self.automata[0].trans:
        t=etree.SubElement(auto,"transition",{"id":str(tran.id),"destination":str(tran.dest),"source":str(tran.src)})
        etree.SubElement(t,"guard",{"equation":tran.guard.raw})
        for act in tran.actions:
          etree.SubElement(t,"action",{"equation":act.raw})

      etree.SubElement(hyxml, "composition", {"automata":self.automata[0].name})

      for prop in propertyList:
        prop=prop[0]
        pt1 = etree.SubElement(hyxml,"property",{"name":prop.name,"type":str(prop.type),
                         "initialSet":prop.initialSetStr,"unsafeSet":prop.unsafeSetStr})
        if prop.status == "Verified" or prop.status == "Verified*" or (prop.paramData[0] != 0 and prop.paramData[1] != 0 and prop.paramData[2] != 0 and prop.paramData[3] != 0) :
            etree.SubElement(pt1,"parameters",{"delta":str(prop.paramData[0]), "timestep":str(prop.paramData[1]),
                        "timehorizon":str(prop.paramData[2]), "taylororder":str(prop.paramData[3])})
      
      return etree.tostring(hyxml,pretty_print=True,xml_declaration=True,encoding="utf-8",doctype="<!DOCTYPE hyxml>")


def hyirXML(fileName):
  parser=etree.XMLParser(remove_blank_text=True)
  tree=etree.parse(fileName,parser)
  root=tree.getroot()

  hybrid_automata = {}
  
  for automata in root.iterfind("automaton"):
    hybrid = HyIR(file_name=fileName)
    name = automata.get("name")
    hybrid_automata[name] = hybrid

    for var in automata.iterfind("variable"):
      v_name = var.get("name")
      v_scope = var.get("scope")
      v_type = var.get("type")
      v = Variable(name=v_name,type=v_type,scope=v_scope)
      hybrid.add_var(v)

    modeIndex=0
    for mode in automata.iterfind("mode"):
      modeIndex+=1  
      m=Mode()
      m.name=mode.get("name")
      m.id=int(mode.get("id"))
      m.initial=(mode.get("initial")=="True")
      for dai in mode.iterfind("dai"):
        v1 = dai.get("equation")
        # m.add_dai(DAI(parse_action(v1)[0],v1))
        m.add_dai(DAI(raw=v1))
      for inv in mode.iterfind("invariant"):
        i = clean_eq(inv.get("equation"))
        # i=inv.get("equation").replace("&lt;","<").replace("&gt;",">").replace("&amp;","&").replace("and","&&").replace("or","||")
        # m.add_inv(Invariant(parse_guardLogicalExp(i)[0],i))
        m.add_inv(Invariant(raw=i))
      hybrid.automata.add_mode(m)  

    for tran in automata.iterfind("transition"):
      guard=tran.find("guard")
      g_id = int(tran.get("id"))
      g_src = int(tran.get("source"))
      g_dest = int(tran.get("destination"))  

      g = clean_eq(guard.get("equation"))
      actions=[]
      for act in tran.iterfind("action"):
        a = clean_eq(act.get("equation"))
        actions.append(Action(raw=a))
      t=Transition(Guard(raw=g),actions,g_id,g_src,g_dest)
      hybrid.automata.add_trans(t)
  
  composition = root.find("composition")
  automata_list = map(lambda x: hybrid_automata[x], composition.get("automata").split(";"))
  automata_list.reverse()
  while len(automata_list)>1:
    hyir1 = automata_list.pop()
    hyir2 = automata_list.pop()
    automata_list.append(HyIR.compose(hyir1, hyir2))

  hybrid = automata_list[0]
  hybrid.automata = [hybrid.automata]
  hybrid.populateInvGuards()
  varList = hybrid.varList
  hybrid.print_all()

  propertyList=[]
  for prop in root.iterfind("property"):
    p=PropertyDatum(len(propertyList))
    p.name=prop.get("name")
    p.type=int(prop.get("type"))    

    p.initialSetStr = clean_eq(prop.get("initialSet"))
    print '\n' + 'initial set equation: ' + p.initialSetStr
    initialSetSplit = p.initialSetStr.split(':')
    p.initialSetParsed = [initialSetSplit[0]] + SymEq.get_eqn_matrix(initialSetSplit[1], varList)
    
    p.unsafeSetStr = clean_eq(prop.get("unsafeSet"))
    print '\n' + 'unsafe set equation: ' + p.unsafeSetStr
    p.unsafeSetParsed = SymEq.get_eqn_matrix(p.unsafeSetStr, varList)    

    for paramSubTree in prop.iterfind("parameters"):
        p.paramData[0] = float(paramSubTree.get("delta"))
        p.paramData[1] = float(paramSubTree.get("timestep"))
        p.paramData[2] = float(paramSubTree.get("timehorizon"))
        p.paramData[3] = float(paramSubTree.get("taylororder"))
    propertyList.append(p)

  return hybrid,propertyList

#Return varsUsed if flag is True
# def convertMatrixToStrEqn(varList, aMatrix, bMatrix, eqMatrix, varsUsedFlag):
#     eqs = []
#     if varsUsedFlag:
#         varsUsed = []
#     for a,b,eq in zip(aMatrix, bMatrix, eqMatrix):
#         lhs = []
#         for i,coeff in enumerate(a):
#             if coeff==1:
#                 lhs.append(varList[i])
#             elif coeff!=0:
#                 lhs.append(str(coeff)+'*'+varList[i])
#         eqs.append('+'.join(lhs)+str(eq[0])+str(b[0]))
#         if varsUsedFlag:
#             varsUsed = [varList[i] for i,coeff in enumerate(a) if varList[i] in varsUsed or coeff!=0 ]
#     print eqs
#     if varsUsedFlag:
#         print varsUsed
#         return eqs, varsUsed
#     else:
#         return eqs

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
                        rate = (str)(k.children[1].value)
                        if not re.search(r'^begin', rate) is None and \
                        not re.search(r'end$', rate) is None:
                            rate = rate[5:-3]
                            rate = re.sub('_',',',rate)
                            rate = re.sub('neg', lambda x: '-', rate)
                            k.children[1].value = "[%s]" % rate
                            k.value = " in "
                        m.add_dai(DAI(k,collapse(k)))
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
                hybrid.automata[0].modes[srid].add_inv(SymEq.construct_invariant(guard))
                # hybrid.automata[0].modes[srid].add_inv(build_invariant(guard.parsed))
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
    hybrid.populateInvGuards()
    return hybrid

# def hyirMdl(mdl_dict, file_name):
#     '''From a given parse tree, returns the hybrid intermediate 
#     representation as an hyir object, consisting of variables and 
#     automata, which are comprised of modes and transitions.
    
#     Maps id's used in the parse tree so that for a given
#     automaton a mode's id corresponds to its location in the mode list
    
#     This function also identifies states of the type AND_STATE as 
#     separate automata and creates a corresponding automaton in hyir.automata
#     by calling the function separateAutomata(hyir)
#     '''
#     hybrid = HyIR()
#     hybrid.file_name = file_name
#     automaton = Automaton()
#     hybrid.automata = [automaton]
    
#     mapIDs = {} #maps ID's so that modes can have incremental id's
#     f_tran_list = []
#     model = mdl_dict['Stateflow'][0]:
#     for attr_dict in model['state']:
#         m = Mode()
#         acceptableMode = True
#         f_tran_id = 0
#         if attr_dict['type'][0]=='AND_STATE':
#             acceptableMode = False
#         if 'id' in attr_dict:
#             m.id = automaton.new_mode_id()
#             mapIDs[attr_dict['id'][0]] = m.id
#         # if j.type == "labelString":
#         #     m.name = j.children[0].value
#         #     for k in j.children[2].children:
#         #         rate = (str)(k.children[1].value)
#         #         if not re.search(r'^begin', rate) is None and \
#         #         not re.search(r'end$', rate) is None:
#         #             rate = rate[5:-3]
#         #             rate = re.sub('_',',',rate)
#         #             rate = re.sub('neg', lambda x: '-', rate)
#         #             k.children[1].value = "[%s]" % rate
#         #             k.value = " in "
#         #         m.add_dai(DAI(k,collapse(k)))
#         if 'labelString' in attr_dict:
#             state_eq = attr_dict['labelString'][0].replace('"','').split('\\n')
#             m.name = state_eq[0]
#             for eq in state_eq[2:]:
#                 m.add_dai(Dai(raw=eq[:-1]))
#         if 'firstTransition' in attr_dict:
#             f_tran_id = attr_dict['firstTransition'][0]
#         if 'position' in attr_dict:
#             pos_arr = filter(lambda x: x not in ['[',']'], attr_dict['position'][0])
#             pos_arr = map(lambda x: float(x), pos_arr.split())
#             xp = int(pos_arr[0]+pos_arr[2])
#             yp = int(pos_arr[1]+pos_arr[3])
#             m.xpos = xp
#             m.ypos = yp

#         if acceptableMode:
#             hybrid.automata[0].add_mode(m)
#         else:
#             hybrid.automata.append(Automaton(m.name))
#             automaton.next_mode_id -= 1
#             f_tran_list.append(f_tran_id)
#             print '\t'.join([str(item) for item in f_tran_list])

#     for i in sftree.children[1].children:
#         if i.type == "SFState":
#             m = Mode()
#             acceptableMode = True
#             f_tran_id = 0
#             for j in i.children:
#                 if j.type == "type" and j.value == "AND_STATE":
#                     acceptableMode = False
#                 if j.type == "id":
#                     m.id = automaton.new_mode_id()
#                     mapIDs[j.value] = m.id
#                 if j.type == "labelString":
#                     m.name = j.children[0].value
#                     for k in j.children[2].children:
#                         rate = (str)(k.children[1].value)
#                         if not re.search(r'^begin', rate) is None and \
#                         not re.search(r'end$', rate) is None:
#                             rate = rate[5:-3]
#                             rate = re.sub('_',',',rate)
#                             rate = re.sub('neg', lambda x: '-', rate)
#                             k.children[1].value = "[%s]" % rate
#                             k.value = " in "
#                         m.add_dai(DAI(k,collapse(k)))
#                 if j.type == "firstTransition":
#                     f_tran_id = j.value
#                 if j.type == "position":
#                     tmp3 = j.value
#                     xp = int((tmp3[0]+tmp3[2]))
#                     yp = int((tmp3[1]+tmp3[3]))
#                     m.xpos = xp
#                     m.ypos = yp
#             if acceptableMode:
#                 hybrid.automata[0].add_mode(m)
#             else:
#                 hybrid.automata.append(Automaton(m.name))
#                 automaton.next_mode_id -= 1
#                 f_tran_list.append(f_tran_id)
#                 print "\t".join([str(item) for item in f_tran_list])

#         if i.type == "SFAnnot":
#             for j in i.children:
#                 if j.type == "labelString":
#                     bufferString = j.value.replace("@","")
#                     hybrid.annotationsRaw += [bufferString]
#                     hybrid.annotations += bufferString + "\n"
            
#         if i.type == "SFData":
#             v = Variable()
#             for j in i.children:
#                 if j.type == "name":
#                     v.name = j.value
#                 if j.type == "scope":
#                     v.scope = j.value
#                 if j.type == "props": 
#                     if j.children[0].value == "SF_CONTINUOUS_TIME_DATA":
#                         v.update_type = "Continuous"
#                         v.type = "Real"
#                     else:
#                         v.update_type = "Discrete"
#                         v.type = ""
#             if v.scope != "INPUT_DATA":
#                 hybrid.add_var(v)

#         if i.type == "SFTransition":
#             actions = []
#             initialFlag = False;
#             for j in i.children:
#                 if j.type == "id":
#                     tid = automaton.new_transition_id()
#                     sl_id = j.value #used for determining initial modes
#                 elif j.type == "Source Block":
#                     if j.children[0].value != 'None' and j.children[0].value !='':
#                         srid = mapIDs[j.children[0].value]
#                     else:
#                         initialFlag = True
#                 elif j.type == "Dest Block":
#                         dsid = mapIDs[j.children[0].value]                           
#                 elif j.type == "labelString":
#                     for k in j.children:
#                         if k.type == 'Logical' or k.type == 'Relational':
#                             guard = Guard(k,collapse2(k))
#                         elif k.type == 'Assignment':
#                             actions.append(Action(k,collapseAction(k)))

#             if not initialFlag: 
#                 hybrid.automata[0].modes[srid].add_inv(SymEq.construct_invariant(guard))
#                 # hybrid.automata[0].modes[srid].add_inv(build_invariant(guard.parsed))
#                 t = Transition(guard,actions,tid,srid,dsid)
#                 hybrid.automata[0].add_trans(t)
#             else:
#                 hybrid.automata[0].modes[dsid].initial = True
#                 for action in actions:
#                     hybrid.automata[0].modes[dsid].initialConditions.append(re.sub(r"'", lambda x: '', action.raw)) 
#                 initialFlag = False
                
#                 #This tells the translator which modes are initial for each automata
#                 #Thus giving allowing further modes in the automata to be deduced
#                 for x in range(0, len(f_tran_list)):
#                     if sl_id == f_tran_list[x]:
#                         print dsid                   
#                         hybrid.automata[x+1].initial_mode_id = dsid
   
#     separateAutomata(hybrid)
#     hybrid.populateInvGuards()
#     return hybrid

     
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

# def build_invariant(guard):
#     '''creates a deep copy of a parsed guard (ie Node tree) and calls the 
#     negate method on that copy and returns the result as an Invariant object'''
#     x = copy.deepcopy(guard)
#     negate(x)
#     return Invariant(x, collapse2(x))

# def negate(node):
#     '''assumes this method is being used to create deterministic transitions
#     by altering mode invariances'''
#     if node.value == 'or':
#         node.value = 'and'
#     elif node.value == 'and':
#         node.value = 'or'
#     elif node.value == '>':
#         node.value = '<='
#     elif node.value == '<':
#         node.value = '>='
#     elif node.value == '>=' or node.value == '=>':
#         node.value = '<'
#     elif node.value == '<=' or node.value == '=<':
#         node.value = '>'
#     elif node.value == '==':
#         node.value = '!='
#     if len(node.children) > 0:
#         for i in node.children:
#             negate(i)
#     return node

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

def clean_eq(eq):
    r_dict = {'&lt;':'<', '&gt;':'>', '&amp;':'&', 'and':'&&', 'or':'||'}
    for term in r_dict:
        eq = eq.replace(term, r_dict[term])
    return eq
