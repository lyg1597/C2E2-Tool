import os
import xml.etree.ElementTree as ET

from frontend.mod.hyir import *
from frontend.mod.session import Property
from frontend.mod.constants import * 
from frontend.mod.SFParse import extract_sf
from frontend.mod.hierarchy import IsHierarchical,RemoveHierarchy
from frontend.mod.automaton import *

import pdb


class FileHandler:

    @staticmethod
    def save_model(hybrid, propertyList,file_path):
        #print ("Starting to convert file to hyxml format string")
        hyxml = ET.Element("hyxml", {"type":"Model"})
        auto = ET.SubElement(hyxml, 'automaton', {"name": hybrid.automata[0].name})
        for var in hybrid.vars:
            ET.SubElement(auto,"variable",{"name":var.name,"scope":var.scope,"type":var.type})
        modeIndex = 0
        for mode in hybrid.automata[0].modes:
            modeIndex+=1
            m=ET.SubElement(auto,"mode",{"id":str(mode.id),"initial":str(mode.initial),"name":mode.name})
            for dai in mode.dais:
                equ = dai.raw[3:-1]
                ET.SubElement(m,"dai",{"equation":equ.replace(",","=",1)})
            for inv in mode.invs:
                ET.SubElement(m,"invariant",{"equation":inv.raw})
        for tran in hybrid.automata[0].trans:
            t = ET.SubElement(auto,"transition",{"id":str(tran.id),"destination":str(tran.dest),"source":str(tran.src)})
            ET.SubElement(t,"guard",{"equation":tran.guard.raw})
            for act in tran.actions:
                ET.SubElement(t,"action",{"equation":act.raw})
        ET.SubElement(hyxml,"composition", {"automata":hybrid.automata[0].name})
        for prop in propertyList:
            if not prop.initial_set_str:
                continue
            pt1 = ET.SubElement(hyxml,"property",{"name":prop.name,"type":str(prop.type),"initialSet":prop.initial_set_str,"unsafeSet":prop.unsafe_set_str})
            ET.SubElement(pt1,"parameters",{"timehorizon":str(prop.time_horizon),"timestep":str(prop.time_step),"kvalue":str(prop.k_value)})

        tree = ET.ElementTree(hyxml)
        
        def indent(elem,level=0):
            i = "\n" + level*"  "
            if len(elem):
                if not elem.text or not elem.text.strip():
                    elem.text = i + "  "
                if not elem.tail or not elem.tail.strip():
                    elem.tail = i
                for elem in elem:
                    indent(elem, level+1)
                if not elem.tail or not elem.tail.strip():
                    elem.tail = i
            else:
                if level and (not elem.tail or not elem.tail.strip()):
                    elem.tail = i

        indent(hyxml)
        tree.write(file_path)
        return    

    @staticmethod
    def open_file(file_path):
        base_name = os.path.basename(file_path) 
        raw_name, ext = os.path.splitext(base_name)

        # Handle HyXML file
        if ext == '.hyxml':
            # Get HyXML type and call corresponding function
            hyxml_tree = ET.parse(file_path)
            if hyxml_tree == None:
                return None

            hyxml_root = hyxml_tree.getroot()
            hyxml_type = hyxml_root.get('type')
            thinvarprop = ""
            thinvarlist = ""
            if hyxml_type == 'Model':
                hybrid = FileHandler.open_hyxml_model(hyxml_root, raw_name)
                prop_list = FileHandler.open_hyxml_properties(hyxml_root, hybrid)
                for var in hybrid.varList:
                    if var in hybrid.thinvarList:
                        thinvarlist += var + "\n"
                        thinvarprop += "1\n"
                    else:
                        thinvarprop += "0\n"
                writer = open("../work-dir/ThinVarProp","w")
                writer.write(thinvarprop)
                writer.close()
                writer = open("../work-dir/ThinVarList","w")
                writer.write(thinvarlist)
                writer.close()
            # TODO
            #elif hyxml_type == 'Simulink':
            #    hybrid = self.open_hyxml_simulink(hyxml_root)
            else:
                return None


        # Handle MDL file
        elif ext == '.mdl':
            hybrid = FileHandler.open_mdl_model(file_path,raw_name)
            prop_list = []

        # Handle all other extensions
        else:
            return None

        return {'hybrid': hybrid, 'prop_list': prop_list}

    # Open HyXML Model file
    @staticmethod
    def open_mdl_model(file_path,file_name):
        print ("start to parse mdl file and construct hyir")
        model=open(file_path,"r")
        rawModel = model.read()
        x = re.search(r"Stateflow {",rawModel)
        sf_data=rawModel[x.start():]
        sf_tree= extract_sf(sf_data)
        if IsHierarchical(sf_tree):
            sf_tree=RemoveHierarchy(sf_tree)
        hybrid = HyIR(file_name = file_name)
        automaton = Automaton()
        hybrid.automata = [automaton]

        mapIDs = {}
        f_tran_list = []
        for i in sf_tree.children[1].children:
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
                    if j.type == 'labelString':
                        m.name = j.children[0].value
                        for k in j.children[2].children:
                            rate = str(k.children[1].value)
                            if not re.search(r'^begin', rate) is None and \
                            not re.search(r'^end$',rate) is None:
                                rate = rate[5:-3]
                                rate = re.sub('_',',',rate)
                                rate = re.sub('neg', lambda x: '-', rate)
                                k.children[1].value = "[%s]" % rate
                                k.value = " in "
                            m.add_dai(DAI(collapse(k)))
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
                                guard = Guard(collapse2(k))
                            elif k.type == 'Assignment':
                                actions.append(Action(collapseAction(k)))
                if not initialFlag: 
                    hybrid.automata[0].modes[srid].add_inv(SymEq.construct_invariant(guard))
                    t = Transition(guard,actions,tid,srid,dsid)
                    hybrid.automata[0].add_trans(t)
                else:
                    hybrid.automata[0].modes[dsid].initial = True
                    for action in actions:
                        hybrid.automata[0].modes[dsid].initialConditions.append(re.sub(r"'", lambda x: '', action.raw)) 
                    initialFlag = False

                    for x in range(0, len(f_tran_list)):
                        if sl_id == f_tran_list[x]:
                            hybrid.automata[x+1].initial_mode_id = dsid

        separateAutomata(hybrid)
        hybrid.populateInvGuards()

        return hybrid
        
    
    @staticmethod
    def open_hyxml_model(root, file_name):
        # Parse xml tree
        hybrid_automata = {}
        for automata in root.iterfind("automaton"):
            hybrid = HyIR(file_name=file_name)
            name = automata.get("name")
            hybrid_automata[name] = hybrid

            for var in automata.iterfind("variable"):
                v_name = var.get("name")
                v_scope = var.get("scope")
                v_type = var.get("type")
                v = Variable(name=v_name,type=v_type,scope=v_scope)
                hybrid.add_var(v)

            for thinvar in automata.iterfind("thin_variable"):
                v_name = thinvar.get("name")
                v_scope = thinvar.get("scope")
                v_type = thinvar.get("type")
                v = ThinVariable(name=v_name, type=v_type, scope=v_scope)
                hybrid.add_thin_var(v)

            modeIndex=0
            for mode in automata.iterfind("mode"):
                modeIndex+=1  
                m=Mode()
                m.name=mode.get("name")
                m.id=int(mode.get("id"))
                m.initial=(mode.get("initial")=="True")

                for dai in mode.iterfind("dai"):
                    v1 = dai.get("equation")
                    m.add_dai(DAI(v1))

                for inv in mode.iterfind("invariant"):
                    i = Invariant(FileHandler.clean_eq(inv.get("equation")))
                    if i.expr: m.add_inv(i)

                hybrid.automata.add_mode(m)  

            for tran in automata.iterfind("transition"):
                guard = tran.find("guard")
                g = Guard(FileHandler.clean_eq(guard.get("equation")))
                g_id = int(tran.get("id"))
                g_src = int(tran.get("source"))
                g_dest = int(tran.get("destination"))

                if g.expr:
                  
                    actions=[]
                    for act in tran.iterfind("action"):
                        a = FileHandler.clean_eq(act.get("equation"))
                        actions.append(Action(a))
                    t=Transition(g,actions,g_id,g_src,g_dest)
                    hybrid.automata.add_trans(t)
          
        composition = root.find("composition")
        automata_list = list(map(lambda x: hybrid_automata[x], composition.get("automata").split(";")))
        automata_list.reverse()
        while len(automata_list) > 1:
            hyir1 = automata_list.pop()
            hyir2 = automata_list.pop()
            automata_list.append(HyIR.compose(hyir1, hyir2))
        hybrid = automata_list[0]
        
        hybrid.automata = [hybrid.automata]
        hybrid.populateInvGuards()
        hybrid.print_all()

        return hybrid

    """
    # FIXME move this up before the previous method
    # Open HyXML Simulink file
    def open_hyxml_simulink(self, hyxml_root):
        # Simulink model
        model = hyxml_root.find('model')
        self.file.simul_name = model.get('name')
        self.file.simul_path = model.get('path')

        # Find input/output variables
        inputs = model.iterfind('input')
        outputs = model.iterfind('output')
        variables = [x for x in inputs] + [x for x in outputs]

        # Iterate all variables
        for var in variables:
            # Extract information depending on type
            type = var.get('type')
            if type == 'constant':
                val1 = float(var.get('value'))
                val2 = None
            elif type == 'radius':
                val1 = float(var.get('center'))
                val2 = float(var.get('delta'))
            else:
                val1 = float(var.get('low'))
                val2 = float(var.get('high'))

            tag = var.tag
            if tag == 'input':
                self.file.simul_inputs.append(var.get('name'))
                self.file.simul_input_values.append([type, val1, val2])
            else:
                self.file.simul_outputs.append(var.get('name'))
                self.file.simul_output_values.append([type, val1, val2])

        # FIXME remove the propList stuff
        # Extract parameters
        params = model.find('parameters')
        time_step = params.get('timestep')
        time_horizon = params.get('timehorizon')
        self.file.prop_list = ['0', time_step, time_horizon]
    """

    @staticmethod
    def open_hyxml_properties(root, hybrid):
        prop_list = []
        for prop in root.iterfind('property'):
            p = Property()
            p.name = prop.get('name')

            p.type = SAFETY # TODO implement the logic for this

            p.initial_set_str = FileHandler.clean_eq(prop.get('initialSet'))
            init_set_split = p.initial_set_str.split(':')
            p.initial_set_obj = [init_set_split[0]] + SymEq.get_eqn_matrix(init_set_split[1], hybrid.varList)
            
            p.unsafe_set_str = FileHandler.clean_eq(prop.get('unsafeSet'))
            p.unsafe_set_obj = SymEq.get_eqn_matrix(p.unsafe_set_str, hybrid.varList)    

            # Handle properties parameters
            param = prop.find('parameters')
            if param is not None:
                time_step = param.get('timestep')
                if time_step is None:
                    p.time_step = 0.0
                else:
                    p.time_step = float(time_step)

                time_horizon = param.get('timehorizon')
                if time_horizon is None:
                    p.time_horizon = 0.0
                else:
                    p.time_horizon = float(time_horizon)

                k_value = param.get('kvalue')
                if k_value is None:
                    p.k_value = 0.0
                else:
                    p.k_value = float(k_value)

            prop_list.append(p)

        return prop_list
    
    @staticmethod
    def clean_eq(eq):
        r_dict = {'&lt;':'<', '&gt;':'>', '&amp;':'&', ' and ':'&&', ' or ':'||'}
        for term in r_dict:
            eq = eq.replace(term, r_dict[term])
        return eq
