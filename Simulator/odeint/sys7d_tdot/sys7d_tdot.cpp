#include <iostream>
#include <vector>
#include <boost/numeric/odeint.hpp> 

using namespace std;
using namespace boost::numeric::odeint;

typedef vector<double> state;

//RHS definition of the ODE
void ode(const state &x, state &dxdt, const double /* t */)
{
  dxdt[0] = 1.4 * x[2] - 0.9 * x[0];
  dxdt[1] = 2.5 * x[4] - 1.5 * x[1];
  dxdt[2] = 0.6 * x[6] - 0.8 * x[2] * x[1];
  dxdt[3] = 2.0 - 1.3 * x[3] * x[2];
  dxdt[4] = 0.7 * x[0] - 1.0 * x[3] * x[4];
  dxdt[5] = 0.3 * x[0] - 3.1 * x[5];
  dxdt[6] = 1.8 * x[5] - 1.5 * x[6] * x[1];
  dxdt[7] = 1.0;
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
  state x(8);
  x[0] = 1.2;
  x[1] = 1.05;
  x[2] = 1.5;
  x[3] = 2.4;
  x[4] = 1;
  x[5] = 0.1;
  x[6] = 0.45;
  x[7] = 1.25;

  //Times 
  //NOTE these need to be of type Time to pass into function
  double dt = 0.001;
  double ts = 0.0;
  double te = 10.0;
  runge_kutta4<state> stepper;
  
  //Integrating all at once
  /*
  vector<state> x_vec;
  vector<double> times;
  size_t steps = integrate_const(stepper, ode, x, 0.0, 10.0, 0.001, 
      push_back_state_and_time(x_vec, times));

  for (size_t i = 0; i <= steps; i++)
  {
    cout << fixed;
    cout << setprecision(3) << times[i];
    
    for(int j = 0; j < 7; j++)
    {
      cout << setprecision(10) << '\t' << x_vec[i][j];
    }
    
    cout << endl;
  }
  */

  //Integrating with steps
  for (double t = ts; t <= te; t += dt)
  {
    cout << fixed;
    cout << setprecision(3) << t;

    for (int j = 0; j < 8; j++)
    {
      cout << setprecision(10) << '\t' << x[j];
    }
    cout << endl;

    stepper.do_step(ode, x, t, dt);
  }
}

