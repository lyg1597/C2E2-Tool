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

// here is a little function that given a symmetric point point on Poincare section y=0
// (x,dx) = (x,0)
// embeds it into energy level, i.e. it computes dy from (x,y=0,dx=0,dy)
interval toEnergyLevel(interval x, interval mu, interval JacobiConstant)
{
  interval mj = 1.-mu;
  return sqrt(sqr(x) + 2.*(mj/abs(x+mu) + mu/abs(x-mj)) + mu*mj - JacobiConstant);
}

int main(){
  cout.precision(17);
  int dim=4, noParam=1;
  IMap vf(pcr3bpVectorField,dim,dim,noParam);
  // set value of parameters mu, which is relative mass of Jupiter
  // 0 is index of parameter, 0.0009537 is its new value
  interval mu = 0.0009537;
  vf.setParameter(0, mu);

  // define solver, section and Poicnare map
  ITaylor solver(vf,30);
  IFunction section("var:x,y,dx,dy;fun:y;");
  IPoincareMap pm(solver,section);

  // the following point lies on Poincare section y=0
  // and is invariant under reversing symmetry of the system
  // it contains L1 Lyapunov orbit for Oterma's energy value C=3.03
  interval JacobiConstant = 3.03;
  IVector L1(4);
  L1[0] = 0.9208034913207435 + interval(-1,1)*4e-15;

  // we will prove that this periodic orbit indeed exist.
  // It is enough to show that there is a point on symmetry line
  // whose trajectory intersect symmetry line
  IVector left = leftVector(L1);
  left[3] = toEnergyLevel(left[0],mu,JacobiConstant);
  IVector right = rightVector(L1);
  right[3] = toEnergyLevel(right[0],mu,JacobiConstant);
  L1[3] = toEnergyLevel(L1[0],mu,JacobiConstant);

  C0HOTripletonSet set(L1);
  C0HOTripletonSet leftSet(left);
  C0HOTripletonSet rightSet(right);
  left = pm(leftSet);
  right = pm(rightSet);

  // compute Poincare map and print output
  cout << "P(left)=(" << left[0] << "," << left[2] << ")" << endl;
  cout << "P(right)=(" << right[0] << "," << right[2] << ")" << endl;

  // check if the Poincare map is well defined on 'set' and get bound for the return time
  interval returnTime;
  pm(set,returnTime);
  cout << "half return time=" << returnTime << endl;

  // check if there is a point that intersect symmetry line
  if(left[2]*right[2]<0)
    cout << "The existence of L1 orbit validated with accuracy " << diam(L1[0]) << endl;
  else
    cout << "Could not validate L1 periodic orbit" << endl;
  return 0;
}
/* output:
P(left)=([0.95228712767828705, 0.95228712767838519],[-4.0432330299428393e-13, -9.0333815771644921e-15])
P(right)=([0.95228712767841728, 0.95228712767851176],[7.2247942701311645e-14, 4.5227167505627575e-13])
half return time=[1.54105956319374, 1.5410595631981727]
The existence of L1 orbit validated with accuracy [8.2156503822261584e-15, 8.2156503822261584e-15]
*/
