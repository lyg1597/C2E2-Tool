#include <iostream>
#include <vector>
#include <boost/numeric/odeint.hpp>

using namespace std;
using namespace boost::numeric::odeint;

typedef vector<double> state;

//RHS definition of the ODE
void ode(const state &x, state &dxdt, const double /* t */)
{
  dxdt[0] = 1 + x[0] * x[0] * x[1] - 2.5 * x[0];
  dxdt[1] = 1.5 * x[0] - x[0] * x[0] * x[1];
}

//Integrator observer
struct push_back_state_and_time
{
  vector<state> &m_states;
  vector<double> &m_times;

  push_back_state_and_time(vector<state> &states, vector<double> &times )
    : m_states(states), m_times(times) { }

  void operator()(const state &x, double t)
  {
    m_states.push_back( x );
    m_times.push_back( t );
  }
};

int main()
{
  //Configuration params
  state x(2);
  x[0] = 1.93;
  x[1] = 1.06;

  //Times 
  //NOTE these need to be of type Time to pass into function
  int time_step = 0.01;
  int time_init = 0.0;
  int time_horiz = 10.0;
  
  vector<state> x_vec;
  vector<double> times;

  runge_kutta4<state> stepper;
  size_t steps = integrate_const(stepper, ode, x, 0.0, 10.0, 0.01, 
      push_back_state_and_time(x_vec, times));

  for (size_t i = 0; i <= steps; i++)
  {
    cout << std::fixed;
    cout << setprecision(2) << times[i] 
         << std::setprecision(10) << '\t' << x_vec[i][0] 
         << setprecision(10) << '\t' << x_vec[i][1] << endl;
  }
}

