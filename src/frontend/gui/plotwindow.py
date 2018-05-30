import tkinter as tk
import PIL.Image
import PIL.ImageTk
from tkinter.ttk import *
from frontend.gui.eventhandler import EventHandler
from frontend.gui.widgets import *
from frontend.mod.session import Session, Property ,PlotProperty
from frontend.mod.plotter import plotGraph
from sys import platform



class PlotterModelTab(Frame):

    def __init__(self, parent, *args):
        Frame.__init__(self, parent)
        
        self.parent = parent
        self._init_widgets(*args)

    def _init_widgets(self,*args):

        Session.write("Initialize the Plot PropertyEditor and plotter view.\n")
        self.display = PlotterDisplay(self)
        self.display.pack(expand=TRUE, fill=BOTH, side=LEFT, anchor =E)
        PlotterPropertyEditor(self, *args)\
            .pack(expand=TRUE, fill=Y, side=TOP, anchor=E)


class PlotterDisplay(tk.Canvas):
    
    def __init__(self,parent, **options):
        tk.Canvas.__init__(self, parent, borderwidth=0, background="#ffffff", 
            **options)
        
        self.frame = tk.Frame(self, **options)
        self.frame.config(bg="white")
        self.vsb = tk.Scrollbar(parent, orient="vertical", command=self.yview)
        self.configure(yscrollcommand=self.vsb.set)
        
        self.pack(side="left", fill="both", expand=True)
        self.vsb.pack(side="left", fill="y")
        
        self.create_window((0,0), window=self.frame, anchor="nw",
            tags="self.frame")
        
        #self.frame.pack(fill=BOTH, expand=TRUE)
        self.frame.bind("<Configure>", self.onFrameConfigure)

        self.parent = parent
        self.result_list = {}
        self.imagelist = {}
        self.large_imagelist = {}
        self.imgcounter = 0
        #self.populate()
    
    #test function for scroll
    def populate(self):
    
        for row in range(50):
            tk.Label(self.frame, text="%s" % row, width=3, borderwidth="1", 
                     relief="solid").grid(row=row, column=0)
            t="this is the second column for row %s" %row
            tk.Label(self.frame, text=t).grid(row=row, column=1)

    def onFrameConfigure(self,event):
        self.configure(scrollregion=self.bbox("all"))

    def _add_image(self, filename, identifier):
        path = '../work-dir/plotresult/'+filename+'.png'
        if platform == 'linux':
            smallimage = PIL.Image.open(path)
        else:
            smallimage = PIL.Image.open(path).convert('RGB')

        smallimage = smallimage.resize((200, 160),PIL.Image.ANTIALIAS)
        smallimage = PIL.ImageTk.PhotoImage(smallimage)
        self.imagelist[identifier] = smallimage
        self.result_list[identifier] = tk.LabelFrame(self.frame, text=filename)
        self.result_list[identifier].config(bg="white", bd = 5)
        #fix this position
        self.result_list[identifier].grid(row=int(self.imgcounter/3),column=self.imgcounter%3) 

        if platform == 'linux':
            display_image = PIL.Image.open(path)
        else:
            display_image = PIL.Image.open(path).convert('RGB')
        display_image = PIL.ImageTk.PhotoImage(display_image)
        self.large_imagelist[filename] = display_image

        image = tk.Label(self.result_list[identifier], image=smallimage)
        image.pack()
        image.config(bg="white")
        image.bind('<Double-Button-1>', lambda event,x=filename :self._callback_btn_press_double(x))
        self.imgcounter+=1

    def _edit_image(self,filename,identifier):
        path = '../work-dir/plotresult/'+filename+'.png'
        if platform == 'linux':
            smallimage = PIL.Image.open(path)
        else:
            smallimage = PIL.Image.open(path).convert('RGB')

        if platform == 'linux':
            display_image = PIL.Image.open(path)
        else:
            display_image = PIL.Image.open(path).convert('RGB')
        display_image = PIL.ImageTk.PhotoImage(display_image)
        self.large_imagelist[filename] = display_image

        
        smallimage = smallimage.resize((200, 160),PIL.Image.ANTIALIAS)
        smallimage = PIL.ImageTk.PhotoImage(smallimage)
        self.imagelist[identifier] = smallimage
        for widget in self.result_list[identifier].winfo_children():
            widget.destroy()
        self.result_list[identifier]['text'] = filename
        image = tk.Label(self.result_list[identifier], image=smallimage)
        image.pack()
        image.config(bg="white")
        image.bind('<Double-Button-1>', lambda event,x=filename :self._callback_btn_press_double(x))

    def upload_new_img(self, filename, identifier):
        if not identifier in self.imagelist:
            self._add_image(filename,identifier)
        else:
            self._edit_image(filename,identifier)

    def destroy_img(self,filename,identifier):
        for widget in self.result_list[identifier].winfo_children():
            widget.destroy()
        self.result_list[identifier].destroy()
        del self.result_list[identifier]
        del self.imagelist[identifier]
        self.imgcounter-=1
        self._re_display_image()

    def _re_display_image(self):
        counter = 0
        for key in sorted(self.result_list):
            self.result_list[key].grid(row=int(counter/3),column=counter%3)
            counter+=1
        return 

    def _callback_btn_press_double(self,filename):
        top = Toplevel()
        width = top.winfo_screenwidth()
        height = top.winfo_screenheight()
        appwidth = 600
        appheight = 480
        offset_x = width//2 - appwidth //2
        offset_y = height//2 - appheight//2
        top.geometry("%dx%d%+d%+d" % (600, 480, offset_x, offset_y))
        top.title(filename)
        image = tk.Label(top, image=self.large_imagelist[filename])
        image.pack(expand=TRUE, fill=BOTH)



