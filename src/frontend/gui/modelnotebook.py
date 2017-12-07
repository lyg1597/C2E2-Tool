from tkinter import *
from tkinter.ttk import *

from frontend.gui.eventhandler import EventHandler
from frontend.gui.modeltab import ModelTab
from frontend.mod.constants import *
from frontend.gui.plotwindow import PlotterModelTab
from frontend.gui.editortab import EditorTab  # LMB +0 12/05/2017, Added Editor Tab


class ModelNotebook(Notebook):
    def __init__(self, parent, **options):
        Notebook.__init__(self, parent, **options)
        self.parent = parent

        self._init_widgets()
        self._bind_events()
        self.plot_tab_dic = {}

    def _bind_events(self):
        self.bind(OPEN_EVENT, self.display_widgets)
        EventHandler.add_event_listeners(self, OPEN_EVENT)
        self.bind(CLOSE_EVENT,self.hide_widgets)
        EventHandler.add_event_listeners(self, CLOSE_EVENT)

    def _init_widgets(self):
        self.model_tab = ModelTab(self)
        self.add(self.model_tab, text='Model')
        self.editor_tab = EditorTab(self)  #LMB +1, 12/05/2017, Added Editor tab
        self.add( self.editor_tab, text='Editor')

    def _init_plot_widgets(self,*args):

        name = args[-1]+" plot"
        if name in self.plot_tab_dic:
            self.forget(self.plot_tab_dic[name])

        plot_model_tab = PlotterModelTab(self, *args)
        self.plot_tab_dic[name] = plot_model_tab
        self.add(plot_model_tab, text =name)

    def _close_plot_tab(self, tabname):
        
        self.forget(self.plot_tab_dic[tabname])
        self.plot_tab_dic.pop(tabname,None)
        return

        

    def display_widgets(self, event=None):
        self.pack(fill=BOTH, expand=True)

    def hide_widgets(self,event=None):
        keys = list(self.plot_tab_dic.keys())
        for key in keys:
            self.forget(self.plot_tab_dic[key])
            self.plot_tab_dic.pop(key,None)
        self.pack_forget()

