from tkinter import *
from tkinter.ttk import *
from frontend.gui.widgets import PopupEntry, ToggleFrame
from frontend.mod.automaton import *
from frontend.mod.constants import *
from frontend.mod.hyir import *
from frontend.mod.session import Session


class VariableEntry( PopupEntry ):
    """ 
    Popup window for Variable editing.    

    The VariableEntry class is designed to be the popup displayed to users when editing their model's variables. It controls the GUI elements of the popup, and interacts with the Session variables to commit changes to the currently active model
    
    Args:
        parent (obj): Popup's parent object
    """

    def __init__( self, parent, automaton ):
        PopupEntry.__init__( self, parent )
        self.title_label.config( text='Variables' )

        self.automaton = automaton 

        self._init_widgets()
        self._load_session()


    def _init_widgets( self ):
        """ Initialize GUI elements """

        self.title_label.grid( row=0, column=0, columnspan=3 )

        Label( self, text='Name' ).grid( row=1, column=0 )
        Label( self, text='Type' ).grid( row=1, column=1 )
        Label( self, text='Thin' ).grid( row=1, column=2 )

        # Variable lists for uknown number of inputs
        self.names = []  # StringVar()
        self.types = []  # StringVar()
        self.thins = []  # BoolVar()
        self.scopes = [] # StringVar()
        self.var_index = 0

        # Buttons

        self.btn_frame = Frame( self )

        self.cancel_btn = Button( self.btn_frame, text='Cancel', command=self._cancel )
        self.add_btn = Button( self.btn_frame, text='Add', command=self._add_row )
        self.confirm_btn = Button( self.btn_frame, text='Confirm', command=self._confirm )

        self.cancel_btn.grid( row=0, column=0 )
        self.add_btn.grid( row=0, column=1 )
        self.confirm_btn.grid( row=0, column=2 )
        
        # self.btn_frame is added to the grid in self._add_row()

       
    def _load_session( self ):
        """ 
        Load current model's values.
        
        NOTE: We only expect the scope to be LOCAL_DATA, but to prevent deleting
              variables with other scopes (without warning), we display scopes and
              make them editable fields if any other scopes are found.

              This design decision is in line with the decision to display all mode
              equations instead of only those containing '_dot', also meant to
              prevent their deletion.
        """
        
        self.scope_options = { 'LOCAL_DATA' }

        for var in self.automaton.vars:
            if( var.scope != 'LOCAL_DATA' ):
                self.scope_options.add( var.scope )
            self._add_row()
            self.names[self.var_index-1].set( var.name )
            self.types[self.var_index-1].set( var.type )
            self.thins[self.var_index-1].set( False )
            self.scopes[self.var_index-1].set( var.scope )

        for var in self.automaton.thinvars:
            if( var.scope != 'LOCAL_DATA' ): 
                self.scope_options.add( var.scope )
            self._add_row()
            self.names[self.var_index-1].set( var.name )
            self.types[self.var_index-1].set( var.type )
            self.thins[self.var_index-1].set( True )
            self.scopes[self.var_index-1].set( var.scope )

        if( len( self.scope_options ) > 1 ):
            self._init_scope_column()


    def _init_scope_column( self ):
        """ Enable scope column - only used if there is already a non-local scope """

        for i in range( self.var_index ):
            OptionMenu( self, self.scopes[i], self.scopes[i].get(), *self.scope_options )\
                .grid( row=i+2, column=3 )

        # Increase columnspan to accomdate extra row
        self.title_label.grid( row=0, column=0, columnspan=4 )


    def _add_row( self ):
        """ 
        Add a new variable row to VariableEntry popup. 
        Grid new entry widgets and regrid button frame.
        """

        self.names.append( StringVar() )
        self.types.append( StringVar() )
        self.thins.append( BooleanVar() )
        self.scopes.append( StringVar() )

        # Name
        Entry( self, textvariable=self.names[self.var_index] )\
            .grid( row=self.var_index+2, column=0 )

        # Type
        self.types[self.var_index].set( REAL )
        OptionMenu( self, self.types[self.var_index], self.types[self.var_index].get(), *VARIABLE_TYPES )\
            .grid( row=self.var_index+2, column=1 )

        # Thin
        Checkbutton( self, var=self.thins[self.var_index] )\
            .grid( row=self.var_index+2, column=2 )

        # Scope
        self.scopes[self.var_index].set( 'LOCAL_DATA' )
        if( len( self.scope_options ) > 1 ):
            OptionMenu( self, self.scopes[self.var_index], self.scopes[self.var_index].get(), *self.scope_options )\
            .grid( row=self.var_index+2, column=3 )

        self.btn_frame.grid( row=self.var_index+3, columnspan=3 )

        self.var_index += 1


    def _confirm( self ):
        """ Commit changes to Session. Does NOT save these changes. """

        hybrid = self.automaton
        hybrid.reset_vars()
        hybrid.reset_thin_vars()

        for i in range( 0, self.var_index ):

            name = ( self.names[i].get() ).strip()
            type_ = self.types[i].get()  # Reserved word
            thin = self.thins[i].get()
            scope = self.scopes[i].get()

            if not name:  # Delete variables by erasing their name 
                continue

            if thin:
                hybrid.add_thin_var( Variable( name=name, type=type_, scope=scope ) )
            else:
                hybrid.add_var( Variable( name=name, type=type_, scope=scope ) )
            
        print( 'Variable Entry Confirmed\n' )
        self.destroy()

 
    def _cancel( self ):
        """ Cancels changes made in popup """

        print( 'Variable Entry Canceled\n' )
        self.destroy()


