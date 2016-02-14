from hyir import *

def gen_simulator(file_path, hybrid_rep):
    #Obtain and parse variables
    vars = []
    for var in hybrid_rep.vars:
        if (var.scope != 'OUTPUT_DATA' and 'dot' not in var.name and 
                'clock' not in var.name):
            vars.append(var.name)

    #Obtain and parse differential algebraic equations
    modes = []
    dxdt = []
    automata = hybrid_rep.automata[0]
    for i, cur_mode in enumerate(automata.modes):
        modes.append(cur_mode.name) 
        dxdt.append([])

        #Find variables with '_dot' and extract rhs
        for dai in cur_mode.dais:
            if '_dot' in dai.raw:
                rhs = dai.raw.split('=', 1)[1]

                #Modify rhs for odeint
                for j, var in enumerate(vars):
                    rhs = rhs.replace(var, 'x[' + str(j) + ']')
                dxdt[i].append(rhs.strip()) 

    #Generate dxdt
    for i, mode in enumerate(modes):
        for j, eqn in enumerate(dxdt[i]):
            print(eqn)
            dxdt[i][j] = 'dxdt[' + str(j) + ']=' + eqn
        dxdt[i] = '\t\t\t' + ';\n\t\t\t'.join(dxdt[i]) + ';'

    #Components for .cpp
    #Auto generated comment
    auto_gen = '/* AUTO-GENERATED SIMULATOR BY C2E2 */\n'

    #Include headers
    includes = ('#include <iostream>\n'
                '#include <vector>\n'
                '#include <boost/numeric/odeint.hpp>\n')

    #Set namespaces
    namespace = ('using namespace std;\n'
                 'using namespace boost::numeric::odeint;\n')

    #Create typdef
    typedef = 'typedef vector<double> state_t;\n'

    #ODE functions
    odes = []
    for i, mode in enumerate(modes):
        ode = ['void ' + mode + '(const state_t &x, state_t &dxdt, const double t) {\n',
               dxdt[i] + '\n',
               '}\n']
        ode = ''.join(ode)
        odes.append(ode)
    odes = '//ODE FUNCTIONS\n' + '\n'.join(odes)

    #ODE function pointer
    ode_ptr = ('//ODE FUNCTION POINTER\n'
               'void (*rhs[' + str(len(modes)) + '])'
               '(const state_t &x, state_t &dxdt, const double t) =\n'
               '\t{' + ', '.join(modes) + '};\n')

    #Initialize variables
    init_vars = ['//VARIABLES\n',
                 'double ts, dt, te;\n',
                 'double abs_err, rel_err;\n',
                 'int cur_mode;\n',
                 'state_t x(' + str(len(vars)) + ');\n',
                 'runge_kutta4<state_t> stepper;\n']
    init_vars = '\t' + '\t'.join(init_vars)


    #Read configuration file
    parse = ['//PARSING CONFIG\n',
             'cin >> ts;\n',
             'for (int i = 0; i < ' + str(len(vars)) + '; i++) {\n',
             '\tcin >> x[i];\n',
             '}\n',
             'cin >> abs_err >> rel_err >> dt >> te >> cur_mode;\n',
             'cur_mode--;\n']
    parse = '\t' + '\t'.join(parse)

    #FIXME need to fix the time prevision variable
    #Pre step
    print_step = ['cout << fixed;\n',
                  'cout << setprecision(9) << ts;\n',
                  'for (int i = 0; i < ' + str(len(vars)) + '; i++) {\n',
                  '\tcout << setprecision(10) << \' \' << x[i];\n',
                  '}\n',
                  'cout << endl;\n']
    print_step = '\t\t' + '\t\t'.join(print_step)
    
    #Integrate ODE
    integrate = ['//INTEGRATING\n',
                 'int end = (int)((te - ts) / dt + 0.5);\n',
                 'for (int i=0; i<end; i++) {\n',
                 '\t//PRINTING PRE-STEP\n',
                 print_step + '\n',
                 '\tstepper.do_step(rhs[cur_mode], x, ts, dt);\n',
                 '\tts += dt;\n\n',
                 '\t//PRINTING POST-STEP\n',
                 print_step,
                 '}\n']
    integrate = '\t' + '\t'.join(integrate)

    #Generate main
    main = ('int main() {\n'
            + init_vars + '\n'
            + parse + '\n'
            + integrate +
            '}')

    cpp_file = (auto_gen + '\n'
                + includes + '\n'
                + namespace + '\n'
                + typedef + '\n'
                + odes + '\n'
                + ode_ptr + '\n'
                + main)

    #Generate .cpp file
    file = open(file_path, 'w')
    file.write(cpp_file);
    file.close();
