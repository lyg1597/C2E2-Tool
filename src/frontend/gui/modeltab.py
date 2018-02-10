import numpy as np
import shlex, subprocess
from tkinter import messagebox
from tkinter import *
from tkinter.ttk import *
from copy import deepcopy
from backend.lib.libc2e2 import IntegerVector, DoubleVector
from frontend.gui.eventhandler import EventHandler
from frontend.gui.popupentry import *
from frontend.gui.widgets import *
from frontend.mod.automaton import SymEq
from frontend.mod.constants import *
from frontend.mod.session import Session, Property
from frontend.mod.simgen import * # FIXME


class ModelTab( Frame ):

    def __init__( self, parent ):
        
        Frame.__init__( self, parent )
        self.parent = parent
        self._init_widgets()

    def _init_widgets( self ):
        """ Initialize the Treeview and Property Editory """
        
        print( "Initialize the Treeview and Property Editor" )

        self.sidebar = ModelSidebar( self ) 
        self.tree = TreeView(self, self.sidebar, selectmode='browse' )

        self.tree.pack( expand=TRUE, fill=BOTH, side=LEFT, anchor=E ) 
        self.sidebar.pack( expand=TRUE, fill=Y, side=TOP, anchor=E )

        return


class TreeView( Treeview ):
        
    def __init__( self, master, sidebar, **options ):
        """ 
        NOTE: We use 'master' here instead of 'parent' due to naming conflict
              with the parent() function in the Treeview class. 
        """
        
        Treeview.__init__( self, master, **options )
        self.pack( fill=BOTH, expand=TRUE )
        self.master = master
        self.sidebar = sidebar

        self._init_selection_vars()
        self._bind_events()
        self._init_rc_menus()
        self.config( show='tree' )
    
    def _init_selection_vars( self ):
        """ Initialize variables used to parse selected object """

        self.slct_root = None
        self.slct_child = None
        self.slct_grandchild = None
        
        self.slct_automaton = None
        self.slct_context = None
        self.slct_mode = None
        self.slct_transition = None

        return

    def _bind_events( self ):
        """ Bind open and close file events and click events """

        self.bind( CLOSE_EVENT, self._clear_model )
        EventHandler.add_event_listeners( self, CLOSE_EVENT )

        self.bind( OPEN_EVENT, self._display_model )
        EventHandler.add_event_listeners( self, OPEN_EVENT )

        self.bind( '<Double-1>', self._on_double_click )
        self.bind( '<Button-1>', self._on_left_click )
        self.bind( '<Button-3>', self._on_right_click )

        return

    def _clear_model( self, event=None ):
        """ Clear treeview display """

        print( 'Clearing Model...' )

        self.delete( *self.get_children() )

        return

    def _display_model( self, event=None ):
        """ Display Session automata in Treeview """
        
        print( 'Displaying Model...' )

        hybrid = Session.hybrid

        self.automaton_dict = {}  # dict[item_id] = automaton object
        self.mode_dict = {}       # dict[item_id] = mode object
        self.trans_dict = {}      # dict[item_id] = transition object

        self.mode_name_dict = {}  # dict[mode.id] = mode.name
        
        for automaton in hybrid.automata:

            # Create automaton parent item
            automaton_id = self.insert( '', 'end', text=automaton.name )
            self.item( automaton_id, open=TRUE )
            
            # Store automaton in dictionary
            self.automaton_dict[automaton_id] = automaton
            
            # Create variable parent and variable items
            var_id = self.insert( automaton_id, 'end', text=VARIABLES )
            var_str = ', '.join( automaton.variables.names )
            self.insert( var_id, 'end', text=var_str )
            self.item( var_id, open=TRUE )

            # Create thin variable parent and thin variable items
            if( len( automaton.thinvariables.all ) !=0 ):
                thin_var_id = self.insert( automaton_id, 'end', text='Thin Variables' )
                thin_var_str = ', '.join( automaton.thinvariables.names )
                self.insert( thin_var_id, 'end', text=thin_var_str )
                self.item( thin_var_id, open=TRUE )

            # Create mode parent item
            modes_id = self.insert( automaton_id, 'end', text=MODES )
            self.item( modes_id, open=TRUE )
    
            # Create mode items
            for mode in automaton.modes:

                mode_str = mode.name + ' (' + str(mode.id) + ')'
                mode_id = self.insert( modes_id, 'end', text=mode_str )

                self.mode_dict[mode_id] = mode
                self.mode_name_dict[mode.id] = mode.name

                # Create flow itmes
                flow_id = self.insert( mode_id, 'end', text='Flows' )
                for dai in mode.dais:
                    # LMB: Commenting out conditional from popup to avoid 'losing' equations when dialog is confirmed. Removing it here too for consistency. May need to go back and add it after talking with leadership.
                    #if '_dot' in dai.raw:
                    dai_str = dai.raw
                    self.insert( flow_id, 'end', text=dai_str )

                # Create invariant items
                inv_id = self.insert( mode_id, 'end', text='Invariants' )
                for inv in mode.invs:
                    self.insert( inv_id, 'end', text=inv.raw )

            # Create transition parent items
            trans_id = self.insert( automaton_id, 'end', text=TRANSITIONS )
            self.item( trans_id, open=TRUE )
            
            # Create transition items
            for tran in automaton.trans:

                # Build transition string
                src, dest = self.mode_name_dict[tran.src], self.mode_name_dict[tran.dest]
                tran_str = src + ' -> ' + dest
                tran_id = self.insert( trans_id, 'end', text=tran_str )

                self.trans_dict[tran_id] = tran

                # Source
                src_str = 'Source: ' + src + ' (' + str(tran.src) + ')'
                self.insert( tran_id, 'end', text=src_str )

                # Destination
                dest_str = 'Destination: ' + dest + ' (' + str(tran.dest) + ')'
                self.insert( tran_id, 'end', text=dest_str )

                # Guard
                guard_str = 'Guards: ' + tran.guard.raw
                self.insert( tran_id, 'end', text=guard_str )

                # Actions
                act_id = self.insert( tran_id, 'end', text='Actions' )
                for act in tran.actions:
                    self.insert( act_id, 'end', text=act.raw )

        return

    def _init_rc_menus( self ):
        """Initialize right-click menus for Treeview"""  

        # Right-click 'Variables' parent
        self.variables_rc_menu = Menu( self, tearoff=0 )
        self.variables_rc_menu.add_command( label='Edit Variables', command=lambda: self.launch_entry_popup( VARIABLES, EDIT ) )
        
        # Right-click 'Modes' parent
        self.modes_rc_menu = Menu( self, tearoff=0 )
        self.modes_rc_menu.add_command( label='Add Mode', command=lambda: self.launch_entry_popup( MODES, ADD ) )
        self.modes_rc_menu.add_command( label='Edit Mode', command=lambda: self.launch_entry_popup( MODES, EDIT) )
        self.modes_rc_menu.add_command( label='Delete Mode', command=lambda:
        self.launch_entry_popup( MODES, DELETE ) ) 

        # Right-click 'Transitions' parent
        self.transitions_rc_menu = Menu( self, tearoff=0 )
        self.transitions_rc_menu.add_command( label='Add Transition', command=lambda: self.launch_entry_popup( TRANSITIONS, ADD ) )
        self.transitions_rc_menu.add_command( label='Edit Transition', command=lambda: self.launch_entry_popup( TRANSITIONS, EDIT ) )
        self.transitions_rc_menu.add_command( label='Delete Transition', command=lambda: self.launch_entry_popup( TRANSITIONS, DELETE ) )

        return

    def _on_right_click( self, event ):
        """ Right-click event callback """

        # Populate selection variables
        self._identify_selection( event )

        # Display appropriate menu popup
        if( self.slct_context == VARIABLES or self.slct_context == THINVARIABLES ):
            context_menu = self.variables_rc_menu
        elif( self.slct_context == MODES ):
            context_menu = self.modes_rc_menu
        elif( self.slct_context == TRANSITIONS ):
            context_menu = self.transitions_rc_menu
        elif( self.slct_context == AUTOMATON ):
            # TODO LMB
            print( 'Automaton right-click menu not yet built' )
            return
        else:
            return

        # Popup menu
        context_menu.tk_popup( event.x_root, event.y_root )

        # Diable/Enable appropriate depending on where user clicked
        if( self.slct_child == self.slct_grandchild ):  # User clicked on a context
            context_menu.entryconfig( 1, state=DISABLED )
            context_menu.entryconfig( 2, state=DISABLED )
        else:  # User clicked on an object
            context_menu.entryconfig( 1, state=NORMAL )
            context_menu.entryconfig( 2, state=NORMAL )

        return

    def _on_left_click( self, event ):
        """ Left-click event callback """

        # Populate selection variables
        self._identify_selection( event )

        return

    def _on_double_click( self, event ):
        """ Double-click event callback """

        # Populate selection variables        
        self._identify_selection( event )

        # Do nothing if no automaton was selected (user didn't click on an item)
        if( self.slct_automaton == None ): 
            return
        
        # Double-click on a parent (heading), launch add popup.
        # Double-click on an object, launch edit popup.
        if( self.slct_context == AUTOMATON ):
            action = ADD
        elif( ( self.slct_mode == None ) and ( self.slct_transition == None ) ):
            action = ADD
        else:
            action = EDIT

        self.launch_entry_popup( self.slct_context, action )

        return

    def _identify_selection( self, event ):
        """ Identify intended selection for right-click and double-click events """

        # Clear selection variables
        self._init_selection_vars()

        # Return if no item selected
        item_id = self.identify_row( event.y )
        if( not item_id ):
            print( 'No item selected ' )
            self.selection_remove( self.selection() ) # Unselect previous selections
            return

        # Highlight selected item
        self.selection_set( item_id )
        
        # Set selection varialbes
        self._get_slct_vars( item_id )
           
        return

    def _get_slct_vars( self, item_id ):
        """ Returns the ids of the automaton, context, and object of selection (item_id) in that order """

        root = item_id
        child = item_id
        grandchild = item_id

        while( self.parent( root ) != '' ):
            grandchild = child
            child = root
            root = self.parent( root )

        self.slct_root = root
        self.slct_child = child
        self.slct_grandchild = grandchild

        self.slct_automaton = self.automaton_dict[root]
        
        if( child == root ):
            # User clicked on an automaton name
            self.slct_context == AUTOMATON
            return
        self.slct_context = self.item( child )['text']

        if( grandchild == child ): 
            # User clicked on Context heading, no object selected
            return

        if( self.slct_context == MODES ):
            self.slct_mode = self.mode_dict[grandchild]
        elif( self.slct_context == TRANSITIONS ):
            self.slct_transition = self.trans_dict[grandchild]
        
        return 

    def launch_entry_popup( self, context, action ):
        
        if( context == VARIABLES or context == THINVARIABLES ):
            entry = VariableEntry( self.master, self.slct_automaton )
        elif( context == MODES ):
            entry = ModeEntry( self.master, action, self.slct_mode, self.slct_automaton )   
        elif( context == TRANSITIONS ):
            entry = TransitionEntry( self.master, action, self.mode_name_dict, self.slct_transition, self.slct_automaton )
        elif( context == AUTOMATON ):
            print( 'Automaton entry not yet built' )
            return
        
        # Wait for user response
        self.master.wait_window( entry )

        # Update property status if the model was changed
        if( entry.is_changed() ):
            self.sidebar._expire_properties()
        
        # Refresh Model
        self._clear_model()
        self._display_model()

        # Clear out selections vars
        self._init_selection_vars()

        return


