#include <iostream>
#include <vector>
#include <boost/numeric/odeint.hpp>
#include <cmath>

using namespace std;
using namespace boost::numeric::odeint; 
typedef vector<double> state_t;

//DATA STUCTURE
struct IntObs
{
  vector<state_t> &m_states;
  vector<double> &m_times;

  IntObs(vector<state_t> &states, vector<double> &times )
    : m_states(states), m_times(times) { }

  void operator()(const state_t &x, double t)
  {
    m_states.push_back(x);
    m_times.push_back(t);
  }
};

//ODE FUNCTIONS
void Const(const state_t &x, state_t &dxdt, const double t) {
  dxdt[0] = 5;
}

//ODE FUNCTION POINTER
void (*rhs[1])(const state_t &x, state_t &dxdt, const double t) =
{Const};

int main() {
  //VARIABLES
  double ts, dt, te;
  double abs_err, rel_err;
  int cur_mode;
  state_t x(1);
  //runge_kutta4<state_t> stepper;

  //INITIAL VALUES
  x[0] = 1;

  ts = 0;
  te = 10;
  dt = 0.1;

  abs_err = 1e-06;
  rel_err = 0.0001;

  cur_mode = 0;

  //INTEGRATING CONST
  /*
  int end = (int)((te - ts) / dt + 0.5);
  for (int i=0; i<end; i++) {
  //PRINTING PRE-STEP
  cout << fixed;
  cout << setprecision(9) << ts;
  for (int i = 0; i < 7; i++) {
  cout << setprecision(10) << ' ' << x[i];
  }
  cout << endl;

  stepper.do_step(rhs[cur_mode], x, ts, dt);
  ts += dt;

  //PRINTING POST-STEP
  cout << fixed;
  cout << setprecision(9) << ts;
  for (int i = 0; i < 7; i++) {
  cout << setprecision(10) << ' ' << x[i];
  }
  cout << endl;
  }
  */

  //INTEGRATING ADAPTIVE
  vector<state_t> x_vector;
  vector<double> times;

  auto stepper = make_controlled(abs_err, rel_err, runge_kutta_dopri5<state_t>());
  size_t steps = integrate_adaptive(stepper, rhs[cur_mode], x, ts, te, dt, 
      IntObs(x_vector, times));

  //PRINTING
  for (size_t i = 0; i <= steps; i++)
  {
    cout << fixed;
    cout << setprecision(9) << times[i];
    
    for (size_t j = 0; j < x.size(); j++)
    {
      cout << setprecision(10) << ' ' << x_vector[i][j];
    }
    cout << endl;
  }
}
