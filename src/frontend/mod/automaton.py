import sympy
from scipy import optimize as opt

from frontend.mod.constants import *


class Automaton:

    def __init__( self, name="default_automaton" ):

        self.name = name

        self.variables = Variables()
        self.thinvariables = ThinVariables()

        self.modes = []  # Mode objects
        self.transitions = []  # Transition objects

        self.prop_list = []
                
        self.next_mode_id = 0
        self.next_transition_id = 0
        self.initial_mode_id = 0

    @property
    def vars( self ):
        return self.variables.all

    @property
    def local_vars( self ):
        return self.variables.local

    @property
    def local_var_names( self ):
        return self.variables.local_names

    @property
    def thinvars( self ):
        return self.thinvariables.all

    @property
    def local_thinvars( self ):
        return self.thinvariables.local

    @property
    def local_thinvar_names( self ):
        return self.thinvariables.local_names
        
    @property
    def mode_names( self ):
        names = []
        for mode in self.modes:
            names.append( mode.name )
        return names

    @property
    def trans( self ):
        print( "******************************************************" )
        print( " WARNING: USING DEPRECATED PROPERTY - AUTOMATON.TRANS" )
        print( "******************************************************" )
        return self.transitions

    def add_var( self, var ):
        self.variables.add_var( var )
        return

    def remove_var( self, var ):
        self.variables.remove_var( var )
        return
    
    def reset_vars( self ):
        self.variables = Variables()
        return

    def add_thinvar( self, thinvar ):
        self.thinvariables.add_thinvar( thinvar )
        return

    def remove_thinvar( self, thinvar ):
        self.thinvariables.remove_thinvar( thinvar )
        return

    def reset_thinvars( self ):
        self.thinvariables = ThinVariables()
        return

    def add_mode( self, mode ):
        self.modes.append( mode )
        return
    
    def remove_mode( self, mode ):
        self.modes.remove( mode )
        return

    def add_transition( self, tran ):
        self.transitions.append( tran )
        return

    def remove_transition( self, tran ):
        self.transitions.remove( tran )
        return

    # DEPRECATED?
        
    def new_mode_id( self ):
        m_id = self.next_mode_id
        self.next_mode_id += 1
        return m_id
    
    def new_transition_id( self ):
        t_id = self.next_transition_id
        self.next_transition_id += 1
        return t_id

    # Prints
    
    def print_trans( self ):
        print( "Transition printing under construction" )
        # print("--- Transitions ---")
        # for i in self.trans:
        #     print("%s: %s %s %s Actions: %s" % \
        #     (i.id, i.src, i.dest, i.guard.raw, 
        #     ", ".join(action.raw for action in i.actions)))
        return
    
    def print_modes( self ):
        print( "Mode printing under construction" )
        # print("--- Modes ---")
        # for i in self.modes:
        #     print('DAIs:')
        #     print(str(i.id)+": "+i.name+" Initial: "+str(i.initial)+" Linear: "+str(i.linear))
        #     print('\n'.join(dai.raw for dai in i.dais))
        #     print('Invariants:')
        #     print('\n'.join(inv.raw for inv in i.invariants))
        return
    
    def print_all( self ):
        print("%s:" % self.name)
        self.print_modes()
        self.print_trans()
        return


class Variables:
    
    def __init__(self):
    
        self.local = []
        self.input = []
        self.output = []

    @property
    def names( self ):
        names = []
        for var in self.all:
            names.append( var.name )
        return names

    @property
    def local_names( self ):
        names = []
        for var in self.local:
            names.append( var.name )
        return names

    @property
    def all( self ):
        return ( self.input + self.local + self.output )

    def add_var(self, v):
    
        if v.scope=='LOCAL_DATA':
            self.local.append(v)
        elif v.scope=='OUTPUT_DATA':
            self.output.append(v)
        elif v.scope=='INPUT_DATA':
            self.input.append(v)
    
        return


class Variable:
    
    def __init__( self, name="default_variable", update_type="", type=REAL, scope='LOCAL_DATA' ):
        self.name = name
        self.type = type
        self.scope = scope
        self.update_type = type

    def __eq__( self, other ):
        return self.name==other.name and self.update_type==other.update_type and self.type==other.type


