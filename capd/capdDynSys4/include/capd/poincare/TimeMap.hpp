/// @addtogroup poincare
/// @{

/////////////////////////////////////////////////////////////////////////////
/// @file TimeMap.hpp
///
/// @author Tomasz Kapela
/////////////////////////////////////////////////////////////////////////////

// Copyright (C) 2008 by the CAPD Group.
//
// Distributed under the terms of the GNU General Public License.
// Consult  http://capd.ii.uj.edu.pl/ for details.

#ifndef _CAPD_POINCARE_TIME_MAP_HPP_
#define _CAPD_POINCARE_TIME_MAP_HPP_

#include "capd/poincare/TimeMap.h"
#include "capd/diffAlgebra/C2Coeff.h"

namespace capd{
namespace poincare{

template <typename SolverT>
TimeMap<SolverT>::TimeMap(SolverT &solver)
  : m_solver(solver),
    m_currentSet(0),
    m_oneStep(false),
    m_currentTime(0.),
    m_completed(true),
    m_solution(0),
    m_c0Coeff(solver.getVectorField().dimension()),
    m_c1Coeff(solver.getVectorField().dimension())
{}


template <typename SolverT>
typename TimeMap<SolverT>::VectorType TimeMap<SolverT>::operator()(ScalarType Time, VectorType& v, ScalarType& in_out_time)
{
  if(this->m_currentSet!=&v)
  {
    m_c0Coeff.setCurrentTime(in_out_time);
    m_c0Coeff = v;
    m_solution = 0;
  }
  this->moveSet(Time, m_c0Coeff,(void*)&v);
  in_out_time = m_c0Coeff.getCurrentTime();
  return VectorType(m_c0Coeff);
}

template <typename SolverT>
typename TimeMap<SolverT>::VectorType TimeMap<SolverT>::operator()(ScalarType Time, VectorType& v)
{
  ScalarType t = TypeTraits<ScalarType>::zero();
  return (*this)(Time,v,t);
}

template <typename SolverT>
typename TimeMap<SolverT>::VectorType
TimeMap<SolverT>::operator()(ScalarType Time, VectorType& v, const MatrixType& initMatrix, MatrixType &der, ScalarType& in_out_time)
{
  if(this->m_currentSet!=&v)
  {
    m_c1Coeff.setCurrentTime(in_out_time);
    m_c1Coeff = v;
    m_c1Coeff.setMatrix(initMatrix);
    m_solution = 0;
  }
  this->moveSet(Time, m_c1Coeff,(void*)&v);
  der = (MatrixType)m_c1Coeff;
  in_out_time = m_c1Coeff.getCurrentTime();
  return (VectorType)m_c1Coeff;
}

template <typename SolverT>
typename TimeMap<SolverT>::VectorType
TimeMap<SolverT>::operator()(ScalarType Time, VectorType& v, MatrixType &der, ScalarType& in_out_time)
{
  return (*this)(Time,v,MatrixType::Identity(v.dimension()),der,in_out_time);
}

template <typename SolverT>
typename TimeMap<SolverT>::VectorType
TimeMap<SolverT>::operator()(ScalarType Time, VectorType& v, MatrixType &der)
{
  ScalarType t = TypeTraits<ScalarType>::zero();
  return (*this)(Time,v,MatrixType::Identity(v.dimension()),der,t);
}

template <typename SolverT>
typename TimeMap<SolverT>::VectorType TimeMap<SolverT>::operator()(ScalarType Time, VectorType& v, SolutionCurve& solution)
{
  if(this->m_currentSet!=&v)
  {
    m_c0Coeff.setCurrentTime(solution.getLeftDomain());
    m_c0Coeff = v;
    m_solution = &solution;
  }
  this->moveSet(Time, m_c0Coeff,(void*)&v);
  return VectorType(m_c0Coeff);
}

template <typename SolverT>
typename TimeMap<SolverT>::VectorType
TimeMap<SolverT>::operator()(ScalarType Time, VectorType& v, const MatrixType& initMatrix, MatrixType &der, SolutionCurve& solution)
{
  if(this->m_currentSet!=&v)
  {
    m_c1Coeff.setCurrentTime(solution.getLeftDomain());
    m_c1Coeff = v;
    m_c1Coeff.setMatrix(initMatrix);
    m_solution = &solution;
  }
  this->moveSet(Time, m_c1Coeff,(void*)&v);
  der = (MatrixType)m_c1Coeff;
  return (VectorType)m_c1Coeff;
}

template <typename SolverT>
typename TimeMap<SolverT>::VectorType
TimeMap<SolverT>::operator()(ScalarType Time, VectorType& v, MatrixType &der, SolutionCurve& solution)
{
  return (*this)(Time,v,MatrixType::Identity(v.dimension()),der,solution);
}

}} // namespace capd::poincare


#endif // #define _CAPD_POINCARE_TIME_MAP_HPP_

/// @}
