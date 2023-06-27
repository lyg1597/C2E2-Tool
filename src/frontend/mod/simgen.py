import numpy as np
import shlex, subprocess
import sympy

from backend.lib.libc2e2 import IntegerVector, DoubleVector
from frontend.mod.constants import *
from frontend.mod.hyir import *
from frontend.mod.jacobiancalc import *
from frontend.mod.session import Session, Property

# Debugging
import pdb

# FIXME make a single file for gen_simulator which will call 
# either the hyxml one or the Simulink c++ generated one
def gen_simulator_master(file_path, **kwargs):
    # Parse kwargs
    sim_type = kwargs.pop('sim_type')

    if sim_type == 'hyxml':
        print(sim_type)
    elif sim_type == 'simulink':
        print(sim_type)
    else:
        print('Incorrect simulation type')


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

    for i, cur_mode in enumerate(hybrid_rep.modes):
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
        del_elem = jacobian(var_str, orig_eqns, i)
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

    # Create typedef
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
    print_steps = ['//PRINTING TRACE',
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
    file.write(cpp_file)
    file.close()


def gen_simulator_simulink(dir_path, model_name, in_labels, in_vals, 
        out_labels, time):
    """Generates the simulator for simulink

    Notes:
        The generated file can't handle varying initial times and assumes
        that they start at 0. However, the code is set up in such a way to
        allow this to be implemented in the future.
        I should allow a variable which says whether the first data point
        is time or not: in_len = time ? len(in_labels + 1) : len(in_labels)
    """

    # Variables
    model_class = model_name + 'ModelClass'
    model_inst = model_name + '_Obj'

    # NOTE: name seems to be limited to 30 chars
    model_rt = ('RT_MODEL_' + model_name)[:29] + '_T'

    pnt_len = len(out_labels) + 1
    pnt_len_str = str(pnt_len)

    src_path = dir_path + '/' + model_name + '_ert_rtw'

    # Create model inputs/outputs
    model_in = []
    for il in in_labels:
        model_in.append(model_inst + '.' + model_name + '_U.' + il)

    model_out = []
    for ol in out_labels:
        model_out.append(model_inst + '.' + model_name + '_Y.' + ol)

    # Share components
    # Auto generated comment
    auto_gen = '/* AUTO-GENERATED SIMULATOR BY C2E2 */\n'

    # Components for .h
    # Header guard
    guard_start = ('#ifndef GENERATOR_H\n'
                   '#define GENERATOR_H\n')

    guard_end = '#endif /* GENERATOR_H */\n'

    # Includes
    h_includes = ('#include <vector>\n'
                  '#include "' + model_name + '.h"\n')
                  
    # Alias templates
    aliases = ('using Point = std::vector<double>;\n'
               'using Trace = std::vector<Point>;\n')

    # Function definition
    func_decl = ('extern "C" Trace generate_trace(Point &point, '
                     'double delta_time, double end_time);\n')

    # Generate header file
    header_file = (auto_gen + '\n'
                   + guard_start + '\n'
                   + h_includes + '\n'
                   + aliases + '\n'
                   + func_decl + '\n'
                   + guard_end)

    # Write header file
    header_path = src_path + '/Generator.h'
    header = open(header_path, 'w');
    header.write(header_file);
    header.close();

    # Components for .cpp
    # Include headers
    includes = '#include "Generator.h"\n'

    # Generate gen_trace variables
    func_vars = ('\t// VARIABLES\n'
                 '\tPoint trace_point(' + pnt_len_str + ', 0);\n'
                 '\tTrace trace;\n'
                 '\t' + model_class + ' ' + model_inst + ';\n'
                 '\t' + model_rt + ' *time_info = ' + model_inst + 
                      '.getRTM();\n')

    # Initialize model inputs
    func_init = []
    init_idx = 1
    func_init.append('// INITIALIZE MODEL')
    func_init.append(model_inst + '.initialize();')
    func_init.append('time_info->Timing.stepSize0 = delta_time;')
    for i, mi in enumerate(model_in):
        if in_vals[i][0] == 'constant':
            func_init.append(mi + ' = ' + str(in_vals[i][1]) + ';')
        else:
            func_init.append(mi + ' = point[' + str(init_idx) + '];')
            init_idx += 1

    func_init = '\t' + '\n\t'.join(func_init) + '\n'

    # Store intial inputs
    func_save_in = []
    func_save_in.append('// SAVE INITIAL POINT')
    for i in xrange(0, pnt_len):
        n = str(i)
        func_save_in.append('trace_point[' + n + '] = point[' + n + '];')
    func_save_in.append('trace.push_back(trace_point);\n')
    func_save_in = '\t' + '\n\t'.join(func_save_in)

    # Save model outputs each step func_save = []
    func_save_out = []
    func_save_out.append('trace_point[0] = time_info->Timing.tArray[0];');
    for i, mo in enumerate(model_out): 
        func_save_out.append('trace_point[' + str(i + 1) + '] = ' + mo + ';') 
    func_save_out = '\t\t' + '\n\t\t'.join(func_save_out)

    # Simulate model
    func_sim = ('\t// SIMULATE MODEL\n' 
                '\twhile (time_info->Timing.tArray[0] < end_time) {\n'
                '\t\t' + model_inst + '.step();\n'
                + func_save_out + '\n'
                '\t\ttrace.push_back(trace_point);\n'
                '\t}\n')


    # Terminate model
    func_term = ('\t// TERMINATING MODEL\n'
                 '\t' + model_inst + '.terminate();\n'
                 '\treturn trace;\n')

    # Create generate_trace function
    func_def = ('// TRACE GENERATOR FUNTION\n'
                'Trace generate_trace(Point &point, double delta_time, '
                    'double end_time) {\n'
                + func_vars + '\n'
                + func_init + '\n'
                + func_save_in + '\n'
                + func_sim + '\n'
                + func_term + 
                '}\n')

    # Generate CPP file
    cpp_file = (auto_gen + '\n'
                + includes + '\n'
                + func_def)

    # Write CPP file
    cpp_path = src_path + '/Generator.cpp'
    cpp = open(cpp_path, 'w');
    cpp.write(cpp_file);
    cpp.close();

    # Componenets for Makefile
    # Opening make file
    makefile_path = src_path + '/' + model_name + '.mk'
    with open(makefile_path, 'r') as makefile:
        data = []
        ignore_count = 0

        # Read file line by line
        for line in makefile:

            # Ignoring lines
            # Check ignore count
            if ignore_count > 0:
                ignore_count -= 1
                continue

            # Modify line if necessary
            # Update to C++11
            if '-std=c++98' in line:
                line = line.replace('-std=c++98', '-std=c++11')

            # Modify SRCS
            elif line.startswith('SRCS = ') and not 'Generator.cpp' in line:
                line = line.rstrip()
                line += ' $(START_DIR)/' + model_name + \
                        '_ert_rtw/Generator.cpp\n'

            # Modify OBJS
            elif line.startswith('OBJS = ') and not 'Generator.o' in line:
                line = line.rstrip()
                line += ' Generator.o\n'

            # Modify TARGET
            elif line.startswith('$(PRODUCT) :'):
                ignore_count = 3
                new_target = ('\t@echo "### Creating standalone executable '
                                  '"$(PRODUCT)" ..."\n'
	                      '\t$(CPP_LD) $(CPP_LDFLAGS) -shared -o '
                                  '$(PRODUCT) $(OBJS) $(SYSTEM_LIBS) '
                                  '$(TOOLCHAIN_LIBS)\n'
	                      '\t@echo "### Created: $(PRODUCT)"\n')
                line += new_target
            
            # Append line to data
            data.append(line)

        # Write data to new Makefile
        makefile = open(makefile_path, 'w')
        makefile.write(''.join(data))
        makefile.close()

def simulate():

    result = _sim_ver(1)

    if(not result):
        Session.cur_prop.status = "Invalid" 
        Session.cur_prop.result = "Invalid"
        return None

    Session.cur_prop.status = Simulated

    if(result == 1):
        Session.cur_prop.result = "Safe"
    elif(result == 0):
        Session.cur_prop.result = "Unknown"
    else:
        Session.cur_prop.result = "Unsafe"
    
    return result

def verify():

    result = _sim_ver(0)

    if(not result):
        Session.cur_prop.status = "Invalid" 
        Session.cur_prop.result = "Invalid"
        return None
        
    Session.cur_prop.status = Verified
    if(result == 1):
        Session.cur_prop.result = "Safe"
    elif(result == 0):
        Session.cur_prop.result = "Unknown"
    else:
        Session.cur_prop.result = "Unsafe"
    
    return result

def _sim_ver(action):

    # Parse and Compose (HyIR.compose_all calss HyIR.parse_all)
    HyIR.compose_all(Session.hybrid)

    if(not Session.cur_prop.is_valid()):
        Session.write("Property invalid, abandoning operation...\n")
        return None

    if(not Session.hybrid.composed):
        Session.write("ERROR: System not composed, abandoning operation...\n")
        return None

    # Generate Simulator
    if(Session.simulator == CAPD):
        Session.hybrid.convertToCAPD('simulator')
    else:
        if(Session.simulator == ODEINT_FIX):
            st = 'constant'
        elif(Session.simulator == ODEINT_ADP):
            st = 'adaptive'
        path = '../work-dir/simulator.cpp'
        gen_simulator(path, Session.hybrid, step_type=st)

    Session.hybrid.printHybridSimGuardsInvariants()
    Session.hybrid.printBloatedSimGuardsInvariants()

    # Simulate selected model
    initialize_cpp_model(action)
    compile_executable()

    Session.write("Running simulate/verify...\n")
    Session.update()
    result = Session.cpp_model.simulate_verify()
    Session.write("RESULT: " + str(result) + "\n")

    return result
    

def initialize_cpp_model(sim_bool):

    model = Session.new_cpp_model()

    # Initialize set variables
    initial_set_obj = Session.cur_prop.initial_set_obj
    initial_mode = initial_set_obj[0]
    initial_eqns = initial_set_obj[3]
    initial_matrix = extract_matrix(initial_set_obj[1], initial_eqns)
    initial_b = extract_matrix(initial_set_obj[2], initial_eqns)
    mode_names = Session.hybrid.mode_names
    initial_mode_idx = mode_names.index(initial_mode) + 1

    # Unsafe set variables
    unsafe_set_obj = Session.cur_prop.unsafe_set_obj
    unsafe_eqns = unsafe_set_obj[2]
    unsafe_matrix = extract_matrix(unsafe_set_obj[0], unsafe_eqns)
    unsafe_b = extract_matrix(unsafe_set_obj[1], unsafe_eqns)

    # FIXME remove file readind and store in memory instead
    mode_linear = []
    gammas = []
    k_consts = []
    for m_i, m in enumerate(Session.hybrid.modes):
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

            # FIXME see if we can avoid create functions dynamically

            codestring  = "def jcalc("
            codestring += "listofvar, "
            codestring += "listvalue"
            codestring += '):\n'
            codestring += " for i in range (len(listofvar)):\n"
            codestring += "   temp = listofvar[i]\n"
            codestring += "   rightside = '='+str(listvalue[i])\n"
            codestring += "   exec(temp+rightside)\n"
            codestring += " ret = []\n"
            for i in range (num_eqn):
                codestring += " "
                codestring += list_eqn[i]
                codestring += '\n'
                codestring += ' ret.append(Entry)\n'
            codestring += ' return ret'
            exec(codestring, globals())

            constant_jacobian = jcalc(list_var, np.ones((1, num_var))[0])
            constant_jacobian = np.reshape(constant_jacobian, (num_var, num_var))

            gamma_rate = np.linalg.eigvals(constant_jacobian).real
            gamma = max(gamma_rate)
            if(abs(max(gamma_rate)) < 0.00001):
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
    model.dimensions = len(Session.hybrid.local_var_names)
    model.modes = len(Session.hybrid.modes)
    model.initial_mode = initial_mode_idx
    model.initial_eqns = len(initial_eqns)
    model.unsafe_eqns = len(unsafe_eqns)
    model.annot_type = 3

    # Integers
    if(Session.refine_strat == DEF_STRAT):
        model.refine = 0
    else:
        model.refine = 1
    model.simu_flag = sim_bool

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

    model.visualize_filename = '../work-dir/' + Session.cur_prop.name
    writeModelFile(model)

def writeModelFile(model):
    code_file=open("../work-dir/main.cpp","w+")
    code_file.write('#include "model.hpp"\n')
    code_file.write("#define SIMU 1\n")
    code_file.write("#define VERI 0\n")
    code_file.write("int main(){\n")
    code_file.write("    Model cpp_Model;\n")
    code_file.write("    cpp_Model.dimensions="+str(model.dimensions)+";\n")
    code_file.write("    cpp_Model.modes="+str(model.modes)+";\n")
    code_file.write("    cpp_Model.initial_mode="+str(model.initial_mode)+";\n")
    code_file.write("    cpp_Model.initial_eqns="+str(model.initial_eqns)+";\n")
    code_file.write("    cpp_Model.unsafe_eqns="+str(model.unsafe_eqns)+";\n")
    code_file.write("    cpp_Model.annot_type="+str(model.annot_type)+";\n")
    code_file.write("    cpp_Model.refine="+str(model.refine)+";\n")
    code_file.write("    cpp_Model.simu_flag=VERI;\n")
    for i in range(len(model.mode_linear)):
        code_file.write("    cpp_Model.mode_linear.push_back("+str(model.mode_linear[i])+");\n")
    code_file.write("    cpp_Model.abs_err="+str(model.abs_err)+";\n")
    code_file.write("    cpp_Model.rel_err="+str(model.rel_err)+";\n")
    code_file.write("    cpp_Model.delta_time="+str(model.delta_time)+";\n")
    code_file.write("    cpp_Model.end_time="+str(model.end_time)+";\n")
    for i in range(len(model.gammas)):
        code_file.write("    cpp_Model.gammas.push_back("+str(model.gammas[i])+");\n")
    
    for i in range(len(model.k_consts)):
        code_file.write("    cpp_Model.k_consts.push_back("+str(model.k_consts[i])+");\n")

    for i in range(len(model.initial_matrix)):
        code_file.write("    cpp_Model.initial_matrix.push_back("+str(model.initial_matrix[i])+");\n")

    for i in range(len(model.initial_b)):
        code_file.write("    cpp_Model.initial_b.push_back("+str(model.initial_b[i])+");\n")

    for i in range(len(model.unsafe_matrix)):
        code_file.write("    cpp_Model.unsafe_matrix.push_back("+str(model.unsafe_matrix[i])+");\n")

    for i in range(len(model.unsafe_b)):
        code_file.write("    cpp_Model.unsafe_b.push_back("+str(model.unsafe_b[i])+");\n")

    code_file.write('    cpp_Model.visualize_filename="'+model.visualize_filename+'";\n')
    code_file.write("    cpp_Model.simulate_verify();\n")
    code_file.write("    return 0;\n") 
    code_file.write("}\n")      
    code_file.close()

def extract_matrix(mat_in, mat_eqn):
    
    mat = []
    for row, eqn in zip(mat_in, mat_eqn):
        if(eqn[0] == '>='):
            row_new = [float(-d) for d in row]
        else:
            row_new = [float(d) for d in row]
        mat.extend(row_new)
    return mat


def compile_executable():

    if(Session.lib_compiled):
        return

    Session.write("Compiling essential libraries for C2E2...\n")
    Session.write("  Compilation may take a few minutes.\n")
    if((Session.simulator == ODEINT_ADP) or (Session.simulator == ODEINT_FIX)):
        Session.write("  Using ODEINT Simulator.\n")
        Session.update()
        command_line = "g++ -g -w -O2 -std=c++11 simulator.cpp -o simu"
        args = shlex.split(command_line)
        p = subprocess.Popen(args, cwd= "../work-dir")
        print()
        p.communicate()
    else:
        Session.write("  Using CAPD Simulator.\n")
        Session.update()
        command_line = "g++ -g -w -O2 simulator.cpp -o simu `../capd/bin/capd-config --cflags --libs`"
        p = subprocess.Popen(command_line, cwd= "../work-dir", shell=True)
        print()
        p.communicate()

    command_line = "g++ -g -fPIC -shared hybridSimGI.cpp -o libhybridsim.so -lppl -lgmp"
    args = shlex.split(command_line)
    p = subprocess.Popen(args, cwd= "../work-dir")
    p.communicate()

    command_line = "g++ -g -fPIC -shared bloatedSimGI.cpp -o libbloatedsim.so -lppl -lgmp"
    args = shlex.split(command_line)
    p = subprocess.Popen(args, cwd= "../work-dir")
    p.communicate()

    Session.lib_compiled = True
    
    Session.write("Libraries successfully compiled.\n")
    Session.update()

    return