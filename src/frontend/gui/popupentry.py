from tkinter import *
from tkinter.ttk import *
from frontend.mod.session import Session


class PopupEntry( object ):

    def __init__( self, parent, item_id ):

        self.popup = Toplevel( parent )
        self.popup.resizable( width=False, height=False )

        self.title_label = Label( self.popup, text='C2E2' )
        self.title_label.grid( row=0, column=0, columnspan=2 )
        
        self.confirm_button = Button( self.popup, text='Confirm', command=self.confirm )
        self.confirm_button.grid( row=2, column=1 )

        self.cancel_button = Button( self.popup, text='Cancel', command=self.cancel )
        self.cancel_button.grid( row=2, column=0 )

        self.TEXTBOX_HEIGHT = 10
        self.TEXTBOX_WIDTH = 30


    def confirm( self ):

        print( 'Saved!\n' )
        self.popup.destroy()


    def cancel( self ):
        
        print( 'Entry Canceled\n' )
        self.popup.destroy()


class VariableEntry( PopupEntry ):

    def __init__( self, parent, item_id ):
        PopupEntry.__init__( self, parent, item_id )

        self.title_label.config( text='Edit Variables' )

        Label( self.popup, text='Variables: ' ).grid( row=2, column=0, sticky=W )

        self.variables_var = StringVar()
        self.variables_var.trace_variable( 'w', self._callback_variables )
        Entry( self.popup, textvariable=self.variables_var ).grid( row=3, column=0, columnspan=2, sticky=W+E )

        self.confirm_button.grid( row=4, column=0, sticky=E+W )
        self.cancel_button.grid( row=4, column=1, sticky=E+W )

    def _callback_variables( self, *args ):

        print( 'Callback Test\n' )


class ModeEntry( PopupEntry ):

    def __init__( self, parent, item_id ):

        PopupEntry.__init__( self, parent, item_id )
        self.title_label.config( text='Mode' )  # row=0, column=0, columnspan=2


        # Name

        Label( self.popup, text='Name:' ).grid( row=1, column=0, sticky=W )
        self.name = StringVar()
        self.name.trace_variable( 'w', self._callback_name )
        Entry( self.popup, textvariable=self.name ).grid( row=1, column=1, sticky=E )


        # ID
        
        Label( self.popup, text='ID:' ).grid( row=2, column=0, sticky=W )
        self.mode_id = StringVar()
        self.mode_id.trace_variable( 'w', self._callback_mode_id )
        Entry( self.popup, textvariable=self.mode_id ).grid( row=2, column=1, sticky=E )


        # Flows

        Label( self.popup, text='Flows: ' ).grid( row=3, column=0, sticky=W )       
        self.flows = Text( self.popup, height=self.TEXTBOX_HEIGHT, width=self.TEXTBOX_WIDTH )
        self.flows.grid( row=4, column=0, columnspan=2, sticky=N+S+E+W )


        # Invariants

        Label( self.popup, text='Invariants: ' ).grid( row=5, column=0, sticky=W )
        self.invariants = Text( self.popup, height=self.TEXTBOX_HEIGHT, width=self.TEXTBOX_WIDTH )
        self.invariants.grid( row=6, column=0, columnspan=2, sticky=N+S+E+W )

        self.confirm_button.grid( row=7, column=0, sticky=E+W )
        self.cancel_button.grid( row=7, column=1, sticky=E+W )


    def _load_current_values( self ):
    


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

    def __init__( self, parent, item_id ):

        PopupEntry.__init__( self, parent, item_id )
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