import re

from backend.lib.libc2e2 import Model
from frontend.mod.automaton import SymEq
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

    @staticmethod
    def validate_cur_prop():

        prop = Session.cur_prop

        Property.validate_name( prop.name )
        Property.validate_type( prop.type )
        Property.validate_time_step( prop.time_step )
        Property.validate_time_horizon( prop.time_horizon )
        Property.validate_k_value( prop.k_value )
        Property.validate_initial_set( prop.initial_set_str )
        Property.validate_unsafe_set( prop.unsafe_set_str )

        return

    @staticmethod
    def validate_name( name ):
        
        valid = True
        if( name == '' ):
            valid = False
        else:
            for prop in Session.prop_list:
                if( prop == Session.cur_prop ):
                    continue
                elif( name == prop.name ):
                    valid = False
                    break

        Session.cur_prop.name_valid = valid
        return valid

    @staticmethod
    def validate_type( type ):

        # TODO: LMB Not sure where this is ever invalidated (after being initialized to True). This function is a placeholder until I learn more.
        Session.cur_prop.type_valid = True
        return True
        
    @staticmethod
    def validate_time_step( time_step ):
        
        #valid = ( is_number(time_step) and (time_step >= 0) )
        valid = ( time_step >= 0 )

        Session.cur_prop.time_step_valid = valid
        return valid

    @staticmethod
    def validate_time_horizon( horizon ):
        
        #valid = ( is_number(horizon) and (horizon >= 0) )
        valid = ( horizon >= 0 )

        Session.cur_prop.time_horizon_valid = valid
        return valid

    @staticmethod
    def validate_k_value( k_value ):

        #valid = ( is_number(k_value) and (k_value >= 0) )
        valid = ( k_value >= 0 )

        Session.cur_prop.k_value_valid = valid
        return valid

    @staticmethod
    def validate_initial_set( initial_set ):
        
        # Regex building blocks
        flt = '(-?(\d*\.?\d+))'
        int = '(-?(\d+))'
        term = '(' + flt + '|(' + int + '\s*/\s*' + int + '))'
        var = '([a-zA-Z]\w*)'
        mode = '(' + var + ':)'
        eql = '((<=?)|(>=?)|(==))'
        expr = '(' + term + '?\s*' + var + '(\s*[-\+]?\s*((' + term + '?\s*' + var + ')|' + term + '))*)'
        eqn = '(' + expr + '\s*' + eql + '\s*' + term + ')'
        eqns = '(' + eqn + '(\s*&&\s*' + eqn + ')*)'
        init_set = '(\s*' + mode + '\s*' + eqns + '\s*){1}$'
        unsafe_set = '(\s*' + eqns + '\s*)+$'

        # Regex strings
        re_mode = mode
        re_var = var
        re_is = init_set
        re_us = unsafe_set

        match = re.match( re_is, initial_set )
        Session.cur_prop.initial_set_str = initial_set

        if( match == None ):
            print( "ERROR/INITIAL SET: Incorrect Syntax" )
            Session.cur_prop.initial_set_obj = None
            Session.cur_prop.initial_set_valid = False
            return False
        else:
            is_sep = initial_set.split( ':' )

            # Validate Mode
            mode = re.search( re_var, is_sep[0] ).group(0)
            mode_list = Session.get_mode_names()

            if( mode not in mode_list ):
                print( "ERROR/INITIAL SET: No matching modes" )
                Session.cur_prop.initial_set_obj = None
                Session.cur_prop.initial_set_valid = False
                return False

            # Validate Vars
            vars_ = re.findall( re_var, is_sep[1] )
            var_list = Session.get_varList()
            var_union= set(vars_) | set(var_list )

            if( len(var_union) > len(var_list) ):
                print( "ERROR/INITIAL SET: Variable mismatch" )
                Session.cur_prop.initial_set_obj = None
                Session.cur_prop.initial_set_valid = False
                return False

            # Parse equations
            a_m, b_m, eq_m = SymEq.get_eqn_matrix( is_sep[1], var_list )
            bounded = SymEq.check_boundedness( a_m, b_m, eq_m, var_list )

            if( is_sep[1].count('>')!= is_sep[1].count('<') ):
                bounded = False
            
            if bounded:
                Session.cur_prop.initial_set_obj = [is_sep[0], a_m, b_m, eq_m]
                Session.cur_prop.initial_set_valid = True
                return True
            else:
                print( "ERROR/INITIAL SET: Set unbounded" )
                Session.cur_prop.initial_set_obj = None
                Session.cur_prop.initial_set_valid = False
                return False


    def validate_unsafe_set( unsafe_set ):

        # Regex building blocks
        flt = '(-?(\d*\.?\d+))'
        int = '(-?(\d+))'
        term = '(' + flt + '|(' + int + '\s*/\s*' + int + '))'
        var = '([a-zA-Z]\w*)'
        mode = '(' + var + ':)'
        eql = '((<=?)|(>=?)|(==))'
        expr = '(' + term + '?\s*' + var + '(\s*[-\+]?\s*((' + term + '?\s*' + var + ')|' + term + '))*)'
        eqn = '(' + expr + '\s*' + eql + '\s*' + term + ')'
        eqns = '(' + eqn + '(\s*&&\s*' + eqn + ')*)'
        init_set = '(\s*' + mode + '\s*' + eqns + '\s*){1}$'
        unsafe_set_reg = '(\s*' + eqns + '\s*)+$'

        # Regex strings
        re_mode = mode
        re_var = var
        re_is = init_set
        re_us = unsafe_set_reg

        match = re.match( re_us, unsafe_set )
        Session.cur_prop.unsafe_set_str = unsafe_set

        # Check if input is valid
        if( match == None ):
            print( "ERROR/UNSAFE SET: Incorrect Syntax" )
            Session.cur_prop.unsafe_set_obj = None
            Session.cur_prop.unsafe_set_valid = False
            return False
        
        # Validate vars
        else:
            vars_ = re.findall( re_var, unsafe_set )
            var_list = Session.get_varList()
            var_union = set(vars_) | set(var_list)

            if( len(var_union) > len(var_list) ):
                print( "ERROR/UNSAFE SET: Variable mismatch" )
                Session.cur_prop.unsafe_set_obj = None
                Session.cur_prop.unsafe_set_valid = False
                return False

            Session.cur_prop.unsafe_set_obj = SymEq.get_eqn_matrix( unsafe_set, var_list)
            Session.cur_prop.unsafe_set_valid = True
            return True


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
    file_type = None

    lib_compiled = False

    # Working Model
    hybrid = None
    cur_prop = None
    
    # Pre-composition (if hyrid was composed in this session)
    hybrid_precomp = None

    # Simulation parameters
    simulator = ODEINT_FIX
    refine_strat = DEF_STRAT

    cpp_model = Model()