from tkinter import *
from tkinter.ttk import *
from tkinter.scrolledtext import *

from frontend.gui.eventhandler import EventHandler
from frontend.gui.modeltab import ModelSidebar
from frontend.gui.widgets import SetText
from frontend.mod.constants import *
from frontend.mod.filehandler import FileHandler
from frontend.mod.session import Session, Property


class EditorTab(Frame):

    def __init__(self, parent):
        Frame.__init__(self, parent)
        
        self.parent = parent

        self._init_widgets()
        self._bind_events()

    def _init_widgets(self):

        print("Initialize the XML Editor and Editor Sidebar")

        # Main Editor
        #self.editor = SetText(self, callback=self._edit_callback)
        self.editor = ScrolledText(self, wrap=WORD)
        self.editor.pack(expand=TRUE, fill=BOTH, side=LEFT, anchor=E)

        # Sidebar
        # self._init_sidebar()
        # self.sidebar.pack(expand=TRUE, fill=Y, side=TOP, anchor=E)
      
        return

    def _edit_callback(self, event=None):
        Session.file_saved = False
        return

    def _bind_events(self):

        #self.bind(CLOSE_EVENT, self._clear_model)
        #EventHandler.add_event_listeners(self, CLOSE_EVENT)
    
        self.bind(OPEN_EVENT, self.open_xml)
        EventHandler.add_event_listeners(self, OPEN_EVENT)

        return

    def _init_sidebar(self):

        # Parent Frame
        self.sidebar = Frame(self)

        # XML Buttons

        self.xml_buttons = LabelFrame(self.sidebar, text='XML')

        self.save_xml_btn = Button(self.xml_buttons, text='Save XML', 
            command=self._save_xml).pack(expand=TRUE, fill=X, side=LEFT)

        self.reload_xml_btn = Button(self.xml_buttons, text='Reload XML', 
            command=self._reload_xml).pack(expand=TRUE, fill=X, side=LEFT)
        
        self.xml_buttons.pack(fill=X)
        
        # Property List
        
        self.property_list = PropertyList(self.sidebar)
        self.property_list.pack(expand=TRUE, fill=BOTH)

        return

    def open_xml(self, event=None):

        self.editor.delete('1.0', 'end-1c')
        
        if not Session.file_path:
            return

        print('Opening xml...')
        with open(Session.file_path, 'r') as f:
            self.editor.insert('end-1c', f.read())
        print('Success!')

        # print('Loading properties...')
        # self.property_list._clear_properties()
        # self.property_list._display_properties()
        # print('Success!')

        return        
    
    def _save_xml(self, event=None):
        
        if(not Session.file_path):
            print('No session filepath\n')
            return
        print('Saving xml...')
        
        text = self.editor.get('1.0', 'end-1c') 
        with open(Session.file_path, 'w') as f:
            f.write(text)
        
        print('Saved as:' + Session.file_path + '\n')

        self._reload_xml()

        return

    def _reload_xml(self, event=None):

        if(not Session.file_path):
            print('No session filepath\n')
            return
        print('Reloading xml...')

        file_path = Session.file_path
        file = FileHandler.open_file(file_path)
        if(HYXML_FILE in file_path): 
            print('.hyxml file reloaded\n')
            Session.file_type = HYXML_FILE
        elif(MDL_FILE in file_path):
            print ('.mdl file opened\n')
            Session.file_type = MDL_FILE

        if file == None:
            print('File reload failed: No file\n')
            return

        # Obtain parsed results
        Session.hybrid = file['hybrid']
        Session.prop_list = file['prop_list']
        if(len(Session.prop_list) == 0):
            Session.prop_list.append(Property())
        Session.file_opened = True

        print('File reloaded!')
        self.open_xml()

        return