class ModeEntry( PopupEntry ):
    """ 
    Popup window for Mode adding, editing, and deleting.

    The ModelEntry class is designed to be the popup displayed to users when editing their model's Modes, or adding/deleting Modes. It controls the GUI elements of the popup, and interacts with the Session variables to commit changes to the currently active models.
    
    Args:
        parent (obj): Popup's parent object
        action (str): Action to be performed (use constants ADD, EDIT, or DELETE)
        mode (Mode obj): Mode to be edited or deleted, not required for ADD action
    """

    def __init__( self, parent, action, mode=None, automaton=None ):
        PopupEntry.__init__( self, parent )
        self.title_label.config( text='Mode' )

        self.mode = mode
        self.automaton = automaton
        self.action = action

        self._init_widgets()

        if( action == ADD ): 
            self._load_new()
        else:
            self._load_session()
            if( action == DELETE ):
                self._disable_fields()


    def _init_widgets( self ):
        """ Initialize GUI elements """ 

        # Name
        Label( self, text='Name:' ).grid( row=1, column=0, sticky=W )
        self.name = StringVar()
        self.name_entry = Entry( self, textvariable=self.name )
        self.name_entry.grid( row=1, column=1, sticky=E )

        # ID
        Label( self, text='ID:' ).grid( row=2, column=0, sticky=W )
        self.mode_id = IntVar()
        self.id_entry = Entry( self, textvariable=self.mode_id )
        self.id_entry.grid( row=2, column=1, sticky=E )

        # Initial
        Label( self, text='Initial:' ).grid( row=3, column=0, sticky=W )
        self.initial = BooleanVar()
        self.initial_checkbutton = Checkbutton( self, var=self.initial )
        self.initial_checkbutton.grid( row=3, column=1 )

        # Flows
        self.flow_toggle = ToggleFrame( self, text='Flows:' )
        self.flow_toggle.grid( row=4, column=0, columnspan=2, sticky=E+W )

        # Invariants
        self.inv_toggle = ToggleFrame( self, text='Invariants:' )
        self.inv_toggle.grid( row=5, column=0, columnspan=2, sticky=E+W )

        # Buttons
        
        self.btn_frame = Frame( self )

        self.cancel_btn = Button( self.btn_frame, text='Cancel', command=self._cancel )
        self.confirm_btn = Button( self.btn_frame, text='Confirm', command=self._confirm )
        
        self.cancel_btn.grid( row=0, column=0 )
        self.confirm_btn.grid( row=0, column=1 )

        self.btn_frame.grid( row=8, column=0, columnspan=2 )


    def _load_session( self ):
        """ Load selected mode's Session values """

        # Name
        self.name.set( self.mode.name )

        # ID
        self.mode_id.set( self.mode.id )

        # Initial
        self.initial.set( self.mode.initial )

        # Flows
        for dai in self.mode.dais:
            self.flow_toggle.add_row( text=dai.raw )
        self.flow_toggle.toggle()
        
        # Invariants
        for inv in self.mode.invs:
            self.inv_toggle.add_row( text=inv.raw )
        self.inv_toggle.toggle()


    def _load_new( self ):
        """ Load blank row and show toggle fields"""

        self.flow_toggle.add_row()
        self.flow_toggle.toggle()

        self.inv_toggle.add_row()
        self.inv_toggle.toggle()

        # Prefill ID assuming IDs are sequential. Not doing this defaults it to 0.
        self.mode_id.set( len( self.automaton.modes ) )


    def _disable_fields( self ):
        """ Disable fields and reconfigure confirm button for deletion """

        self.name_entry.config( state=DISABLED )
        self.id_entry.config( state=DISABLED )
        self.initial_checkbutton.config( state=DISABLED )
        self.flow_toggle.disable_fields()
        self.inv_toggle.disable_fields()

        self.confirm_btn.config( text='DELETE', command=self._delete )


    def _confirm( self ):
        """ Confirm button callback - call confirm method based on action """

        if( self.action == ADD ):
            self._confirm_add()
        else:
            self._confirm_edit()


    def _confirm_add( self ):
        """ Confirm new mode addition """
        
        self.mode = Mode()
        self._confirm_edit()
        self.automaton.add_mode( self.mode )


    def _confirm_edit( self ):
        """ Commit changes to Session. Does NOT save changes """

        # Name
        self.mode.name = self.name.get()
        
        # ID
        self.mode.id = self.mode_id.get()
        
        # Initial
        self.mode.initial = self.initial.get()

        # Flows
        self.mode.clear_dai()
        for raw_text in self.flow_toggle.get_rows():
            if( (raw_text.get()).strip() ): 
                self.mode.add_dai( DAI( raw_text.get() ) )

        # Invariants
        self.mode.clear_inv()
        for raw_text in self.inv_toggle.get_rows():
            if( (raw_text.get()).strip() ): 
                self.mode.add_inv( Invariant( raw_text.get() ) )
        
        print( 'Mode Entry Confirmed\n' )
        self.destroy()


    def _delete( self ):
        """ Delete active Mode """

        if( messagebox.askyesno( 'Delete Transition', 'Delete ' + self.mode.name + '(' + str(self.mode.id) + ') ' + '?' ) ):
            self.automaton.remove_mode( self.mode )
        
        print( 'Mode Deleted\n' )
        self.destroy()


    def _cancel( self ):
        """ Cancels changes made in popup """

        print( 'Mode Entry Canceled\n' )
        self.destroy()


