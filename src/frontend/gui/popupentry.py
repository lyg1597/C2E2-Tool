from tkinter import *
from tkinter.ttk import *
from frontend.mod.constants import *
from frontend.mod.session import Session


class PopupEntry( object ):

    def __init__( self, parent ):

        self.popup = Toplevel( parent )
        self.popup.resizable( width=False, height=False )

        self.title_label = Label( self.popup, text='C2E2' )
        self.title_label.grid( row=0, column=0, columnspan=2 )
        
        self.TEXTBOX_HEIGHT = 10
        self.TEXTBOX_WIDTH = 30


class VariableEntry( PopupEntry ):

    def __init__( self, parent ):
        PopupEntry.__init__( self, parent )
        self.title_label.config( text='Variables' )

        self._init_widgets()
        self._load_session_vars()


    def _init_widgets( self ):

        self.title_label.grid( row=0, column=0, columnspan=3 )

        Label( self.popup, text='Name' ).grid( row=1, column=0 )
        Label( self.popup, text='Type' ).grid( row=1, column=1 )
        Label( self.popup, text='Thin' ).grid( row=1, column=2 )

        # Variable lists for uknown number of inputs
        self.names = []  # StringVar()
        self.types = []  # StringVar()
        self.thins = []  # BoolVar()
        self.var_index = 0

        # Buttons
        self.btn_frame = Frame( self.popup )

        self.add_btn = Button( self.btn_frame, text='Add', command=self._add_row )
        self.confirm_btn = Button( self.btn_frame, text='Confirm', command=self._confirm )
        self.cancel_btn = Button( self.btn_frame, text='Cancel', command=self._cancel )

        self.add_btn.grid( row=0, column=0 )
        self.confirm_btn.grid( row=0, column=1 )
        self.cancel_btn.grid( row=0, column=2 )

    
    def _load_session_vars( self ):
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
        Entry( self.popup, textvariable=self.names[self.var_index] )\
            .grid( row=self.var_index+2, column=0 )

        # Type
        OptionMenu( self.popup, self.types[self.var_index], *VARIABLE_TYPES )\
            .grid( row=self.var_index+2, column=1 )

        # Thin
        Checkbutton( self.popup, var=self.thins[self.var_index] )\
            .grid( row=self.var_index+2, column=2 )

        self.btn_frame.grid( row=self.var_index+3, columnspan=3 )

        self.var_index += 1


    def _confirm( self ):

        print( 'Variable Entry, Confirm Button Callback\n' )
        self.popup.destroy()

    def _cancel( self ):

        print( 'Variable Entry, Cancel Button Callback\n' )
        self.popup.destroy()


class ModeEntry( PopupEntry ):

    def __init__( self, parent, mode ):

        PopupEntry.__init__( self, parent )
        self.title_label.config( text='Mode' )  # row=0, column=0, columnspan=2


        # Name

        Label( self.popup, text='Name:' ).grid( row=1, column=0, sticky=W )
        self.name = StringVar()
        self.name.set( mode.name )
        #self.name.trace_variable( 'w', self._callback_name )
        Entry( self.popup, textvariable=self.name ).grid( row=1, column=1, sticky=E )


        # ID
        
        Label( self.popup, text='ID:' ).grid( row=2, column=0, sticky=W )
        self.mode_id = StringVar()
        self.mode_id.set( mode.id )
        #self.mode_id.trace_variable( 'w', self._callback_mode_id )
        Entry( self.popup, textvariable=self.mode_id ).grid( row=2, column=1, sticky=E )


        # Flows

        Label( self.popup, text='Flows: ' ).grid( row=3, column=0, sticky=W )       
        self.flows = Text( self.popup, height=self.TEXTBOX_HEIGHT, width=self.TEXTBOX_WIDTH )
        self.flows.grid( row=4, column=0, columnspan=2, sticky=N+S+E+W )

        for dai in mode.dais:
            if '_dot' in dai.raw:
                dai_str = dai.raw[3:-1]
                idx = dai_str.index(',')
                dai_str = dai_str[:idx]+'='+dai_str[idx+1:]
                self.flows.insert( INSERT, dai_str + '\n' )

        # Invariants

        Label( self.popup, text='Invariants: ' ).grid( row=5, column=0, sticky=W )
        self.invariants = Text( self.popup, height=self.TEXTBOX_HEIGHT, width=self.TEXTBOX_WIDTH )
        self.invariants.grid( row=6, column=0, columnspan=2, sticky=N+S+E+W )

        #for inv in mode.invs:
         #   self.invariants.insert( inv_i

        # Buttons

        self.confirm_button.grid( row=7, column=0, sticky=E+W )
        self.cancel_button.grid( row=7, column=1, sticky=E+W )


    def _load_current_values( self ):

        # Find current mode
        hybrid = Session.hybrid

    def _callback_flows( self, input ):
        #TODO
        print( 'Callback Flows\n' )

    def _callback_name( self, input ):
        #TODO
        print( 'Callback Name\n' )

    def _callback_mode_id( self, input ):
        #TODO
        print( 'Callback Mode ID\n' )


class TransitionEntry( PopupEntry ):

    def __init__( self, parent ):

        PopupEntry.__init__( self, parent )
        self.title_label.config( text='Transition' )

        # TODO: Temporary - for testing/design
        self.mode_list = [ 'Mode 1', 'Mode 2', 'Mode 3' ]

        self.transition_var = StringVar()
        Label( self.popup, textvariable=self.transition_var ).grid( row=1, column=0, columnspan=2 )


        # Source and Destination

        self.source_var = StringVar()
        self.source_var.set( self.mode_list[0] )
        self.destination_var = StringVar()
        self.destination_var.set( self.mode_list[1] )
        self._callback_mode_select # Initialize transition label (self.transition_var)

        Label( self.popup, text='Source:' ).grid( row=2, column=0, sticky=W )
        Label( self.popup, text='Destination: ' ).grid( row=3, column=0, sticky=W )
        
        self.source_var.trace_variable( 'w', self._callback_mode_select )
        self.destination_var.trace_variable( 'w', self._callback_mode_select )

        OptionMenu( self.popup, self.source_var, self.mode_list[0], *self.mode_list ).grid( row=2, column=1, sticky=W+E )
        OptionMenu( self.popup, self.destination_var, self.mode_list[1], *self.mode_list ).grid( row=3, column=1, sticky=W+E )
       

        # Guards
        
        Label( self.popup, text='Guards: ' ).grid( row=4, column=0, sticky=W )
       
        self.guards = Text( self.popup, height=self.TEXTBOX_HEIGHT, width=self.TEXTBOX_WIDTH )
        self.guards.grid( row=5, column=0, columnspan=2, sticky=N+S+E+W )

        
        # Actions 
        
        Label( self.popup, text='Actions: ' ).grid( row=6, column=0, sticky=W )

        self.actions = Text( self.popup, height=self.TEXTBOX_HEIGHT, width=self.TEXTBOX_WIDTH )
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