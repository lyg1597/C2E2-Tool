# File:         editortab.py
# Purpose:      EditorTab class to be used in modelnotebook. Generates an "Editor" tab to edit XML directly.  
# Created By:   Lucas Brown (LMB)
# Created On:   10/06/2017
# Last Edit:    10/06/2017 (LMB)

from tkinter import *
from frontend.gui.modeltab import PropertyEditor

class EditorTab( Frame ):

    def __init__( self, parent ):
        Frame.__init__( self, parent )
        self.parent = parent
        self._init_widgets()

    def _init_widgets( self ):
        print ("Initialize the Editor Tab and Property Editor")

        Text(self).pack(expand=TRUE, fill=BOTH, side=LEFT, anchor =E)
        PropertyEditor(self).pack(expand=TRUE, fill=Y, side=TOP, anchor=E)


