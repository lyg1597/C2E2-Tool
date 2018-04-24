from tkinter import *
from tkinter.ttk import *
import sys
import webbrowser
from frontend.gui.eventhandler import EventHandler
from frontend.gui.menubar import MenuBar
from frontend.gui.modelnotebook import *
from frontend.mod.session import *


class C2E2(Tk):

    def __init__(self, parent):
        Tk.__init__(self, parent)

        self.parent = parent
        self.title('C2E2')
        #self.iconbitmap('@./frontend/res/c2e2.ico')
        img = PhotoImage(file='./frontend/res/c2e2.gif')
        self.tk.call('wm','iconphoto',self._w,img)
        width = self.winfo_screenwidth()
        height = self.winfo_screenheight()
        appwidth = int(width*2/3)
        appheight = int(height*2/3)
        if appheight < 650:
            appheight = 650
        offset_x = width//2 - appwidth //2
        offset_y = height//2 - appheight//2

        def callback(event):
            webbrowser.open_new(r"http://publish.illinois.edu/c2e2-tool/files/2013/05/ECE584-Manual.pdf")

        self.geometry('{}x{}+{}+{}'.format(appwidth,appheight,offset_x,offset_y))
        self.label = Label(self, text="Open a new model File -> Open")
        self.manual = Label(self, text = "Read C2E2 manual (click me!)")
        self.email = Label(self, text = "Send Email To: c2e2help@gmail.com for support")
        self.email.config(font=("Courier", 24))
        self.label.pack(fill=Y)
        self.label.config(font=("Courier", 30))
        self.manual.pack(fill=Y)
        self.email.pack(fill=Y)
        self.manual.config(font=("Courier", 30))
        self.manual.bind("<Button-1>", callback)

        self._init_widgets()
    

    def _init_widgets(self):
        """ Initialize Notebook and MenuBar widgets """

        Session.window = self
        self.notebook = ModelNotebook(self)
        self.menu = MenuBar(self, self.notebook)