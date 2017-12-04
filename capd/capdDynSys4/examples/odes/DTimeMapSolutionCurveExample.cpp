#include <iostream>
#include "capd/capdlib.h"

using namespace capd;
using namespace std;

int main(){
  // define an instance of class DMap that describes the vector field
  DMap pendulum("time:t;par:omega;var:x,dx;fun:dx,sin(omega*t)-sin(x);");
  pendulum.setParameter("omega",1.);

  int order=20;
  DTaylor solver(pendulum,order);
  DTimeMap timeMap(solver);

  // specify initial condition
  DVector u(2);
  u[0] = 1.;
  u[1] = 2.;
  double initTime = 4;
  double finalTime = 10;

  // define functional object
  DTimeMap::SolutionCurve solution(initTime);

  // and integrate
  timeMap(finalTime,u,solution);

  // then you can ask for any intermediate point on the trajectory
  cout << "domain = [" << solution.getLeftDomain() << "," << solution.getRightDomain() << "]\n";
  cout << "solution(4) =" << solution(4) << endl;
  cout << "solution(7) =" << solution(7) << endl;
  cout << "solution(10)=" << solution(10) << endl;

  return 0;
}

/* output:
domain = [4,10]
solution(4) ={1,2}
solution(7) ={-0.451828,-1.67523}
solution(10)={1.20484,1.17041}
*/
