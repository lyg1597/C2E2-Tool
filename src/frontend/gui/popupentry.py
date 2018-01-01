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
        
        hybrid = Session.hybrid

        for var in hybrid.vars:
            if( var.scope != 'LOCAL_DATA' ): continue
            self._add_row()
            self.names[self.var_index-1].set( var.name )
            self.types[self.var_index-1].set( var.type )
            self.thins[self.var_index-1].set( False )

        for var in hybrid.thinvars:
            if( var.scope != 'LOCAL_DATA' ): continue
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
        hybrid.reset_local_vars()
        hybrid.reset_local_thin_vars()

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
        """ Load selected mode's Session values """

        # Name
        self.name.set( self.mode.name )

        # ID
        self.mode_id.set( self.mode.id )

        # Initial
        self.initial.set( self.mode.initial )

        # Flows
        for dai in self.mode.dais:
            # LMB: Commenting out conditional from popup to avoid 'losing' equations when dialog is confirmed. May need to go back and add it after talking with leadership. TODO
            #if '_dot' in dai.raw:
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

    def __init__( self, parent, trans, mode_dict ):
        PopupEntry.__init__( self, parent )
        self.title_label.config( text='Transition' )

        self.trans = trans
        self.mode_dict = mode_dict  # mode_dict[mode.id] = mode.name

        # Load Mode list for Source/Destination Option Menus
        self.mode_list = []
        for mode_id in mode_dict:
            self.mode_list.append( mode_dict[mode_id] )

        self._init_widgets()
        self._load_session()


    def _init_widgets( self ):
        """ Initialize GUI elements """

        # Transition Label
        self.trans_str = StringVar()
        Label( self, textvariable=self.trans_str ).grid( row=1, column=0, columnspan=2 )

        # ID
        Label( self, text='ID:' ).grid( row=2, column=0, sticky=W )
        self.trans_id = IntVar()
        Entry( self, textvariable=self.trans_id ).grid( row=2, column=1, sticky=E )

        # Source and Destination
        
        Label( self, text='Source:' ).grid( row=3, column=0, sticky=W )
        Label( self, text='Destination:' ).grid( row=4, column=0, sticky=W )

        self.src_str = StringVar()
        self.dest_str = StringVar()

        self.src_str.trace_variable( 'w', self._callback_mode_select )
        self.dest_str.trace_variable( 'w', self._callback_mode_select )

        # Aribtrarily set default source/destination.
        # These are overwritten to be correct in load_session
        OptionMenu( self, self.src_str, self.mode_list[0], *self.mode_list )\
            .grid( row=3, column=1, sticky=W+E )        
        OptionMenu( self, self.dest_str, self.mode_list[1], *self.mode_list )\
            .grid( row=4, column=1, sticky=W+E )

        # Guards
        Label( self, text='Guards:' ).grid( row=5, column=0, sticky=W )
        self.guard_str = StringVar()
        Entry( self, textvariable=self.guard_str ).grid( row=5, column=1, sticky=E )

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


    def _callback_mode_select( self, *args ):
        """ OptionMenu callback, updates transition label at top of window """
        self.trans_str.set( self.src_str.get() + " -> " + self.dest_str.get() )


    def _confirm( self ):
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
                
        print( 'Transition Entry Confirmed' )
        self.destroy()


    def _cancel( self ):
        """ Cancels changes made in popup """
        print( 'Transition Entry Canceled\n' )
        self.destroy()