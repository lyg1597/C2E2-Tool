#include <iostream>
#include "capd/capdlib.h"
// The following example is a complete proof of the existence of 3 different
// First we define a class for easy computation of Poincare map and its derivative
class RosslerPoincareMap
  IFunction section;
  RosslerPoincareMap (int order, interval _a, interval _b)
  // this operator computes period-iteration of Poincare map
  IVector operator()(const IVector& u, interval& returnTime, int period)
    // we define a doubleton representation of the set
    // here we project the image 'x' onto 2-dimensional section
    // The above is functionally equivalent to
  // This operator computes derivative of the Poincare map.