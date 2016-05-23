from hyir import *
from jacobiancalc import *
import sympy
# import re

def gen_simulator(file_path, hybrid_rep, **kwargs):
    # Get kwargs
    step_type = kwargs.pop('step_type', 'adaptive')

    # Setting spefic variables
    if step_type == 'constant':
        integrator = ['runge_kutta4<state_t> stepper;',
                      'size_t steps = integrate_const(stepper, rhs[cur_mode], '
                      'x, ts, te, dt,'
                      '\tIntObs(trace, times));']
    else:
        integrator = ['auto stepper = make_controlled(abs_err, rel_err, '
                      'runge_kutta_dopri5<state_t>());',
                      'size_t steps = integrate_adaptive(stepper, '
                      'rhs[cur_mode], x, ts, te, dt,',
                      '\tIntObs(trace, times));']

    # Obtain and parse variables
    vars = []
    for var in hybrid_rep.vars:
        if (var.scope == 'LOCAL_DATA' and 'dot' not in var.name and 
                'clock' not in var.name):
            vars.append(var.name)


    # Iterate through all the mode of the automata to
    # obtain and parse differential algebraic equations
    # dxdt is a list modes, each with a list of equations
    modes = []
    dxdt = []
    del_list = []

    # FIXME allow for more automata in the future
    automata = hybrid_rep.automata[0] 

    for i, cur_mode in enumerate(automata.modes):
        modes.append(cur_mode.name) 
        dxdt.append([])
        orig_eqns = []

        # Find variables with '_dot' and extract rhs
        for dai in cur_mode.dais:
            if '_dot' in dai.raw:
                # Split the equation and get lhs index
                # lhs, rhs = dai.raw.split('=')
                lhs, rhs = str(dai.expr.lhs), dai.expr.rhs
                lhs = lhs.split('_dot')[0] 
                lhs_idx = vars.index(lhs)
                #print "orig: ", rhs
                #rhs = SymEq.convert_pow(rhs)
                #print "convert: ", rhs
                rhs = str(rhs)
                # Generate jacobian in correct order
                orig_eqns.insert(lhs_idx,rhs)

                # Replace variables with 'x[i]'
                rhs = dai.expr.rhs
                for j, var in enumerate(vars):
                    rhs = rhs.subs(var, sympy.Symbol('x['+str(j)+']'))
                    # rhs = re.sub(r'\b%s\b' % var, 'x[' + str(j) + ']', rhs)
                rhs = SymEq.convert_pow(rhs)


                # Generate dxdt in correct order
                dxdt[i].insert(lhs_idx, rhs) 

        # Bloat factor calculation/generation
        var_str = ','.join(vars)
        eqn_str = ','.join(orig_eqns)
        del_elem = jacobian(var_str, eqn_str, i)
        del_list.append(del_elem)

    # Generate python file for bloating
    createCDFfunction(del_list)

    # Generate dxdt
    for i, mode in enumerate(modes):
        for j, eqn in enumerate(dxdt[i]):
            dxdt[i][j] = 'dxdt[' + str(j) + ']=' + eqn
        dxdt[i] = '\t\t\t' + ';\n\t\t\t'.join(dxdt[i]) + ';'

    # Components for .cpp
    # Auto generated comment
    auto_gen = '/* AUTO-GENERATED SIMULATOR BY C2E2 */\n'

    # Include headers
    includes = ('# include <iostream>\n'
                '# include <vector>\n'
                '# include <boost/numeric/odeint.hpp>\n'
                '# include <math.h>\n')

    # Set namespaces
    namespace = ('using namespace std;\n'
                 'using namespace boost::numeric::odeint;\n')

    # Create typdef
    typedef = 'typedef vector<double> state_t;\n'

    # Create integrator observer
    int_obs = ('//INTEGRATOR OBSERVER\n'
               'class IntObs {\n'
               '\tprivate:\n'
               '\t\tvector<state_t> &io_states;\n'
               '\t\tvector<double> &io_times;\n\n'
               '\tpublic:\n'
               '\t\tIntObs(vector<state_t> &states, vector<double> &times)\n'
               '\t\t\t: io_states(states), io_times(times) { }\n\n'
               '\t\tvoid operator()(const state_t &x, double t) {\n'
               '\t\t\tio_states.push_back(x);\n'
               '\t\t\tio_times.push_back(t);\n'
               '\t\t}\n'
               '};\n')

    # ODE functions
    odes = []
    for i, mode in enumerate(modes):
        ode = ['void ' + mode + '(const state_t &x, state_t &dxdt, const double t) {\n',
               dxdt[i] + '\n',
               '}\n']
        ode = ''.join(ode)
        odes.append(ode)
    odes = '//ODE FUNCTIONS\n' + '\n'.join(odes)

    # ODE function pointer
    ode_ptr = ('//ODE FUNCTION POINTER\n'
               'void (*rhs[' + str(len(modes)) + '])'
               '(const state_t &x, state_t &dxdt, const double t) =\n'
               '\t{' + ', '.join(modes) + '};\n')

    # Initialize variables
    init_vars = ['//VARIABLES',
                 'double ts, dt, te;',
                 'double abs_err, rel_err;',
                 'int cur_mode;',
                 'state_t x(' + str(len(vars)) + ');',
                 'vector<double> times;',
                 'vector<state_t> trace;']
    init_vars = '\t' + '\n\t'.join(init_vars) + '\n'

    # Read configuration file
    parse = ['//PARSING CONFIG',
             'cin >> ts;',
             'for (int i = 0; i < ' + str(len(vars)) + '; i++) {',
             '\tcin >> x[i];',
             '}',
             'cin >> abs_err >> rel_err >> dt >> te >> cur_mode;',
             'cur_mode--;']
    parse = '\t' + '\n\t'.join(parse) + '\n'

    # Integrate ODE
    integrate = ['//INTEGRATING']
    integrate.extend(integrator)
    integrate = '\t' + '\n\t'.join(integrate) + '\n'

    # FIXME have it only print the steps once without duplicate
    # Print step
    # FIXME code that I should be using
    """
    print_steps = ['//PRINTING STEPS',
                   'for (size_t i = 0; i <= steps; i++) {',
                   '\tcout << fixed;',
                   '\tcout << setprecision(9) << times[i];',
                   '\tfor (int j = 0; j < ' + str(len(vars)) + '; j++) {',
                   '\t\tcout << setprecision(10) << \' \' << trace[i][j];',
                   '\t}',
                   '\tcout << endl;',
                   '}']
    print_steps = '\t' + '\n\t'.join(print_steps)
    """

    # FIXME temporary to match CAPD simulator output
    print_steps = ['//PRINTING STEPS',
                   'for (size_t i = 0; i <= steps; i++) {',
                   '\tcout << fixed;',
                   '\tcout << setprecision(9) << times[i];',
                   '\tfor (int j = 0; j < ' + str(len(vars)) + '; j++) {',
                   '\t\tcout << setprecision(10) << \' \' << trace[i][j];',
                   '\t}',
                   '\tcout << endl;\n',
                   '\tif (i != 0 && i != steps) {',
                   '\t\tcout << fixed;',
                   '\t\tcout << setprecision(9) << times[i];',
                   '\t\tfor (int j = 0; j < ' + str(len(vars)) + '; j++) {',
                   '\t\t\tcout << setprecision(10) << \' \' << trace[i][j];',
                   '\t\t}',
                   '\t\tcout << endl;',
                   '\t}',
                   '}']
    print_steps = '\t' + '\n\t'.join(print_steps)
    
    # Generate main
    main = ['int main() {',
            init_vars,
            parse,
            integrate,
            print_steps,
            '}']
    main = '\n'.join(main)

    # Generate CPP file
    cpp_file = [auto_gen,
                includes,
                namespace,
                typedef,
                int_obs,
                odes,
                ode_ptr,
                main]
    cpp_file = '\n'.join(cpp_file)

    # Write CPP file
    file = open(file_path, 'w')
    file.write(cpp_file);
    file.close();
