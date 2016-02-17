/* AUTO-GENERATED SIMULATOR BY C2E2 */

#include <iostream>
#include <vector>
#include <boost/numeric/odeint.hpp>

using namespace std;
using namespace boost::numeric::odeint;

typedef vector<double> state_t;

//ODE FUNCTIONS
void Brussellator(const state_t &x, state_t &dxdt, const double t) {
			dxdt[0]=1+x[0]*x[0]*x[1]-2.5*x[0];
			dxdt[1]=1.5*x[0]-x[0]*x[0]*x[1];
}

//ODE FUNCTION POINTER
void (*rhs[1])(const state_t &x, state_t &dxdt, const double t) =
	{Brussellator};

int main() {
	//VARIABLES
	double ts, dt, te;
	double abs_err, rel_err;
	int cur_mode;
	state_t x(2);
	runge_kutta4<state_t> stepper;

	//PARSING CONFIG
	cin >> ts;
	for (int i = 0; i < 2; i++) {
		cin >> x[i];
	}
	cin >> abs_err >> rel_err >> dt >> te >> cur_mode;
	cur_mode--;

	//INTEGRATING
	int end = (int)((te - ts) / dt + 0.5);
	for (int i=0; i<end; i++) {
		//PRINTING PRE-STEP
			cout << fixed;
		cout << setprecision(9) << ts;
		for (int i = 0; i < 2; i++) {
			cout << setprecision(10) << ' ' << x[i];
		}
		cout << endl;

		stepper.do_step(rhs[cur_mode], x, ts, dt);
		ts += dt;

		//PRINTING POST-STEP
			cout << fixed;
		cout << setprecision(9) << ts;
		for (int i = 0; i < 2; i++) {
			cout << setprecision(10) << ' ' << x[i];
		}
		cout << endl;
	}
}
