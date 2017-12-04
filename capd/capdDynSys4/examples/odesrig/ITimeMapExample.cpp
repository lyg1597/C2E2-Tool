#include <iostream>
#include "capd/capdlib.h"
using namespace std;
using namespace capd;
using capd::autodiff::Node;

/*
* This is vector field of the PCR3BP
* @param in is an array of independent variables
* @param out is an array of dependent variables, i.e. out = f(in)
* @param params - parameters of the map. Here we assume that mu=params[0] is a relative mass of Jupiter
*/
void pcr3bpVectorField(Node t, Node in[], int dimIn, Node out[], int dimOut, Node params[], int noParams)
{
  // Try to factorize expression as much as possible.
  // Usually we have to define some intermediate quantities.
  Node mj = 1-params[0]; // relative mass of the Sun
  Node xMu = in[0] + params[0];
  Node xMj = in[0] - mj;
  Node xMuSquare = xMu^2; // square
  Node xMjSquare = xMj^2;
  Node ySquare = in[1]^2;

  // power -1.5, for rigorous computation use ONLY REPRESENTABLE CONSTANTS.
  // If exponent is not representable or it is an interval then it should be a parameter of the map.
  Node factor1 = mj*((xMuSquare+ySquare)^-1.5);
  Node factor2 = params[0]*((xMjSquare+ySquare)^-1.5);
  out[0] = in[2];
  out[1] = in[3];
  out[2] = in[0] - xMu*factor1 - xMj*factor2 + 2*in[3];
  out[3] = in[1]*(1 - factor1 - factor2) - 2*in[2];
}


int main(){
  cout.precision(17);
  int dim=4, noParam=1;
  IMap vf(pcr3bpVectorField,dim,dim,noParam);
  // set value of parameters mu, which is relative mass of Jupiter
  // 0 is index of parameter, 0.0009537 is its new value
  interval mu = 0.0009537;
  interval mj = 1.-mu;
  vf.setParameter(0, mu);

  // define solver and TimeMap
  ITaylor solver(vf,20);
  ITimeMap timeMap(solver);

  // initial condition - very close to L1 Lyapunov orbit for Oterma's energey value C=3.03
  interval JacobiConstant = 3.03;
  IVector L1(4);
  L1[0] = 0.9208034913207400196;
  L1[3] = sqrt(sqr(L1[0]) + 2.*(mj/abs(L1[0]+mu) + mu/abs(L1[0]-mj)) + mu*mj - JacobiConstant);

  // just to show two different methods: Taylor and Hermite-Obreshkov
  C0Rect2Set set1 (L1);
  C0HORect2Set set2(L1);

  // integrate and print output
  cout << "T=0\n" << L1 << endl;
  // actually this is rigorous bound for the return time of this orbit to Poincare section y=0 as we can see from the output
  interval finalTime1 = 3.082119126376508;
  interval finalTime2 = 3.082119126392904;

  cout << "T=" << finalTime1 << "\n" << timeMap(finalTime1,set1) << endl;
  cout << "T=" << finalTime2 << "\n" << timeMap(finalTime2,set2) << endl;

  return 0;
}
/* output:
T=0
{[0.92080349132074002, 0.92080349132074002],[0, 0],[0, 0],[0.10444767270690263, 0.10444767270691328]}
T=[3.082119126376508, 3.082119126376508]
{[0.92080349131463157, 0.92080349132215111],[-1.99290905625963e-12, -1.1573077254434056e-17],[-1.5705560492200326e-11, 1.5083164041259187e-12],[0.10444767270547141, 0.10444767271280907]}
T=[3.0821191263929042, 3.0821191263929042]
{[0.92080349131579897, 0.92080349132098105],[2.3926219026911148e-17, 1.4319031982648132e-12],[-1.1223570249036191e-11, 6.1501586242050097e-13],[0.10444767270666519, 0.10444767271161758]}
*/