class ThinVariables:

    def __init__( self ):
        self.local = []
        self.input = []
        self.output = []

    @property
    def names( self ):
        names = []
        for var in self.all:
            names.append( var.name )
        return names
    
    @property
    def local_names( self ):
        names = []
        for var in self.local:
            names.append( var.name )
        return names

    @property
    def all( self ):
        return ( self.input + self.local + self.output )

    def add_thinvar(self, v):

        if v.scope=='LOCAL_DATA':
            self.local.append(v)
        elif v.scope=='OUTPUT_DATA':
            self.output.append(v)
        elif v.scope=='INPUT_DATA':
            self.input.append(v)

        return


class ThinVariable:

    def __init__( self, name="default_thinvariable", update_type="", type=REAL, scope='LOCAL_DATA' ):
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
    def __init__( self, name="default_mode", id=-1, initial=False ):
        self.name = name
        self.id = id
        self.initial = initial
        self.initialConditions = []
        self.invariants = []
        self.dais = []
        self.linear = True

    @property
    def invs( self ):
        print( "************************************************" )
        print( " WARNING: USING DEPRECATED PROPERTY - MODE.INV" )
        print( "************************************************" )
        return self.invariants

    @invs.setter
    def invs( self, invariants ):
        print( "************************************************" )
        print( " WARNING: USING DEPRECATED PROPERTY - MODE.INV" )
        print( "************************************************" )
        self.invariants = invariants
        return

    def add_invariant( self, inv ):
        self.invariants.append( inv )
        return

    def remove_invariants( self, inv ):
        self.invariants.remove( inv )
        return

    def clear_invariants( self ):
        self.invariants = []
        return

    def add_dai( self, dai ):
        self.dais.append( dai )
        return

    def remove_dai( self, dai ):
        self.dais.remove( dai )
        return

    def clear_dais( self ):
        self.dais = []
        return

    def get_name( self ):
        return self.name

    def parse( self ):
        """ Parse DAI equation and Invariant Equations """
        
        self.linear = True  # LMB: Default to True, based on original code
        for dai in self.dais:
            dai_eqn = dai.parse()
            if( dai_eqn ):  # Eqn is returned only if parsing fails.
                return { 'flow': dai_eqn }
            if self.linear:
                self.linear = SymEq.is_linear( dai.expr.rhs )

        for inv in self.invariants:
            inv_eqn = inv.parse()
            if( inv_eqn ):  # Eqn is returned only if parsing fails.
                return { 'invariant': inv_eqn }
            if not inv.expr:
                self.remove_inv( inv )

        return None


class Transition:
    '''guard - node representing the guard for the transition
    actions - list of nodes representing the resets of the transition
    id - unique identifier for the transition
    src - the source of the transition
    dest - the destination of the transition
    ''' 
    def __init__( self, guard, actions, id=-1, source=-1, destination=-1 ):
        self.guard = guard
        self.actions = actions
        self.id = id
        self.source = source
        self.destination = destination
    
    @property
    def src( self ):
        print( "********************************************************" )
        print( " WARNING: USING DEPRECATED PROPERTY - TRANSITION.SOURCE" )
        print( "********************************************************" )
        return self.source

    @src.setter
    def src( self, source ):
        print( "********************************************************" )
        print( " WARNING: USING DEPRECATED PROPERTY - TRANSITION.SOURCE" )
        print( "********************************************************" )
        self.source = source
        return

    @property
    def dest( self ):
        print( "*************************************************************" )
        print( " WARNING: USING DEPRECATED PROPERTY - TRANSITION.DESTINATION" )
        print( "*************************************************************" )
        return self.destination

    @dest.setter
    def dest( self, destination ):
        print( "*************************************************************" )
        print( " WARNING: USING DEPRECATED PROPERTY - TRANSITION.DESTINATION" )
        print( "*************************************************************" )
        self.destination = destination
        return

    def parse( self ):

        guard_eqn = self.guard.parse()
        if( guard_eqn ): # Eqn is returned only if parsing fails.
            return { 'guard': guard_eqn }

        if( self.guard.expr ):
            for action in self.actions:
                action_eqn = action.parse()
                if( action_eqn ): # Eqn is returned only if parsing fails.
                    return { 'action': action_eqn }
        else:
            self.clear_actions

        return None

    def add_action( self, action ):
        self.actions.append( action )
        return

    def clear_actions( self ):
        self.actions = []
        return


class DAI:
    '''Deterministic algebraic inequalities'''

    def __init__( self, raw=None ):
        
        self.raw = raw
        self.expr = None
    
    def parse( self ):

        if( self.raw is None ):
            return "No Expression"
        
        constructed = SymEq.construct_eqn( self.raw, True, False )
        if( constructed is None ):
            return self.raw

        self.expr = constructed
        
        return None


