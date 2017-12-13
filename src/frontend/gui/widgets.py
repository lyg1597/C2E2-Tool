from tkinter import *
from tkinter.ttk import *
import re
from sys import platform
from frontend.mod.constants import *
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


class PopupEntry( object ):

    def __init__( self, parent, item_id ):

        self.popup = Toplevel( parent )
        self.popup.resizable( width=False, height=False )

        self.title_label = Label( self.popup, text='C2E2' )
        self.title_label.grid( row=0, column=0, columnspan=2 )
        
        self.confirm_button = Button( self.popup, text='Confirm', command=self.confirm )
        self.confirm_button.grid( row=2, column=1 )

        self.cancel_button = Button( self.popup, text='Cancel', command=self.cancel )
        self.cancel_button.grid( row=2, column=0 )


    def confirm( self ):

        print( 'Saved!\n' )
        self.popup.destroy()


    def cancel( self ):
        
        print( 'Entry Canceled\n' )
        self.popup.destroy()


class VariablesEntry( PopupEntry ):

    def __init__( self, parent, item_id ):
        PopupEntry.__init__( self, parent, item_id )

        self.title_label.config( text='Variables' )

        Label( self.popup, text='Enter variables separated by a comma' ).grid( row=2, columnspan=2 )
        
        Label( self.popup, text='(var_1, var_2, var_3, ... )' ).grid( row=3, columnspan=2 )

        self.variables_entry = Entry( self.popup )
        self.variables_entry.grid( row=4, column=0, columnspan=2 )

        self.confirm_button.grid( row=5, column=1 )
        self.cancel_button.grid( row=5, column=0 )


class ModelEntry( PopupEntry ):

    def __init__( self, parent, item_id ):
        PopupEntry.__init__( self, parent, item_id )

        self.title_label.config( text='Model' )


class TransitionEntry( PopupEntry ):

    def __init__( self, parent, item_id ):
        PopupEntry.__init__( self, parent, item_id )

        self.title_label.config( text='Transition' )