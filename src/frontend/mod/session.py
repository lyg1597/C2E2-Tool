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

    hybrid = None
    cur_prop = None
    prop_list = None
    simulator = ODEINT_FIX
    refine_strat = DEF_STRAT

    file_type = None




    cpp_model = Model()
