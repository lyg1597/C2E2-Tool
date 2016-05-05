import sympy

#Symbolic Equation library
class SymEq:
    @staticmethod
    def construct_eqn(eqn, is_eq, rationalize):
        try:
            if is_eq:
                eqn_split = eqn.split('=')
                lhs, rhs = eqn_split[0], eqn_split[1]
                eqn = sympy.Eq(sympy.sympify(lhs), sympy.sympify(rhs))
            else:
                eqn = sympy.sympify(eqn)
                if type(eqn) is bool:
                    return eqn
        except SyntaxError:
            print "Invalid expression."

        if rationalize:
            eqn = SymEq.rationalize(eqn)
        return eqn

    def to_str(self):
        return str(self.eqn)

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
        # exprs = [sympy.sympify(expr) for expr in exprs.split('&&')]
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

    @staticmethod
    def convert_pow(expr):
        return str(SymEq.convert_pow_helper(expr))

    @staticmethod
    def convert_pow_helper(expr):
        if not expr.args:
            return expr

        conv_args = [SymEq.convert_pow_helper(arg) for arg in expr.args]
        if expr.is_Pow:
            return sympy.Symbol('*'.join(['('+str(conv_args[0])+')']*int(conv_args[1])))
        return expr.func(*conv_args)

    # @staticmethod
    # def convert_pow(expr):
    #     pow_eq = str(SymEq.convert_pow_helper(expr))
    #     pow_l = re.findall('(pow\(([a-zA-Z0-9\[\]]*), ([0-9]*)\))', pow_eq)
    #     for pow_t, pow_b, pow_e in pow_l:
    #         pow_eq = pow_eq.replace(pow_t, '*'.join([pow_b]*int(pow_e)))
    #     return pow_eq

    # @staticmethod
    # def convert_pow_helper(expr):
    #     if not expr.args:
    #         return expr

    #     conv_args = [SymEq.convert_pow_helper(arg) for arg in expr.args]
    #     if expr.is_Pow:
    #         return sympy.sympify(sympy.Function('pow')(conv_args[0],conv_args[1]))
    #     return expr.func(*conv_args)

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

    @staticmethod
    def vars_used(exprs):
        var_set = set()
        for eqn in exprs:
            var_set = var_set.union(eqn.free_symbols)
        return [str(v) for v in var_set]

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
