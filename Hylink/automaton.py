class Automaton:
    def __init__(self, name="default_automaton", modes=None, trans=None):
        self.name = name
        self.next_mode_id = 0
        self.next_transition_id = 0
        self.initial_mode_id = 0

        if modes == None:
            self.modes = []
        else:
            self.modes = modes
        if trans == None:
            self.trans = []
        else:
            self.trans = trans

    def print_trans(self):
        print "--- Transitions ---"
        for i in self.trans:
            if i.extraG is None:
                extraG = ""
            else:
                extraG = "and %s " % i.extraG.raw
            print "%s: %s %s %s %sActions: %s" % \
            (i.id, i.src, i.dest, i.guard.raw, extraG, 
            ", ".join(action.raw for action in i.actions))
    
    def print_modes(self):
        print "--- Modes ---"
        for i in self.modes:
            print str(i.id)+": "+i.name+" "+str(i.initial)
            print "\n".join(dai.raw for dai in i.dais)
            print " and ".join(inv.raw for inv in i.invs)
    
    def print_all(self):
        print "%s:" % self.name
        self.print_modes()
        self.print_trans()
        
    def add_mode(self,mode):
        self.modes.append(mode)
    
    def add_trans(self,trans):
        self.trans.append(trans)
        
    def remove_tran(self, tran):
        self.trans.remove(tran)
        
    def new_mode_id(self):
        id = self.next_mode_id
        self.next_mode_id += 1
        return id
    
    def new_transition_id(self):
        id = self.next_transition_id
        self.next_transition_id += 1
        return id

class Variables:
    def __init__(self):
        self.local = []
        self.input = []
        self.output = []

    def add_var(self, v):
        if v.scope=='LOCAL_DATA':
            self.local.append(v)
        elif v.scope=='OUTPUT_DATA':
            self.output.append(v)
        elif v.scope=='INPUT_DATA':
            self.input.append(v)

    def toVariable(self):
        return self.input+self.local+self.output        
    
class Variable:
    def __init__(self,name="",update_type="",type="",scope=""):
        self.name = name
        self.type = type
        self.scope = scope
        self.update_type = type

    def __eq__(self, other):
        return self.name==other.name and self.update_type==other.update_type and self.type==other.type
        
class Mode:
    '''name - name of the mode (can be different from the one specified in simulink/stateflow
    id - uniquely identifies mode. if mode created through HyIR, then id corresponds to its index in the HyIR's modes list
    initial - True if this is the starting mode of the hybrid automata, false otherwise
    invs - list of Invariant objects representing the mode's invariants
    dais - list of DAI objects representing the mode's governing differential equations
    dummyIdx - strictly used in converting the HyIR into a hybrid automata that hytech 
               can handle.  A positive id indicates the mode's dummy mode. -1 indicates
               that a dummy mode has not been created for this mode yet.
    needDummyTran - strictly used in converting to representable automata for hytech.
                    this field is set to false when a dummy mode has already created transitions due to 
                    the fact that it has an outgoing transition with a deterministic conjunction of guard
                    conditions.  This field ensures that this process is not replicated in further 
                    iterations of the loop.
    xpos, ypos - The graphical information required to place the modes in UPPAAL
    '''
    def __init__(self,name="",id=-1,initial=False,invs=None,dais=None,dummyIdx=-1,needDummyTran=True,xpos=-1,ypos=-1):
        self.name = name
        self.id = id
        self.initial = initial
        self.initialConditions = []
        if invs is None:
            self.invs = []
        else:
            self.invs = invs
        if dais is None:
            self.dais = []
        else:
            self.dais = dais
        self.dummyIdx = dummyIdx
        self.needDummyTran = needDummyTran
        self.xpos = xpos
        self.ypos = ypos
            
    def add_inv(self,inv):
        self.invs.append(inv)
    
    def clear_inv(self):
        self.invs = []
    
    def add_dai(self,dai):
        self.dais.append(dai)
        
class Invariant:
    def __init__(self, parsed=[], raw=""):
        self.parsed = parsed
        self.raw = raw
        
class DAI:
    '''Deterministic algebraic inequalities'''
    def __init__(self,parsed,raw=""):
        self.parsed = parsed
        self.raw = raw
        
class Transition:
    '''guard - node representing the guard for the transition
    actions - list of nodes representing the resets of the transition
    id - unique identifier for the transition
    src - the source of the transition
    dest - the destination of the transition
    fac - NO IDEA
    nd - 0 indicates a deterministic transition, otherwise non-deterministic
    ap - "already parsed" - this is strictly used during the translation 
         stage to indicate that a transition has already been accounted for
         to avoid infinite loops
    extraG - 
    ''' 
    def __init__(self,guard,actions,id=-1,src=-1,dest=-1,fac=0,nd=0, ap=False, extraG=None):
        self.guard = guard
        self.actions = actions
        self.id = id
        self.src = src
        self.dest = dest
        self.fac = fac
        self.nd = nd
        self.ap = ap
        self.extraG = extraG
        
class Guard:
    def __init__(self, parsed, raw):
        self.parsed = parsed
        self.raw = raw
        
class Action:
    def __init__(self,parsed, raw):
        self.parsed = parsed
        self.raw = raw
