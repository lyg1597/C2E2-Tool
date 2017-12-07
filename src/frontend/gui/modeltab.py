import numpy as np
import shlex, subprocess
from tkinter import messagebox
from tkinter import *
from tkinter.ttk import *
from copy import deepcopy
from backend.lib.libc2e2 import IntegerVector, DoubleVector
from frontend.gui.eventhandler import EventHandler
from frontend.gui.widgets import *
from frontend.mod.automaton import SymEq
from frontend.mod.constants import *
from frontend.mod.session import Session, Property
from frontend.mod.simgen import * # FIXME

class ModelTab(Frame):
    def __init__(self, parent):
        Frame.__init__(self, parent)
        self.parent = parent

        self._init_widgets()

    def _init_widgets(self):
        print ("initialize the Treeview and Property Editor")
        self.tree_view = TreeView(self).pack(expand=TRUE, fill=BOTH, side=LEFT, anchor =E)
        PropertyEditor(self).pack(expand=TRUE, fill=Y, side=TOP, anchor=E)
      

class TreeView(Treeview):
    def __init__(self, parent, **options):
        Treeview.__init__(self, parent, **options)
        self.pack(fill=BOTH, expand=TRUE)
        self.parent = parent

        self._bind_events()


        self.config(show='tree')

    def _bind_events(self):
        self.bind(CLOSE_EVENT, self._clear_model)
        EventHandler.add_event_listeners(self, CLOSE_EVENT)

        self.bind(OPEN_EVENT, self._display_model)
        EventHandler.add_event_listeners(self, OPEN_EVENT) 

    def _clear_model(self, event=None):
        self.delete(*self.get_children())


    def _display_model(self, event=None):
        hybrid = Session.hybrid
        # Display variables
        var_id = self.insert('', 'end', text='Variables')
        var_str = ', '.join(hybrid.varList)
        self.insert(var_id, 'end', text=var_str)
        self.item(var_id, open=TRUE)
        
        if len(hybrid.thinvarList)!=0:
            thin_var_id = self.insert('', 'end', text='Thin Variables')
            thin_var_str = ', '.join(hybrid.thinvarList)
            self.insert(thin_var_id, 'end', text=thin_var_str)
            self.item(thin_var_id, open=TRUE)
            

        mode_dict = {}
        modes_id = self.insert('', 'end', text='Modes')
        self.item(modes_id, open=TRUE)
        for mode in hybrid.automata[0].modes:
            mode_dict[mode.id] = mode.name
            mode_str = mode.name + ' (' + str(mode.id) + ')'
            mode_id = self.insert(modes_id, 'end', text=mode_str)

            # Display flows
            flow_id = self.insert(mode_id, 'end', text='Flows')
            for dai in mode.dais:
                if '_dot' in dai.raw:
                    dai_str = dai.raw[3:-1]
                    idx = dai_str.index(',')
                    dai_str = dai_str[:idx]+'='+dai_str[idx+1:]
                    self.insert(flow_id, 'end', text=dai_str)

            # Display invariants
            inv_id = self.insert(mode_id, 'end', text='Invariants')
            for inv in mode.invs:
                self.insert(inv_id, 'end', text=inv.raw)

        # Display transistions
        trans_id = self.insert('', 'end', text='Transitions')
        self.item(trans_id, open=TRUE)
        for tran in hybrid.automata[0].trans:
            src, dest = mode_dict[tran.src], mode_dict[tran.dest]
            tran_str = src + ' -> ' + dest
            tran_id = self.insert(trans_id, 'end', text=tran_str)

            src_str = 'Source: ' + src + ' (' + str(tran.src) + ')'
            self.insert(tran_id, 'end', text=src_str)

            dest_str = 'Destination: ' + dest + ' (' + str(tran.dest) + ')'
            self.insert(tran_id, 'end', text=dest_str)

            guard_str = 'Guards: ' + tran.guard.raw
            self.insert(tran_id, 'end', text=guard_str)

            act_id = self.insert(tran_id, 'end', text='Actions')
            for act in tran.actions:
                self.insert(act_id, 'end', text=act.raw)
        

