/// @addtogroup poincare
/// @{

/////////////////////////////////////////////////////////////////////////////
/// @file BasicPoincareMap.hpp
///
/// @author Daniel Wilczak
/// @author Tomasz Kapela
/////////////////////////////////////////////////////////////////////////////

// Copyright (C) 2000-2008 by the CAPD Group.
//
// Distributed under the terms of the GNU General Public License.
// Consult  http://capd.ii.uj.edu.pl/ for details.


#ifndef _CAPD_POINCARE_BASIC_POINCARE_MAP_HPP_
#define _CAPD_POINCARE_BASIC_POINCARE_MAP_HPP_

#include <stdexcept>
#include "capd/poincare/BasicPoincareMap.h"
#include "capd/poincare/BasicPoincareMap_operator.hpp"
#include "capd/diffAlgebra/CnCoeff.hpp"
#include "capd/basicalg/factrial.h"

namespace capd{
namespace poincare{

// ------------------------------------------------------------------------------------------

template <typename SolverT, typename FunT>
BasicPoincareMap<SolverT, FunT>::BasicPoincareMap(
   Solver& solver,
   FunctionType & section,
   CrossingDirection direction,
   const RealType & errorTolerance
 ) : m_solver(solver),
     m_section(section),
     m_crossingDirection(direction),
     m_sectionFactor(errorTolerance),
     m_blowUpMaxNorm(1e+10),
     m_maxReturnTime(1000.)
{}

// ------------------------------------------------------------------------------------------

/// Crosses the section and returns the value of Poincare Map.
template <typename SolverT, typename FunT>
typename BasicPoincareMap<SolverT, FunT>::ScalarType
BasicPoincareMap<SolverT, FunT>::findCrossingTime(ScalarType timeJustBeforeSection, const CurveType & c)
{
  ScalarType t0 = (c.getLeftDomain()+c.getRightDomain())/2;
  int maxNumberOfNewtonIterations= 15;

  // now we resolve for the return time
  for(int i=0; i<maxNumberOfNewtonIterations; ++i) {
    VectorType v = c(t0);
    ScalarType s_t = this->m_section(v);
    if(abs(s_t) < this->m_sectionFactor)
      break;

    // note: c'(t0) = f(t0,c(t0))
    ScalarType Ds_t = this->m_section.gradient(v) * this->getVectorField()(timeJustBeforeSection+t0,v);
    ScalarType N = t0 - s_t/Ds_t;

    // we know that zero is in the domain
    if(N < c.getLeftDomain())
      N = c.getLeftDomain();
    if(N > c.getRightDomain())
      N = c.getRightDomain();
    t0 = N;
  }
  return t0;
}

// -----------------------------------------------------------------------------------------

template <typename SolverT, typename FunT>
void BasicPoincareMap<SolverT, FunT>::computeDT(
      const VectorType& Px,
      const MatrixType& derivativeOfFlow,
      const VectorType& gradientOnPx,
      const ScalarType& denominator,
      VectorType& result
   )
{
  for (size_type j = 0; j < this->getVectorField().dimension(); ++j)
  {
    ScalarType val = TypeTraits<ScalarType>::zero();
    for (size_type k = 0; k < this->getVectorField().dimension(); ++k)
      val -= gradientOnPx[k] * derivativeOfFlow(k + 1, j + 1);
    result[j] = val / denominator;
  }
}

// -----------------------------------------------------------------------------------------

template <typename SolverT, typename FunT>
typename BasicPoincareMap<SolverT, FunT>::VectorType
BasicPoincareMap<SolverT, FunT>::computeDT(
      const VectorType& Px,
      const MatrixType& derivativeOfFlow,
      ScalarType returnTime
   )
{
   VectorType result(this->getVectorField().dimension(),false);
   VectorType gradientOnPx = this->m_section.gradient(Px);
   ScalarType denominator = this->m_solver.getVectorField()(returnTime,Px)*gradientOnPx;
   this->computeDT(Px,derivativeOfFlow,gradientOnPx,denominator,result);
   return result;
}

// -------------------------------------------------------------------

/**
 * Computes derivative of Poincare Map dP
 *
 *  @param Px                value of Poincare map P(x)
 *  @param derivativeOfFlow  derivative of the flow \f$ \partial \varphi(T(x),x)/ \partial x \f$
 *  @param[out] dT           gradient of T(x) : first return time function
 *  @param returnTime        return time
 */

template <typename SolverT, typename FunT>
typename BasicPoincareMap<SolverT, FunT>::MatrixType
BasicPoincareMap<SolverT, FunT>::computeDP(
      const VectorType& Px,
      const MatrixType& derivativeOfFlow,
      VectorType& dT,
      ScalarType returnTime
   )
{
  const size_type dim = this->getVectorField().dimension();
  MatrixType result(dim,dim);
  VectorType fieldOnPx = m_solver.getVectorField()(returnTime,Px);
  VectorType gradientOnPx = m_section.gradient(Px);
  ScalarType denominator = fieldOnPx*gradientOnPx;

  this->computeDT(Px,derivativeOfFlow,gradientOnPx,denominator,dT);

  for(size_type i=1;i<=dim;++i)
    for(size_type j=1;j<=dim;++j)
      result(i,j) = fieldOnPx[i-1]*dT[j-1] + derivativeOfFlow(i,j);
   return result;
}

// -------------------------------------------------------------------

/**
 * Computes derivative of Poincare Map dP
 *
 *  @param Px                value of Poincare map P(x)
 *  @param derivativeOfFlow  defivative of the flow \f$ \partial \varphi(T(x),x)/ \partial x \f$
 */

template <typename SolverT, typename FunT>
typename BasicPoincareMap<SolverT, FunT>::MatrixType
BasicPoincareMap<SolverT, FunT>::computeDP(
      const VectorType& Px,
      const MatrixType& derivativeOfFlow,
      ScalarType returnTime
   )
{
  VectorType dT(this->getVectorField().dimension());
  return this->computeDP(Px,derivativeOfFlow,dT,returnTime);
}

// -------------------------------------------------------------------

template <typename SolverT, typename FunT>
void BasicPoincareMap<SolverT, FunT>::computeDP(
      const VectorType& Px,
      const MatrixType& derivativeOfFlow,
      const HessianType& hessianOfFlow,
      MatrixType& DP,
      HessianType& D2P,
      VectorType& dT,
      MatrixType& d2T,
      ScalarType returnTime
    )
{
  size_type i,j,k;
  const size_type dim = this->getVectorField().dimension();
  m_solver.computeCoefficientsAtCenter(returnTime,Px,2);
  VectorType fieldOnPx(dim);
  VectorType d2Phidt2(dim);
  for(i=0;i<dim;++i){
    fieldOnPx[i] = m_solver.centerCoefficient(i,1);
    d2Phidt2[i] = m_solver.centerCoefficient(i,2);
  }
  MatrixType derOfVectorFieldOnPx = m_solver.getVectorField().derivative(returnTime,Px);
  VectorType gradientOnPx = m_section.gradient(Px);
  ScalarType denominator = fieldOnPx*gradientOnPx;

  // here we compute first order derivatives of return time.
  this->computeDT(Px,derivativeOfFlow,gradientOnPx,denominator,dT);

  // first derivative of Poincare map
  for(j=0;j<dim;++j)
    for(i=0;i<dim;++i)
      DP(j+1,i+1) = fieldOnPx[j]*dT[i] + derivativeOfFlow(j+1,i+1);

  // second derivatives of return time
  MatrixType dPhiVectorFieldOnPPx = derOfVectorFieldOnPx*derivativeOfFlow;

  for(i=0;i<dim;++i)
  {
    ScalarType d2Tii = TypeTraits<ScalarType>::zero();
    for(k=0;k<dim;++k)
    {
      D2P(k,i,i) = hessianOfFlow(k,i,i) + dT[i]*( dPhiVectorFieldOnPPx(k+1,i+1) + d2Phidt2[k] * dT[i]) ;
      d2Tii -= gradientOnPx[k]* D2P(k,i,i);
    }
    d2T(i+1,i+1) = d2Tii/denominator;

    for(j=i+1;j<dim;++j)
    {
      ScalarType d2Tij = TypeTraits<ScalarType>::zero();
      for(k=0;k<dim;++k)
      {
        D2P(k,i,j) = hessianOfFlow(k,i,j)
                   + dPhiVectorFieldOnPPx(k+1,i+1) * dT[j]
                   + dPhiVectorFieldOnPPx(k+1,j+1) * dT[i]
                   + 2.*d2Phidt2[k] * dT[i]*dT[j]
                                               ;
        d2Tij -= gradientOnPx[k]*D2P(k,i,j);
      }
      d2T(j+1,i+1) = d2T(i+1,j+1) = d2Tij/denominator;
    }
  }

  // second derivatives of Poincare map
  for(k=0;k<dim;++k)
    for(i=0;i<dim;++i)
      for(j=i;j<dim;++j)
        D2P(k,i,j) += fieldOnPx[k] * d2T(i+1,j+1);

}

}} // namespace capd::poincare

#endif // #define _CAPD_POINCARE_BASIC_POINCARE_MAP_HPP_

/// @}
