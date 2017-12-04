/// @addtogroup poincare
/// @{

/////////////////////////////////////////////////////////////////////////////
/// @file  PoincareMap_templateOperator.h
///
/// @author Daniel Wilczak
/// @author Tomasz Kapela
/////////////////////////////////////////////////////////////////////////////

// Copyright (C) 2008 by the CAPD Group.
//
// Distributed under the terms of the GNU General Public License.
// Consult  http://capd.ii.uj.edu.pl/ for details.

#ifndef _CAPD_POINCARE_POINCARE_MAP_TEMPLATE_OPERATOR_H_
#define _CAPD_POINCARE_POINCARE_MAP_TEMPLATE_OPERATOR_H_

#include <cassert>
#include "capd/poincare/PoincareMap.h"
#include "capd/poincare/BasicPoincareMap.hpp"

namespace capd{
namespace poincare{

/*__________________________________________________________________________*/

template <typename DS, typename FunT>
template<typename T>
typename PoincareMap<DS,FunT>::VectorType
PoincareMap<DS,FunT>::computePoincareMap(T& originalSet, int n)
{
  // We move the set to be very close to the section
  T setAfterTheSection = this->reachSection(originalSet, n);
  VectorType bound(this->m_solver.dimension(),true);
  ScalarType oneStepReturnTime;

  // try to cross section in one step and use Newton method
  if(this->crossSection<T>(originalSet,oneStepReturnTime,bound)){
    *(this->returnTime) = originalSet.getCurrentTime() + oneStepReturnTime;
    originalSet = setAfterTheSection;
    if(this->derivativeOfFlow!=0)
      *(this->derivativeOfFlow) = this->m_solver.getCurve().derivative(oneStepReturnTime);
    if(this->hessianOfFlow!=0)
      *(this->hessianOfFlow) = this->m_solver.getCurve().hessian(oneStepReturnTime);
    return bound;
  }

  // if not possible, try to cross section by the old method.
  return this->crossSection<T>(originalSet, setAfterTheSection);
}

/*__________________________________________________________________________*/

template <typename DS, typename FunT>
template<typename T>
typename PoincareMap<DS,FunT>::VectorType
PoincareMap<DS,FunT>::operator()(T& originalSet, ScalarType& out_returnTime, int n)
{
  // The originalSet after this function contains a set just after section.
  this->derivativeOfFlow = NULL;
  this->hessianOfFlow = NULL;
  this->jet = NULL;
  this->returnTime = &out_returnTime;

  return this->computePoincareMap(originalSet,n);
}

/*__________________________________________________________________________*/

template <typename DS, typename FunT>
template<typename T>
typename PoincareMap<DS,FunT>::VectorType
PoincareMap<DS,FunT>::operator()(T& originalSet, int n)
{
  ScalarType returnTime = TypeTraits<ScalarType>::zero();
  return (*this)(originalSet,returnTime,n);
}

/*__________________________________________________________________________*/

template <typename DS, typename FunT>
template<typename T>
typename PoincareMap<DS,FunT>::VectorType
PoincareMap<DS,FunT>::operator()(T& originalSet, MatrixType& der, ScalarType& out_returnTime, int n)
{
  // the originalSet after this function contains a set just after section
  // we move the set close to section
  this->derivativeOfFlow = &der;
  this->hessianOfFlow = NULL;
  this->jet = NULL;
  this->returnTime = &out_returnTime;

  return this->computePoincareMap(originalSet,n);
}

/*__________________________________________________________________________*/

template <typename DS, typename FunT>
template<typename T>
typename PoincareMap<DS,FunT>::VectorType
PoincareMap<DS,FunT>::operator()(T& originalSet, MatrixType& der, int n)
{
  ScalarType returnTime = TypeTraits<ScalarType>::zero();
  return (*this)(originalSet,der,returnTime,n);
}

/*__________________________________________________________________________*/

template <typename DS, typename FunT>
template<typename T>
typename PoincareMap<DS,FunT>::VectorType
PoincareMap<DS,FunT>::operator()(T& originalSet, MatrixType& der, HessianType& hessian, ScalarType& out_returnTime, int n)
{
  // the originalSet after this function contains a set just after section
  // we move the set close to section
  this->derivativeOfFlow = &der;
  this->hessianOfFlow = &hessian;
  this->jet = NULL;
  this->returnTime = &out_returnTime;

  return this->computePoincareMap(originalSet,n);
}

/*__________________________________________________________________________*/

template <typename DS, typename FunT>
template<typename T>
typename PoincareMap<DS,FunT>::VectorType
PoincareMap<DS,FunT>::operator()(T& originalSet, MatrixType& der, HessianType& hessian, int n)
{
  ScalarType returnTime = TypeTraits<ScalarType>::zero();
  return (*this)(originalSet,der,hessian,returnTime,n);
}

/*__________________________________________________________________________*/

template <typename DS, typename FunT>
template<typename T>
typename PoincareMap<DS,FunT>::VectorType
PoincareMap<DS,FunT>::operator()(T& originalSet, typename T::JetType& result, ScalarType& out_returnTime, int n)
{
  // the originalSet after this function contains a set just after section
  // we move the set close to section
  this->derivativeOfFlow = NULL;
  this->hessianOfFlow = NULL;
  this->jet = &result;
  this->returnTime = &out_returnTime;

  // We move the set to be very close to the section
  T setAfterTheSection = this->reachSection(originalSet, n);
  VectorType bound(this->m_solver.dimension(),true);
  ScalarType oneStepReturnTime;

  // try to cross section in one step and use Newton method
  if(this->crossSection<T>(originalSet,oneStepReturnTime,bound)){
    out_returnTime = originalSet.getCurrentTime() + oneStepReturnTime;
    originalSet = setAfterTheSection;
    result = this->m_solver.getCurve().jet(oneStepReturnTime);
    return bound;
  }

  // if not possible cross section by
  // and compute the PoincareMap image
  return this->crossSection<T>(originalSet, setAfterTheSection);
}

/*__________________________________________________________________________*/

template <typename DS, typename FunT>
template<typename T>
typename PoincareMap<DS,FunT>::VectorType
PoincareMap<DS,FunT>::operator()(T& originalSet, typename T::JetType& result, int n)
{
  ScalarType returnTime = TypeTraits<ScalarType>::zero();
  return (*this)(originalSet,result,returnTime,n);
}

}} // namespace capd::poincare

#endif // _CAPD_POINCARE_POINCARE_MAP_TEMPLATE_OPERATOR_H_

/// @}
