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

        self.TEXTBOX_HEIGHT = 100
        self.TEXTBOX_WIDTH = 300


    def confirm( self ):

        print( 'Saved!\n' )
        self.popup.destroy()


    def cancel( self ):
        
        print( 'Entry Canceled\n' )
        self.popup.destroy()


class VariableEntry( PopupEntry ):

    def __init__( self, parent, item_id ):
        PopupEntry.__init__( self, parent, item_id )

        self.title_label.config( text='Edit Variables' )

        Label( self.popup, text='Variables: ' ).grid( row=2, column=0, sticky=W )
        self.variables_vl = ValidLabel( self.popup )
        self.variables_vl.grid( row=2, column=1, sticky=E )

        self.variables_var = StringVar()
        self.variables_var.trace_variable( 'w', self._callback_variables )
        Entry( self.popup, textvariable=self.variables_var ).grid( row=3, column=0, columnspan=2, sticky=W+E )

        self.confirm_button.grid( row=4, column=0, sticky=E+W )
        self.cancel_button.grid( row=4, column=1, sticky=E+W )

    def _callback_variables( self, *args ):

        print( 'Callback Test\n' )


class ModeEntry( PopupEntry ):

    def __init__( self, parent, item_id ):

        PopupEntry.__init__( self, parent, item_id )
        self.title_label.config( text='Mode' )


        # Flows

        Label( self.popup, text='Flows: ' ).grid( row=2, column=0, sticky=W )

        self.flows_vl = ValidLabel( self.popup )
        self.flows_vl.grid( row=2, column=1, sticky=E )
        # TODO: Add Error once parsing
        
        self.flows = SetText( self.popup, height=self.TEXTBOX_HEIGHT, width=self.TEXTBOX_WIDTH, callback=self._callback_flows)
        self.flows.grid( row=3, column=0, columnspan=2, sticky=N+S+E+W )


        # Invariants

        Label( self.popup, text='Invariants: ' ).grid( row=4, column=0, sticky=W )

        self.invariants_vl = ValidLabel( self.popup )
        self.invariants_vl.grid( row=4, column=1, sticky=E )
        # TODO: Add Error once parsing

        self.invariants = SetText( self.popup, height=self.TEXTBOX_HEIGHT, width=self.TEXTBOX_WIDTH, callback=self._callback_flows)
        self.invariants.grid( row=5, column=0, columnspan=2, sticky=N+S+E+W )

        self.confirm_button.grid( row=6, column=0, sticky=E+W )
        self.cancel_button.grid( row=6, column=1, sticky=E+W )


    def _callback_flows( self, input ):

        print( 'Callback Test\n' )


class TransitionEntry( PopupEntry ):

    def __init__( self, parent, item_id ):

        PopupEntry.__init__( self, parent, item_id )
        self.title_label.config( text='Transition' )

        # TODO: Temporary - for testing/design
        self.mode_list = [ 'Mode 1', 'Mode 2', 'Mode 3' ]


        # Source and Destination

        self.source_var = StringVar()
        self.destination_var = StringVar()

        Label( self.popup, text='Source:' ).grid( row=2, column=0, sticky=W )
        Label( self.popup, text='Destination: ' ).grid( row=3, column=0, sticky=W )
        
        self.source_var.trace_variable( 'w', self._callback_mode_select )
        self.destination_var.trace_variable( 'w', self._callback_mode_select )

        OptionMenu( self.popup, self.source_var, self.mode_list[0], *self.mode_list ).grid( row=2, column=1, sticky=W+E )
        OptionMenu( self.popup, self.destination_var, self.mode_list[1], *self.mode_list ).grid( row=3, column=1, sticky=W+E )
       

        # Guards
        
        Label( self.popup, text='Guards: ' ).grid( row=4, column=0, sticky=W )

        self.guards_vl = ValidLabel( self.popup )
        self.guards_vl.grid( row=4, column=1, sticky=E )
        # TODO: Add Error parsing
        
        self.guards = SetText( self.popup, height=self.TEXTBOX_HEIGHT, width=self.TEXTBOX_WIDTH, callback=self._callback_guards )
        self.guards.grid( row=5, column=0, columnspan=2, sticky=N+S+E+W )

        
        # Actions 
        
        Label( self.popup, text='Actions: ' ).grid( row=6, column=0, sticky=W )
        
        self.actions_vl = ValidLabel( self.popup )
        self.actions_vl.grid( row=6, column=1, sticky=E )
        # TODO: Add Error Parsing
        
        self.actions = SetText( self.popup, height=self.TEXTBOX_HEIGHT, width=self.TEXTBOX_WIDTH, callback=self._callback_actions )
        self.actions.grid( row=7, column=0, columnspan=2, sticky=N+S+E+W )


        # Confirm / Cancel buttons

        self.confirm_button.grid( row=8, column=0, sticky=E+W )
        self.cancel_button.grid( row=8, column=1, sticky=E+W )

    def _callback_mode_select( self, *args ):

        print('Source: ', self.source_var.get() )
        print('Destination: ', self.destination_var.get() )
        print()

    def _callback_guards( self, *args ):
        
        print( 'Callback Test: Guards\n' )

    def _callback_actions( self, *args ):

        print( 'Callback Test: Actions\n' )