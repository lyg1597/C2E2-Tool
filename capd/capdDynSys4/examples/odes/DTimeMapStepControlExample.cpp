#include <cstdio>
#include "capd/capdlib.h"

using namespace capd;

void integrate(DTimeMap& timeMap){
  // Specify initial condition
  DVector u(2);
  u[0] = 1.;
  u[1] = 2.;
  double initTime = 0;
  double finalTime = 100;
  int stepsCounter = 0;

  timeMap.stopAfterStep(true);
  do{
    u = timeMap(finalTime,u,initTime);
    stepsCounter++;
  }while(!timeMap.completed());
  printf("x=%.10f, dx=%.10f\n",u[0],u[1]);
  printf("order=%d, steps=%d\n\n",timeMap.getSolver().getOrder(),stepsCounter);
}

int main(){
  // Define an instance of class DMap that describes the vector field
  DMap pendulum("time:t;par:omega;var:x,dx;fun:dx,sin(omega*t)-sin(x);");
  pendulum.setParameter("omega",1.);

  // Define an instance of ODE solver, time step does not matter because we will use class TimeMap
  int order = 20;
  DTaylor solver(pendulum,order);
  DTimeMap timeMap(solver);

  printf("------- Integration of pendulum over time range 0-100 -----------------\n\n");

  double step = 0.125;
  solver.setStep(step);
  printf("Constant time step %f:\n",step);
  integrate(timeMap);

  timeMap.turnOnStepControl();
  printf("Default step control and default tolerance:\n");
  integrate(timeMap);

  // change order
  solver.setOrder(10);
  printf("Default step control and default tolerance:\n");
  integrate(timeMap);

  // change tolerance
  double tol = 1e-6;
  solver.setAbsoluteTolerance(tol);
  solver.setRelativeTolerance(tol);
  printf("Default step control with absolute and relative tolerances set to %.1e:\n",tol);
  integrate(timeMap);

  // change order
  solver.setOrder(20);
  printf("Default step control with absolute and relative tolerances set to %.1e:\n",tol);
  integrate(timeMap);

  return 0;
}

/* output:
------- Integration of pendulum over time range 0-100 -----------------

Constant time step 0.125000:
x=256.1915412352, dx=1.9673787155
order=20, steps=800

Default step control and default tolerance:
x=256.1915412353, dx=1.9673787155
order=20, steps=661

Default step control and default tolerance:
x=256.1915412355, dx=1.9673787156
order=10, steps=7698

Default step control with absolute and relative tolerances set to 1.0e-06:
x=256.1930836905, dx=1.9681846652
order=10, steps=359

Default step control with absolute and relative tolerances set to 1.0e-06:
x=256.1948739677, dx=1.9691182352
order=20, steps=156
*/
