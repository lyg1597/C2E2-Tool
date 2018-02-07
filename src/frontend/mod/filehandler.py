import os
import xml.etree.ElementTree as ET

from frontend.mod.hyir import *
from frontend.mod.session import Session, Property
from frontend.mod.constants import * 
from frontend.mod.SFParse import extract_sf
from frontend.mod.hierarchy import IsHierarchical,RemoveHierarchy
from frontend.mod.automaton import *

import pdb


class FileHandler:

    @staticmethod
    def save_model(hybrid_automata, propertyList, file_path):

        #print ("Starting to convert file to hyxml format string")
        hyxml = ET.Element("hyxml", {"type":"Model"})

        for hybrid in hybrid_automata:

            auto = ET.SubElement(hyxml, 'automaton', {"name": hybrid.automata.name})
            
            # Variables
            for var in hybrid.vars:
                ET.SubElement(auto,"variable",{"name":var.name,"scope":var.scope,"type":var.type})

            # Thin Variables
            for thinvar in hybrid.thinvars:
                ET.SubElement( auto, "thin_variable", { "name":thinvar.name, "scope":thinvar.scope, "type":thinvar.type } )

            # Modes
            for mode in hybrid.automata.modes:
                m=ET.SubElement(auto,"mode",{"id":str(mode.id),"initial":str(mode.initial),"name":mode.name})
                for dai in mode.dais:
                    #equ = dai.raw[3:-1]
                    #ET.SubElement(m,"dai",{"equation":equ.replace(",","=",1)})
                    ET.SubElement( m, "dai", {"equation":dai.raw} )
                for inv in mode.invs:
                    ET.SubElement(m,"invariant",{"equation":inv.raw} )
            
            # Transitions
            for tran in hybrid.automata.trans:
                t = ET.SubElement(auto,"transition",{"id":str(tran.id),"destination":str(tran.dest),"source":str(tran.src)})
                ET.SubElement(t,"guard",{"equation":tran.guard.raw})
                for act in tran.actions:
                    ET.SubElement(t,"action",{"equation":act.raw})

        # Composition    
        #ET.SubElement(hyxml,"composition", {"automata":hybrid.automata[0].name})
            
        # Properties
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
    def open_file( file_path ):
        """ Open file entry point. """
        print( "Opening File..." )

        base_name = os.path.basename( file_path ) 
        raw_name, ext = os.path.splitext( base_name )

        # Handle HyXML file
        if ext == '.hyxml':
            
            Session.file_type = HYXML_FILE

            # Get HyXML type and call corresponding function
            hyxml_tree = ET.parse(file_path)
            if hyxml_tree == None:
                return None

            hyxml_root = hyxml_tree.getroot()
            hyxml_type = hyxml_root.get('type')

            thinvarprop = ""
            thinvarlist = ""

            if hyxml_type == 'Model':

                hybrid_automata = FileHandler.open_hyxml_model( hyxml_root, raw_name )
                prop_list = FileHandler.open_hyxml_properties( hyxml_root )

            else:
                return None

        # Handle MDL file
        elif ext == '.mdl':

            Session.file_type = MLD_FILE

            hybrid = FileHandler.open_mdl_model(file_path,raw_name)
            prop_list = []

        # Handle all other extensions
        else:
            return None
        
        Session.hybrid_automata = hybrid_automata
        Session.prop_list = prop_list

        print( "File opened." )
        return hybrid_automata

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
    def open_hyxml_model( root, file_name ):
        """ Load model from hyxml """
        print( "Loading hyxml model..." )

        hybrid_automata = []

        for automata in root.iterfind( "automaton" ):

            # Create new HyIR object for each automaton
            name = automata.get( "name" )
            hybrid_name = name + " HyIR"
            hybrid = HyIR( name=hybrid_name, file_name=file_name )
            hybrid.automata.name = name
            hybrid_automata.append( hybrid )

            for var in automata.iterfind( "variable" ):
                
                # Load variables
                v_name = var.get( "name" )
                v_scope = var.get( "scope" )
                v_type = var.get( "type" )
               
                v = Variable( name=v_name, type=v_type, scope=v_scope )
                hybrid.add_var( v )

            for thinvar in automata.iterfind( "thin_variable" ):
                
                # Load thin variables
                v_name = thinvar.get( "name" )
                v_scope = thinvar.get( "scope" )
                v_type = thinvar.get( "type")
                
                v = ThinVariable( name=v_name, type=v_type, scope=v_scope )
                hybrid.add_thin_var( v )

            for mode in automata.iterfind( "mode" ):
                
                # Load modes
                mode_name = mode.get( "name" )
                mode_id = int( mode.get( "id" ) )
                mode_initial = ( mode.get( "initial" ) == "True" )
                
                mode_obj = Mode( name=mode_name, id=mode_id, initial=mode_initial )
        
                for dai in mode.iterfind( "dai" ):

                    # Load Flows 
                    raw_eq = dai.get( "equation" )                    
                    mode_obj.add_dai( DAI(raw_eq) )

                for inv in mode.iterfind( "invariant" ):
                    
                    # Load Invariants
                    raw_eq = inv.get( "equation" )
                    # Equation 'cleaning' is needed for inequalities
                    clean_eq = FileHandler.clean_eq( raw_eq )
                    mode_obj.add_inv( Invariant( clean_eq ) )
                
                hybrid.automata.add_mode( mode_obj )  

            for tran in automata.iterfind( "transition" ):

                # Load transitions
                g = tran.find( "guard" )
                guard = Guard( FileHandler.clean_eq( g.get("equation") ) )

                tran_id = int( tran.get( "id" ) )
                tran_src = int( tran.get( "source" ) )
                tran_dest = int( tran.get( "destination" ) )
                
                # Actions
                actions = []
                for act in tran.iterfind( "action" ):

                    raw_eq = act.get( "equation" )
                    clean_eq = FileHandler.clean_eq( raw_eq )
                    actions.append( Action(clean_eq) )

                transition = Transition( guard, actions, tran_id, tran_src, tran_dest )
                hybrid.automata.add_trans( transition )

        print( "Model Loaded." )
        return hybrid_automata

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
    def open_hyxml_properties( root ):
        """ Load properties from hyxml """
        print( "Loading hyxml properties..." )

        prop_list = []
        for prop in root.iterfind('property'):
            
            p = Property()
            p.name = prop.get('name')

            p.type = SAFETY # TODO implement the logic for this
            p.initial_set_str = FileHandler.clean_eq(prop.get('initialSet'))
            p.unsafe_set_str = FileHandler.clean_eq(prop.get('unsafeSet'))

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

        print( "Properties loaded." )
        return prop_list
    
    @staticmethod
    def clean_eq(eq):
        r_dict = {'&lt;':'<', '&gt;':'>', '&amp;':'&', ' and ':'&&', ' or ':'||'}
        for term in r_dict:
            eq = eq.replace(term, r_dict[term])
        return eq
