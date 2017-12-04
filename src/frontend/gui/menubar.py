from tkinter import *
from tkinter import filedialog
from tkinter.ttk import *
import xml.dom.minidom

from frontend.gui.eventhandler import EventHandler
from frontend.mod.filehandler import FileHandler
from frontend.mod.constants import *
from frontend.mod.session import *

class MenuBar(Menu):
    def __init__(self, parent):
        Menu.__init__(self, parent)
        self.parent = parent

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
        # File menu
        file_menu = Menu(self, tearoff=0)
        file_menu.add_command(label='Open', accelerator='Crtl+O', underline=0,
                command=self.open_callback)
        file_menu.add_command(label='Save', accelerator='Ctrl+S', underline=0, 
                command=self.save_callback)
        file_menu.add_command(label='Save As', accelerator='Ctrl+Shift+S', 
                underline=5, command=self.save_as_callback)
        file_menu.add_command(label='Close', accelerator='Ctrl+L', underline=1, 
                command=self.close_callback)
        file_menu.add_command(label='Quit', accelerator='Ctrl+Q', underline=0, 
                command=self.quit_callback)
        self.add_cascade(label='File', menu=file_menu)

        # Help menu
        help_menu = Menu(self, tearoff=0)
        help_menu.add_command(label='About')
        self.add_cascade(label='Help', menu=help_menu)
        self.parent.config(menu=self)

        # Bind accelerators
        self.parent.bind_all('<Control-o>', lambda event: self.open_callback())
        self.parent.bind_all('<Control-s>', lambda event: self.save_callback())
        self.parent.bind_all('<Control-Shift-s>', lambda event: self.save_as_callback())
        self.parent.bind_all('<Control-l>', lambda event: self.close_callback())
        self.parent.bind_all('<Control-q>', lambda event: self.quit_callback())

    def open_callback(self):
        # Open selected file
        self.parent.label.pack_forget() 
        self.parent.manual.pack_forget()
        self.parent.email.pack_forget()
        if Session.file_opened:
            self.close_callback()
        file_path = filedialog.askopenfilename(**self.OPEN_OPT)
        if file_path:
            Session.file_path = file_path
            file = FileHandler.open_file(file_path)
            if HYXML_FILE in file_path:
                print ("hyxml file opened")
                Session.file_type = HYXML_FILE
            elif MDL_FILE in file_path:
                print ("mdl file opened")
                Session.file_type = MDL_FILE

            if file == None:
                return

            # Obtain parsed results
            Session.hybrid = file['hybrid']
            Session.prop_list = file['prop_list']
            if len(Session.prop_list) == 0:
                Session.prop_list.append(Property())
            Session.file_opened = True
            EventHandler.event_generate(OPEN_EVENT)

    def save_callback(self):
        if not Session.file_opened:
            return 

        if Session.file_type == MDL_FILE and not Session.file_saved:
            self.save_as_callback()
            return
            
        self.save_model(Session.file_path)

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
        savedModelString = FileHandler.save_model(Session.hybrid, Session.prop_list, filepath)


    # FIXME destroy the session
    def close_callback(self, event=None):

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


