from tkinter import *
from tkinter.ttk import *
from frontend.gui.eventhandler import EventHandler
from frontend.gui.modeltab import PropertyEditor
from frontend.mod.constants import *
from frontend.mod.filehandler import FileHandler
from frontend.mod.session import Session, Property


class EditorTab( Frame ):

    def __init__( self, parent, menu_bar ):
        Frame.__init__( self, parent )
        
        self.parent = parent
        self.menu_bar = menu_bar

        self._init_widgets()
        self._bind_events()

    def _init_widgets( self ):

        print( "Initialize the XML Editor and Editor Sidebar" )

        # Main Editor
        self.editor = Text( self )
        self.editor.pack( expand=TRUE, fill=BOTH, side=LEFT, anchor=E )

        # Sidebar
        self._init_sidebar()
        self.sidebar.pack( expand=TRUE, fill=Y, side=TOP, anchor=E )
      
    
    def _bind_events( self ):

        #self.bind(CLOSE_EVENT, self._clear_model)
        #EventHandler.add_event_listeners(self, CLOSE_EVENT)
    
        self.bind( OPEN_EVENT, self._open_xml )
        EventHandler.add_event_listeners( self, OPEN_EVENT )


    def _init_sidebar( self ):

        # Parent Frame
        self.sidebar = Frame( self )

        # XML Buttons

        self.xml_buttons = LabelFrame( self.sidebar, text='XML' )

        self.save_xml_btn = Button( self.xml_buttons, text='Save XML', command=self._save_xml )\
            .pack( expand=TRUE, fill=X, side=LEFT )

        self.reload_xml_btn = Button( self.xml_buttons, text='Reload XML', command=self._reload_xml )\
            .pack( expand=TRUE, fill=X, side=LEFT )
        
        self.xml_buttons.pack( fill=X )
        
        # Property List
        
        self.property_list = PropertyList( self.sidebar )
        self.property_list.pack( expand=TRUE, fill=BOTH )


    def _open_xml( self, event=None ):

        self.editor.delete( '1.0', END )
        
        if( not Session.file_path ):
            return

        print( 'Opening xml...' )
        with open( Session.file_path, 'r' ) as f:
            self.editor.insert( INSERT, f.read() )
        print( 'Success!' )

        print( 'Loading properties...' )
        self.property_list._clear_properties()
        self.property_list._display_properties()
        print( 'Success!' )
        
    
    def _save_xml( self, event=None ):
        
        if( not Session.file_path ):
            print( 'No session filepath\n' )
            return
        print( 'Saving xml...' )
        
        text = self.editor.get( '1.0', 'end-1c') 
        with open( Session.file_path, 'w' ) as f:
            f.write( text )
        
        print( 'Saved as:' + Session.file_path + '\n' )

        self._reload_xml()


    def _reload_xml( self, event=None ):

        if( not Session.file_path ):
            print( 'No session filepath\n' )
            return
        print( 'Reloading xml...' )

        file_path = Session.file_path
        file = FileHandler.open_file( file_path )
        if( HYXML_FILE in file_path ): 
            print( '.hyxml file reloaded\n' )
            Session.file_type = HYXML_FILE
        elif( MDL_FILE in file_path ):
            print ( '.mdl file opened\n' )
            Session.file_type = MDL_FILE

        if file == None:
            print( 'File reload failed: No file\n' )
            return

        # Obtain parsed results
        Session.hybrid = file['hybrid']
        Session.prop_list = file['prop_list']
        if( len(Session.prop_list) == 0 ):
            Session.prop_list.append( Property() )
        Session.file_opened = True

        print( 'File reloaded!' )
        self._open_xml()


class PropertyList( PropertyEditor ):

    def __init__( self, parent, **options ):
        PropertyEditor.__init__( self, parent, **options )
        

    def _init_widgets( self ):
        """ Override to remove prop_view """

        self._init_prop_list()
        self.prop_list.pack( expand=TRUE, fill=BOTH )


    def _init_prop_list( self ):
        """ Override to replace New, Copy, and Remove buttons with Simulator and Refinement dropdowns """

        # Property List Frame
        self.prop_list = LabelFrame( self, text='Properties' )

        # Property list
        self.list_view = Treeview( self.prop_list )
        self.list_view.pack( fill=BOTH, expand=TRUE)
        self.list_view.bind( '<Button-1>', self._callback_btn_press )
        self.list_view.bind( '<Double-Button-1>', self._callback_btn_press_double )

        self.list_view['show'] = 'headings'
        self.list_view['columns'] = ( 'name', 'status', 'result' )
        self.list_view.column( 'name', width=100 )
        self.list_view.column( 'status', width=100 )
        self.list_view.column( 'result', width=100 )
        self.list_view.heading( 'name', text='Property' )
        self.list_view.heading( 'status', text='Status' )
        self.list_view.heading( 'result', text='Result' )

        
        # Simulator and Refine Type

        simref_row = Frame( self.prop_list )
        simref_row.pack( fill=X )

        sim_opts = ( ODEINT_FIX, ODEINT_ADP, CAPD )
        Label( simref_row, text='Simulator:' ).grid( row=0, sticky=W )
        self.sim_var = StringVar()
        self.sim_var.set( sim_opts[0] )
        self.sim_var.trace_variable( 'w', self._callback_simulator )
        
        OptionMenu( simref_row, self.sim_var, '', *sim_opts )\
            .grid( row=0, column=1, columnspan=3, sticky=W+E )


        ref_opts = ( DEF_STRAT, USR_STRAT )
        Label( simref_row, text='Refinement:' ).grid( row=1, sticky=W )
        self.ref_var = StringVar()
        self.ref_var.set( ref_opts[0] )
        self.ref_var.trace_variable( 'w', self._callback_refine_strat )

        OptionMenu( simref_row, self.ref_var, '', *ref_opts )\
            .grid( row=1, column=1, columnspan=3, sticky=W+E )


        # Simulate and Verify Buttons

        simver_row = Frame( self.prop_list )
        simver_row.pack( fill=X )

        self.sim_btn = Button( simver_row, text='Simulate', command=self._callback_sim )
        self.sim_btn.pack( expand=TRUE, fill=X, side=LEFT )

        self.ver_btn = Button( simver_row, text='Verify', command=self._callback_ver )
        self.ver_btn.pack( expand=TRUE, fill=X, side=LEFT )


    def _display_property( self, prop ):
        """ Override to remove variables associated with prop_view """

        self.sim_var.set( Session.simulator )
        self.ref_var.set( Session.refine_strat )


    def _disable_enable_button( self, disable ):
        """ Override to remove missing buttons """

        if disable:
            self.sim_btn['state'] = 'disabled'
            self.ver_btn['state'] = 'disabled'
        else:
            self.sim_btn['state'] = 'normal'
            self.ver_btn['state'] = 'normal'

        self.update()
