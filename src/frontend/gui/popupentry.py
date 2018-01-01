from tkinter import *
from tkinter.ttk import *
from frontend.gui.widgets import PopupEntry, ToggleFrame
from frontend.mod.automaton import *
from frontend.mod.constants import *
from frontend.mod.hyir import *
from frontend.mod.session import Session


class VariableEntry( PopupEntry ):

    def __init__( self, parent ):
        PopupEntry.__init__( self, parent )
        self.title_label.config( text='Variables' )

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
        """ Load current model's values """
        
        #TODO LMB: I'm not sure if hybrid.vars and hybrid.thinvars are the correct variables to be looping over for this. Confirm that I have loaded the values correctly or fix it

        hybrid = Session.hybrid

        for var in hybrid.vars:
            self._add_row()
            self.names[self.var_index-1].set( var.name )
            self.types[self.var_index-1].set( var.type )
            self.thins[self.var_index-1].set( False )

        for var in hybrid.thinvars:
            self._add_row()
            self.names[self.var_index-1].set( var.name )
            self.types[self.var_index-1].set( var.type )
            self.thins[self.var_index-1].set( True )


    def _add_row( self ):
        """ 
        Add a new variable row to VariableEntry popup. 
        Grid new entry widgets and regrid button frame
        """

        self.names.append( StringVar() )
        self.types.append( StringVar() )
        self.thins.append( BooleanVar() )

        # Name
        Entry( self, textvariable=self.names[self.var_index] )\
            .grid( row=self.var_index+2, column=0 )

        # Type
        OptionMenu( self, self.types[self.var_index], *VARIABLE_TYPES )\
            .grid( row=self.var_index+2, column=1 )

        # Thin
        Checkbutton( self, var=self.thins[self.var_index] )\
            .grid( row=self.var_index+2, column=2 )

        self.btn_frame.grid( row=self.var_index+3, columnspan=3 )

        self.var_index += 1


    def _confirm( self ):
        """ Commit changes to Session. Does NOT save these changes.
        
        NOTE: Scope hard-coded to LOCAL_DATA after discussions with leaderhsip.
              LOCAL_DATA is the only scope we expect to use with editing, so there
              is no need for users to enter it (or for it to be anything else)
        """
        hybrid = Session.hybrid
        hybrid.reset_vars()
        hybrid.reset_thin_vars()

        for i in range( 0, self.var_index ):

            name = ( self.names[i].get() ).strip()
            type_ = self.types[i].get()  # Reserved word
            thin = self.thins[i].get()
            scope = 'LOCAL_DATA'

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

    def __init__( self, parent, mode ):
        PopupEntry.__init__( self, parent )
        self.title_label.config( text='Mode' )

        self.mode = mode

        self._init_widgets()
        self._load_session()


    def _init_widgets( self ):
        """ Initialize GUI elements """ 

        # Name
        Label( self, text='Name:' ).grid( row=1, column=0, sticky=W )
        self.name = StringVar()
        Entry( self, textvariable=self.name ).grid( row=1, column=1, sticky=E )

        # ID
        Label( self, text='ID:' ).grid( row=2, column=0, sticky=W )
        self.mode_id = IntVar()
        Entry( self, textvariable=self.mode_id ).grid( row=2, column=1, sticky=E )

        # Initial
        Label( self, text='Initial:' ).grid( row=3, column=0, sticky=W )
        self.initial = BooleanVar()
        Checkbutton( self, var=self.initial ).grid( row=3, column=1 )

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
        """ Load selected mode's values """

        # Name
        self.name.set( self.mode.name )

        # ID
        self.mode_id.set( self.mode.id )

        # Initial
        self.initial.set( self.mode.initial )

        # Flows
        for dai in self.mode.dais:
            if '_dot' in dai.raw:
                dai_str = dai.raw[3:-1]
                idx = dai_str.index(',')
                dai_str = dai_str[:idx]+'='+dai_str[idx+1:]
                self.flow_toggle.add_row( text=dai_str )
        self.flow_toggle.toggle()
        
        # Invariants
        for inv in self.mode.invs:
            self.inv_toggle.add_row( text=inv.raw )
        self.inv_toggle.toggle()


    def _confirm( self ):
        """ Commit changes to Session. Does NOT save changes """

        # Name
        self.mode.name = self.name.get()
        
        # ID
        self.mode.id = self.mode_id.get()
        
        # Initial
        self.mode.initial = self.initial.get()

        # Flows
        self.mode.clear_dai()
        for raw_text in self.flow_toggle.rows:
            if( (raw_text.get()).strip() ): 
                self.mode.add_dai( DAI( raw_text.get() ) )

        # Invariants
        self.mode.clear_inv()
        for raw_text in self.inv_toggle.rows:
            if( (raw_text.get()).strip() ): 
                self.mode.add_inv( Invariant( raw_text.get() ) )
        
        print( 'Mode Entry Confirmed\n' )
        self.destroy()


    def _cancel( self ):
        """ Cancels changes made in popup """
        print( 'Mode Entry Canceled\n' )
        self.destroy()


class TransitionEntry( PopupEntry ):

    def __init__( self, parent ):

        PopupEntry.__init__( self, parent )
        self.title_label.config( text='Transition' )

        # TODO: Temporary - for testing/design
        self.mode_list = [ 'Mode 1', 'Mode 2', 'Mode 3' ]

        self.transition_var = StringVar()
        Label( self, textvariable=self.transition_var ).grid( row=1, column=0, columnspan=2 )


        # Source and Destination

        self.source_var = StringVar()
        self.source_var.set( self.mode_list[0] )
        self.destination_var = StringVar()
        self.destination_var.set( self.mode_list[1] )
        self._callback_mode_select # Initialize transition label (self.transition_var)

        Label( self, text='Source:' ).grid( row=2, column=0, sticky=W )
        Label( self, text='Destination: ' ).grid( row=3, column=0, sticky=W )
        
        self.source_var.trace_variable( 'w', self._callback_mode_select )
        self.destination_var.trace_variable( 'w', self._callback_mode_select )

        OptionMenu( self, self.source_var, self.mode_list[0], *self.mode_list ).grid( row=2, column=1, sticky=W+E )
        OptionMenu( self, self.destination_var, self.mode_list[1], *self.mode_list ).grid( row=3, column=1, sticky=W+E )
       

        # Guards
        
        Label( self, text='Guards: ' ).grid( row=4, column=0, sticky=W )
       
        self.guards = Text( self, height=self.TEXTBOX_HEIGHT, width=self.TEXTBOX_WIDTH )
        self.guards.grid( row=5, column=0, columnspan=2, sticky=N+S+E+W )

        
        # Actions 
        
        Label( self, text='Actions: ' ).grid( row=6, column=0, sticky=W )

        self.actions = Text( self, height=self.TEXTBOX_HEIGHT, width=self.TEXTBOX_WIDTH )
        self.actions.grid( row=7, column=0, columnspan=2, sticky=N+S+E+W )


        # Confirm / Cancel buttons

        self.confirm_button.grid( row=8, column=0, sticky=E+W )
        self.cancel_button.grid( row=8, column=1, sticky=E+W )


    def _callback_mode_select( self, *args ):

        self.transition_var.set( self.source_var.get() + " -> " + self.destination_var.get() )

        print( 'File: ', Session.file_path )
        print( 'Source: ', self.source_var.get() )
        print( 'Destination: ', self.destination_var.get() )
        print()

    def _callback_guards( self, *args ):
        
        print( 'Callback Test: Guards\n' )

    def _callback_actions( self, *args ):

        print( 'Callback Test: Actions\n' )