class ModelSidebar( Frame ):

    def __init__( self, parent, **options ):
        Frame.__init__( self, parent, **options )
        
        self.parent = parent
        self.sel_iid = None

        self._bind_events()
        self._init_widgets()

    def _bind_events( self ):
        """ Bind open and close file events and click events """

        self.bind( CLOSE_EVENT, self._clear_properties )
        EventHandler.add_event_listeners( self, CLOSE_EVENT )

        self.bind( OPEN_EVENT, self._display_properties )
        EventHandler.add_event_listeners( self, OPEN_EVENT ) 

        return

    def _init_widgets(self):
        """ Initialize Property View and Property List """
        
        self._init_prop_view()
        self._init_prop_list()

        # Pack child frames
        self.prop_view.pack( fill=X )
        self.prop_view.columnconfigure( 1, weight=1 )
        self.prop_list.pack( expand=TRUE, fill=BOTH )

        return

    ''' PROPERTY VIEW GUI AND FUNCTIONS '''
    def _init_prop_view(self):
        """ Initialize Property View """
        
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
        self.re_mode = mode
        self.re_var = var
        self.re_is = init_set
        self.re_us = unsafe_set

        # Property view frame
        self.prop_view = LabelFrame(self, text='Property')

        # Name
        Label(self.prop_view, text='Property name:').grid(row=0, sticky=W)
        self.name_vl = ValidLabel(self.prop_view)
        self.name_vl.grid(row=0, column=2, sticky=E)
        self.name_var = StringVar()
        self.name_var.trace_variable('w', self._callback_name)
        Entry(self.prop_view, textvariable=self.name_var).grid(row=0, column=1, sticky=W+E)

        # Verification type
        # TODO only for show; complete when more functionality available
        self.type_var = IntVar()
        Radiobutton(self.prop_view, text='Safety', variable=self.type_var, value=0).grid(row=1)

        # Time step 
        Label(self.prop_view, text='Time step:').grid(row=2, sticky=W)
        self.ts_vl = ValidLabel(self.prop_view)
        self.ts_vl.grid(row=2, column=2, sticky=E)
        self.time_step_var = DoubleVar()
        self.time_step_var.trace_variable('w', self._callback_time_step)
        Entry(self.prop_view, textvariable=self.time_step_var).grid(row=2, column=1, sticky=W+E)

        # Time horizon
        Label(self.prop_view, text='Time horizon:').grid(row=3, sticky=W)
        self.th_vl = ValidLabel(self.prop_view)
        self.th_vl.grid(row=3, column=2, sticky=E)
        self.time_horizon_var = DoubleVar()
        self.time_horizon_var.trace_variable('w', self._callback_time_horizon)
        Entry(self.prop_view, textvariable=self.time_horizon_var).grid(row=3, column=1, sticky=W+E)

        # K value
        Label(self.prop_view, text='K value:').grid(row=4, sticky=W)
        self.kv_vl = ValidLabel(self.prop_view)
        self.kv_vl.grid(row=4, column=2, sticky=E)
        self.k_value_var = DoubleVar()
        self.k_value_var.trace_variable('w', self._callback_k_value)
        Entry(self.prop_view, textvariable=self.k_value_var).grid(row=4, column=1, sticky=W+E)

        # Simulator type
        opts = (ODEINT_FIX, ODEINT_ADP, CAPD)
        Label(self.prop_view, text='Simulator:').grid(row=5, sticky=W)
        self.sim_var = StringVar()
        self.sim_var.set(opts[0])
        self.sim_var.trace_variable('w', self._callback_simulator)
        OptionMenu(self.prop_view, self.sim_var, '', *opts)\
                .grid(row=5, column=1, columnspan=2, sticky=W+E)

        # Refine type
        opts = (DEF_STRAT, USR_STRAT)
        Label(self.prop_view, text='Refinement:').grid(row=6, sticky=W)
        self.ref_var = StringVar()
        self.ref_var.set(opts[0])
        self.ref_var.trace_variable('w', self._callback_refine_strat)
        OptionMenu(self.prop_view, self.ref_var, '', *opts)\
                .grid(row=6, column=1, columnspan=2, sticky=W+E)

        # Initial set
        Label(self.prop_view, text='Initial set:').grid(row=7, sticky=W)
        self.is_err = StringVar()
        self.is_err.set('')
        Label(self.prop_view, textvariable=self.is_err).grid(row=7, column=1, sticky=W+E)
        self.is_vl = ValidLabel(self.prop_view)
        self.is_vl.grid(row=7, column=2, sticky=E)
        self.initial_set = SetText(self.prop_view, height=65, callback=self._callback_is)
        self.initial_set.grid(row=8, rowspan=4, columnspan=3, sticky=N+S+E+W)

        # Unsafe set
        Label(self.prop_view, text='Unsafe set:').grid(row=12, sticky=W)
        self.us_err = StringVar()
        self.us_err.set('')
        Label(self.prop_view, textvariable=self.us_err).grid(row=12, column=1, sticky=E)
        self.us_vl = ValidLabel(self.prop_view)
        self.us_vl.grid(row=12, column=2, sticky=E)
        self.unsafe_set = SetText(self.prop_view, height=65, callback=self._callback_us)
        self.unsafe_set.grid(row=13, rowspan=4, columnspan=3, sticky=N+S+E+W)

        return 

    def _callback_name( self, *args ):
        """ Verify property name, update Session, and conditionally update property status """

        name = self.name_var.get()
        
        # Update property status if name changed and model is Simulated or Verified
        if( name != Session.cur_prop.name ):
            if( Session.cur_prop.status == Simulated or Session.cur_prop.status == Verified ):
                self._update_property_status( 0, 0, 1 )

        # Validate name
        valid = True
        if( name == '' ):
            valid = False
        else:
            for prop in Session.hybrid.properties:
                if( prop.is_visible ):
                    continue
                elif( name == prop.name ):
                    valid = False
                    break

        # Update Session
        Session.cur_prop.name = name
        Session.cur_prop.name_valid = valid

        # Update GUI elements
        self.name_vl.set_state( valid )
        self.list_view.set( self.sel_iid, 0, name )

        return
    
    def _callback_time_step( self, *args ):
        """ Verify time step, update Session, and conditionally update property status """

        # Validate time step
        try:
            time_step = self.time_step_var.get()
            valid = True
        except ValueError:
            time_step = 0.0
            valid = False

        # Update property status if time step changed and model is Simulated or Verified
        if( Session.cur_prop.time_step != time_step ): 
            if( Session.cur_prop.status == Simulated or Session.cur_prop.status == Verified ):
                self._update_property_status( 0, 0, 1 )

        # Update Session
        Session.cur_prop.time_step = time_step
        Session.cur_prop.time_step_valid = valid
        
        # Update GUI Elements
        self.ts_vl.set_state( valid) 

        return
    
    def _callback_time_horizon( self, *args ):
        """ Verify time horizon, update Session, and conditionally update property status """

        # Validate time horizon
        try:
            time_horizon = self.time_horizon_var.get()
            valid = True
        except ValueError:
            time_horizon = 0.0
            valid = False

        # Update property status if time horizon changed and model is Simulated or Verified
        if( Session.cur_prop.time_horizon != time_horizon ): 
            if( Session.cur_prop.status == Simulated or Session.cur_prop.status == Verified ):
                self._update_property_status( 0, 0, 1 )

        # Update Session
        Session.cur_prop.time_horizon = time_horizon
        Session.cur_prop.time_horizon_valid = valid
        
        # Update GUI Elements
        self.th_vl.set_state( valid )

        return

    def _callback_k_value( self, *args ):
        """ Verify k value, update Session, and conditionally update property status """

        # Validate k value
        try:
            k_value = self.k_value_var.get()
            valid = True
        except ValueError:
            k_value = 0.0
            valid = False

        # Update property status if k value changed and model is Simulated or Verified
        if Session.cur_prop.k_value != k_value: 
            if Session.cur_prop.status == Simulated or Session.cur_prop.status == Verified:
                self._update_property_status( 0, 0, 1 )

        # Update Session
        Session.cur_prop.k_value = k_value
        Session.cur_prop.k_value_valid = valid
        
        # Update GUI Elements
        self.kv_vl.set_state( valid )

        return
    
    def _callback_simulator( self, *args ):
        """ Update Session simulator """
        Session.simulator = self.sim_var.get()
        return

    def _callback_refine_strat( self, *args ):
        """ Update Session refine strategy """
        Session.refine_strat = self.ref_var.get()
        return

    # TODO optimize this
    def _callback_is(self, input):
        #if Session.cur_prop.status == "Simulated" or Session.cur_prop.status == "Verified":
            #self._update_property_status(0,0,1)

        #print ("value form callback is ")
        #print (Session.cur_prop.initial_set_str)
        match = re.match(self.re_is, input)
        if Session.cur_prop.initial_set_str != input: 
            if Session.cur_prop.status == Simulated or Session.cur_prop.status == Verified:
                self._update_property_status(0,0,1)

        Session.cur_prop.initial_set_str = input

        # Check if input is valid
        if match == None:
            self.is_vl.set_state(False)
            Session.cur_prop.initial_set_obj = None
            Session.cur_prop.init_set_valid = False
            if not input == '':
                self.is_err.set('Incorrect syntax')
            else:
                self.is_err.set('')

        # Parse individual components from input
        else:
            is_sep = input.split(':')

            # Validate mode
            mode = re.search(self.re_var, is_sep[0]).group(0)
            mode_list = Session.hybrid.mode_names

            if mode not in mode_list:
                self.is_err.set('No matching modes')
                self.is_vl.set_state(False)
                Session.cur_prop.initial_set_obj = None
                Session.cur_prop.initial_set_valid = False
                return

            # Validate vars
            vars = re.findall(self.re_var, is_sep[1])
            var_list = Session.hybrid.local_var_names
            var_union = set(vars) | set(var_list)
            if len(var_union) > len(var_list):
                self.is_err.set('Variable mismatch')
                self.is_vl.set_state(False)
                Session.cur_prop.initial_set_obj = None
                Session.cur_prop.initial_set_valid = False
                return

            # Parse equations
            a_m, b_m, eq_m = SymEq.get_eqn_matrix(is_sep[1], var_list)
            bounded = SymEq.check_boundedness(a_m, b_m, eq_m, var_list)
            if is_sep[1].count('>')!= is_sep[1].count('<'):
                bounded = False
            
            if bounded:
                self.is_err.set('') 
                self.is_vl.set_state(True)
                Session.cur_prop.initial_set_obj = [is_sep[0], a_m, b_m, eq_m]
                Session.cur_prop.initial_set_valid = True
            else:
                self.is_err.set('Set unbounded')
                self.is_vl.set_state(False)
                Session.cur_prop.initial_set_obj = None
                Session.cur_prop.initial_set_valid = False


    def _callback_us(self, input):
        #if Session.cur_prop.status == "Simulated" or Session.cur_prop.status == "Verified":
            #self._update_property_status(0,0,1)
        match = re.match(self.re_us, input)
        if Session.cur_prop.unsafe_set_str != input: 
            if Session.cur_prop.status == Simulated or Session.cur_prop.status == Verified:
                self._update_property_status(0,0,1)
        Session.cur_prop.unsafe_set_str = input

        # Check if input is valid
        if match == None:
            self.us_vl.set_state(False)
            Session.cur_prop.unsafe_set_obj = None
            Session.cur_prop.unsafe_set_valid = False
            if not input == '':
                self.us_err.set('Incorrect syntax')
            else:
                self.us_err.set('')
            return
        
        # Validate vars
        else:
            vars = re.findall(self.re_var, input)
            var_list = Session.hybrid.local_var_names
            var_union = set(vars) | set(var_list)
            if len(var_union) > len(var_list):
                self.us_err.set('Variable mismatch')
                self.us_vl.set_state(False)
                Session.cur_prop.unsafe_set_obj = None
                Session.cur_prop.unsafe_set_valid = False
                return

            self.us_err.set('')
            self.us_vl.set_state(True)
            Session.cur_prop.unsafe_set_obj = SymEq.get_eqn_matrix(input, var_list)
            Session.cur_prop.unsafe_set_valid = True


    def _display_property(self, prop):
        prop.is_visible = True
        self.name_var.set(prop.name)
        self.time_step_var.set(prop.time_step)
        self.time_horizon_var.set(prop.time_horizon)
        self.k_value_var.set(prop.k_value)
        self.sim_var.set(Session.simulator)
        self.ref_var.set(Session.refine_strat)
        self.initial_set.set(prop.initial_set_str)
        #print (prop.initial_set_str)
        self.unsafe_set.set(prop.unsafe_set_str)


    ''' PROPERTY LIST GUI AND FUNCTIONS'''
    def _init_prop_list(self):
        # Property list frame
        self.prop_list = LabelFrame(self, text='Properties')

        # Property list
        self.list_view = Treeview(self.prop_list)
        self.list_view.pack(fill=BOTH, expand=TRUE)
        self.list_view.bind('<Button-1>', self._callback_btn_press)
        self.list_view.bind('<Double-Button-1>', self._callback_btn_press_double)

        self.list_view['show'] = 'headings'
        self.list_view['columns'] = ('name', 'status', 'result')
        self.list_view.column('name', width=100)
        self.list_view.column('status', width=100)
        self.list_view.column('result', width=100)
        self.list_view.heading('name', text='Property')
        self.list_view.heading('status', text='Status')
        self.list_view.heading('result', text='Result')

        # New, copy, and remove buttons
        row = Frame(self.prop_list)
        row.pack(fill=X)

        self.new_btn = Button(row, text='New', command=self._callback_new)
        self.new_btn.pack(expand=TRUE, fill=X, side=LEFT)

        self.cpy_btn = Button(row, text='Copy', command=self._callback_cpy)
        self.cpy_btn.pack(expand=TRUE, fill=X, side=LEFT)

        self.rmv_btn = Button(row, text='Remove', command=self._callback_rmv)
        self.rmv_btn.pack(expand=TRUE, fill=X, side=LEFT)

        # Simulate and verify buttons
        row = Frame(self.prop_list)
        row.pack(fill=X)

        self.sim_btn = Button(row, text='Simulate', command=self._callback_sim)
        self.sim_btn.pack(expand=TRUE, fill=X, side=LEFT)

        self.ver_btn = Button(row, text='Verify', command=self._callback_ver)
        self.ver_btn.pack(expand=TRUE, fill=X, side=LEFT)

        # Parse and Compose buttons
        row = Frame( self.prop_list )
        row.pack( fill=X )

        self.parse_btn = Button( row, text='Parse', command=self._callback_parse )
        self.parse_btn.pack( expand=TRUE, fill=X, side=LEFT )

        self.compose_btn = Button( row, text='Compose', command=self._callback_compose )
        self.compose_btn.pack( expand=TRUE, fill=X, side=LEFT )


    def _add_property(self, prop):
        iid = self.list_view.insert('', 'end', values=(prop.name, prop.status, prop.result))
        return iid


    def _callback_btn_press_double(self,event):
        if Session.cur_prop.status == Simulated:
            self._open_the_plotter_window(3)
        elif Session.cur_prop.status == Verified:
            if Session.cur_prop.simulator == ODEINT_ADP:
                self._open_the_plotter_window(2)
            else:
                self._open_the_plotter_window(1)
        return


    def _callback_btn_press(self, event):
        # Unset current selection
        Session.cur_prop.is_visible = False

        # Get selected property
        iid = self.list_view.identify_row(event.y)
        if iid:
            self.sel_iid = iid
            idx = self.list_view.index(self.sel_iid)
            Session.cur_prop = Session.hybrid.properties[idx]
            self._display_property(Session.cur_prop)


    def _callback_new(self):
        # Create new property
        Session.cur_prop = Property()
        Session.hybrid.properties.append(Session.cur_prop)

        # Display property in view and list
        self.sel_iid = self._add_property(Session.cur_prop)
        self.list_view.selection_set(self.sel_iid)
        self._display_property(Session.cur_prop)


    def _callback_cpy(self):
        # Create a deep copy of the current session
        new_prop = deepcopy(Session.cur_prop)
        new_prop.status = "Not verified"
        new_prop.result = ""
        new_prop.name = new_prop.name + '_Copy'
        Session.cur_prop = new_prop
        Session.hybrid.properties.append(Session.cur_prop)

        # Display property in view and list
        self.sel_iid = self._add_property(Session.cur_prop)
        self.list_view.selection_set(self.sel_iid)
        self._display_property(Session.cur_prop)


    def _callback_rmv(self):
        # Find next property to select
        idx = self.list_view.index(self.sel_iid)
        plen = len(Session.hybrid.properties) - 1
        if plen == 0:
            Session.cur_prop = Property()
            Session.hybrid.properties.append(Session.cur_prop)
            up_iid = self._add_property(Session.cur_prop)
        elif idx == plen:
            Session.cur_prop = Session.hybrid.properties[idx - 1]
            up_iid = self.list_view.prev(self.sel_iid)
        else:
            Session.cur_prop = Session.hybrid.properties[idx + 1]
            up_iid = self.list_view.next(self.sel_iid)
            
        # Remove the selected property 
        self.list_view.delete(self.sel_iid)
        del Session.hybrid.properties[idx]

        # Display property in view and list
        self.sel_iid = up_iid
        self.list_view.selection_set(self.sel_iid)
        self._display_property(Session.cur_prop)


    def _callback_parse( self ):
        HyIR.parse_all( Session.hybrid_automata )


    def _callback_compose( self ):
        HyIR.parse_all( Session.hybrid_automata )
        HyIR.compose_all( Session.hybrid_automata )


    def _callback_sim(self):

        self._disable_enable_button(1)

        # Guarantee simulator is updated
        Session.cur_prop.simulator = self.sim_var.get()

        result = simulate()

        self._disable_enable_button( 0 )
        self._update_property_status( 1, result, 0 )

        if( result != 0 ):
            self._open_the_plotter_window( 3 )

        return

    def _callback_ver(self):
        
        self._disable_enable_button(1)

        # Guarantee simulator is updated
        Session.cur_prop.simulator = self.sim_var.get()
        
        result = verify()
        
        self._disable_enable_button( 0 )
        self._update_property_status( 0, result, 0 )

        if( result!=0 ):
            
            if( Session.cur_prop.simulator == ODEINT_ADP ):
                self._open_the_plotter_window(2)        
            else:
                self._open_the_plotter_window(1)

        return
    
    def _open_the_plotter_window(self,sim_adpative):
        #construct arguments to open the plot window
        #Sim == 1, FIX_STEP == 0, Adaptive == 2
        #Different result uses different plotter
        unsafe_set = Session.cur_prop.unsafe_set_obj
        file_path = '../work-dir/'+Session.cur_prop.name
        time_step = Session.cur_prop.time_step
        time_horizon = Session.cur_prop.time_horizon
        varlist = Session.get_varList()
        modelist = Session.get_mode_names()

        self.parent.parent._init_plot_widgets(varlist,modelist,time_step,time_horizon,unsafe_set,file_path,sim_adpative,Session.cur_prop.name)
        
    def _expire_properties( self ):
        """ Expire all Simulated and Verified properties """
        
        sel_iid = self.sel_iid

        self._clear_properties()

        for prop in Session.hybrid.properties:
            if( prop.status == Simulated or prop.status == Verified ):
                prop.status += '*'
                prop.result = 'Expired'

        self._display_properties()

        self.sel_iid = sel_iid
        self._display_property( Session.cur_prop )

        return

    def _update_property_status(self, sim, result, expire):
        if expire:
            Session.cur_prop.status += '*'
            Session.cur_prop.result = 'Expired'
            self.list_view.item(self.sel_iid, values=(Session.cur_prop.name, Session.cur_prop.status, Session.cur_prop.result))

            return 

        # if sim:
        #     Session.cur_prop.status = Simulated
        # else:
        #     Session.cur_prop.status = Verified

        # if result == 1:
        #     Session.cur_prop.result = "Safe"
        # elif result == 0 :
        #     Session.cur_prop.result = "Unknown"
        # else:
        #     Session.cur_prop.result = "Unsafe"
        self.list_view.item(self.sel_iid, values=(Session.cur_prop.name, Session.cur_prop.status, Session.cur_prop.result))

        return 


    def _disable_enable_button(self,disable):
        if disable:
            self.new_btn['state'] = 'disabled'
            self.cpy_btn['state'] = 'disabled'
            self.rmv_btn['state'] = 'disabled'
            self.sim_btn['state'] = 'disabled'
            self.ver_btn['state'] = 'disabled'
        else:
            self.new_btn['state'] = 'normal'
            self.cpy_btn['state'] = 'normal'
            self.rmv_btn['state'] = 'normal'
            self.sim_btn['state'] = 'normal'
            self.ver_btn['state'] = 'normal'

        self.update()


    def _set_list_property(self, name):
        iid = self.list_view.focus()
        self.list_view(iid, 0, name)


    ''' PROPERTY EDITOR FUNCTIONS '''
    # TODO implement me
    def _clear_properties(self, event=None):
        for i in self.list_view.get_children():
            self.list_view.delete(i)
        self.sel_iid = None


    def _display_properties(self, event=None):

        for prop in Session.hybrid.properties:
            if self.sel_iid:
                self._add_property(prop)
            else:
                self.sel_iid = self._add_property(prop)
        
        self.list_view.selection_set(self.sel_iid)
        
        Session.cur_prop = Session.hybrid.properties[0]
        Session.cur_prop.is_visible = True
        self._display_property(Session.cur_prop)