class Invariant:

    def __init__( self, raw=None ):

        self.raw = raw
        self.expr = None

    def parse( self ):
        
        if( self.raw is None ):
            return "No Expression"

        eqns = self.raw.split( '||' )
        
        #self.expr = [ SymEq.construct_eqn( eqn, False, True ) for eqn in eqns ]
        expr = []
        for eqn in eqns:
            constructed = SymEq.construct_eqn( eqn, False, True )
            if constructed is None:
                return eqn
            expr.append( constructed )
        self.expr = expr

        # Filter out equations that evaluate to False
        self.expr = filter( lambda eqn: eqn is not False, self.expr )
        self.expr = list( self.expr )
        if True in self.expr: 
            print( 'Redundant Inv: ' + self.raw )
            self.expr = []

        return None


class Guard:

    def __init__( self, raw=None ):

        self.raw = raw
        self.expr = None

    def parse( self ):

        if( self.raw is None ):
            return "No Expression!" 

        eqns = self.raw.split( '&&' )
        
        #self.expr = [ SymEq.construct_eqn( eqn, False, True ) for eqn in eqns ]
        expr = []
        for eqn in eqns:
            constructed = SymEq.construct_eqn( eqn, False, True )
            if( constructed is None ):
                return eqn
            expr.append( constructed )
        self.expr = expr

        # Filter out equations that evaluate to True
        self.expr = filter( lambda eqn: eqn is not True, self.expr )
        self.expr = list( self.expr )
        if False in self.expr: 
            print( 'Redundant Guard: ' + self.raw )
            self.expr = None
        
        return None

        
class Action:

    def __init__( self, raw=None ):

        self.raw = raw
        self.expr = None

    def parse( self ):

        if( self.raw is None ):
            return "No Expression"

        eqns = self.raw.split('&&')

        #self.expr = [SymEq.construct_eqn(eqn, True, True) for eqn in eqns]
        expr = []
        for eqn in eqns:
            constructed = SymEq.construct_eqn( eqn, True, True )
            if( constructed is None ):
                return eqn
            expr.append( constructed )
        self.expr = expr

        return None


