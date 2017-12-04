/// @addtogroup poincare
/// @{

/////////////////////////////////////////////////////////////////////////////
/// @file BasicPoincareMap_operator.hpp
///
/// @author Daniel Wilczak
/// @author Tomasz Kapela
/////////////////////////////////////////////////////////////////////////////

// Copyright (C) 2000-2008 by the CAPD Group.
//
// Distributed under the terms of the GNU General Public License.
// Consult  http://capd.ii.uj.edu.pl/ for details.

#ifndef _CAPD_POINCARE_BASIC_POINCARE_MAP_OPERATOR_HPP_
#define _CAPD_POINCARE_BASIC_POINCARE_MAP_OPERATOR_HPP_

#include "capd/poincare/BasicPoincareMap.h"
#include "capd/diffAlgebra/C2Coeff.h"

namespace capd{
namespace poincare{

template <typename SolverT, typename FunT>
typename BasicPoincareMap<SolverT, FunT>::VectorType
BasicPoincareMap<SolverT, FunT>::operator()(VectorType v)
{
  ScalarType returnTime = 0.;
  return this->operator()(v,returnTime);
}

// ------------------------------------------------------------------------------------------

template <typename SolverT, typename FunT>
typename BasicPoincareMap<SolverT, FunT>::VectorType
BasicPoincareMap<SolverT, FunT>::operator()(const VectorType& v, ScalarType& in_out_time)
{
  SaveStepControl<SolverT> ssc(this->m_solver);

  // constructor sets currentTime=0 by default
  capd::diffAlgebra::C0Coeff<VectorType> coeff(v);
  coeff.setCurrentTime(in_out_time);
  this->reachSection(coeff, 1);
  ScalarType timeJustBeforeSection = coeff.getCurrentTime();
  const CurveType& c = this->m_solver.getCurve();
  ScalarType t0 = findCrossingTime(timeJustBeforeSection,c);
  in_out_time = timeJustBeforeSection + t0;

  return c(t0);
}

// ------------------------------------------------------------------------------------------


template <typename SolverT, typename FunT>
typename BasicPoincareMap<SolverT, FunT>::VectorType
BasicPoincareMap<SolverT, FunT>::operator()(const VectorType& v, VectorType& pointAfterSection, ScalarType& in_out_time)
{
  SaveStepControl<SolverT> ssc(this->m_solver);

  // constructor sets currentTime=0 by default
  capd::diffAlgebra::C0Coeff<VectorType> coeff(v);
  coeff.setCurrentTime(in_out_time);

  pointAfterSection = this->reachSection(coeff, 1);
  ScalarType timeJustBeforeSection = coeff.getCurrentTime();
  const CurveType& c = this->m_solver.getCurve();
  ScalarType t0 = findCrossingTime(timeJustBeforeSection,c);
  in_out_time = timeJustBeforeSection + t0;

  return c(t0);
}

// ------------------------------------------------------------------------------------------

template <typename SolverT, typename FunT>
typename BasicPoincareMap<SolverT, FunT>::VectorType
BasicPoincareMap<SolverT, FunT>::operator()(const VectorType& v, MatrixType& dF)
{
  ScalarType returnTime = 0.;
  return this->operator()(v,dF,returnTime);
}

// ------------------------------------------------------------------------------------------

template <typename SolverT, typename FunT>
typename BasicPoincareMap<SolverT, FunT>::VectorType
BasicPoincareMap<SolverT, FunT>::operator()(const VectorType& v, MatrixType& dF, ScalarType& in_out_time)
{
  SaveStepControl<SolverT> ssc(this->m_solver);
  capd::diffAlgebra::C1Coeff<MatrixType> coeffs(v, MatrixType::Identity(this->getVectorField().dimension()));
  coeffs.setCurrentTime(in_out_time);

  reachSection(coeffs, 1);
  ScalarType timeJustBeforeSection = coeffs.getCurrentTime();
  const CurveType& c = this->m_solver.getCurve();

  ScalarType t0 = findCrossingTime(timeJustBeforeSection,c);
  in_out_time = timeJustBeforeSection + t0;

  dF = c[t0];
  return c(t0);
}

// ------------------------------------------------------------------------------------------

template <typename SolverT, typename FunT>
typename BasicPoincareMap<SolverT, FunT>::VectorType
BasicPoincareMap<SolverT, FunT>::operator()(const VectorType& v, MatrixType& dF, HessianType& h)
{
  ScalarType returnTime = 0.;
  return this->operator()(v,dF,h,returnTime);
}

// ------------------------------------------------------------------------------------------

template <typename SolverT, typename FunT>
typename BasicPoincareMap<SolverT, FunT>::VectorType
BasicPoincareMap<SolverT, FunT>::operator()(const VectorType& v, MatrixType& dF, HessianType& h, ScalarType& in_out_time)
{
  SaveStepControl<SolverT> ssc(this->m_solver);
  capd::diffAlgebra::C2Coeff<MatrixType> coeffs(v, MatrixType::Identity(this->getVectorField().dimension()));
  coeffs.setCurrentTime(in_out_time);

  reachSection(coeffs, 1);
  ScalarType timeJustBeforeSection = coeffs.getCurrentTime();

  const CurveType& c = this->m_solver.getCurve();
  ScalarType t0 = findCrossingTime(timeJustBeforeSection,c);
  dF = c[t0];
  h = c.hessian(t0);
  in_out_time = timeJustBeforeSection + t0;
  return c(t0);
}

}} // namespace capd::poincare

#endif // #define _CAPD_POINCARE_BASIC_POINCARE_MAP_OPERATOR_HPP_

/// @}
