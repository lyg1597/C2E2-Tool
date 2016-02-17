/* AUTO-GENERATED SIMULATOR BY C2E2 */

#include <iostream>
#include <vector>
#include <boost/numeric/odeint.hpp>

using namespace std;
using namespace boost::numeric::odeint;

typedef vector<double> state_t;

//INTEGRATOR OBSERVER
class IntObs {
	private:
		vector<state_t> &io_states;
		vector<double> &io_times;

	public:
		IntObs(vector<state_t> &states, vector<double> &times)
			: io_states(states), io_times(times) { }

		void operator()(const state_t &x, double t) {
			io_states.push_back(x);
			io_times.push_back(t);
		}
};

//ODE FUNCTIONS
void VO(const state_t &x, state_t &dxdt, const double t) {
			dxdt[0]=1.4*x[2]-0.9*x[0];
			dxdt[1]=2.5*x[4]-1.5*x[1];
			dxdt[2]=0.6*x[6]-0.8*x[2]*x[1];
			dxdt[3]=2.0-1.3*x[3]*x[2];
			dxdt[4]=0.7*x[0]-1.0*x[3]*x[4];
			dxdt[5]=0.3*x[0]-3.1*x[5];
			dxdt[6]=1.8*x[5]-1.5*x[6]*x[1];
      dxdt[7]=2;
}

//ODE FUNCTION POINTER
void (*rhs[1])(const state_t &x, state_t &dxdt, const double t) =
	{VO};

int main() {
	//VARIABLES
	double ts, dt, te;
	double abs_err, rel_err;
	int cur_mode;
	state_t x(8);
	vector<double> times;
	vector<state_t> trace;

	//PARSING CONFIG
	cin >> ts;
	for (int i = 0; i < 8; i++) {
		cin >> x[i];
	}
	cin >> abs_err >> rel_err >> dt >> te >> cur_mode;
	cur_mode--;

	//INTEGRATING
	runge_kutta4<state_t> stepper;
	size_t steps = integrate_const(stepper, rhs[cur_mode], x, ts, te, dt,	IntObs(trace, times));

	//PRINTING STEPS
	for (size_t i = 0; i <= steps; i++) {
		cout << fixed;
		cout << setprecision(9) << times[i];
		for (int j = 0; j < 8; j++) {
			cout << setprecision(10) << ' ' << trace[i][j];
		}
		cout << endl;

		if (i != 0 && i != steps) {
			cout << fixed;
			cout << setprecision(9) << times[i];
			for (int j = 0; j < 8; j++) {
				cout << setprecision(10) << ' ' << trace[i][j];
			}
			cout << endl;
		}
	}
}