class SymEq:
    """ Symbolic Equation Library """

    @staticmethod
    def construct_eqn( eqn, is_eq, rationalize ):
        """ 
        Construct equation with sympy. Rationalize it if flag is set to True. is_eq is True for DAIs and Actions 
        
        NOTE: sympy.sympify(eqn) converts eqn into a type that can be used inside SymPy
        """

        try:
            if is_eq:
                eqn_split = eqn.split('=')
                lhs, rhs = eqn_split[0], eqn_split[1]
                #print (lhs, rhs)
                eqn = sympy.Eq(sympy.sympify(lhs), sympy.sympify(rhs))
            else:
                eqn = sympy.sympify(eqn)
                if type(eqn) is bool:
                    return eqn
        except:
            print("Invalid expression.")
            return None

        if rationalize:
            eqn = SymEq.rationalize(eqn)
        return eqn

    def to_str(self):
        return str(self.eqn)

    # Given an equation convert all decimals to integers by multiplying by LCM
    @staticmethod
    def rationalize(expr):
        if expr.is_Relational:
            mult = max(SymEq.get_factor(expr.lhs), SymEq.get_factor(expr.rhs))
            return expr.func(mult*expr.lhs, mult*expr.rhs)

    @staticmethod
    def get_factor(expr):
        exp = 0
        if expr.is_Add:
            terms = expr.args
        else:
            terms = [expr]

        for term in terms:
            if term.is_Number:
                exp = max(exp, SymEq.get_term_exp(term))
            for unit in term.args:
                if unit.is_Number:
                    exp = max(exp, SymEq.get_term_exp(unit))

        return 10**exp

    @staticmethod
    def get_term_exp(unit):
        return len(str(float(unit)))-str(float(unit)).index('.')-1

    # Return A, B matrices for expressions with x as varList
    @staticmethod
    def get_eqn_matrix(expressions, varList):
        exprs = []
        for expr in expressions.split('&&'):
            if '==' in expr:
                eq = expr.split('==')
                exprs.append(sympy.sympify(eq[0]+'<='+eq[1]))
                exprs.append(sympy.sympify(eq[0]+'>='+eq[1]))
            else:
                sym_expr = sympy.sympify(expr)
                if sym_expr.func==sympy.StrictLessThan:
                    sym_expr = sympy.LessThan(sym_expr.lhs, sym_expr.rhs)
                elif sym_expr.func==sympy.StrictGreaterThan:
                    sym_expr = sympy.GreaterThan(sym_expr.lhs, sym_expr.rhs)
                exprs.append(sym_expr)
        aMatrix=[[0 for v in varList] for expr in exprs]
        bMatrix=[0 for expr in exprs]
        eqMatrix=[]
        for i,expr in enumerate(exprs):
            eqMatrix.append([expr.rel_op])
            expr = expr.lhs-expr.rhs
            expr *= SymEq.get_factor(expr)
            for v in expr.free_symbols:
                aMatrix[i][varList.index(str(v))] = expr.as_coefficients_dict()[v]
                bMatrix[i] = [-expr.as_coefficients_dict()[sympy.numbers.One()]]
        return [aMatrix, bMatrix, eqMatrix]

    # Return whether the initial set is bounded or not
    @staticmethod
    def check_boundedness(a_m, b_m, eq_m, var_list):
        A, B = [], []
        for r, a in enumerate(a_m):
            if eq_m[r][0] == '<=':
                A.append(a)
                B.append(b_m[r][0])
            else:
                A.append([-1 * x for x in a])
                B.append(-1 * b_m[r][0])

        # Solve minimum
        C = [0.0 for v in var_list]
        bounds = [(None, None) for v in var_list]
        for i, v in enumerate(var_list):
            C[i] = 1.0
            min = opt.linprog(C, A_ub=A, b_ub=B, bounds=bounds)
            if not min.success:
                return False

            C[i] = 0.0

        # A, B = [], []
        # for r, a in enumerate(a_m):
        #     if eq_m[r][0] == '>=' or eq_m[r][0] == '==':
        #         A.append(a)
        #         B.append(-b_m[r][0])
        #     else:
        #         A.append([-1 * x for x in a])
        #         B.append(b_m[r][0])

        # # Solve minimum
        # C = [0.0 for v in var_list]
        # bounds = [(None, None) for v in var_list]
        # for i, v in enumerate(var_list):
        #     C[i] = 1.0
        #     min = opt.linprog(C, A_ub=A, b_ub=B, bounds=bounds)
        #     if not min.success:
        #         print ("max unbound")
        #         return False
        #     C[i] = 0.0
        return True

    # Represent u**n as u*u*...u for the simulators.
    @staticmethod
    def convert_pow(expr):
        #SymEq.pow_to_mul(expr)
        return str(SymEq.convert_pow_helper(expr))

    @staticmethod
    def convert_pow_helper(expr):
        if not expr.args:
            return expr
        conv_args = [SymEq.convert_pow_helper(arg) for arg in expr.args]
        if expr.is_Pow and expr.exp>0:
            print("this expr is pow:", expr)
            print(expr.base, expr.exp)
            return sympy.Symbol('*'.join(['('+str(conv_args[0])+')']*int(conv_args[1])))
        return expr.func(*conv_args)

    @staticmethod
    def pow_to_mul(expr):
        """
        Convert integer powers in an expression to Muls, like a**2 => a*a.
        """
        pows = list(expr.atoms(sympy.Pow))
        if any(not e.is_Integer for b, e in (i.as_base_exp() for i in pows)):
            raise ValueError("A power contains a non-integer exponent")
        print(pows)
        for b,e in (i.as_base_exp() for i in pows):
            print(b,e)
        repl = zip(pows, (sympy.Mul(*[b]*e,evaluate=False) for b,e in (i.as_base_exp() for i in pows)))
        #print repl
        return expr.subs(repl)

    # Negate guard to construct invariant
    @staticmethod
    def construct_invariant(guard):
        inv_eqn = []
        for eqn in guard.expr:
            print('Eqn is: ' + str(eqn))
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
        return Invariant('||'.join(inv_eqn))

    # Return all free vars in exprs
    @staticmethod
    def vars_used(exprs):
        var_set = set()
        for eqn in exprs:
            var_set = var_set.union(eqn.free_symbols)
        return [str(v) for v in var_set]

    # Return if expr is linear
    @staticmethod
    def is_linear(expr):
        syms = expr.free_symbols
        for x in syms:
            for y in syms:
                try:
                    if not sympy.Eq(sympy.diff(expr, x, y), 0):
                        return False
                except TypeError:
                    return False
        return True
