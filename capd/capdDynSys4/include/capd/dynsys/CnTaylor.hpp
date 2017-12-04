/// @addtogroup dynsys
/// @{

/////////////////////////////////////////////////////////////////////////////
/// @file CnTaylor.hpp
///
/// @author Daniel Wilczak
/////////////////////////////////////////////////////////////////////////////

// Copyright (C) 2000-2012 by the CAPD Group.
//
// This file constitutes a part of the CAPD library, 
// distributed under the terms of the GNU General Public License.
// Consult  http://capd.ii.uj.edu.pl/ for details.

#ifndef _CAPD_DYNSYS_CNTAYLOR_HPP_ 
#define _CAPD_DYNSYS_CNTAYLOR_HPP_ 

#include "capd/dynsys/DynSys.hpp"
#include "capd/dynsys/BasicCnTaylor.hpp"
#include "capd/dynsys/Taylor.hpp"
#include "capd/dynsys/CnTaylor.h"
#include "capd/dynsys/CnTaylor_phi.hpp"
#include "capd/dynsys/CnTaylor_enclosure.hpp"
#include "capd/dynsys/CnTaylor_remainder.hpp"

namespace capd{
namespace dynsys{

template<typename MapT, typename StepControlT, typename CurveT>
CnTaylor<MapT,StepControlT,CurveT>::CnTaylor(MapT& vectorField, size_type order, const StepControlType& stepControl)
  : BasicCnTaylor<MapType,StepControlT,CurveT>(vectorField,order,stepControl){}


// ####################################################################

template<typename MapType,typename StepControlT, typename CurveT>
void CnTaylor<MapType,StepControlT,CurveT>::encloseC2Map(
    const ScalarType& t,
    const VectorType& x,
    const VectorType& xx,
    VectorType& o_phi,
    VectorType& o_rem,
    VectorType& o_enc,
    MatrixType& o_jacPhi,
    MatrixType& o_jacRem,
    MatrixType& o_jacEnc,
    HessianType& o_hessianPhi,
    HessianType& o_hessianRem,
    HessianType& o_hessianEnc
  )
{
  this->setCurrentTime(t);

  const size_type order = this->getOrder();
  // set initial condition
  VectorType* center = this->getCoefficientsAtCenter();
  //unsigned i,j,c;
  size_type i,j,c;

  for(i=0;i<this->dimension();++i)
  {
    center[0][i] = x[i];
    this->coefficient(i,0) = xx[i];
    for(j=0;j<this->dimension();++j)
    {
      this->coefficient(i,j,0) =
          (i==j) ? TypeTraits<ScalarType>::one() : TypeTraits<ScalarType>::zero();
      for(c=j;c<this->dimension();++c)
        this->coefficient(i,j,c,0) = TypeTraits<ScalarType>::zero();
    }
  }

  this->m_vField->computeODECoefficients(center,order);
  this->m_vField->computeODECoefficients(this->getCoefficients(),2,order);
  this->computeTimeStep(t,xx);

  o_enc = this->enclosure(t,xx);
  this->c2Enclosure(o_enc,o_jacEnc,o_hessianEnc);
  this->c2Remainder(o_enc,o_jacEnc,o_hessianEnc,o_rem,o_jacRem,o_hessianRem);

  for(i=0;i<this->dimension();++i)
  {
    o_phi[i] = center[order][i];
    for(j=0;j<this->dimension();++j)
    {
      o_jacPhi(i+1,j+1) = this->coefficient(i,j,order);
      for(c=j;c<this->dimension();++c)
        o_hessianPhi(i,j,c) = this->coefficient(i,j,c,order);
    }
  }
  for(int r=order-1;r>=0;--r)
  {
    for(i=0;i<this->dimension();++i)
    {
      o_phi[i] = o_phi[i]*this->m_step + center[r][i];
      for(j=0;j<this->dimension();++j)
      {
        o_jacPhi(i+1,j+1) = o_jacPhi(i+1,j+1)*this->m_step + this->coefficient(i,j,r);
        for(c=j;c<this->dimension();++c)
          o_hessianPhi(i,j,c) = o_hessianPhi(i,j,c)*this->m_step + this->coefficient(i,j,c,r);
      }
    }
  }
}

// ####################################################################

template<typename MapType,typename StepControlT, typename CurveT>
void CnTaylor<MapType,StepControlT,CurveT>::encloseC1Map(
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
  this->setCurrentTime(t);

  const size_type order = this->getOrder();
  // set initial condition
  VectorType* center = this->getCoefficientsAtCenter();
  size_type i,j;

  for(i=0;i<this->dimension();++i)
  {
    center[0][i] = x[i];
    this->coefficient(i,0) = xx[i];
    for(j=0;j<this->dimension();++j)
    {
      this->coefficient(i,j,0) =
          (i==j) ? TypeTraits<ScalarType>::one() : TypeTraits<ScalarType>::zero();
    }
  }

  this->m_vField->computeODECoefficients(center,order);
  this->m_vField->computeODECoefficients(this->getCoefficients(),1,order);
  this->computeTimeStep(t,xx);

  o_enc = this->enclosure(t,xx);
  o_jacEnc = this->jacEnclosure(t,o_enc);
  this->JacRemainder(t,o_enc,o_jacEnc,o_rem,o_jacRem);

  for(i=0;i<this->dimension();++i)
  {
    o_phi[i] = center[order][i];
    for(j=0;j<this->dimension();++j)
      o_jacPhi(i+1,j+1) = this->coefficient(i,j,order);
  }
  for(int r=order-1;r>=0;--r)
  {
    for(i=0;i<this->dimension();++i)
    {
      o_phi[i] = o_phi[i]*this->m_step + center[r][i];
      for(j=0;j<this->dimension();++j)
        o_jacPhi(i+1,j+1) = o_jacPhi(i+1,j+1)*this->m_step + this->coefficient(i,j,r);
    }
  }
}

// ####################################################################

template<typename MapType,typename StepControlT, typename CurveT>
void CnTaylor<MapType,StepControlT,CurveT>::encloseC0Map(
    const ScalarType& t,
    const VectorType& x,
    const VectorType& xx,
    VectorType& o_phi,
    VectorType& o_rem,
    VectorType& o_enc,
    MatrixType& o_jacPhi
  )
{
  this->setCurrentTime(t);

  const size_type order = this->getOrder();
  // set initial condition
  VectorType* center = this->getCoefficientsAtCenter();
  size_type i,j;

  for(i=0;i<this->dimension();++i)
  {
    center[0][i] = x[i];
    this->coefficient(i,0) = xx[i];
    for(j=0;j<this->dimension();++j)
    {
      this->coefficient(i,j,0) =
          (i==j) ? TypeTraits<ScalarType>::one() : TypeTraits<ScalarType>::zero();
    }
  }

  this->m_vField->computeODECoefficients(center,order);
  this->m_vField->computeODECoefficients(this->getCoefficients(),1,order);
  this->computeTimeStep(t,xx);

  o_rem = this->Remainder(t,xx,o_enc);

  for(i=0;i<this->dimension();++i)
  {
    o_phi[i] = center[order][i];
    for(j=0;j<this->dimension();++j)
      o_jacPhi(i+1,j+1) = this->coefficient(i,j,order);
  }
  for(int r=order-1;r>=0;--r)
  {
    for(i=0;i<this->dimension();++i)
    {
      o_phi[i] = o_phi[i]*this->m_step + center[r][i];
      for(j=0;j<this->dimension();++j)
        o_jacPhi(i+1,j+1) = o_jacPhi(i+1,j+1)*this->m_step + this->coefficient(i,j,r);
    }
  }
}

// ####################################################################

template<typename MapType,typename StepControlT, typename CurveT>
typename CnTaylor<MapType,StepControlT,CurveT>::ScalarType
CnTaylor<MapType,StepControlT,CurveT>::getCoeffNorm(size_type r, size_type degree) const
{
  typename TypeTraits<ScalarType>::Real result = 0;
  size_type i;
  for(i=0;i<this->dimension();++i){
    result = capd::max(result,rightBound(this->remainderCoefficient(i,r)) - leftBound(this->remainderCoefficient(i,r)));
    result = capd::max(result,rightBound(this->coefficient(i,r)) - leftBound(this->coefficient(i,r)));
  }

  if(degree)
  {
    const typename CurveT::JetType& c = this->getCoefficients()[r];
    const typename CurveT::JetType& rem = this->getRemainderCoefficients()[r];

    for(i=0;i<this->dimension();++i)
    {
      typename CurveT::JetType::const_iterator b = c.begin(i,1), e=c.end(i,degree);
      typename CurveT::JetType::const_iterator b1 = rem.begin(i,1);
      while(b!=e)
      {
        result = capd::max(result,rightBound(*b)-leftBound(*b));
        result = capd::max(result,rightBound(*b1)-leftBound(*b1));
        ++b;
        ++b1;
      }
    }
  }

  return ScalarType(result);
}

}}

#endif // _CAPD_DYNSYS_CNTAYLOR_HPP_ 

/// @}
