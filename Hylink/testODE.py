#TODO
#replace all hyb with self after incorporating it with hyir.py

#def generate_odeint_simulator(self, hyb):
def generate_odeint_simulator(hyb):
    
    print('TESTING ODEINT')


    file = open('../Testing/odeint/simulator.cpp', 'w')

    info = '/* AUTO-GENERATED FILE BY C2E2 */\n\n'
    declarations = ('#include <iostream>\n'
                    '#include <vector>\n'
                    '#include <boost/numeric/odeint.hpp>\n\n')
    typedef = 'typedef std::vector<double> state_type;\n\n'

    #Find all vars and equations
    for var in hyb.vars
    print(hyb.vars)

    #ode_func = 

    ode_class = ('class ode\n'
                 '{\n'
                 'public:'
                 '\tvoid operator() (const state_type &x, state_type &dxdt, const double /* t*/)\n'
                 '{\n')
                 
