/// @addtogroup dynsys
/// @{

/////////////////////////////////////////////////////////////////////////////
/// @file Taylor.hpp
///
/// @author Daniel Wilczak
/////////////////////////////////////////////////////////////////////////////

// Copyright (C) 2000-2012 by the CAPD Group.
//
// This file constitutes a part of the CAPD library,
// distributed under the terms of the GNU General Public License.
// Consult  http://capd.ii.uj.edu.pl/ for details.

#ifndef _CAPD_DYNSYS_TAYLOR_HPP_
#define _CAPD_DYNSYS_TAYLOR_HPP_

#include <sstream>
#include <string>
#include <stdexcept>

#include "capd/basicalg/factrial.h"

#include "capd/dynsys/Taylor.h"
#include "capd/dynsys/TaylorException.h"

#include "capd/dynsys/DynSys.hpp"
#include "capd/dynsys/BasicTaylor.hpp"
#include "capd/dynsys/enclosure.hpp"

namespace capd{
namespace dynsys{

//###########################################################//

template<typename MapType, typename StepControlType,typename CurveType>
Taylor<MapType, StepControlType,CurveType>::Taylor(MapType& vectorField, size_type a_order, const StepControlType& stepControl)
  : BaseTaylor(vectorField,a_order,stepControl), psiCurve(1.0,1.0,vectorField.dimension(),a_order,vectorField.degree())
{}

//###########################################################//

template <typename MapType, typename StepControlType, typename CurveT>
void Taylor<MapType, StepControlType, CurveT>::setOrder(size_type order)
{
  BaseTaylor::setOrder(order);
  this->psiCurve.setOrder(order);
}

//###########################################################//

template<typename MapType, typename StepControlType,typename CurveType>
typename Taylor<MapType,StepControlType,CurveType>::VectorType
Taylor<MapType,StepControlType,CurveType>::Phi(const ScalarType& t, const VectorType& v)
{
  VectorType result(v.dimension());
  this->setCurrentTime(t);
  this->computeCoefficientsAtCenter(v,this->getOrder());
  this->sumTaylorSeries(result,this->getCoefficientsAtCenter(),this->getOrder());
  return result;
}

//###########################################################//

template<typename MapType, typename StepControlType,typename CurveType>
typename Taylor<MapType, StepControlType,CurveType>::MatrixType
Taylor<MapType, StepControlType,CurveType>::JacPhi(const ScalarType& t, const VectorType &iv)
{
  this->computeCoefficients(t,iv,this->getOrder());

  // the summation of the Taylor series
  MatrixType* matrixCoeff = this->getMatrixCoefficients();
  MatrixType result = matrixCoeff[this->getOrder()];
  for(int r=this->getOrder()-1;r>=0;--r)
    capd::vectalg::multiplyAssignObjectScalarAddObject(result,this->m_step,matrixCoeff[r]);

  return result;
}

//###########################################################//

template<typename MapType, typename StepControlType,typename CurveType>
typename Taylor<MapType, StepControlType,CurveType>::VectorType
Taylor<MapType, StepControlType,CurveType>::Remainder(const ScalarType& t, const VectorType &iv, VectorType& o_enc)
{
  const static ScalarType I(TypeTraits<ScalarType>::zero().leftBound(),TypeTraits<ScalarType>::one().rightBound());
  // we compute an enclosure for \varphi([0,timestep],iv)
  o_enc = this->enclosure(t,iv);
  this->computeRemainderCoefficients(t + I*this->m_step,o_enc);
  return this->getRemainderCoefficients()[this->getOrder()+1]*power(this->m_step,this->getOrder()+1);
}

//###########################################################//

template<typename MapType, typename StepControlType,typename CurveType>
typename Taylor<MapType, StepControlType,CurveType>::VectorType
Taylor<MapType, StepControlType,CurveType>::enclosure(const ScalarType& t, const VectorType &x)
///< the function finds an enclosure for \varphi([0,step],x)
{
  return capd::dynsys::enclosure(*this->m_vField,t,x, this->m_step);
}

//###########################################################//

template<typename MapType, typename StepControlType,typename CurveType>
typename Taylor<MapType, StepControlType,CurveType>::MatrixType
Taylor<MapType, StepControlType,CurveType>::jacEnclosure(const ScalarType& t, const VectorType &enc, ScalarType* logNormOfDerivative)
/**< the function finds enclosure for Jacobian matrix (variational part)
     source- "C^1-Lohner algorithm" by P. Zgliczynski */
{
  return capd::dynsys::jacEnclosure(*this->m_vField,t,this->m_step,enc,capd::vectalg::EuclLNorm<VectorType,MatrixType>(),logNormOfDerivative);
}

//###########################################################//

template<typename MapType, typename StepControlType,typename CurveType>
typename Taylor<MapType, StepControlType,CurveType>::MatrixType
Taylor<MapType, StepControlType,CurveType>::jacEnclosure(const ScalarType& t, const VectorType &enc)
/**< the function finds enclosure for Jacobian matrix (variational part)
     source- "C^1-Lohner algorithm" by P. Zgliczynski */
{
  return capd::dynsys::jacEnclosure(*this->m_vField,t,this->m_step,enc,capd::vectalg::EuclLNorm<VectorType,MatrixType>());
}

//###########################################################//

template<typename MapType, typename StepControlType,typename CurveType>
void Taylor<MapType, StepControlType,CurveType>::JacRemainder(
      const ScalarType& t,
      const VectorType &vecEnc,
      const MatrixType &jacEnc,
      VectorType &Rem,
      MatrixType &jacRem
  )
/// the remainder for varaiational part is computed
/// vecEnc - enclosure for \varphi([0,step],x)
/// jacEnc -  enclosure for \partial/\partial x \varphi([0,step],x)
{
  const static ScalarType I(TypeTraits<ScalarType>::zero().leftBound(),TypeTraits<ScalarType>::one().rightBound());
  this->computeRemainderCoefficients(t+I*this->m_step,vecEnc,jacEnc);
  ScalarType fac = power(this->m_step,this->getOrder()+1);
  Rem = this->getRemainderCoefficients()[this->getOrder()+1] * fac;
  jacRem = this->getMatrixRemainderCoefficients()[this->getOrder()+1] * fac;
}

//###########################################################//

template<typename MapType, typename StepControlType,typename CurveType>
void Taylor<MapType,StepControlType,CurveType>::encloseC0Map(
      const ScalarType& t,
      const VectorType& x,
      const VectorType& xx,
      VectorType& o_phi,
      VectorType& o_rem,
      VectorType& o_enc,
      MatrixType& o_jacPhi
  )
{
  int order = this->getOrder();
  MatrixType* matrixCoeff = this->getMatrixCoefficients();
  VectorType* center = this->getCoefficientsAtCenter();

  // here we compute all the coefficients for phi(t) and DPhi(t)
  this->setCurrentTime(t);
  this->computeCoefficientsAtCenter(x,order);
  this->computeCoefficients(xx,order);
  this->computeTimeStep(t,xx);

  // in the following function the time step can be adjusted
  o_rem = this->Remainder(t,xx,o_enc);

  o_phi = center[order];
  o_jacPhi = matrixCoeff[order];
  for(int i=order-1;i>=0;--i)
  {
    capd::vectalg::multiplyAssignObjectScalarAddObject(o_jacPhi,this->m_step,matrixCoeff[i]);
    capd::vectalg::multiplyAssignObjectScalarAddObject(o_phi,this->m_step,center[i]);
  }
}

//###########################################################//

template<typename MapType, typename StepControlType,typename CurveType>
void Taylor<MapType,StepControlType,CurveType>::encloseC1Map(
      const ScalarType& t,
      const VectorType& x,
      const VectorType& xx,
      VectorType& o_phi,
      VectorType& o_rem,
      VectorType& o_enc,
      MatrixType& o_jacPhi,
      MatrixType& o_jacRem,
      MatrixType& o_jacEnc
  )
{
  int order = this->getOrder();
  MatrixType* matrixCoeff = this->getMatrixCoefficients();
  VectorType* center = this->getCoefficientsAtCenter();

  // here we compute all the coefficients for phi(t) and DPhi(t)
  this->setCurrentTime(t);
  this->computeCoefficientsAtCenter(x,order);
  this->computeCoefficients(xx,order);
  this->computeTimeStep(t,xx);

  // in the following function the time step can be adjusted
  o_enc = this->enclosure(t,xx);
  o_jacEnc = this->jacEnclosure(t,o_enc);
  this->JacRemainder(t,o_enc,o_jacEnc,o_rem,o_jacRem);

  o_phi = center[order];
  o_jacPhi = matrixCoeff[order];
  for(int i=order-1;i>=0;--i)
  {
    capd::vectalg::multiplyAssignObjectScalarAddObject(o_jacPhi,this->getStep(),matrixCoeff[i]);
    capd::vectalg::multiplyAssignObjectScalarAddObject(o_phi,this->getStep(),center[i]);
  }
}

//###########################################################//

template<typename MapType, typename StepControlType,typename CurveType>
typename Taylor<MapType, StepControlType,CurveType>::ScalarType
Taylor<MapType, StepControlType,CurveType>::getCoeffNorm(size_type r, size_type degree) const
{
  typename TypeTraits<ScalarType>::Real result = 0;
  for(size_type i=0;i<this->dimension();++i){
    result = capd::max(result,rightBound(abs(this->remainderCoefficient(i,r))));
    result = capd::max(result,rightBound(abs(this->coefficient(i,r))));
  }
  if(degree)
  {
    for(size_type i=0;i<this->dimension();++i){
      for(size_type j=0;j<this->dimension();++j){
        result = capd::max(result,rightBound(abs(this->remainderCoefficient(i,j,r))));
        result = capd::max(result,rightBound(abs(this->coefficient(i,j,r))));
      }
    }
  }
  return ScalarType(result);
}

}} //namespace capd::dynsys

#endif // _CAPD_DYNSYS_TAYLOR_HPP_

/// @}
