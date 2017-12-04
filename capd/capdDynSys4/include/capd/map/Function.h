/// @addtogroup map
/// @{

/////////////////////////////////////////////////////////////////////////////
/// @file Function.h
///
/// @author Daniel Wilczak
/////////////////////////////////////////////////////////////////////////////

// Copyright (C) 2000-2012 by the CAPD Group.
//
// This file constitutes a part of the CAPD library, 
// distributed under the terms of the GNU General Public License.
// Consult  http://capd.ii.uj.edu.pl/ for details.

#ifndef _CAPD_MAP_FUNCTION_H_ 
#define _CAPD_MAP_FUNCTION_H_ 

#include <string>
#include <vector>
#include "capd/map/BasicFunction.h"

namespace capd{
namespace map{
/**
 * Class Function represents a function \f$ R^n\to R\f$.
 * It provides methods for computation of value, gradient and normalized derivatives (Taylor coefficients) of this function at a given point.
 *
 * The function is internally represented as an optimized array that encodes Direct Acyclic Graph associated to the expression defining function.
 * The expression defining function can be specified as a human readable string or a routine.
 *
 * The template parameter is a Vector that satisfies capd::vectalg::Vector interface. This includes:
 *  <ul>
 *  <li>vectors of variable length. They give flexibility but they are slower due to memory allocations and deallocations. Recommended for short programs.</li>
 *  <li>vectors of fixed length - they are allocated on stack instead of storage (dynamic allocation) and thus tey are fast. Recommended for programs running long time.</li>
 *  <li>arbitrary precision vectors - with static and dynamic allocation</li>
 *  </ul>
 */
template<typename VectorType>
class Function : public BasicFunction<typename VectorType::ScalarType>
{
public:
  typedef typename VectorType::ScalarType ScalarType;
  typedef BasicFunction<ScalarType> BaseFunction;
  typedef typename BaseFunction::size_type size_type;

  Function();                     ///< creates an univariate constant function f(x)=0
  Function(const std::string& s); ///< parses expression from given string. Allocates memory for ODE solver of order 'order'
  Function(const char* s);        ///< parses expression from given string. Allocates memory for ODE solver of order 'order'
  
  Function& operator=(const std::string&); ///< substitution from string. Parse expression and compute new DAG.
  Function& operator=(const char* );       ///< substitution from string. Parse expression and compute new DAG.

  ScalarType operator() (const ScalarType& v) const;          ///< evaluates function (for univariate case)
  ScalarType operator() (const VectorType& v) const ;         ///< evaluates function at a given vector
  
  VectorType gradient(VectorType u);              ///< returns gradient of the function computed at vector u
  void setParameters(const VectorType& values);   ///< simultaneously sets values of many parameters. It is assumed that given vector contains values of subsequent parameters.

protected:
  void check(const std::string& s);
};

}} // the end of the namespace capd::map

#endif // _CAPD_MAP_FUNCTION_H_ 

/// @}
