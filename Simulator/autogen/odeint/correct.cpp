/* AUTO-GENERATED SIMULATOR BY C2E2 */

#include <iostream>
#include <vector>
#include <boost/numeric/odeint.hpp>

using namespace std;
using namespace boost::numeric::odeint;

typedef vector<double> state_t;

void  stimOn(const state_t &x, state_t &dxdt, const double t) {
  dxdt[0]=-0.9*x[0]*x[0]-x[0]*x[0]*x[0]-0.9*x[0]-x[1]+1;
  dxdt[1]=x[0]-2*x[1];
}

void stimOff(const state_t &x, state_t &dxdt, const double t) {
  dxdt[0]=-0.9*x[0]*x[0]-x[0]*x[0]*x[0]-0.9*x[0]-x[1];
  dxdt[1]=x[0]-2*x[1];
}

void (*rhs[2])(const state_t &x, state_t &dxdt, const double t) =
  {stimOn, stimOff};

int main() {
	//VARIABLES
	double ts, dt, te, t = 0;
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
		cout << setprecision(3) << t;
		for (int i = 0; i < 2; i++) {
			cout << setprecision(10) << ' ' << x[i];
		}
		cout << endl;

		stepper.do_step(rhs[cur_mode], x, t, dt);
		t += dt;

		//PRINTING POST-STEP
			cout << fixed;
		cout << setprecision(3) << t;
		for (int i = 0; i < 2; i++) {
			cout << setprecision(10) << ' ' << x[i];
		}
		cout << endl;
	}
}
