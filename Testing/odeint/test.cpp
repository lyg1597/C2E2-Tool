#include <iostream>
//#include <utility>
#include <vector>
#include <boost/numeric/odeint.hpp>

typedef std::vector<double> state_type;

void test_model (const state_type &x, state_type &dxdt, const double /* t */)
{
  dxdt[0] = 1 + x[0] * x[0] * x[1] - 2.5 * x[0];
  dxdt[1] = 1.5 * x[0] - x[0] * x[0] * x[1];
}

struct push_back_state_and_time
{
  std::vector< state_type >& m_states;
  std::vector< double >& m_times;

  push_back_state_and_time( std::vector< state_type > &states , std::vector< double > &times )
    : m_states( states ) , m_times( times ) { }

  void operator()( const state_type &x , double t )
  {
    m_states.push_back( x );
    m_times.push_back( t );
  }
};

int main()
{
  using namespace std;
  using namespace boost::numeric::odeint;

  //Initialization
  state_type x(2);
  x[0] = 1.93;
  x[1] = 1.06;

  //Integrating with observer
  vector<state_type> x_vec;
  vector<double> times;
  runge_kutta4<state_type> stepper;
  size_t steps = integrate_const(stepper, test_model, x, 0.0, 10.0, 0.01, 
      push_back_state_and_time(x_vec, times));

  for (size_t i = 0; i <= steps; i++)
  {
    cout << setprecision(3) << times[i] << '\t' 
         << setprecision(10) << x_vec[i][0] << '\t' 
         << setprecision(10) << x_vec[i][1] << '\n';
  }

  return 0;
}

