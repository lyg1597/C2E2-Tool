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
        self.tree = notebook.model_tab.tree # Menu interacts with TreeView

        # Open file constants
        self.OPEN_OPT = {
            'defaultextension': ['.hyxml','.mdl'],
            'filetypes': [('HyXML files', '.hyxml'), ('Simulink files', '.mdl')],
            'multiple': 0,
            'parent': self.parent,
            'title': 'Open File',
        }

        self.SAVE_OPT = {
            'defaultextension': '.hyxml',
            'filetypes': [('HyXML files', '.hyxml')],
            'parent': self.parent,
            'title': 'Save File',
        }

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

        # Edit menu 

        edit_menu = Menu(self, tearoff=0, postcommand=self._update_edit_menu)

        self.variables_submenu = Menu(self, tearoff=0)
        self.variables_submenu.add_command(label='Edit Variables', command=lambda: self.tree.launch_entry_popup(VARIABLES, EDIT))

        self.modes_submenu = Menu(self, tearoff=0)
        self.modes_submenu.add_command(label='Add Mode', command=lambda: self.tree.launch_entry_popup(MODES, ADD))
        self.modes_submenu.add_command(label='Edit Mode', command=lambda: 
        self.tree.launch_entry_popup(MODES, EDIT))
        self.modes_submenu.add_command(label='Delete Mode', command=lambda: self.tree.launch_entry_popup(MODES, DELETE))
        
        self.transitions_submenu = Menu(self, tearoff=0)
        self.transitions_submenu.add_command(label='Add Transition', command=lambda: self.tree.launch_entry_popup(TRANSITIONS, ADD))
        self.transitions_submenu.add_command(label='Edit Transition', command=lambda: 
        self.tree.launch_entry_popup(TRANSITIONS, EDIT))
        self.transitions_submenu.add_command(label='Delete Transition', command=lambda: self.tree.launch_entry_popup(TRANSITIONS, DELETE))
 
        edit_menu.add_cascade(label='Variables', menu=self.variables_submenu)
        edit_menu.add_cascade(label='Modes', menu=self.modes_submenu)
        edit_menu.add_cascade(label='Transitions', menu=self.transitions_submenu)

        self.add_cascade(label='Edit', menu=edit_menu)

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

        if not Session.file_opened:
            return

        if Session.file_path is None:
            self.save_as_callback()
            return

        if Session.file_type == MDL_FILE and not Session.file_saved:
            self.save_as_callback()
            return
            
        self.save_model(Session.file_path)
        Session.file_saved = True

        return

    def save_as_callback(self, event=None):

        if not Session.file_opened:
            return 

        file_path = filedialog.asksaveasfile(**self.SAVE_OPT)
        if file_path:
            self.save_model(file_path.name)
            if Session.file_type == MDL_FILE and not Session.file_saved:
                Session.file_path = file_path.name
            Session.file_saved = True
        
        return 

    def save_model(self, filepath):
        savedModelString = FileHandler.save_model(Session.hybrid, 
                                                  Session.hybrid.properties, 
                                                  filepath)
        return

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
