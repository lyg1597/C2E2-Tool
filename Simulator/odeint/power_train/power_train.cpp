/* AUTO-GENERATED SIMULATOR BY C2E2 */

#include <iostream>
#include <vector>
#include <boost/numeric/odeint.hpp>

using namespace std;
using namespace boost::numeric::odeint;

typedef vector<double> state_t;

//DATA STUCTURE
struct ObsInt
{
  vector<state_t> &m_states;
  vector<double> &m_times;

  ObsInt(vector<state_t> &states, vector<double> &times )
    : m_states(states), m_times(times) { }

  void operator()(const state_t &x, double t)
  {
    m_states.push_back(x);
    m_times.push_back(t);
  }
};

//ODE FUNCTIONS
void Mode0(const state_t &x, state_t &dxdt, const double t) {
			dxdt[0]=0.41328*(2*247*(-2.3421*x[0]*x[0]+2.7799*x[0]-0.3273)-0.9*(-3.66+0.08979*104.71975511*x[0]-0.0337*104.71975511*x[0]*x[0]+0.0001*104.71975511*104.71975511*x[0]));
			dxdt[1]=4*(13.893-35.2518*1*((1/14.7)*(-3.66+0.08979*104.71975511*x[2]-0.0337*104.71975511*x[2]*x[2]+0.0001*x[2]*104.71975511*104.71975511))+20.7364*1*1*((1/14.7)*(-3.66+0.08979*104.71975511*x[2]-0.0337*104.71975511*x[2]*x[2]+0.0001*x[2]*104.71975511*104.71975511))*((1/14.7)*(-3.66+0.08979*104.71975511*x[2]-0.0337*104.71975511*x[2]*x[2]+0.0001*x[2]*104.71975511*104.71975511))+2.6287*(0.9*(-3.66+0.08979*104.71975511*x[0]-0.0337*104.71975511*x[0]*x[0]+0.0001*x[0]*104.71975511*104.71975511))-1.592*(0.9*(-3.66+0.08979*104.71975511*x[0]-0.0337*104.71975511*x[0]*x[0]+0.0001*x[0]*104.71975511*104.71975511))*1*((1/14.7)*(-3.66+0.08979*104.71975511*x[2]-0.0337*104.71975511*x[2]*x[2]+0.0001*x[2]*104.71975511*104.71975511))-x[1]);
			dxdt[2]=0.41328*(2*1*(247)*(-2.3421*x[0]*x[0]+2.7799*x[0]-0.3273)-(-3.66+0.08979*104.71975511*x[2]-0.0337*104.71975511*x[2]*x[2]+0.0001*x[2]*104.71975511*104.71975511));
			dxdt[3]=0;
}

void Mode1(const state_t &x, state_t &dxdt, const double t) {
			dxdt[0]=0.41328*(2*247.0*(-2.3421*x[0]*x[0]+2.7799*x[0]-0.3273)-0.9*(-0.366+0.08979*104.71975511*x[0]-0.0337*104.71975511*x[0]*x[0]+0.0001*104.71975511*104.71975511*x[0]));
			dxdt[1]=4*(13.893-35.2518*1*((1/14.7)*(1+x[3]+0.04*(1*x[1]-14.7))*(-0.366+0.08979*104.71975511*x[2]-0.0337*104.71975511*x[2]*x[2]+0.0001*x[2]*104.71975511*104.71975511))+20.7364*1*1*((1/14.7)*(1+x[3]+0.04*(1*x[1]-14.7))*(-0.366+0.08979*104.71975511*x[2]-0.0337*104.71975511*x[2]*x[2]+0.0001*x[2]*104.71975511*104.71975511))*((1/14.7)*(1+x[3]+0.04*(1*x[1]-14.7))*(-0.366+0.08979*104.71975511*x[2]-0.0337*104.71975511*x[2]*x[2]+0.0001*x[2]*104.71975511*104.71975511))+2.6287*(0.9*(-0.366+0.08979*104.71975511*x[0]-0.0337*104.71975511*x[0]*x[0]+0.0001*x[0]*104.71975511*104.71975511))-1.592*(0.9*(-0.366+0.08979*104.71975511*x[0]-0.0337*104.71975511*x[0]*x[0]+0.0001*x[0]*104.71975511*104.71975511)) *1*((1/14.7)*(1+x[3]+0.04*(1*x[1]-14.7))*(-0.366+0.08979*104.71975511*x[2]-0.0337*104.71975511*x[2]*x[2]+0.0001*x[2]*104.71975511*104.71975511))-x[1]);
			dxdt[2]=0.41328*(2*1*(247.0)*(-2.3421*x[0]*x[0]+2.7799*x[0]-0.3273)-(-0.366+0.08979*104.71975511*x[2]-0.0337*104.71975511*x[2]*x[2]+0.0001*x[2]*104.71975511*104.71975511));
			dxdt[3]=0.14*(1*x[1]-14.7);
}

//ODE FUNCTION POINTER
void (*rhs[2])(const state_t &x, state_t &dxdt, const double t) =
	{Mode0, Mode1};

int main() {
	//VARIABLES
	double ts, dt, te;
	double abs_err, rel_err;
	int cur_mode;
	state_t x(4);
	runge_kutta4<state_t> stepper;

	//PARSING CONFIG
	cin >> ts;
	for (int i = 0; i < 4; i++) {
		cin >> x[i];
	}
	cin >> abs_err >> rel_err >> dt >> te >> cur_mode;
	cur_mode--;

	//INTEGRATING CONST
  /*
	int end = (int)((te - ts) / dt + 0.5);
	for (int i=0; i<end; i++) {
		//PRINTING PRE-STEP
			cout << fixed;
		cout << setprecision(9) << ts;
		for (int i = 0; i < 4; i++) {
			cout << setprecision(10) << ' ' << x[i];
		}
		cout << endl;

		stepper.do_step(rhs[cur_mode], x, ts, dt);
		ts += dt;

		//PRINTING POST-STEP
			cout << fixed;
		cout << setprecision(9) << ts;
		for (int i = 0; i < 4; i++) {
			cout << setprecision(10) << ' ' << x[i];
		}
		cout << endl;
	}
  */

  //INTEGRATING ADAPTIVE
  vector<state_t> x_vector;
  vector<double> times;

  auto stepper_adpt = make_controlled(abs_err, rel_err, runge_kutta_dopri5<state_t>());
  size_t steps = integrate_adaptive(stepper_adpt, rhs[cur_mode], x, ts, te, dt, 
      ObsInt(x_vector, times));

  //PRINTING
  for (size_t i = 0; i <= steps; i++)
  {
    cout << fixed;
    cout << setprecision(9) << times[i];
    
    for (int j = 0; j < 4; j++)
    {
      cout << setprecision(10) << ' ' << x_vector[i][j];
    }
    cout << endl;
  }
}
