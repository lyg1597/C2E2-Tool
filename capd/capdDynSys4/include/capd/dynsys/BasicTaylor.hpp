/// @addtogroup dynsys
/// @{

/////////////////////////////////////////////////////////////////////////////
/// @file BasicTaylor.hpp
///
/// @author Daniel Wilczak
/////////////////////////////////////////////////////////////////////////////

// Copyright (C) 2000-2005 by the CAPD Group.
//
// This file constitutes a part of the CAPD library, 
// distributed under the terms of the GNU General Public License.
// Consult  http://capd.ii.uj.edu.pl/ for details. 

#ifndef _CAPD_DYNSYS_BASICTAYLOR_HPP_
#define _CAPD_DYNSYS_BASICTAYLOR_HPP_

#include <sstream>
#include <string>
#include <stdexcept>

#include "capd/basicalg/factrial.h"

#include "capd/dynsys/BasicTaylor.h"
#include "capd/diffAlgebra/BasicCurve.hpp"
#include "capd/diffAlgebra/Curve.hpp"

namespace capd {
namespace dynsys {

// -----------------------------------------------------------------------------

template <typename MapType, typename StepControlType, typename CurveT>
BasicTaylor<MapType, StepControlType, CurveT>::BasicTaylor(MapType& vectorField, size_type order, const StepControlType& stepControl)
  : capd::dynsys::StepControlInterface<StepControlType,ScalarType>(stepControl), 
    SolutionCurve(1.0,1.0,vectorField.dimension(),order,vectorField.degree()),
    m_vField(&vectorField), 
    m_step(TypeTraits<ScalarType>::one())
{
  if(order<=0) throw std::logic_error("BasicTaylor constructor: order must be a positive integer");
  this->m_vField->setOrder(order + 1);
  this->m_vField->differentiateTime();
}

// -----------------------------------------------------------------------------

template <typename MapType, typename StepControlType, typename CurveT>
void BasicTaylor<MapType, StepControlType, CurveT>::setOrder(size_type order)
{
  if(order > this->getAllocatedOrder())
  {
    this->m_vField->setOrder(order + 1);
    this->m_vField->differentiateTime();
  }
  SolutionCurve::setOrder(order);
}

// -----------------------------------------------------------------------------


template <typename MapType, typename StepControlType, typename CurveT>
typename BasicTaylor<MapType, StepControlType, CurveT>::VectorType
BasicTaylor<MapType, StepControlType, CurveT>::operator()(VectorType v)
{
  VectorType* coeff = this->getCoefficientsAtCenter();
  coeff[0] = v;
  this->m_vField->computeODECoefficients(coeff,this->getOrder());
  this->computeTimeStep(v);
  this->sumTaylorSeries(v,coeff,this->getOrder());
  return v;
}

//###########################################################//


template <typename MapType, typename StepControlType, typename CurveT>
typename BasicTaylor<MapType, StepControlType, CurveT>::VectorType
BasicTaylor<MapType, StepControlType, CurveT>::operator()(VectorType v, MatrixType& der)
{
  VectorType* coeff = this->getCoefficientsAtCenter();
  MatrixType* matrixCoeff = this->getMatrixCoefficients();
  coeff[0] = v;
  matrixCoeff[0].setToIdentity();

  this->m_vField->computeODECoefficients(coeff,matrixCoeff,this->getOrder());
  this->computeTimeStep(v);
  this->sumTaylorSeries(v,der,coeff,matrixCoeff,this->getOrder());
  return v;
}

//###########################################################//

template <typename MapType, typename StepControlType, typename CurveT>
typename BasicTaylor<MapType, StepControlType, CurveT>::VectorType
BasicTaylor<MapType, StepControlType, CurveT>::operator()(VectorType v, const MatrixType& derivative, MatrixType & resultDerivative)
{
  VectorType* coeff = this->getCoefficientsAtCenter();
  MatrixType* matrixCoeff = this->getMatrixCoefficients();
  coeff[0] = v;
  matrixCoeff[0] = derivative;

  this->m_vField->computeODECoefficients(coeff,matrixCoeff,this->getOrder());
  this->computeTimeStep(v);
  this->sumTaylorSeries(v,resultDerivative,coeff,matrixCoeff,this->getOrder());
  return v;
}

//###########################################################//

template <typename MapType, typename StepControlType,typename CurveT>
void BasicTaylor<MapType,StepControlType,CurveT>::sumTaylorSeries(
      VectorType& v,
      VectorType* coeff,
      size_type order
  )
{
  // summation of the Taylor series
  v = coeff[order];
  for(int r = order - 1; r >= 0; --r)
    capd::vectalg::multiplyAssignObjectScalarAddObject(v,this->m_step,coeff[r]);
}

//###########################################################//

template <typename MapType, typename StepControlType,typename CurveT>
void BasicTaylor<MapType,StepControlType,CurveT>::sumTaylorSeries(
      VectorType& v,
      MatrixType& der,
      VectorType* coeff,
      MatrixType* matrixCoeff,
      size_type order
  )
{
  // summation of the Taylor series
  v = coeff[order];
  der = matrixCoeff[order];
  for(int r = order - 1; r >= 0; --r)
  {
    capd::vectalg::multiplyAssignObjectScalarAddObject(v,this->m_step,coeff[r]);
    capd::vectalg::multiplyAssignObjectScalarAddObject(der,this->m_step,matrixCoeff[r]);
  }
}

//###########################################################//

template <typename MapType, typename StepControlType, typename CurveT>
const typename BasicTaylor<MapType, StepControlType, CurveT>::SolutionCurve&
BasicTaylor<MapType, StepControlType, CurveT>::getCurve()
{
  this->setDomain(0.,rightBound(this->m_step));
  return *this;
}

//###########################################################//

template <typename MapType, typename StepControlType, typename CurveT>
typename BasicTaylor<MapType, StepControlType, CurveT>::ScalarType
BasicTaylor<MapType, StepControlType, CurveT>::getCoeffNorm(size_type r, size_type degree) const
{
  ScalarType result = TypeTraits<ScalarType>::zero();
  for(size_type i=0;i<this->dimension();++i)
  {
    result = capd::max(result,capd::abs(this->centerCoefficient(i,r)));
    result = capd::max(result,capd::abs(this->coefficient(i,r)));
  }
  if(degree){
    for(size_type i=0;i<this->dimension();++i)
      for(size_type j=0;j<this->dimension();++j)
        result = capd::max(result,capd::abs(this->coefficient(i,j,r)));
  }
  return result;
}

}} //namespace capd::dynsys

#endif // _CAPD_DYNSYS_BASICTAYLOR_HPP_
/// @}