class PropertyEditor(Frame):
    def __init__(self, parent, **options):
        Frame.__init__(self, parent, **options)
        self.parent = parent
        self.sel_iid = None

        self._bind_events()
        self._init_widgets()


    def _bind_events(self):
        self.bind(CLOSE_EVENT, self._clear_properties)
        EventHandler.add_event_listeners(self, CLOSE_EVENT)

        self.bind(OPEN_EVENT, self._display_properties)
        EventHandler.add_event_listeners(self, OPEN_EVENT) 

    def _init_widgets(self):
        self._init_prop_view()
        self._init_prop_list()

        # Pack child frames
        self.prop_view.pack(fill=X)
        self.prop_view.columnconfigure(1, weight=1)
        self.prop_list.pack(expand=TRUE, fill=BOTH)
        
    ''' PROPERTY VIEW GUI AND FUNCTIONS '''
    def _init_prop_view(self):
        # Regex building blocks
        flt = '(-?(\d*\.?\d+))'
        int = '(-?(\d+))'
        term = '(' + flt + '|(' + int + '\s*/\s*' + int + '))'
        var = '([a-zA-Z]\w*)'
        mode = '(' + var + ':)'
        eql = '((<=?)|(>=?)|(==))'
        expr = '(' + term + '?\s*' + var + '(\s*[-\+]?\s*((' + term + '?\s*' + var + ')|' + term + '))*)'
        eqn = '(' + expr + '\s*' + eql + '\s*' + term + ')'
        eqns = '(' + eqn + '(\s*&&\s*' + eqn + ')*)'
        init_set = '(\s*' + mode + '\s*' + eqns + '\s*){1}$'
        unsafe_set = '(\s*' + eqns + '\s*)+$'

        # Regex strings
        self.re_mode = mode
        self.re_var = var
        self.re_is = init_set
        self.re_us = unsafe_set

        # Property view frame
        self.prop_view = LabelFrame(self, text='Property')

        # Name
        Label(self.prop_view, text='Property name:').grid(row=0, sticky=W)
        self.name_vl = ValidLabel(self.prop_view)
        self.name_vl.grid(row=0, column=2, sticky=E)
        self.name_var = StringVar()
        self.name_var.trace_variable('w', self._callback_name)
        Entry(self.prop_view, textvariable=self.name_var).grid(row=0, column=1, sticky=W+E)

        # Verification type
        # TODO only for show; complete when more functionality available
        self.type_var = IntVar()
        Radiobutton(self.prop_view, text='Safety', variable=self.type_var, value=0).grid(row=1)

        # Time step 
        Label(self.prop_view, text='Time step:').grid(row=2, sticky=W)
        self.ts_vl = ValidLabel(self.prop_view)
        self.ts_vl.grid(row=2, column=2, sticky=E)
        self.time_step_var = DoubleVar()
        self.time_step_var.trace_variable('w', self._callback_time_step)
        Entry(self.prop_view, textvariable=self.time_step_var).grid(row=2, column=1, sticky=W+E)

        # Time horizon
        Label(self.prop_view, text='Time horizon:').grid(row=3, sticky=W)
        self.th_vl = ValidLabel(self.prop_view)
        self.th_vl.grid(row=3, column=2, sticky=E)
        self.time_horizon_var = DoubleVar()
        self.time_horizon_var.trace_variable('w', self._callback_time_horizon)
        Entry(self.prop_view, textvariable=self.time_horizon_var).grid(row=3, column=1, sticky=W+E)

        # K value
        Label(self.prop_view, text='K value:').grid(row=4, sticky=W)
        self.kv_vl = ValidLabel(self.prop_view)
        self.kv_vl.grid(row=4, column=2, sticky=E)
        self.k_value_var = DoubleVar()
        self.k_value_var.trace_variable('w', self._callback_k_value)
        Entry(self.prop_view, textvariable=self.k_value_var).grid(row=4, column=1, sticky=W+E)

        # Simulator type
        opts = (ODEINT_FIX, ODEINT_ADP, CAPD)
        Label(self.prop_view, text='Simulator:').grid(row=5, sticky=W)
        self.sim_var = StringVar()
        self.sim_var.set(opts[0])
        self.sim_var.trace_variable('w', self._callback_simulator)
        OptionMenu(self.prop_view, self.sim_var, '', *opts)\
                .grid(row=5, column=1, columnspan=2, sticky=W+E)

        # Refine type
        opts = (DEF_STRAT, USR_STRAT)
        Label(self.prop_view, text='Refinement:').grid(row=6, sticky=W)
        self.ref_var = StringVar()
        self.ref_var.set(opts[0])
        self.ref_var.trace_variable('w', self._callback_refine_strat)
        OptionMenu(self.prop_view, self.ref_var, '', *opts)\
                .grid(row=6, column=1, columnspan=2, sticky=W+E)

        # Initial set
        Label(self.prop_view, text='Initial set:').grid(row=7, sticky=W)
        self.is_err = StringVar()
        self.is_err.set('')
        Label(self.prop_view, textvariable=self.is_err).grid(row=7, column=1, sticky=W+E)
        self.is_vl = ValidLabel(self.prop_view)
        self.is_vl.grid(row=7, column=2, sticky=E)
        self.initial_set = SetText(self.prop_view, height=65, callback=self._callback_is)
        self.initial_set.grid(row=8, rowspan=4, columnspan=3, sticky=N+S+E+W)

        # Unsafe set
        Label(self.prop_view, text='Unsafe set:').grid(row=12, sticky=W)
        self.us_err = StringVar()
        self.us_err.set('')
        Label(self.prop_view, textvariable=self.us_err).grid(row=12, column=1, sticky=E)
        self.us_vl = ValidLabel(self.prop_view)
        self.us_vl.grid(row=12, column=2, sticky=E)
        self.unsafe_set = SetText(self.prop_view, height=65, callback=self._callback_us)
        self.unsafe_set.grid(row=13, rowspan=4, columnspan=3, sticky=N+S+E+W)

    def _callback_name(self, *args):
        name = self.name_var.get()
        if name != Session.cur_prop.name:
            if Session.cur_prop.status == Simulated or Session.cur_prop.status == Verified:
                self._update_property_status(0,0,1)

        valid = True
        if name == '':
            valid = False
        else:
            for prop in Session.prop_list:
                if prop.is_visible:
                    continue
                elif name == prop.name:
                    valid = False
                    break

        Session.cur_prop.name = name
        Session.cur_prop.name_valid = valid
        self.name_vl.set_state(valid)
        self.list_view.set(self.sel_iid, 0, name)

    def _callback_time_step(self, *args):

        try:
            time_step = self.time_step_var.get()
            valid = True
        except ValueError:
            time_step = 0.0
            valid = False

        if Session.cur_prop.time_step != time_step: 
            if Session.cur_prop.status == Simulated or Session.cur_prop.status == Verified:
                self._update_property_status(0,0,1)

        Session.cur_prop.time_step = time_step
        Session.cur_prop.time_step_valid = valid
        self.ts_vl.set_state(valid)

    def _callback_time_horizon(self, *args):

        try:
            time_horizon = self.time_horizon_var.get()
            valid = True
        except ValueError:
            time_horizon = 0.0
            valid = False
        if Session.cur_prop.time_horizon != time_horizon: 
            if Session.cur_prop.status == Simulated or Session.cur_prop.status == Verified:
                self._update_property_status(0,0,1)

        Session.cur_prop.time_horizon = time_horizon
        Session.cur_prop.time_horizon_valid = valid
        self.th_vl.set_state(valid)

    def _callback_k_value(self, *args):

        try:
            k_value = self.k_value_var.get()
            valid = True
        except ValueError:
            k_value = 0.0
            valid = False

        if Session.cur_prop.k_value != k_value: 
            if Session.cur_prop.status == Simulated or Session.cur_prop.status == Verified:
                self._update_property_status(0,0,1)

        Session.cur_prop.k_value = k_value
        Session.cur_prop.k_value_valid = valid
        self.kv_vl.set_state(valid)

    def _callback_simulator(self, *args):


        if Session.simulator != self.sim_var.get():
            Session.simulator = self.sim_var.get()
            Session.lib_compiled = False
            if Session.simulator == ODEINT_FIX:
                st = 'constant'
                path = '../work-dir/simulator.cpp'
                gen_simulator(path, Session.hybrid, step_type=st)

            elif Session.simulator == ODEINT_ADP:
                st = 'adaptive'
                path = '../work-dir/simulator.cpp'
                gen_simulator(path, Session.hybrid, step_type=st)

            else:
                Session.hybrid.convertToCAPD("simulator")



    def _callback_refine_strat(self, *args):
        #change the refine strategy
        Session.refine_strat = self.ref_var.get()
        
    # TODO optimize this
    def _callback_is(self, input):
        #if Session.cur_prop.status == "Simulated" or Session.cur_prop.status == "Verified":
            #self._update_property_status(0,0,1)

        #print ("value form callback is ")
        #print (Session.cur_prop.initial_set_str)
        match = re.match(self.re_is, input)
        if Session.cur_prop.initial_set_str != input: 
            if Session.cur_prop.status == Simulated or Session.cur_prop.status == Verified:
                self._update_property_status(0,0,1)

        Session.cur_prop.initial_set_str = input

        # Check if input is valid
        if match == None:
            self.is_vl.set_state(False)
            Session.cur_prop.initial_set_obj = None
            Session.cur_prop.init_set_valid = False
            if not input == '':
                self.is_err.set('Incorrect syntax')
            else:
                self.is_err.set('')

        # Parse individual components from input
        else:
            is_sep = input.split(':')

            # Validate mode
            mode = re.search(self.re_var, is_sep[0]).group(0)
            mode_list = [m.name for m in Session.hybrid.automata[0].modes]
            if mode not in mode_list:
                self.is_err.set('No matching modes')
                self.is_vl.set_state(False)
                Session.cur_prop.initial_set_obj = None
                Session.cur_prop.initial_set_valid = False
                return

            # Validate vars
            vars = re.findall(self.re_var, is_sep[1])
            var_list = Session.hybrid.varList
            var_union = set(vars) | set(var_list)
            if len(var_union) > len(var_list):
                self.is_err.set('Variable mismatch')
                self.is_vl.set_state(False)
                Session.cur_prop.initial_set_obj = None
                Session.cur_prop.initial_set_valid = False
                return

            # Parse equations
            a_m, b_m, eq_m = SymEq.get_eqn_matrix(is_sep[1], var_list)
            bounded = SymEq.check_boundedness(a_m, b_m, eq_m, var_list)
            if is_sep[1].count('>')!= is_sep[1].count('<'):
                bounded = False
            
            if bounded:
                self.is_err.set('') 
                self.is_vl.set_state(True)
                Session.cur_prop.initial_set_obj = [is_sep[0], a_m, b_m, eq_m]
                Session.cur_prop.initial_set_valid = True
            else:
                self.is_err.set('Set unbounded')
                self.is_vl.set_state(False)
                Session.cur_prop.initial_set_obj = None
                Session.cur_prop.initial_set_valid = False

    def _callback_us(self, input):
        #if Session.cur_prop.status == "Simulated" or Session.cur_prop.status == "Verified":
            #self._update_property_status(0,0,1)
        match = re.match(self.re_us, input)
        if Session.cur_prop.unsafe_set_str != input: 
            if Session.cur_prop.status == Simulated or Session.cur_prop.status == Verified:
                self._update_property_status(0,0,1)
        Session.cur_prop.unsafe_set_str = input

        # Check if input is valid
        if match == None:
            self.us_vl.set_state(False)
            Session.cur_prop.unsafe_set_obj = None
            Session.cur_prop.unsafe_set_valid = False
            if not input == '':
                self.us_err.set('Incorrect syntax')
            else:
                self.us_err.set('')
            return
        
        # Validate vars
        else:
            vars = re.findall(self.re_var, input)
            var_list = Session.hybrid.varList
            var_union = set(vars) | set(var_list)
            if len(var_union) > len(var_list):
                self.us_err.set('Variable mismatch')
                self.us_vl.set_state(False)
                Session.cur_prop.unsafe_set_obj = None
                Session.cur_prop.unsafe_set_valid = False
                return

            self.us_err.set('')
            self.us_vl.set_state(True)
            Session.cur_prop.unsafe_set_obj = SymEq.get_eqn_matrix(input, var_list)
            Session.cur_prop.unsafe_set_valid = True

    def _display_property(self, prop):
        prop.is_visible = True
        self.name_var.set(prop.name)
        self.time_step_var.set(prop.time_step)
        self.time_horizon_var.set(prop.time_horizon)
        self.k_value_var.set(prop.k_value)
        self.sim_var.set(Session.simulator)
        self.ref_var.set(Session.refine_strat)
        self.initial_set.set(prop.initial_set_str)
        #print (prop.initial_set_str)
        self.unsafe_set.set(prop.unsafe_set_str)

    ''' PROPERTY LIST GUI AND FUNCTIONS'''
    def _init_prop_list(self):
        # Property list frame
        self.prop_list = LabelFrame(self, text='Properties')

        # Property list
        self.list_view = Treeview(self.prop_list)
        self.list_view.pack(fill=BOTH, expand=TRUE)
        self.list_view.bind('<Button-1>', self._callback_btn_press)
        self.list_view.bind('<Double-Button-1>', self._callback_btn_press_double)

        self.list_view['show'] = 'headings'
        self.list_view['columns'] = ('name', 'status', 'result')
        self.list_view.column('name', width=100)
        self.list_view.column('status', width=100)
        self.list_view.column('result', width=100)
        self.list_view.heading('name', text='Property')
        self.list_view.heading('status', text='Status')
        self.list_view.heading('result', text='Result')

        # New, copy, and remove buttons
        row = Frame(self.prop_list)
        row.pack(fill=X)

        self.new_btn = Button(row, text='New', command=self._callback_new)
        self.new_btn.pack(expand=TRUE, fill=X, side=LEFT)

        self.cpy_btn = Button(row, text='Copy', command=self._callback_cpy)
        self.cpy_btn.pack(expand=TRUE, fill=X, side=LEFT)

        self.rmv_btn = Button(row, text='Remove', command=self._callback_rmv)
        self.rmv_btn.pack(expand=TRUE, fill=X, side=LEFT)

        # Simulate and verify buttons
        row = Frame(self.prop_list)
        row.pack(fill=X)

        self.sim_btn = Button(row, text='Simulate', command=self._callback_sim)
        self.sim_btn.pack(expand=TRUE, fill=X, side=LEFT)

        self.ver_btn = Button(row, text='Verify', command=self._callback_ver)
        self.ver_btn.pack(expand=TRUE, fill=X, side=LEFT)

    def _add_property(self, prop):
        iid = self.list_view.insert('', 'end', values=(prop.name, prop.status, prop.result))
        return iid

    def _callback_btn_press_double(self,event):
        if Session.cur_prop.status == Simulated:
            self._open_the_plotter_window(3)
        elif Session.cur_prop.status == Verified:
            if Session.cur_prop.simulator == ODEINT_ADP:
                self._open_the_plotter_window(2)
            else:
                self._open_the_plotter_window(1)
        return




    def _callback_btn_press(self, event):
        # Unset current selection
        Session.cur_prop.is_visible = False

        # Get selected property
        iid = self.list_view.identify_row(event.y)
        if iid:
            self.sel_iid = iid
            idx = self.list_view.index(self.sel_iid)
            Session.cur_prop = Session.prop_list[idx]
            self._display_property(Session.cur_prop)

    def _callback_new(self):
        # Create new property
        Session.cur_prop = Property()
        Session.prop_list.append(Session.cur_prop)

        # Display property in view and list
        self.sel_iid = self._add_property(Session.cur_prop)
        self.list_view.selection_set(self.sel_iid)
        self._display_property(Session.cur_prop)

    def _callback_cpy(self):
        # Create a deep copy of the current session
        new_prop = deepcopy(Session.cur_prop)
        new_prop.status = "Not verified"
        new_prop.result = ""
        new_prop.name = new_prop.name + '_Copy'
        Session.cur_prop = new_prop
        Session.prop_list.append(Session.cur_prop)

        # Display property in view and list
        self.sel_iid = self._add_property(Session.cur_prop)
        self.list_view.selection_set(self.sel_iid)
        self._display_property(Session.cur_prop)

    def _callback_rmv(self):
        # Find next property to select
        idx = self.list_view.index(self.sel_iid)
        plen = len(Session.prop_list) - 1
        if plen == 0:
            Session.cur_prop = Property()
            Session.prop_list.append(Session.cur_prop)
            up_iid = self._add_property(Session.cur_prop)
        elif idx == plen:
            Session.cur_prop = Session.prop_list[idx - 1]
            up_iid = self.list_view.prev(self.sel_iid)
        else:
            Session.cur_prop = Session.prop_list[idx + 1]
            up_iid = self.list_view.next(self.sel_iid)
            
        # Remove the selected property 
        self.list_view.delete(self.sel_iid)
        del Session.prop_list[idx]

        # Display property in view and list
        self.sel_iid = up_iid
        self.list_view.selection_set(self.sel_iid)
        self._display_property(Session.cur_prop)

    # TODO implement me
    def _callback_sim(self):
        if not Session.cur_prop.is_valid():
            return

        self._disable_enable_button(1)
        
        Session.hybrid.printHybridSimGuardsInvariants();
        Session.hybrid.printBloatedSimGuardsInvariants()

        # Simulate the selected model
        self._initialize_cpp_model(1)
        self._compile_executable()
  
        
        result = Session.cpp_model.simulate_verify()
        self._disable_enable_button(0)
        self._update_property_status(1,result,0)

        #update simulator result
        Session.cur_prop.simulator = self.sim_var.get()

        if result!=0:
            self._open_the_plotter_window(3)

    def _open_the_plotter_window(self,sim_adpative):
        #construct arguments to open the plot window
        #Sim == 1, FIX_STEP == 0, Adaptive == 2
        #Different result uses different plotter
        unsafe_set = Session.cur_prop.unsafe_set_obj
        file_path = '../work-dir/'+Session.cur_prop.name
        time_step = Session.cur_prop.time_step
        time_horizon = Session.cur_prop.time_horizon
        varlist = Session.hybrid.varList
        modelist = [m.name for m in Session.hybrid.automata[0].modes]

        self.parent.parent._init_plot_widgets(varlist,modelist,time_step,time_horizon,unsafe_set,file_path,sim_adpative,Session.cur_prop.name)


        
    
    def _update_property_status(self, sim, result, expire):
        if expire:
            Session.cur_prop.status += '*'
            Session.cur_prop.result = 'Expired'
            self.list_view.item(self.sel_iid, values=(Session.cur_prop.name, Session.cur_prop.status, Session.cur_prop.result))

            return 

        if sim:
            Session.cur_prop.status = Simulated
        else:
            Session.cur_prop.status = Verified

        if result == 1:
            Session.cur_prop.result = "Safe"
        elif result == 0 :
            Session.cur_prop.result = "Unknown"
        else:
            Session.cur_prop.result = "Unsafe"
        self.list_view.item(self.sel_iid, values=(Session.cur_prop.name, Session.cur_prop.status, Session.cur_prop.result))


        return 

    def _disable_enable_button(self,disable):
        if disable:
            self.new_btn['state'] = 'disabled'
            self.cpy_btn['state'] = 'disabled'
            self.rmv_btn['state'] = 'disabled'
            self.sim_btn['state'] = 'disabled'
            self.ver_btn['state'] = 'disabled'
        else:
            self.new_btn['state'] = 'normal'
            self.cpy_btn['state'] = 'normal'
            self.rmv_btn['state'] = 'normal'
            self.sim_btn['state'] = 'normal'
            self.ver_btn['state'] = 'normal'

        self.update()


    def _compile_executable(self):
        # FIXME hardcoded compiling simulator.cpp to executable

        if Session.lib_compiled:
            return 

        Session.lib_compiled = True

        top = Toplevel()
        width = top.winfo_screenwidth()
        height = top.winfo_screenheight()
        appwidth = 200
        appheight = 150
        offset_x = width//2 - appwidth //2
        offset_y = height//2 - appheight//2
        top.geometry("%dx%d%+d%+d" % (250, 100, offset_x, offset_y))
        top.title("compiling message")
        msg = Message(top,text="Compiling essential libraries for C2E2, compilation may last few minutes", width = 200)
        msg.pack(expand=TRUE, fill=BOTH)
        
        msg.update()
        #messagebox.showinfo("Information","Informative message")
        if Session.simulator == ODEINT_ADP or Session.simulator == ODEINT_FIX:
            print ("Using ODEINT Simulator")
            command_line = "g++ -w -O2 -std=c++11 simulator.cpp -o simu"
            args = shlex.split(command_line)
            p = subprocess.Popen(args, cwd= "../work-dir")
            p.communicate()
        else:
            print ("Using CAPD Simulator")
            command_line = "g++ -w -O2 simulator.cpp -o simu `../capd/bin/capd-config --cflags --libs`"
            p = subprocess.Popen(command_line, cwd= "../work-dir", shell=True)
            p.communicate()

        command_line = "g++ -fPIC -shared hybridSimGI.cpp -o libhybridsim.so -lppl -lgmp"
        args = shlex.split(command_line)
        p = subprocess.Popen(args, cwd= "../work-dir")
        p.communicate()

        command_line = "g++ -fPIC -shared bloatedSimGI.cpp -o libbloatedsim.so -lppl -lgmp"
        args = shlex.split(command_line)
        p = subprocess.Popen(args, cwd= "../work-dir")
        p.communicate()

        top.destroy()

    def _callback_ver(self):
        if not Session.cur_prop.is_valid():
            return
        self._disable_enable_button(1)
        Session.hybrid.printHybridSimGuardsInvariants();
        Session.hybrid.printBloatedSimGuardsInvariants()

        # Simulate the selected model
        self._initialize_cpp_model(0)
        Session.cpp_model.print_model()
        Session.cur_prop.simulator = self.sim_var.get()
        #FIXME for debug use
        self._compile_executable()
        
        result = Session.cpp_model.simulate_verify()
        self._disable_enable_button(0)
        self._update_property_status(0,result,0)

        if result!=0:
            if Session.cur_prop.simulator == ODEINT_ADP:
                self._open_the_plotter_window(2)
            else:
                self._open_the_plotter_window(1)

    def _set_list_property(self, name):
        iid = self.list_view.focus()
        self.list_view(iid, 0, name)


    def _initialize_cpp_model(self, simulate):
        model = Session.cpp_model

        # Initial set variables
        initial_set_obj = Session.cur_prop.initial_set_obj
        initial_mode = initial_set_obj[0]
        initial_eqns = initial_set_obj[3]
        initial_matrix = self._extract_matrix(initial_set_obj[1], initial_eqns)
        initial_b = self._extract_matrix(initial_set_obj[2], initial_eqns)
        mode_names = [m.name for m in Session.hybrid.automata[0].modes]
        initial_mode_idx = mode_names.index(initial_mode) + 1

        # Unsafe set variables
        unsafe_set_obj = Session.cur_prop.unsafe_set_obj
        unsafe_eqns = unsafe_set_obj[2]
        unsafe_matrix = self._extract_matrix(unsafe_set_obj[0], unsafe_eqns)
        unsafe_b = self._extract_matrix(unsafe_set_obj[1], unsafe_eqns)

        # Annotation
        # FIXME remove file reading and store in memory instead
        mode_linear = []
        gammas = []
        k_consts = []
        for m_i, m in enumerate(Session.hybrid.automata[0].modes):
            fn = '../work-dir/jacobiannature' + str(m_i+1) + '.txt'
            fid = open(fn, 'r').read().split('\n')
            num_var = int(fid[0])

            if num_var == 0:
                m.linear = False

            if m.linear:
                list_var = []
                for i in range(num_var):
                    list_var.append(fid[i+1])

                eqn_pos = num_var + 1
                num_eqn = int(fid[eqn_pos])
                eqn_pos += 1

                list_eqn = []
                for i in range(num_eqn):
                    list_eqn.append(fid[eqn_pos+i])

                # FIXME see if can avoid creating functions dynamically

                codestring = "def jcalc("
                codestring += "listofvar, "
                codestring += "listvalue"
                codestring+='):\n'
                codestring+=" for i in range (len(listofvar)):\n"
                codestring+="   temp = listofvar[i]\n"
                codestring+="   rightside = '='+str(listvalue[i])\n"
                codestring+="   exec(temp+rightside)\n"
                codestring+=" ret = []\n"
                for i in range (num_eqn):
                  codestring+=" "
                  codestring+=list_eqn[i]
                  codestring+='\n'
                  codestring+=' ret.append(Entry)\n'
                codestring+=' return ret'
                exec(codestring,globals())

                constant_jacobian = jcalc(list_var, np.ones((1, num_var))[0])
                constant_jacobian = np.reshape(constant_jacobian, (num_var, num_var))

                gamma_rate = np.linalg.eigvals(constant_jacobian).real
                gamma = max(gamma_rate)
                if abs(max(gamma_rate)) < 0.00001:
                    gamma = 0
                k = np.linalg.norm(constant_jacobian)

            else:
                gamma = 0
                k = Session.cur_prop.k_value

            # Append calculated value
            mode_linear.append(int(m.linear))
            gammas.append(gamma)
            k_consts.append(k)

        # Unsigned integers
        model.dimensions = len(Session.hybrid.varList)
        model.modes = len(Session.hybrid.automata[0].modes)
        model.initial_mode = initial_mode_idx
        model.initial_eqns = len(initial_eqns)
        model.unsafe_eqns = len(unsafe_eqns)
        model.annot_type = 3 

        # Integers
        if Session.refine_strat == DEF_STRAT:
            model.refine = 0
        else:
            mode.refine = 1
        model.simu_flag = simulate
        #model.simu_flag = 0

        # Integer vectors
        model.mode_linear = IntegerVector()
        model.mode_linear[:] = mode_linear

        # Doubles
        model.abs_err = 0.0000000001
        model.rel_err = 0.000000001
        model.delta_time = Session.cur_prop.time_step
        model.end_time = Session.cur_prop.time_horizon

        # Double vectors
        model.gammas = DoubleVector()
        model.k_consts = DoubleVector()
        model.initial_matrix = DoubleVector()
        model.initial_b = DoubleVector()
        model.unsafe_matrix = DoubleVector()
        model.unsafe_b = DoubleVector()

        model.gammas[:] = gammas
        model.k_consts[:] = k_consts
        model.initial_matrix[:] = initial_matrix
        model.initial_b[:] = initial_b
        model.unsafe_matrix[:] = unsafe_matrix
        model.unsafe_b[:] = unsafe_b

        # Strings
        #model.annot_str = ''
        #model.beta_str = ''
        #model.opt_str = ''
        model.visualize_filename = '../work-dir/'+Session.cur_prop.name
    
    def _extract_matrix(self, mat_in, mat_eqn):
        mat = []
        for row, eqn in zip(mat_in, mat_eqn):
            if eqn[0] == '>=':
                row_new = [float(-d) for d in row]
            else:
                row_new = [float(d) for d in row]
            mat.extend(row_new)
        return mat


    ''' PROPERTY EDITOR FUNCTIONS '''
    # TODO implement me
    def _clear_properties(self, event=None):
        for i in self.list_view.get_children():
            self.list_view.delete(i)
        self.sel_iid = None

    def _display_properties(self, event=None):
        st = 'constant'
        path = '../work-dir/simulator.cpp'
        gen_simulator(path, Session.hybrid, step_type=st)

        for prop in Session.prop_list:
            if self.sel_iid:
                self._add_property(prop)
            else:
                self.sel_iid = self._add_property(prop)

        
        self.list_view.selection_set(self.sel_iid)
        
        Session.cur_prop = Session.prop_list[0]
        Session.cur_prop.is_visible = True
        self._display_property(Session.cur_prop)
