from tkinter import *
from tkinter import filedialog
from tkinter.ttk import *
import xml.dom.minidom

from frontend.gui.eventhandler import EventHandler
from frontend.gui.modeltab import *
from frontend.mod.filehandler import FileHandler
from frontend.mod.constants import *
from frontend.mod.session import *


class MenuBar(Menu):

    def __init__(self, parent, notebook):
        Menu.__init__(self, parent)
    
        self.parent = parent
        self.notebook = notebook
        self.tree = notebook.model_tab.tree
        self.model_sidebar = notebook.model_tab.sidebar
        self.plot_sidebar = notebook.plot_tab.sidebar

        # Open file constants
        self.OPEN_OPT = {
            'defaultextension': ['.hyxml','.mdl'],
            'filetypes': [('HyXML files', '.hyxml'), 
                ('Simulink files', '.mdl')],
            'multiple': 0,
            'parent': self.parent,
            'title': 'Open File',
        }

        """
        self.SAVE_OPT = {
            'defaultextension': '.hyxml',
            'filetypes': [('HyXML files', '.hyxml')],
            'parent': self.parent,
            'title': 'Save File',
        }
        """

        self._init_widgets()
        

    def _init_widgets(self):
        """ Initialize menus """

        # File menu

        file_menu = Menu(self, tearoff=0)
        file_menu.add_command(label='Open', accelerator='Crtl+O', underline=0,
            command=self.open_callback)
        file_menu.add_command(label='New', accelerator='Ctrl+N', underline=0, 
            command=self.new_callback)
        file_menu.add_command(label='Save', accelerator='Ctrl+S', underline=0, 
            command=self.save_callback)
        file_menu.add_command(label='Save As', accelerator='Ctrl+Shift+S', 
            underline=5, command=self.save_as_callback)
        file_menu.add_command(label='Close', accelerator='Ctrl+L', underline=1, 
            command=self.close_callback)
        file_menu.add_command(label='Quit', accelerator='Ctrl+Q', underline=0, 
            command=self.quit_callback)
        self.add_cascade(label='File', menu=file_menu)

        # Model menu

        model_menu = Menu(self, tearoff=0)

        #   Hybrid Submenu

        hybrid_submenu = Menu(self, tearoff=0)
        hybrid_submenu.add_command(label="Parse",
            command=self.model_sidebar._callback_parse)
        hybrid_submenu.add_command(label="Compose",
            command=self.model_sidebar._callback_compose)
        hybrid_submenu.add_command(label="Verify",
            command=self.model_sidebar._callback_ver)
        hybrid_submenu.add_command(label="Simulate",
            command=self.model_sidebar._callback_sim)
        model_menu.add_cascade(label="Hybrid", menu=hybrid_submenu)

        #   Edit Submenu

        edit_submenu = Menu(self, tearoff=0, postcommand=self._update_edit_menu)
        
        self.variables_submenu = Menu(self, tearoff=0)
        self.variables_submenu.add_command(label='Edit Variables', 
            command=lambda: self.tree.launch_entry_popup(VARIABLES, EDIT))
        
        self.modes_submenu = Menu(self, tearoff=0)
        self.modes_submenu.add_command(label='Add Mode', 
            command=lambda: self.tree.launch_entry_popup(MODES, ADD))
        self.modes_submenu.add_command(label='Edit Mode', 
            command=lambda: self.tree.launch_entry_popup(MODES, EDIT))
        self.modes_submenu.add_command(label='Delete Mode', 
            command=lambda: self.tree.launch_entry_popup(MODES, DELETE))
        
        self.transitions_submenu = Menu(self, tearoff=0)
        self.transitions_submenu.add_command(label='Add Transition', 
            command=lambda: self.tree.launch_entry_popup(TRANSITIONS, ADD))
        self.transitions_submenu.add_command(label='Edit Transition', 
            command=lambda: self.tree.launch_entry_popup(TRANSITIONS, EDIT))
        self.transitions_submenu.add_command(label='Delete Transition', 
            command=lambda: self.tree.launch_entry_popup(TRANSITIONS, DELETE))
        
        edit_submenu.add_cascade(label="Variables",menu=self.variables_submenu)
        edit_submenu.add_cascade(label="Modes", menu=self.modes_submenu)
        edit_submenu.add_cascade(label="Transitions", 
            menu=self.transitions_submenu)

        model_menu.add_cascade(label="Edit", menu=edit_submenu)

        #   Properties Submenu

        model_prop_submenu = Menu(self, tearoff=0)
        model_prop_submenu.add_command(label="New",
            command=self.model_sidebar._callback_new)
        model_prop_submenu.add_command(label="Copy",
            command=self.model_sidebar._callback_cpy)
        model_prop_submenu.add_command(label="Remove",
            command=self.model_sidebar._callback_rmv)
        model_menu.add_cascade(label="Properties", menu=model_prop_submenu)

        self.add_cascade(label="Model", menu=model_menu)

        # Plot menu

        plot_menu = Menu(self, tearoff=0)
        
        plot_menu.add_command(label="Plot",
            command=self.plot_sidebar._callback_plot)
        
        plot_prop_submenu = Menu(self, tearoff=0)
        plot_prop_submenu.add_command(label="New",
            command=self.plot_sidebar._callback_new)
        plot_prop_submenu.add_command(label="Copy",
            command=self.plot_sidebar._callback_copy)
        plot_prop_submenu.add_command(label="Remove",
            command=self.plot_sidebar._callback_remove)
        plot_menu.add_cascade(label="Properties", menu=plot_prop_submenu)

        self.add_cascade(label="Plotter", menu=plot_menu)

        # Help menu

        help_menu = Menu(self, tearoff=0)
        help_menu.add_command(label='About')
        self.add_cascade(label='Help', menu=help_menu)
        self.parent.config(menu=self)

        # Bind accelerators
        
        self.parent.bind_all('<Control-o>', lambda event: self.open_callback())
        self.parent.bind_all('<Control-n>', lambda event: self.new_callback())
        self.parent.bind_all('<Control-s>', lambda event: self.save_callback())
        self.parent.bind_all('<Control-Shift-s>', 
            lambda event: self.save_as_callback())
        self.parent.bind_all('<Control-l>', 
            lambda event: self.close_callback())
        self.parent.bind_all('<Control-q>', lambda event: self.quit_callback())


    def _update_edit_menu(self):
        """ Disable menu options based on item selected in TreeView """

        self.variables_submenu.entryconfig(0, state=DISABLED)
        for i in range(3):
            self.modes_submenu.entryconfig(i, state=DISABLED)
            self.transitions_submenu.entryconfig(i, state=DISABLED)

        if self.notebook.current_tab == EDITOR:
            return  # Edit menu options not valid in Editor tab

        if (not Session.file_opened) or (self.tree.slct_automaton is None):
            return
        else:
            self.variables_submenu.entryconfig(0, state=NORMAL)
            self.modes_submenu.entryconfig(0, state=NORMAL)
            self.transitions_submenu.entryconfig(0, state=NORMAL)
       
        if self.tree.slct_mode is not None:       
            self.modes_submenu.entryconfig(1, state=NORMAL)
            self.modes_submenu.entryconfig(2, state=NORMAL)
        elif self.tree.slct_transition is not None:
            self.transitions_submenu.entryconfig(1, state=NORMAL)
            self.transitions_submenu.entryconfig(2, state=NORMAL)

        return           

    def open_callback(self):
        """ Select and open file """

        # Forget welcome screen widgets
        self.parent.label.pack_forget() 
        self.parent.manual.pack_forget()
        self.parent.email.pack_forget()

        # If a file is already open, close it.
        if Session.file_opened:
            self.close_callback()
       
        file_path = filedialog.askopenfilename(**self.OPEN_OPT)
        if file_path:            
            Session.file_path = file_path
            status = FileHandler.open_file(file_path)
            if status:
                Session.file_opened = True
                EventHandler.event_generate(OPEN_EVENT)
                
        return

    def new_callback(self):
        """ Open template file """

        # Forget welcome screen widgets
        self.parent.label.pack_forget()
        self.parent.manual.pack_forget()
        self.parent.email.pack_forget()

        # If a file is already open, close it.
        if Session.file_opened:
            self.close_callback()

        Session.hybrid = HyIR.create_template()
        Session.file_path = None
        Session.file_opened = True
        Session.file_saved = False
        EventHandler.event_generate(OPEN_EVENT)

        return

    def save_callback(self):

        if self.notebook.current_tab == EDITOR:
            FileHandler.save(self.notebook.editor_tab.editor.get())
            # Load XML into HyIR object
            FileHandler.open_file(Session.file_path)
            # Refresh Model Tab
            self.tree._clear_model()
            self.tree._display_model()
            # Refresh Property Sidebar
            self.notebook.model_tab.sidebar._display_property(Session.cur_prop)
        else:
            FileHandler.save()
            # Refresh Editor Tab
            self.editor_tab.open_xml()
        
        return

    def save_as_callback(self, event=None):

        if self.notebook.current_tab == EDITOR:
            FileHandler.save_as(self.notebook.editor_tab.editor.get())
        else:
            FileHandler.save_as()
        
        return 

    """
    def save_model(self, filepath):
        savedModelString = FileHandler.save_model(Session.hybrid, 
                                                  Session.hybrid.properties, 
                                                  filepath)
        return
    """

    # FIXME destroy the session
    def close_callback(self, event=None):

        if not Session.file_saved:
            if messagebox.askyesno('Save', "Save file before closing?"):
                self.save_callback()

        Session.lib_compiled = False
        Session.simulator = ODEINT_FIX
        Session.refine_strat = DEF_STRAT
        Session.file_saved = False
        Session.file_type = ''
        Session.file_path = ''

        if Session.file_opened:
            EventHandler.event_generate(CLOSE_EVENT)
        Session.file_opened = False


    def quit_callback(self, event=None):

        self.parent.destroy()
