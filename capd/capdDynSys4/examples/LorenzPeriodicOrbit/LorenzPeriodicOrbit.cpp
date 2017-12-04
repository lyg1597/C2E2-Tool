/////////////////////////////////////////////////////////////////////////////
//
/// @file LorenzPeriodicOrbit.cpp
///
/// @author Daniel Wilczak
//
/////////////////////////////////////////////////////////////////////////////

// Copyright (C) CAPD group
//
// This file constitutes a part of the CAPD library,
// distributed under the terms of the GNU General Public License.
// Consult  http://capd.ii.uj.edu.pl/ for details.

/*
 * This example is a complete proof of the existence od 116 periodic orbits for the Lorenz system.
 * It also shows how to speed up computations by means of usage of static allocation.
 *
 * Allocating vectors on storage (malloc or new) is time consuming.
 * If the dimension is fixed and known at compile time
 * you can use the following header file
 *    capd/fdcapdlib.h
 * instead of
 *    capd/capdlib.h
 * Before including this file you MUST define two macros
 * 1. CAPD_USER_NAMESPACE
 *    - is the namespace in which most important types (like vectors, matrices, solvers, poincare maps)
 *      will be defined for you.
 * 2. CAPD_DEFAULT_DIMENSION
 *    - is a nonnegative number that stands for the dimension.
 *
 * NOTE: if you need few different dimensions in the same program you can undef these macros and define them again.
 *       For example:
 *
 * #define CAPD_USER_NAMESPACE capd3
 * #define CAPD_DEFAULT_DIMENSION 3
 * #include "capd/fdcapdlib.h
 * #undef CAPD_USER_NAMESPACE
 * #undef CAPD_DEFAULT_DIMENSION
 *
 * #define CAPD_USER_NAMESPACE capd5
 * #define CAPD_DEFAULT_DIMENSION 5
 * #include "capd/fdcapdlib.h
 * #undef CAPD_USER_NAMESPACE
 * #undef CAPD_DEFAULT_DIMENSION
 *
 * Now capd3::IVector and capd5::IVector are 3 and 5 dimensional vectors, respectively.
 * The same for matrices, ODE solvers, etc.
 *
 * In this example we will use 3-dimensional vectors when integration of the Lorenz system
 * and variable size vectors and matrices when computing Interval Newton Operator
 * (this is not time-critical operation in this example).
 */

#include <iostream>
#include <sstream>

// This file defines all types in default namespace "capd".
// Objects are of arbitrary dimension.
#include "capd/capdlib.h"

// Here we define 3-dimensional objects
#define CAPD_DEFAULT_DIMENSION 3
#define CAPD_USER_NAMESPACE capd3
#include "capd/fdcapdlib.h"
#undef CAPD_DEFAULT_DIMENSION
#undef CAPD_USER_NAMESPACE

// data for periodic orbits
#include "LorenzPeriodicOrbits.dat"

using namespace capd;
using namespace std;

double tolerance = 2.e-6;

// First we define a class for easy computation of Poincare map and its derivative
class LorenzPoincareMap
{
public:
  capd3::IFunction section;
  capd3::IMap vectorField;
  capd3::ITaylor solver;
  capd3::IPoincareMap pm;
  interval r;

  LorenzPoincareMap (interval s, interval r, interval q, int order, double tolerance)
    : section("par:r;var:x,y,z;fun:z-r+1;"),
      vectorField("par:s,r,q;var:x,y,z;fun:s*(y-x),x*(r-z)-y,x*y-q*z;"),
      solver(vectorField,order),
      pm(solver,section)
  {
    vectorField.setParameter("s",s);
    vectorField.setParameter("r",r);
    vectorField.setParameter("q",q);
    section.setParameter("r",r);
    this->r = r;
    solver.setAbsoluteTolerance(tolerance);
    solver.setRelativeTolerance(tolerance);
  }

  // this operator computes Poincare map
  IVector operator()(const IVector& u)
  {
    /*
      * u is assumed to be on the Poincare section.
      * It is 2-dim, represented by (x,y) coordinates .
      * We simply add r-1 as the last coordinate i.e. we embed the vector into the full 3d-space
     */
    capd3::IVector x(3);
    x[0] = u[0];
    x[1] = u[1];
    x[2] = this->r-1.;

    // we define a doubleton representation of the set
    capd3::C0HORect2Set s(x);
    // and compute Poincare map
    x = this->pm(s);

    // here we project the image 'x' onto 2-dimensional section
    return IVector(2,x.begin());
  }

