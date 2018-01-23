from backend.lib.libc2e2 import Model
from frontend.mod.constants import *


class Property():

    def __init__(self):
        # Property values
        self.name = ''
        self.type = SAFETY
        self.time_step = 0.0
        self.time_horizon = 0.0
        self.k_value = 0.0
        self.initial_set_str = ''
        self.initial_set_obj = None
        self.unsafe_set_str = ''
        self.unsafe_set_obj = None
        self.simulator = ''


        # Property values valid
        self.name_valid = False
        self.type_valid = True
        self.time_step_valid = False
        self.time_horizon_valid = False
        self.k_value_valid = False
        self.initial_set_valid = False
        self.unsafe_set_valid = False

        # Misc
        self.status = "Not verified"
        self.result = ''
        self.is_visible = False

    def is_valid(self):
        if self.name_valid and self.type_valid and self.time_step_valid and \
                self.time_horizon_valid and self.k_value_valid and \
                self.initial_set_valid and self.unsafe_set_valid:
            return True 
        else:
            return False


class PlotProperty():

    def __init__(self,identifier):

        # Property values
        self.name = ''
        self.x_axis_label = ''
        self.y_axis_label = ''
        self.plot_name = ''
        self.x = ''
        

        # Property values valid
        self.name_valid = False
        self.x_valid = True
        self.y_check_box = []


        # Misc
        self.status = "Not Plotted"
        self.is_visible = False
        self.identifier = identifier

    def is_valid(self):
        if not self.name_valid:
            return False
        if not self.x_valid:
            return False
        return True

    def __str__(self):
        print("name: "+self.name)
        print("x_axis_label: "+self.x_axis_label)
        print("y_axis_label: "+self.y_axis_label)
        print("plot_name: "+self.plot_name)
        print("x: "+self.x)
        return ""


class Session():

    file_opened = False
    file_modified = False
    file_saved = False
    file_path = ''

    lib_compiled = False

    # Pre-composition
    hybrid_automata = None

    # Post-composition
    hybrid = None
    cur_prop = None
    prop_list = None

    simulator = ODEINT_FIX
    refine_strat = DEF_STRAT

    file_type = None

    cpp_model = None


    def parse():

        for hybrid in hybrid_automata:
            for mode in hybrid.automata.modes:
                mode.construct()
            for tran in hybrid.automata.trans:
                trans.construct()

        if( len( hybrid_automata ) == 1 ):
            hybrid = hybrid_automata[0]

    def compose():
        
        # LMB  1/16/2018  Originally took place in filehandler.py, this is a copy
        parse()

        automata_list = hybrid_automata
        automata_list.reverse()
        while len( automata_list ) > 1:
            hyir1 = automata_list.pop()
            hyir2 = automata_list.pop()
            automata_list.apped( HyIR.compose( hyir1, hyir2 ) )
        
        hybrid = automata_list[0]
        hybrid.populateInvGuards()
        hybrid.print_all()

        hyxml_root = ET.parse( file_path ).getroot()
        prop_list = FileHandler.open_hyxml_properties(hyxml_root, hybrid)
        
        thinvarprop = ""
        thinvarlist = ""

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

        composed = True

        cpp_mode = Model()

    @classmethod
    def get_mode_names( cls ):

        mode_list = cls.get_modes()
        mode_name_list = []
        
        for mode in mode_list:
            mode_name_list.append( mode.get_name() )

        return mode_name_list

    @classmethod
    def get_modes( cls ):

        mode_list = []

        if( cls.hybrid ):
            return cls.hybrid.automata.modes

        for hybrid in cls.hybrid_automata:
            for mode in hybrid.automata.modes:
                mode_list.append( mode )

        return mode_list


    @classmethod
    def get_vars( cls ):
        """ Get variable objects from composed hybrid, or from all automata for a non-composed sytem """

        vars = []

        if( cls.hybrid ):
            return cls.hybrid.automata.vars

        for hybrid in cls.hybrid_automata:
            for var in hybrid.vars:
                vars.append( var )

        return vars


    @classmethod
    def get_varList( cls ):
        """ 
        Get varList ( variable objects with scope LOCAL_DATA ) from composed hybrid, or fram all automata in a non-composed system 
        
        NOTE: The unusual naming convention is a blend of the underscore standard and the camelCase used in the HyIR object. -- LMB, 1/22/2018
        """

        varList = []

        if( cls.hybrid ):
            return cls.hybrid.varList

        for hybrid in cls.hybrid_automata:
            for local_var in hybrid.varList:
                varList.append( local_var )

        return varList


    @classmethod
    def get_thinvars( cls ):
        """ Get thin variable objects from composed hybrid, or from all automata for a non-composed system """

        thinvars = []

        if( cls.hybrid ):
            return cls.hyrid.thinvars

        for hybrid in cls.hybrid_automata:
            for thinvar in hybrid.thinvars:
                thinvars.append( thinvar )

        return thinvars


    @classmethod
    def get_thinvarList( cls ):
        """ 
        Get thinvarList ( thin variable objects with scope LOCAL_DATA ) from composed hybrid, or from all automata in a non-composed system 
        
        NOTE: The unusual naming convention is a blend of the underscore standard and the camelCase used in the HyIR object. -- LMB, 1/22/2018
        """

        thinvarList = []

        if( cls.hybrid ):
            return cls.hybrid.thinvarList

        for hybrid in cls.hybrid_automata:
            for local_thinvar in hybrid.thinvarList:
                thinvarList.append( local_thinvar )

        return thinvarList