class TransitionEntry( PopupEntry ):
    """ 
    Popup window for Transition adding, editing, and deleting.

    The TransitionEntry class is designed to be the popup displayed to users when editing their model's Modes, or adding/deleting Modes. It controls the GUI elements of the popup, and interacts with the Session variables to commit changes to the currently active models.
    
    Args:
        parent (obj): Popup's parent object
        action (str): Action to be performed (use constants ADD, EDIT, or DELETE)
        mode_dict (dictionary: int keys, str values ): Dictionary connect mode IDs to mode names
        trans (Transition obj): Transition to be edited or deleted, not required for ADD action
    """    

    def __init__( self, parent, action, mode_dict, trans=None, automatn=None ):
        PopupEntry.__init__( self, parent )
        self.title_label.config( text='Transition' )

        self.trans = trans
        self.automaton = automaton
        self.mode_dict = mode_dict  # mode_dict[mode.id] = mode.name
        self.action = action

        # Load Mode list for Source/Destination Option Menus
        self.mode_list = []
        for mode_id in mode_dict:
            self.mode_list.append( mode_dict[mode_id] )

        self._init_widgets()

        if( action == ADD ): 
            self._load_new()
        else:
            self._load_session()
            if( action == DELETE ):
                self._disable_fields()


    def _init_widgets( self ):
        """ Initialize GUI elements """

        # Transition Label
        self.trans_str = StringVar()
        Label( self, textvariable=self.trans_str ).grid( row=1, column=0, columnspan=2 )

        # ID
        Label( self, text='ID:' ).grid( row=2, column=0, sticky=W )
        self.trans_id = IntVar()
        self.id_entry = Entry( self, textvariable=self.trans_id )
        self.id_entry.grid( row=2, column=1, sticky=E )

        # Source and Destination
        
        Label( self, text='Source:' ).grid( row=3, column=0, sticky=W )
        Label( self, text='Destination:' ).grid( row=4, column=0, sticky=W )

        self.src_str = StringVar()
        self.dest_str = StringVar()

        self.src_str.trace_variable( 'w', self._callback_mode_select )
        self.dest_str.trace_variable( 'w', self._callback_mode_select )

        # Aribtrarily set default source/destination. These are overwritten to be correct in _load_session when appropriate
        self.src_opt_menu = OptionMenu( self, self.src_str, self.mode_list[0], *self.mode_list )
        self.src_opt_menu.grid( row=3, column=1, sticky=W+E )        
        self.dest_opt_menu = OptionMenu( self, self.dest_str, self.mode_list[1], *self.mode_list )
        self.dest_opt_menu.grid( row=4, column=1, sticky=W+E )

        # Guards
        Label( self, text='Guards:' ).grid( row=5, column=0, sticky=W )
        self.guard_str = StringVar()
        self.guard_entry = Entry( self, textvariable=self.guard_str )
        self.guard_entry.grid( row=5, column=1, sticky=E )

        # Actions
        self.action_toggle = ToggleFrame( self, text='Actions:' )
        self.action_toggle.grid( row=6, column=0, columnspan=2, sticky=E+W )

        # Buttons
         
        self.btn_frame = Frame( self )

        self.cancel_btn = Button( self.btn_frame, text='Cancel', command=self._cancel )
        self.confirm_btn = Button( self.btn_frame, text='Confirm', command=self._confirm )

        self.cancel_btn.grid( row=0, column=0 )
        self.confirm_btn.grid( row=0, column=1 )

        self.btn_frame.grid( row=7, column=0, columnspan=2 )


    def _load_session( self ):
        """ Load selected transition's Session values """

        # ID
        self.trans_id.set( self.trans.id )

        # Source and Destination
        self.src_str.set( self.mode_dict[self.trans.src] )
        self.dest_str.set( self.mode_dict[self.trans.dest] )

        # Guard
        self.guard_str.set( self.trans.guard.raw )

        # Actions
        for action in self.trans.actions:
            self.action_toggle.add_row( text=action.raw )
        self.action_toggle.toggle()

    
    def _load_new( self ):
        """ Load blank rows and show toggle fields """

        self.action_toggle.add_row()
        self.action_toggle.toggle()

        self.trans_id.set( len( Session.hybrid.automata[0].trans ))


    def _disable_fields( self ):
        """ Disable fields and reconfigure confirm button for deletion """

        self.id_entry.config( state=DISABLED )
        self.src_opt_menu.config( state=DISABLED )
        self.dest_opt_menu.config( state=DISABLED )
        self.guard_entry.config( state=DISABLED )
        self.action_toggle.disable_fields()

        self.confirm_btn.config( text='DELETE', command=self._delete )


    def _callback_mode_select( self, *args ):
        """ OptionMenu callback, updates transition label at top of window """

        self.trans_str.set( self.src_str.get() + " -> " + self.dest_str.get() )


    def _confirm( self ):
        """ Confirm button callback - call confirm method based on action """
        
        if( self.action == ADD ):
            self._confirm_add()
        else:
            self._confirm_edit()


    def _confirm_add( self ):
        """ Confirm new mode addition """

        # ID
        trans_id = self.trans_id.get()

        # Source and Destination
        for mode_id in self.mode_dict:
            if( self.mode_dict[mode_id] == self.src_str.get() ):
                src = mode_id
            elif( self.mode_dict[mode_id] == self.dest_str.get() ):
                dest = mode_id

        # Guard
        guard = Guard( self.guard_str.get() )

        # Actions
        actions = []
        for action in self.action_toggle.get_rows():
            if( (action.get()).strip() ):
                actions.append( Action( action.get() ) )

        transition = Transition( guard, actions, trans_id, src, dest )
        Session.hybrid.automata[0].add_trans( transition )

        print( 'Transition Entry Confirmed\n' )
        self.destroy()     
        

    def _confirm_edit( self ):
        """" Commits changes to Session. Does NOT save changes """

        # ID
        self.trans.id = self.trans_id.get()
        
        # Source and Destination
        for mode_id in self.mode_dict:
            if( self.mode_dict[mode_id] == self.src_str.get() ):
                self.trans.src = mode_id
            elif( self.mode_dict[mode_id] == self.dest_str.get() ):
                self.trans.dest = mode_id

        # Guard
        self.trans.guard = Guard( self.guard_str.get() )

        # Actions
        self.trans.clear_actions()
        for action in self.action_toggle.rows:
            if( (action.get()).strip() ):
                self.trans.add_action( Action(action.get()) )
                
        print( 'Transition Entry Confirmed\n' )
        self.destroy()


    def _delete( self ):
        """ Delete active Transiiton """
        
        if( messagebox.askyesno( 'Delete Transition', 'Delete ' + self.trans_str.get() + '?' ) ):
            Session.hybrid.automata[0].remove_trans( self.trans )
        
        print( 'Transition Deleted\n' )
        self.destroy()


    def _cancel( self ):
        """ Cancels changes made in popup """

        print( 'Transition Entry Canceled\n' )
        self.destroy()