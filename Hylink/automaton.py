import sympy
import re

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
        eqns = raw.split('||')
        self.expr = [SymEq.construct_eqn(eqn, False, True) for eqn in eqns]
        
class DAI:
    '''Deterministic algebraic inequalities'''
    def __init__(self,parsed,raw=""):
        self.parsed = parsed
        self.raw = raw
        self.expr = SymEq.construct_eqn(raw, True, False)
        
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
        eqns = raw.split('&&')
        self.expr = [SymEq.construct_eqn(eqn, False, True) for eqn in eqns]
        
class Action:
    def __init__(self,parsed, raw):
        self.parsed = parsed
        self.raw = raw
        self.expr = SymEq.construct_eqn(raw, True, True)

class SymEq:
    @staticmethod
    def construct_eqn(eqn, is_eq, rationalize):
        print 'Raw: ' + eqn
        try:
            if is_eq:
                eqn_split = eqn.split('=')
                lhs, rhs = eqn_split[0], eqn_split[1]
                eqn = sympy.Eq(sympy.sympify(lhs), sympy.sympify(rhs))
            else:
                eqn = sympy.sympify(eqn)
        except SyntaxError:
            print "Invalid expression."

        if rationalize:
            eqn = SymEq.rationalize(eqn)
        print 'Eq: ' + str(eqn)
        return eqn

    def to_str(self):
        return str(self.eqn)

    @staticmethod
    def rationalize(expr):
        if expr.is_Relational:
            return expr.func(SymEq.rationalize(expr.lhs), SymEq.rationalize(expr.rhs))

        exp = 0
        if expr.is_Add:
            terms = expr.args
        else:
            terms = [expr]

        for term in terms:
            for unit in term.args:
                if unit.is_Number:
                    term_exp = len(str(float(unit)))-str(float(unit)).index('.')-1
                    exp = max(term_exp, exp)

        return expr*(10**exp)

    @staticmethod
    def convert_pow(expr):
        pow_eq = str(SymEq.convert_pow_helper(expr))
        pow_l = re.findall('(pow\(([a-zA-Z0-9]*), ([0-9]*)\))', pow_eq)
        for pow_t, pow_b, pow_e in pow_l:
            pow_eq = pow_eq.replace(pow_t, '*'.join([pow_b]*int(pow_e)))
        return pow_eq

    @staticmethod
    def convert_pow_helper(expr):
        if not expr.args:
            return expr

        conv_args = [SymEq.convert_pow_helper(arg) for arg in expr.args]
        if expr.is_Pow:
            return sympy.sympify('pow('+str(conv_args[0])+','+str(conv_args[1])+')')

        return expr.func(*conv_args)

    @staticmethod
    def construct_invariant(guard):
        inv_eqn = []
        for eqn in guard.expr:
            print 'Eqn is: ' + str(eqn)
            if eqn.func==sympy.LessThan:
                inv = sympy.GreaterThan(*eqn.args)
            elif eqn.func==sympy.GreaterThan:
                inv = sympy.LessThan(*eqn.args)    
            elif eqn.func==sympy.StrictLessThan:
                inv = sympy.StrictGreaterThan(*eqn.args)    
            elif eqn.func==sympy.StrictGreaterThan:
                inv = sympy.StrictLessThan(*eqn.args)
            inv_eqn.append(inv)
        inv_eqn = [str(inv) for inv in inv_eqn]
        return Invariant(raw='||'.join(inv_eqn))
