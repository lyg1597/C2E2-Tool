#include <cstdio>
#include "capd/capdlib.h"

using namespace capd;

int main(){
  // define an instance of class DMap that describes the vector field
  DMap pendulum("time:t;par:omega;var:x,dx;fun:dx,sin(omega*t)-sin(x);");
  pendulum.setParameter("omega",1.);

  int order = 30;
  DTaylor solver(pendulum,order);
  DTimeMap timeMap(solver);

  // specify initial condition
  DVector u(2);
  u[0] = 1.;
  u[1] = 2.;
  double initTime = 4;
  double finalTime = 8;

  // use Taylor method with step control to integrate
  timeMap.stopAfterStep(true);

  do{
    u = timeMap(finalTime,u,initTime);
    printf("t=%6f, x=%6f, dx=%6f\n",initTime,u[0],u[1]);
  }while(!timeMap.completed());

  return 0;
}

/* output:
t=4.328125, x=1.562278, dx=1.409454
t=4.703125, x=1.953892, dx=0.679453
t=5.140625, x=2.070803, dx=-0.134637
t=5.640625, x=1.787823, dx=-0.980052
t=6.062500, x=1.243576, dx=-1.571640
t=6.437500, x=0.587743, dx=-1.878208
t=6.796875, x=-0.092332, dx=-1.847124
t=7.156250, x=-0.698870, dx=-1.478668
t=7.546875, x=-1.154391, dx=-0.823943
t=7.968750, x=-1.330555, dx=-0.004576
t=8.000000, x=-1.329739, dx=0.056758
*/