class PlotterPropertyEditor(Frame):
    
    def __init__(self, parent, *args, **options):
        Frame.__init__(self, parent, **options)
        
        # args for plotter
        self.parent = parent
        self.sel_iid = None
        self.varlist = list(args[0])
        self.varlist.insert(0,'time')
        self.modelist = list(args[1])
        self.time_step = args[2]
        self.time_horizon = args[3]
        self.unsafe_set = args[4]
        #print (self.unsafe_set)
        self.file_path = args[5]
        self.sim_adpative = args[6]
        self.propname = args[7]

        self.y_var_list=[]
        for i in range(len(self.varlist)):
            self.y_var_list.append(StringVar())
            self.y_var_list[i].set('0')
        self.y_var_list[1].set('1')
        self.firstvaridx = int(str(self.y_var_list[0])[6:])

        self._bind_events()
        self._init_widgets()

        self.cur_prop = None
        self.propertylist = []
        self.sel_iid = None

        self.identifier = 0

        self._callback_new()

    def _bind_events(self):
        print("complete me for event bind in plotter")
        # self.bind(CLOSE_EVENT, self._clear_plotter_properties)
        # EventHandler.add_event_listeners(self, CLOSE_EVENT)

        # self.bind(OPEN_EVENT, self._display_plotter_properties)
        # EventHandler.add_event_listeners(self, OPEN_EVENT)

    def _init_widgets(self):
        print("complete me for init plotter widget")
        self._init_plot_prop_view()
        self.prop_view.pack(fill=X)
        self.prop_view.columnconfigure(1, weight=1)
        self._init_prop_list()
        self.prop_list.pack(expand=TRUE, fill=BOTH)


    def _init_plot_prop_view(self):
        # Property view frame
        self.prop_view = LabelFrame(self, text='Plot Property')
        # Name
        Label(self.prop_view, text='File name:').grid(row=0, sticky=W)
        self.name_vl = ValidLabel(self.prop_view)
        self.name_vl.grid(row=0, column=2, sticky=E)
        self.name_var = StringVar()
        self.name_var.trace_variable('w', self._callback_name)
        Entry(self.prop_view, textvariable=self.name_var).grid(row=0, column=1, sticky=W+E)

        # Plot_Title
        Label(self.prop_view, text='Plot name:').grid(row=1, sticky=W)
        self.plot_name_var = StringVar()
        self.plot_name_var.trace_variable('w', self._callback_plot_name)
        Entry(self.prop_view, textvariable=self.plot_name_var).grid(row=1, column=1, sticky=W+E)

        # X_axis label
        #Label(self.prop_view, text='X Label:').grid(row=2, sticky=W)
        self.x_axis_label_var = StringVar()
        self.x_axis_label_var.trace_variable('w', self._callback_x_axis_name)
        #Entry(self.prop_view, textvariable=self.x_axis_label_var).grid(row=2, column=1, sticky=W+E)

        # Y_axis label
        #Label(self.prop_view, text='Y Label:').grid(row=3, sticky=W)
        self.y_axis_label_var = StringVar()
        self.y_axis_label_var.trace_variable('w', self._callback_y_axis_name)
        #Entry(self.prop_view, textvariable=self.y_axis_label_var).grid(row=3, column=1, sticky=W+E)

        opts = self.varlist
        #x-axis
        Label(self.prop_view, text='x-axis:').grid(row=2, sticky=W)
        self.x_axis_vl = ValidLabel(self.prop_view)
        self.x_axis_vl.grid(row=2,column=2,sticky=E)
        self.x_axis_vl.set_state(True)
        self.x_axis_var = StringVar()
        self.x_axis_var.set(opts[0])
        self.x_axis_var.trace_variable('w', self._callback_x_axis)
        OptionMenu(self.prop_view, self.x_axis_var, '', *opts)\
                .grid(row=2, column=1, sticky=W+E)

        self.y_axis_frame = LabelFrame(self.prop_view, text='y-axis')
        self.y_axis_frame.grid(columnspan=3,rowspan=1,sticky=W+E)
        
        counter = 0
        for idx,checkBoxName in enumerate(self.varlist):
            #fix the position
            Checkbutton(self.y_axis_frame, text=checkBoxName, variable=self.y_var_list[idx]).grid(row=int(counter/4),column=counter%4,sticky=W)
            self.y_var_list[idx].trace_variable('w',self._callback_checkbox)
            counter+=1

        

    def _display_plotter_properties(self):
        #I don't think there should be anything that binds to open event
        #Just left here to be safe
        return 
    def _clear_plotter_properties(self):
        #I don't think there should be anything that binds to close event
        #Just left here to be safe
        return 
    def _callback_checkbox(self,*args):

        idx = int(args[0][6:])-self.firstvaridx       
        self.cur_prop.y_check_box[idx].set(self.y_var_list[idx].get())
        self._verifiy_list()
        self._update_validation()


    def _callback_name(self,*args):
        #print ("complete Me for call_back_name")
        name = self.name_var.get()
        valid = True
        if name == '':
            valid = False
        else:
            for prop in self.propertylist:
                if prop.is_visible:
                    #print (prop.name)
                    continue
                elif name == prop.name:
                    valid = False
                    break
        self.cur_prop.name = name
        self.cur_prop.name_valid = valid
        self.name_vl.set_state(valid)
        self.list_view.set(self.sel_iid, 0, name)

    def _verifiy_list(self):
        idx = self.varlist.index(self.cur_prop.x)

        if self.cur_prop.y_check_box[idx].get() == '1':
            self.cur_prop.x_valid = False
        else:
            self.cur_prop.x_valid = True
        for strvar in self.cur_prop.y_check_box:
            if strvar.get()=='1':
                return
        self.cur_prop.x_valid = False
        return
        #self.cur_prop.y_valid_list = tablelist[1:]

    def _update_validation(self):
        #FIXME to support mutilple y-axis val
        self.x_axis_vl.set_state(self.cur_prop.x_valid)
        #self.y_axis_vl.set_state(self.cur_prop.y_valid_list[0])
        return


    def _callback_x_axis(self,*args):
        #print ("complete Me for call_back_axis")
        self.cur_prop.x = self.x_axis_var.get()
        self._verifiy_list()
        self._update_validation()
        #self.x_axis_vl.set_state(self.cur_prop.x_valid)


    def _callback_plot_name(self, *args):
        #print ("Complete Me for the call back_plot_name")
        self.cur_prop.plot_name = self.plot_name_var.get()

    def _callback_x_axis_name(self, *args):
        #print ("Complete Me for the call back x_axis_label")
        self.cur_prop.x_axis_label = self.x_axis_label_var.get()

    def _callback_y_axis_name(self, *args):
        #print ("Complete Me for the call back y_axis_label")
        self.cur_prop.y_axis_label = self.y_axis_label_var.get()

    def _init_prop_list(self):
        self.prop_list = LabelFrame(self, text='Properties')
        
        #make a tree to display different properties
        self.list_view = Treeview(self.prop_list)
        self.list_view.pack(fill=BOTH, expand=TRUE)
        self.list_view.bind('<Button-1>', self._callback_btn_press)

        self.list_view['show'] = 'headings'
        self.list_view['columns'] = ('name', 'status')
        self.list_view.column('name', width=150)
        self.list_view.column('status', width=150)
        self.list_view.heading('name', text='Property')
        self.list_view.heading('status', text='Status')

        # New, copy, and remove buttons
        row = Frame(self.prop_list)
        row.pack(fill=X)

        self.new_btn = Button(row, text='New', command=self._callback_new)
        self.new_btn.pack(expand=TRUE, fill=X, side=LEFT)

        self.cpy_btn = Button(row, text='Copy', command=self._callback_cpy)
        self.cpy_btn.pack(expand=TRUE, fill=X, side=LEFT)

        self.rmv_btn = Button(row, text='Remove', command=self._callback_rmv)
        self.rmv_btn.pack(expand=TRUE, fill=X, side=LEFT)

        #plot button
        row = Frame(self.prop_list)
        row.pack(fill=X)

        self.plot_btn = Button(row, text='Plot', command=self._callback_plot)
        self.plot_btn.pack(expand=TRUE, fill=X, side=LEFT)

        self.close_btn = Button(row, text='Close', command=self._callback_close)
        self.close_btn.pack(expand=TRUE, fill=X, side=LEFT)



    def _callback_btn_press(self, event):
        #print ("Complete Me for the select properties")
        self.cur_prop.is_visible = False
        
        iid = self.list_view.identify_row(event.y)
        if iid:
            self.sel_iid = iid
            idx = self.list_view.index(self.sel_iid)
            self.cur_prop = self.propertylist[idx]
            self._display_property(self.cur_prop)

    def _callback_close(self):
        tabname = self.propname + " plot"
        self.parent.parent._close_plot_tab(tabname)


    def _callback_new(self):
        #print ("Complete Me for the new button")
        self.cur_prop =PlotProperty(self.identifier)
        self.identifier+=1
        self.propertylist.append(self.cur_prop)

        self.sel_iid = self._add_property(self.cur_prop)
        self.list_view.selection_set(self.sel_iid)
        self.cur_prop.x = self.varlist[0]
        #self.cur_prop.y_list.append(self.varlist[1])
        for i in range(len(self.varlist)):
            self.cur_prop.y_check_box.append(StringVar())
            self.cur_prop.y_check_box[i].set('0')
        self.cur_prop.y_check_box[1].set('1')

        self._display_property(self.cur_prop)

    def _add_property(self, prop):
        iid = self.list_view.insert('', 'end', values=(prop.name, prop.status))
        return iid

    def _display_property(self, prop):
        prop.is_visible = True
        self.name_var.set(prop.name)
        self.plot_name_var.set(prop.plot_name)
        self.x_axis_label_var.set(prop.x_axis_label)
        self.y_axis_label_var.set(prop.y_axis_label)
        self.x_axis_var.set(prop.x)
        #self.y_axis_var.set(prop.y_list[0])
        for i in range(len(self.y_var_list)):
            self.y_var_list[i].set(prop.y_check_box[i].get())
        return 

    def _copyhandler(self,prop):
        new_prop = PlotProperty(self.identifier)
        self.identifier+=1
        new_prop.name = prop.name
        new_prop.x_axis_label = prop.x_axis_label
        new_prop.y_axis_label = prop.y_axis_label
        new_prop.plot_name = prop.plot_name
        new_prop.x = prop.x
        

        # Property values valid
        new_prop.name_valid = prop.name_valid
        new_prop.x_valid = prop.x_valid
        for i in range(len(self.varlist)):
            new_prop.y_check_box.append(StringVar())
            new_prop.y_check_box[i].set(prop.y_check_box[i].get())


        # Misc
        new_prop.status = "Not Plotted"
        new_prop.is_visible = False
        return new_prop

    def _callback_cpy(self):
        #print ("Complete Me for the copy button")
        self.cur_prop.is_visible = False
        new_prop = self._copyhandler(self.cur_prop)
        new_prop.name = new_prop.name + '_Copy'
        self.cur_prop = new_prop
        self.propertylist.append(self.cur_prop)

        self.sel_iid = self._add_property(self.cur_prop)
        self.list_view.selection_set(self.sel_iid)
        self._display_property(self.cur_prop)

    def _callback_rmv(self):
        #print ("Complete Me for the remove button")
        if self.cur_prop.status == "Plotted":
            self.parent.display.destroy_img(self.cur_prop.name,self.cur_prop.identifier)
        idx = self.list_view.index(self.sel_iid)
        plen = len(self.propertylist) - 1

        if plen == 0:
            return
        elif idx == plen:
            self.cur_prop = self.propertylist[idx - 1]
            up_iid = self.list_view.prev(self.sel_iid)
        else:
            self.cur_prop = self.propertylist[idx + 1]
            up_iid = self.list_view.next(self.sel_iid)
            
        # Remove the selected property 
        self.list_view.delete(self.sel_iid)
        del self.propertylist[idx]

        # Display property in view and list
        self.sel_iid = up_iid
        self.list_view.selection_set(self.sel_iid)
        self._display_property(self.cur_prop)
        
        return

    def _callback_plot(self):
        #print ("Complete me for calling the plot function")
        #str(self.cur_prop)
        if self.cur_prop.is_valid():
            self._disable_enable_button(True)
            indexlist = []
            y_list = []
            indexlist.append(self.varlist.index(self.cur_prop.x))
            for i in range(len(self.cur_prop.y_check_box)):
                if self.cur_prop.y_check_box[i].get()=='1':
                    indexlist.append(i)
                    y_list.append(self.varlist[i])

            indexlist = tuple(indexlist)
            #print indexlist
            plotGraph(self.file_path, 
                      self.unsafe_set, 
                      self.varlist, 
                      self.modelist,
                      indexlist, 
                      self.time_step,
                      self.time_horizon,
                      self.cur_prop.plot_name, 
                      self.cur_prop.name, 
                      self.cur_prop.x, 
                      y_list, 
                      self.sim_adpative)
            self._update_property_status()
            self._disable_enable_button(False)
            self.parent.display.upload_new_img(self.cur_prop.name,self.cur_prop.identifier)

        return

    def _update_property_status(self):
        #print ("Complete me for status update")
        self.cur_prop.status = "Plotted"
        self.list_view.item(self.sel_iid, values=(self.cur_prop.name, self.cur_prop.status))
        return


    def _disable_enable_button(self,disable):
        if disable:
            self.new_btn['state'] = 'disabled'
            self.cpy_btn['state'] = 'disabled'
            self.rmv_btn['state'] = 'disabled'
            self.plot_btn['state'] = 'disabled'
            self.close_btn['state'] = 'disabled'
        else:
            self.new_btn['state'] = 'normal'
            self.cpy_btn['state'] = 'normal'
            self.rmv_btn['state'] = 'normal'
            self.plot_btn['state'] = 'normal'
            self.close_btn['state'] = 'normal'

        self.update()


