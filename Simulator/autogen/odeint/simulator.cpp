/* AUTO-GENERATED SIMULATOR BY C2E2 */

# include <iostream>
# include <vector>
# include <boost/numeric/odeint.hpp>

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
void SlowDown(const state_t &x, state_t &dxdt, const double t) {
			dxdt[0]=x[1]-2.5;
			dxdt[1]=0.1*x[2];
			dxdt[2]=-0.01*x[0] -0.01*10.3 + 0.3*2.8 - 0.3*x[1] - 0.5*x[2];
			dxdt[3]=-2*x[3];
			dxdt[4]=-2*x[4];
			dxdt[5]=0.1*x[3];
}

void StartTurn1(const state_t &x, state_t &dxdt, const double t) {
			dxdt[0]=x[1]-2.5;
			dxdt[1]=0.1*x[2];
			dxdt[2]=-0.5*x[1]+1.4-0.5*x[2];
			dxdt[3]=2.5*3 - 0.15*3*x[4] + 0.5 - 0.025*x[5] - 0.05*x[3];
			dxdt[4]=3 - 3*0.05*x[4] + 0.2-0.01*x[5];
			dxdt[5]=0.1*x[3];
}

void EndTurn1(const state_t &x, state_t &dxdt, const double t) {
			dxdt[0]=x[1]-2.5;
			dxdt[1]=0.1*x[2];
			dxdt[2]=-0.5*x[1]+1.4-0.5*x[2];
			dxdt[3]=-0.1*2.5*x[4] + 0.5 - 0.025*x[5] - 0.05*x[3];
			dxdt[4]=-0.1*x[4] + 0.2 - 0.01*x[5];
			dxdt[5]=0.1*x[3];
}

void EndTurn2(const state_t &x, state_t &dxdt, const double t) {
			dxdt[0]=x[1]-2.5;
			dxdt[1]=0.1*x[2];
			dxdt[2]=-0.5*x[1]+1.4-0.5*x[2];
			dxdt[3]=-0.1*2.5*x[4] + 0.5 - 0.025*x[5] - 0.05*x[3];
			dxdt[4]=-0.1*x[4] + 0.2 - 0.01*x[5];
			dxdt[5]=0.1*x[3];
}

void StartTurn2(const state_t &x, state_t &dxdt, const double t) {
			dxdt[0]=x[1]-2.5;
			dxdt[1]=0.1*x[2];
			dxdt[2]=-0.5*x[1]+1.4-0.5*x[2];
			dxdt[3]=-2.5*3 - 0.15*3*x[4] + 0.5 - 0.025*x[5] - 0.05*x[3];
			dxdt[4]=-3 - 3*0.05*x[4] + 0.2-0.01*x[5];
			dxdt[5]=0.1*x[3];
}

void SpeedUp(const state_t &x, state_t &dxdt, const double t) {
			dxdt[0]=x[1]-2.5;
			dxdt[1]=0.1*x[2];
			dxdt[2]=-0.01*x[0] + 0.01*10.3 + 0.3*2.8 - 0.3*x[1] - 0.5*x[2];
			dxdt[3]=-2*x[3];
			dxdt[4]=-2*x[4];
			dxdt[5]=0.1*x[3];
}

void Continue(const state_t &x, state_t &dxdt, const double t) {
			dxdt[0]=x[1]-2.5;
			dxdt[1]=0.1*x[2];
			dxdt[2]=-0.5*x[1]+1.4-0.5*x[2];
			dxdt[3]=-2*x[3];
			dxdt[4]=-2*x[4];
			dxdt[5]=0.1*x[3];
}

//ODE FUNCTION POINTER
void (*rhs[7])(const state_t &x, state_t &dxdt, const double t) =
	{SlowDown, StartTurn1, EndTurn1, EndTurn2, StartTurn2, SpeedUp, Continue};

int main() {
	//VARIABLES
	double ts, dt, te;
	double abs_err, rel_err;
	int cur_mode;
	state_t x(6);
	vector<double> times;
	vector<state_t> trace;

	//PARSING CONFIG
	cin >> ts;
	for (int i = 0; i < 6; i++) {
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
		for (int j = 0; j < 6; j++) {
			cout << setprecision(10) << ' ' << trace[i][j];
		}
		cout << endl;

		if (i != 0 && i != steps) {
			cout << fixed;
			cout << setprecision(9) << times[i];
			for (int j = 0; j < 6; j++) {
				cout << setprecision(10) << ' ' << trace[i][j];
			}
			cout << endl;
		}
	}
}