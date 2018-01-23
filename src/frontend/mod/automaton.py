import sympy
from scipy import optimize as opt


class Automaton:

    def __init__(self, name="default_automaton"):

        self.name = name
        self.next_mode_id = 0
        self.next_transition_id = 0
        self.initial_mode_id = 0
        self.modes = []
        self.trans = []


    def print_trans(self):
        print("--- Transitions ---")
        for i in self.trans:
            print("%s: %s %s %s Actions: %s" % \
            (i.id, i.src, i.dest, i.guard.raw, 
            ", ".join(action.raw for action in i.actions)))
    
    def print_modes(self):
        print("--- Modes ---")
        for i in self.modes:
            print('DAIs:')
            print(str(i.id)+": "+i.name+" Initial: "+str(i.initial)+" Linear: "+str(i.linear))
            print('\n'.join(dai.raw for dai in i.dais))
            print('Invariants:')
            print('\n'.join(inv.raw for inv in i.invs))
    
    def print_all(self):
        print("%s:" % self.name)
        self.print_modes()
        self.print_trans()
        
    def add_mode(self,mode):
        self.modes.append(mode)
    
    def remove_mode( self, mode ):
        self.modes.remove( mode )

    def add_trans(self,trans):
        self.trans.append(trans)
        
    def remove_trans(self, tran):
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


class ThinVariables:

    def __init__(self):
        self.local = []
        self.input = []
        self.output = []

    def add_thin_var(self, v):
        if v.scope=='LOCAL_DATA':
            self.local.append(v)
        elif v.scope=='OUTPUT_DATA':
            self.output.append(v)
        elif v.scope=='INPUT_DATA':
            self.input.append(v)

    def toThinVariable(self):
        return self.input+self.local+self.output


class ThinVariable:

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
    def __init__( self, name='UNNAMED', id=-1, initial=False ):
        self.name = name
        self.id = id
        self.initial = initial
        self.initialConditions = []
        self.invs = []
        self.dais = []
        self.linear = True
            
    def add_inv( self, inv ):
        self.invs.append( inv )

    def remove_inv( self, inv ):
        self.invs.remove( inv )
    
    def clear_invs( self ):
        self.invs = []
    
    def add_dai( self, dai ):
        self.dais.append( dai )

    def remove_dai( self, dai ):
        self.dais.remove( dai )

    def clear_dais( self ):
        self.dais = []

    def get_name( self ):
        return self.name

    def construct( self ):
        """ Construct DAI equation and Invariant Equations """
        
        if( len( self.dais ) == 0 ):
            self.linear = False
        else:
            self.linear = True
            for dai in self.dais:
                dai.construct()
                if self.linear:
                    self.linear = SymEq.is_linear( dai.expr.rhs )

        for inv in self.invs:
            inv.construct()
            if not inv.expr:
                self.remove_inv( inv )
 
class Transition:
    '''guard - node representing the guard for the transition
    actions - list of nodes representing the resets of the transition
    id - unique identifier for the transition
    src - the source of the transition
    dest - the destination of the transition
    ''' 
    def __init__( self, guard, actions, id=-1, src=-1, dest=-1 ):
        self.guard = guard
        self.actions = actions
        self.id = id
        self.src = src
        self.dest = dest

    def construct( self ):
        self.guard.construct()
        for action in actions:
            action.construct()

    def add_action( self, action ):
        self.actions.append( action )

    def clear_actions( self ):
        self.actions = []

class DAI:
    '''Deterministic algebraic inequalities'''
    def __init__( self, raw ):
        self.raw = raw
    
    def construct( self ):
        self.expr = SymEq.construct_eqn( raw, True, False )

class Invariant:

    def __init__( self, raw ):
        self.raw = raw

    def construct( self ):
        eqns = self.raw.split( '||' )
        self.expr = [ SymEq.construct_eqn( eqn, False, True ) for eqn in eqns ]
        # Filter out equations that evaluate to False
        self.expr = filter( lambda eqn: eqn is not False, self.expr )
        self.expr = list( self.expr )
        if True in self.expr: 
            print('Redundant Inv: ' + self.raw)
            self.expr = []
        
class Guard:

    def __init__(self, raw):
        self.raw = raw

    def construct( self ):
        eqns = self.raw.split( '&&' )
        self.expr = [ SymEq.construct_eqn( eqn, False, True ) for eqn in eqns ]
        # Filter out equations that evaluate to True
        self.expr = filter( lambda eqn: eqn is not True, self.expr )
        self.expr = list( self.expr )
        if False in self.expr: 
            print( 'Redundant Guard: ' + self.raw )
            self.expr = []

        
class Action:

    def __init__( self, raw ):
        self.raw = raw

    def construct( self ):
        eqns = self.raw.split('&&')
        self.expr = [SymEq.construct_eqn(eqn, True, True) for eqn in eqns]

#Symbolic Equation library
class SymEq:
    # Construct equation with sympy
    # rationalize it if flag is set to True
    # is_eq is True for DAI's and actions
    @staticmethod
    def construct_eqn(eqn, is_eq, rationalize):
        try:
            if is_eq:
                eqn_split = eqn.split('=')
                lhs, rhs = eqn_split[0], eqn_split[1]
                print (lhs, rhs)
                eqn = sympy.Eq(sympy.sympify(lhs), sympy.sympify(rhs))
            else:
                eqn = sympy.sympify(eqn)
                if type(eqn) is bool:
                    return eqn
        except SyntaxError:
            print("Invalid expression.")

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
