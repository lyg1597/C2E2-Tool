/////////////////////////////////////////////////////////////////////////////
//
/// @file cndemo3.cpp
///
/// @author kapela  @date 2010-02-09
//
/////////////////////////////////////////////////////////////////////////////

// Copyright (C) CAPD group
//
// This file constitutes a part of the CAPD library,
// distributed under the terms of the GNU General Public License.
// Consult  http://capd.ii.uj.edu.pl/ for details.


#include "capd/capdlib.h"
#include "capd/dynset/CnRect2Set.hpp"

const int rank = 5;
using namespace capd;
using namespace std;

int main(){

  std::cout << "\n michelson equation \n----------------------\n";
  // vector field
  IMap f("par:c;var:x,y,z;fun:y,z,c^2-y-0.5*x*x;", rank);
  f.setParameter("c",DInterval(1.0));

  // ICnTaylor - numerical method for ODE integration
  int order = 10;                 // order of numerical method
  ICnTaylor dynsys(f, order);
  dynsys.setStep(0.1);

  // initial condition
  IVector v(3);
  double sizeOfSet = 0.005;
  v[0]=0.0;    v[1]= DInterval(1.563-sizeOfSet, 1.563+sizeOfSet);  v[2]=0.0;
  CnRect2Set set(v, rank);

  // Time map with automatic Time Step Control (TSC)
  ICnTimeMap timeMap(dynsys);
  DInterval time = 2;

  timeMap(time, set);
  cout << set.currentSet().toString();

  return 0;
}