  // This operator computes derivative of the Poincare map.
  IMatrix dx(const IVector& u)
  {
    // Again u is two dimensional, so embed it.
    capd3::IVector x(3);
    x[0] = u[0];
    x[1] = u[1];
    x[2] = this->r-1.;

    /*
     * We define a doubleton representation of the set and its derivative
     * constructor sets initial condition for variational equation to Identity
     */
    capd3::C1Rect2Set s(x);

    // the matrix monodromyMatrix will store derivatives of the FLOW not Poincare map
    capd3::IMatrix monodromyMatrix(3,3);
    x = this->pm(s,monodromyMatrix);

    // This member function recomputes derivatives of the flow into derivatives of Poincare map
    capd3::IMatrix DP = this->pm.computeDP(x,monodromyMatrix);

    // as before, we extract derivative of Poincare map (2x2 slice) from 3x3 matrix.
    IMatrix result(2,2);
    result(1,1) = DP(1,1);
    result(1,2) = DP(1,2);
    result(2,1) = DP(2,1);
    result(2,2) = DP(2,2);
    // and return it
    return result;
  }

}; //< end of class LorenzPoincareMap

// global counter for the number of validated periodic orbits
int verified = 0;

// ---------------------------------------------------------------------

/*
  * The following function computes the Interval Newton Operator for the Poincare map on a given set
  * and verifies existence and uniqueness of a periodic point in a given set X.
  *
  * Parameters are:
  * @param pm - an instance of LorenzPoincareMap
  * @param X[] - array of interval vectors in which we want to prove the existence of an unique periodic point.
  * @param period - is a period of the point with respect to (half) Poincare map
 */
void verifyExistenceOfPeriodicOrbit(LorenzPoincareMap& pm, IVector X[], int period)
{
  IVector center(2*period);
  IVector Y(2*period);
  IVector imCenter(2*period);

  int i;
  // preparing data
  for(i=0;i<period;++i)
  {
    IVector c = midVector(X[i]);
    center[2*i] = c[0]; center[2*i+1] = c[1];
    Y[2*i] = X[i][0]; Y[2*i+1] = X[i][1];
  }

  /*
   * computing map F: (x0,x1,x2,...x_{n-1}) -> (x0-P(x_{n-1}),x1-P(x0), .... , x_{n-1} - P(x_{n-2}))
   * and its derivative
   * imCenter will contain F(center)
   * D will enclose DF(Y)
   */
  IMatrix D = IMatrix::Identity(2*period);
  for(i=0;i<period;++i)
  {
    int prev = (period+i-1)%period;
    IVector c = midVector(X[prev]);
    IVector im = pm(c);
    imCenter[2*i] = center[2*i]-im[0];
    imCenter[2*i+1] = center[2*i+1] - im[1];

    IMatrix DP = pm.dx(X[prev]);
    D[2*i][2*prev] = DP[0][0];
    D[2*i][2*prev+1] = DP[0][1];
    D[2*i+1][2*prev] = DP[1][0];
    D[2*i+1][2*prev+1] = DP[1][1];
  }

  // compute interval Newton operator
  IVector N = center - capd::matrixAlgorithms::gauss(D,imCenter);

  // Verification if N is a subset of X
  if(subsetInterior(N,Y))
  {
    verified++;
    cout << "the existence of period " << period << " orbit verified" << endl;
    cout << "already validated " << verified << endl;
  }
  else
  {
    cout << "N is not a subset of Y\n" << endl;
    cout << "diam(N)=" << diam(N) << endl;
    cout << "diam(Y)=" << diam(Y) << endl;
  }
}

// ----------------------------------- MAIN ----------------------------------------

int main(int argc, char* argv[])
{
  cout.precision(5);
  double xMin, xMax, yMin,yMax, temp;
  int period;
  int order = 8;
  if(argc<3){
    cout << "Default order of Taylor method set to " << order << endl;
    cout << "Default tolerance (absolute and relative) set to " << tolerance << endl;
    cout << "Usage:" << endl << "\t ./LorenzPeriodicOrbit order tolerance" << endl << endl;
  }
  else
  {
    order = atoi(argv[1]);
    tolerance = strtod(argv[2],NULL);
  }
  const interval s(10.), r(28.), q=interval(8.)/interval(3.);

  try{
    // Define an instance of LorenzPoincareMap with chaotic parameter values
    LorenzPoincareMap pm(s,r,q,order,tolerance);

    const int numberOfPeriodicOrbits = 116;

    // array of 20 two-dimensional vectors.
    // 20 is the maximal period of (half) Poincare map we consider in this program.
    IVector* X = new (2) IVector[20];

    for(int i = 0;i<numberOfPeriodicOrbits;++i){
      istringstream in(lorenzPOData[i]);
      // read period
      in >> period;
      period*=2;
      // read initial condition
      for(int j=0;j<period;++j){
        in >> xMin >> xMax >> yMin >> yMax >> temp >> temp;
        X[j][0] = (xMin+xMax)/2. + interval(-1,1)*(xMin-xMax)/.1;
        X[j][1] = (yMin+yMax)/2. + interval(-1,1)*(yMax-yMin)/.1;
      }
      verifyExistenceOfPeriodicOrbit(pm,X,period);
    }
    cout << "validated " << verified << " from " << numberOfPeriodicOrbits << endl;
    delete[] X;
  }catch(exception& e)
  {
    cout << "\n\nException caught: "<< e.what() << endl;
  }
  return 0;
} // END
