from symEq import SymEq

class Automaton:
    def __init__(self, name="default_automaton"):
        self.name = name
        self.next_mode_id = 0
        self.next_transition_id = 0
        self.initial_mode_id = 0
        self.modes = []
        self.trans = []

    def print_trans(self):
        print "--- Transitions ---"
        for i in self.trans:
            print "%s: %s %s %s Actions: %s" % \
            (i.id, i.src, i.dest, i.guard.raw, 
            ", ".join(action.raw for action in i.actions))
    
    def print_modes(self):
        print "--- Modes ---"
        for i in self.modes:
            print 'DAIs:'
            print str(i.id)+": "+i.name+" Initial: "+str(i.initial)+" Linear: "+str(i.linear)
            print '\n'.join(dai.raw for dai in i.dais)
            print 'Invariants:'
            print '\n'.join(inv.raw for inv in i.invs)
    
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
        m_id = self.next_mode_id
        self.next_mode_id += 1
        return m_id
    
    def new_transition_id(self):
        t_id = self.next_transition_id
        self.next_transition_id += 1
        return t_id

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
    '''
    def __init__(self,name='',id=-1,initial=False):
        self.name = name
        self.id = id
        self.initial = initial
        self.initialConditions = []
        self.invs = []
        self.dais = []
        self.linear = True
            
    def add_inv(self,inv):
        self.invs.append(inv)
    
    def clear_inv(self):
        self.invs = []
    
    def add_dai(self,dai):
        self.dais.append(dai)
        if self.linear: self.linear = SymEq.is_linear(dai.expr.rhs)
 
class Transition:
    '''guard - node representing the guard for the transition
    actions - list of nodes representing the resets of the transition
    id - unique identifier for the transition
    src - the source of the transition
    dest - the destination of the transition
    ''' 
    def __init__(self,guard,actions,id=-1,src=-1,dest=-1):
        self.guard = guard
        self.actions = actions
        self.id = id
        self.src = src
        self.dest = dest

class DAI:
    '''Deterministic algebraic inequalities'''
    def __init__(self, raw):
        self.expr = SymEq.construct_eqn(raw, True, False)
        self.raw = str(self.expr)

class Invariant:
    def __init__(self, raw):
        self.raw = raw
        eqns = raw.split('||')
        self.expr = [SymEq.construct_eqn(eqn, False, True) for eqn in eqns]
        self.expr = filter(lambda eqn: eqn is not False, self.expr)
        if True in self.expr: 
            print 'Redundant Inv: ' + raw
            self.expr = []
        
class Guard:
    def __init__(self, raw):
        self.raw = raw
        eqns = raw.split('&&')
        self.expr = [SymEq.construct_eqn(eqn, False, True) for eqn in eqns]
        self.expr = filter(lambda eqn: eqn is not True, self.expr)
        if False in self.expr: 
            print 'Redundant Guard: ' + raw
            self.expr = []
        
class Action:
    def __init__(self, raw):
        self.raw = raw
        eqns = raw.split('&&')
        self.expr = [SymEq.construct_eqn(eqn, True, True) for eqn in eqns]

