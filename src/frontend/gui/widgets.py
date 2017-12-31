from tkinter import *
from tkinter.ttk import *
import re
from sys import platform
from frontend.mod.constants import *
from frontend.mod.session import Session, Property
import PIL.Image
import PIL.ImageTk

# TODO remove the following class
class FloatEntry(Entry):
    def __init__(self, parent, callback=None, **options):
        Entry.__init__(self, parent, **options)
        self.parent = parent
        self.callback = callback

        val_cmd = (self.register(self._validate), '%d', '%P', '%W')
        self.config(validate='key', validatecommand=val_cmd)

    def _validate(self, event, nstr, widget):
        try:
            float(nstr)
            if self.callback: 
                self.callback(True, float(nstr))

            return True
        except ValueError:
            if self.callback:
                self.callback(False, nstr, 0.0)

            return True 

# TODO remove the following class
class IntegerEntry(Entry):
    def __init__(self, parent, callback=None, **options):
        Entry.__init__(self, parent, **options)
        self.parent = parent
        self.callback = callback

        val_cmd = (self.register(self._validate), '%d', '%P', '%W')
        self.config(validate='key', validatecommand=val_cmd, width=10)

    def _validate(self, event, nstr, widget):
        try:
            int(nstr)
            if self.callback: 
                self.callback(True, int(nstr))

            return True
        except ValueError:
            if self.callback:
                self.callback(False, 0)

            return True 

class ValidLabel(Label):
    def __init__(self, parent, ses_var=None, **options):
        Label.__init__(self, parent, **options)
        self.parent = parent
        if platform == 'linux':
            self.inv_img = PhotoImage(file=CROSS_IMG)
            self.val_img = PhotoImage(file=TICK_IMG)
        else:
            self.inv_img = PIL.Image.open(CROSS_IMG).convert('RGB')
            self.val_img = PIL.Image.open(TICK_IMG).convert('RGB')

            self.inv_img = PIL.ImageTk.PhotoImage(self.inv_img)
            self.val_img = PIL.ImageTk.PhotoImage(self.val_img)

        self.config(image=self.inv_img)

    def set_state(self, state):
        if state == True:
            self.config(image=self.val_img)
        else:
            self.config(image=self.inv_img)

class SetText(Frame):
    def __init__(self, parent, width=0, height=0, callback=None, **options):
        Frame.__init__(self, parent, width=width, height=height)
        self.parent = parent
        self.callback = callback

        self.text = Text(self, **options)
        self.text.bind('<KeyRelease>', self._callback)
        self.text.pack(expand=TRUE, fill=Y)

    def _callback(self, event=None):
        txt = self.get()
        if self.callback:
            self.callback(txt)

    def pack(self, *args, **kwargs):
        Frame.pack(self, *args, **kwargs)
        self.pack_propagate(False)

    def grid(self, *args, **kwargs):
        Frame.grid(self, *args, **kwargs)
        self.pack_propagate(False)

    def get(self):
        txt = self.text.get('1.0', END).strip()
        return txt

    def set(self, str):
        self.text.delete('1.0', END)
        self.text.insert(END, str)
        self._callback()


class PopupEntry( Toplevel ):

    def __init__( self, parent ):
        Toplevel.__init__( self, parent )

        self.parent = parent
        self.resizable( width=False, height=False )

        self.title_label = Label( self, text='C2E2' )
        self.title_label.grid( row=0, column=0, columnspan=2 )
        
        self.TEXTBOX_HEIGHT = 10
        self.TEXTBOX_WIDTH = 30

        # Prevent interaction with main window until Popup is Confirmed/Canceled
        self.wait_visibility()
        self.focus_set()
        self.grab_set()

        
class ToggleFrame( Frame ):

    def __init__( self, parent, title ):
        Frame.__init__( self, parent )

        self.visible = BooleanVar()
        self.visible.set( False )

        self.header_frame = Frame( self )

        Label( self.header_frame, text=title ).pack( side=LEFT, fill=X, expand=TRUE )
        
        self.toggle_btn = Checkbutton( self.header_frame, width=2, text='+', command=self.toggle_visibile, variable=self.visible, style='Toolbutton' )
        self.toggle_btn.pack( side=LEFT )
        
        self.header_frame.pack( fill=X, expand=TRUE )

        self.sub_frame = Frame( self, relief='sunken', borderwidth=1 )


    def toggle_visibile( self ):

        if( self.visible.get() ):
            self.sub_frame.pack( fill=X, expand=TRUE )
            self.toggle_btn.config( text='-' )
        else:
            self.sub_frame.forget()
            self.toggle_btn.config( text='+' )