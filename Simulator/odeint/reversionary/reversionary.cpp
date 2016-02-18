#include <iostream>
#include <vector>
#include <boost/numeric/odeint.hpp>
#include <cmath>

using namespace std;
using namespace boost::numeric::odeint; 
typedef vector<double> state_t;

//CONSTANTS
double DEG2RAD = M_PI / 180.0;
double MPH2FPS = 5280.0 / 3600.0;

double g = 32.2;
double Kv = 1.0;
double Kr = 1.0;
double KPsi = 1.0;
double Kd = 0.0019;
double Km = 4.0e-06;
double tauT = 1.0;
double tauL = 1.0;
double tauPhi = 1.0;
double Vd = 90.0 * MPH2FPS;
double Rd = 1.0 * DEG2RAD;
double Psid = 10.0 * DEG2RAD;

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
void Reversionary(const state_t &x, state_t &dxdt, const double t) {
  //Inputs
  double T = x[0];
  double L = x[1];
  double Phi = x[2];
  double V = x[3];
  double Gamma = x[4];
  double Psi = x[5];
  double M = x[6];

  //Variables
  double T_RC = M*Kv*(Vd-V)+M*g*sin(Gamma);
  double L_RC = M*(sqrt(pow(g*cos(Gamma)+V*Kr*(Rd-Gamma), 2)+pow(V*KPsi*(Psid-Psi)*cos(Gamma), 2)));
  double Phi_RC = atan((V*KPsi*(Psid-Psi)*cos(Gamma))/(g*cos(Gamma)+V*Kr*(Rd-Gamma)));

  dxdt[0] = (T_RC - T)/tauT;
  dxdt[1] = (L_RC-L)/tauL;
  dxdt[2] = (Phi_RC-Phi)/tauPhi;
  dxdt[3] = (T-Kd*V*V)/M - g*sin(Gamma);
  dxdt[4] = L*cos(Phi)/M/V - g*cos(Gamma)/V;
  dxdt[5] = L*sin(Phi)/M/V/cos(Gamma);
  dxdt[6] = -Km*T;
}

//ODE FUNCTION POINTER
void (*rhs[1])(const state_t &x, state_t &dxdt, const double t) =
{Reversionary};

int main() {
  //VARIABLES
  double ts, dt, te;
  double abs_err, rel_err;
  int cur_mode;
  state_t x(7);
  //runge_kutta4<state_t> stepper;

  //INITIAL VALUES
  x[0] = 30;
  x[1] = 980;
  x[2] = 0;
  x[3] = 85 * MPH2FPS;
  x[4] = 0;
  x[5] = 0;
  x[6] = 980 / g;

  ts = 0;
  te = 60;
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
      ObsInt(x_vector, times));

  //PRINTING
  for (size_t i = 0; i <= steps; i++)
  {
    cout << fixed;
    cout << setprecision(9) << times[i];
    
    for (int j = 0; j < 7; j++)
    {
      cout << setprecision(10) << ' ' << x_vector[i][j];
    }
    cout << endl;
  }
}
