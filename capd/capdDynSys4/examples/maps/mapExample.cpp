#include <iostream>
#include "capd/capdlib.h"

using namespace capd;
using namespace std;
using capd::autodiff::Node;

// ####################################################

/*
 * This is a map we will evaluate and differentiate - vector field of the PCR3BP
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

// ####################################################

int main(){
  int dimIn=4, dimOut=4, noParam=1;
  DMap f(pcr3bpVectorField,dimIn,dimOut,noParam);

  // set value of parameters mu, which is relative mass of Jupiter
  // 0 is index of parameter, 0.125 is its new value
  f.setParameter(0, 0.125);

  double v[] = {2,3,4,5};
  DVector x(dimIn,v);

  DVector y = f(x);
  DMatrix Df = f[x];

  cout << "f(x)=" << y << endl;
  cout << "Df(x)=" << Df << endl;

  // simultaneous computation of value and derivative - much faster
  DMatrix Df2(dimIn,dimIn);
  DVector y2 = f(x,Df2);

  // check the result
  cout << "y-y2=" << y-y2 << endl;
  cout << "Df-Df2=" << Df-Df2 << endl;
}

// ####################################################

/* output:
f(x)={4,5,11.9583,-5.06423}
Df(x)={
{0,0,1,0},
{0,0,0,1},
{0.997646,0.0286667,0,2},
{0.0286667,1.02376,-2,0}
}
y-y2={0,0,0,0}
Df-Df2={
{0,0,0,0},
{0,0,0,0},
{0,0,0,0},
{0,0,0,0}
}
*/
