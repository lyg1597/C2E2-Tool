#include <iostream>
#include <vector>
#include <boost/numeric/odeint.hpp>

using namespace std;
using namespace boost::numeric::odeint;

typedef vector<double> state;

class ode
{
  public:
    ode() { }

    void operator() (const state &x, state &dxdt, const double t)
    {
      dxdt[0] = 1 + x[0] * x[0] * x[1] - 2.5 * x[0];
      dxdt[1] = 1.5 * x[0] - x[0] * x[0] * x[1];
    }
};

//Integrator observer
/*
struct push_back_state_and_time
{
  vector<state> &m_states;
  vector<double> &m_times;

  push_back_state_and_time(vector<state> &states, vector<double> &times ) : m_states(states), m_times(times) { }

  void operator()(const state &x, double t)
  {
    m_states.push_back( x );
    m_times.push_back( t );
  }
};
*/

int main()
{
  //Configuration params
  state x(2);
  x[0] = 1.93;
  x[1] = 1.06;
  ode rhs;

  //Times 
  double dt = 0.01;
  double ts = 0.0;
  double te = 10.0;
  runge_kutta4<state> stepper;

  //Integrating all at once
  /*
  vector<state> x_vec;
  vector<double> times;
  size_t steps = integrate_const(stepper, ode, x, 0.0, 10.0, 0.01, 
      push_back_state_and_time(x_vec, times));

  for (size_t i = 0; i <= steps; i++)
  {
    cout << std::fixed;
    cout << setprecision(2) << times[i] 
         << std::setprecision(10) << '\t' << x_vec[i][0] 
         << setprecision(10) << '\t' << x_vec[i][1] << endl;
  }
  */

  //Integrating with steps
  for (double t = ts; t <= te; t += dt)
  {
    cout << fixed;
    cout << setprecision(2) << t 
         << setprecision(10) << '\t' << x[0] 
         << setprecision(10) << '\t' << x[1] << endl;

    stepper.do_step(rhs, x, t, dt);
  }
}
