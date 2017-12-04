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
  double data[] = {1,1,3,4};
  DMatrix initMatrix(2,2,data); // {{1,1},{3,4}}
  DMatrix monodromyMatrix(2,2);

  // define functional object
  DTimeMap::SolutionCurve solution(initTime);

  // and integrate
  timeMap(finalTime,u,initMatrix,monodromyMatrix,solution);

  // then you can ask for any intermediate point on the trajectory
  cout.precision(17);
  cout << "domain = [" << solution.getLeftDomain() << "," << solution.getRightDomain() << "]\n";
  cout << "solution(4) =" << solution(4) << endl;
  cout << "solution(7) =" << solution(7) << endl;
  cout << "solution(10)=" << solution(10) << endl;

  cout << "monodromyMatrix(4)=" << solution.derivative(4) << endl;
  cout << "monodromyMatrix(7)=" << solution.derivative(7) << endl;
  cout << "monodromyMatrix(10)=" << solution.derivative(10) << endl;
  cout << monodromyMatrix << endl;
  return 0;
}

/* output:
domain = [4,10]
solution(4) ={1,2}
solution(7) ={-0.45182824803348859,-1.6752282368321691}
solution(10)={1.2048370333482579,1.1704148266177468}
monodromyMatrix(4)={
{1,1},
{3,4}
}
monodromyMatrix(7)={
{10.196945592072506,13.189357190345262},
{-3.0607216445834844,-3.8608572219154982}
}
monodromyMatrix(10)={
{-7.8056566772311324,-10.001004006679393},
{-2.618904877016027,-3.4835862365644141}
}
{
{-7.8056566772311324,-10.001004006679393},
{-2.618904877016027,-3.4835862365644141}
}
*/
