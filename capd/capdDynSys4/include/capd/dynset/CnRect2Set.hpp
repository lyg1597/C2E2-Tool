/////////////////////////////////////////////////////////////////////////////
/// @file CnRect2Set.hpp
///
/// @author Daniel Wilczak
/////////////////////////////////////////////////////////////////////////////

// Copyright (C) 2000-2012 by the CAPD Group.
//
// This file constitutes a part of the CAPD library, 
// distributed under the terms of the GNU General Public License.
// Consult  http://capd.ii.uj.edu.pl/ for details. 

#ifndef _CAPD_DYNSET_CNRECT2SET_HPP_
#define _CAPD_DYNSET_CNRECT2SET_HPP_

#include "capd/dynset/CnRect2Set.h"
#include "capd/matrixAlgorithms/floatMatrixAlgorithms.hpp"
#include "capd/vectalg/iobject.hpp"
#include "capd/diffAlgebra/Jet.hpp"

namespace capd{
namespace dynset{

// ----------------------------- CONSTRUCTORS --------------------------------
template<typename MatrixType, typename Policies, __size_type DEGREE>
CnRect2Set<MatrixType,Policies,DEGREE>::CnRect2Set(const VectorType& x, size_type degree, ScalarType t)
  : SetType(x,degree,t),
    m_x(x.dimension(),degree),
    m_r(x.dimension(),degree),
    m_r0(x.dimension(),degree),
    phi(x.dimension(),degree),
    rem(x.dimension(),degree),
    enc(x.dimension(),degree),
    m_B(x.dimension(),x.dimension()),
    m_invB(x.dimension(),x.dimension()),
    m_C(x.dimension(),x.dimension()),
    m_Bjac(x.dimension(),x.dimension()),
    m_invBjac(x.dimension(),x.dimension()),
    m_Cjac(x.dimension(),x.dimension())
{
  m_x() = x;
  split(m_x,m_r0);
  m_C.setToIdentity();
  m_B.setToIdentity();
  m_invB.setToIdentity();
  m_Cjac.setToIdentity();
  m_Bjac.setToIdentity();
  m_invBjac.setToIdentity();
  m_x.setMatrix(m_Cjac);
  this->m_currentSet.setMatrix(m_Cjac);
  this->m_lastEnclosure.setMatrix(m_Cjac);
}

template<typename MatrixType, typename Policies, __size_type DEGREE>
CnRect2Set<MatrixType,Policies,DEGREE>::CnRect2Set(const VectorType& x, const VectorType& r0, size_type degree, ScalarType t)
  : SetType(x+r0,degree,t),
    m_x(x.dimension(),degree),
    m_r(x.dimension(),degree),
    m_r0(x.dimension(),degree),
    phi(x.dimension(),degree),
    rem(x.dimension(),degree),
    enc(x.dimension(),degree),
    m_B(x.dimension(),x.dimension()),
    m_invB(x.dimension(),x.dimension()),
    m_C(x.dimension(),x.dimension()),
    m_Bjac(x.dimension(),x.dimension()),
    m_invBjac(x.dimension(),x.dimension()),
    m_Cjac(x.dimension(),x.dimension())
{
  m_x() = x;
  m_r0() = r0;
  m_C.setToIdentity();
  m_B.setToIdentity();
  m_invB.setToIdentity();
  m_Cjac.setToIdentity();
  m_Bjac.setToIdentity();
  m_invBjac.setToIdentity();
  m_x.setMatrix(m_Cjac);
  this->m_currentSet.setMatrix(m_Cjac);
  this->m_lastEnclosure.setMatrix(m_Cjac);
}

template<typename MatrixType, typename Policies, __size_type DEGREE>
CnRect2Set<MatrixType,Policies,DEGREE>::CnRect2Set(const VectorType& x, const MatrixType& C, const VectorType& r0, size_type degree, ScalarType t)
  : SetType(x+C*r0,degree,t),
    m_x(x.dimension(),degree),
    m_r(x.dimension(),degree),
    m_r0(x.dimension(),degree),
    phi(x.dimension(),degree),
    rem(x.dimension(),degree),
    enc(x.dimension(),degree),
    m_B(x.dimension(),x.dimension()),
    m_invB(x.dimension(),x.dimension()),
    m_C(x.dimension(),x.dimension()),
    m_Bjac(x.dimension(),x.dimension()),
    m_invBjac(x.dimension(),x.dimension()),
    m_Cjac(x.dimension(),x.dimension())
{
  m_x() = x;
  m_r0() = r0;
  m_C = C;
  m_B.setToIdentity();
  m_invB.setToIdentity();
  m_Cjac.setToIdentity();
  m_Bjac.setToIdentity();
  m_invBjac.setToIdentity();
  m_x.setMatrix(m_Cjac);
  this->m_currentSet.setMatrix(m_Cjac);
  this->m_lastEnclosure.setMatrix(m_Cjac);
}

template<typename MatrixType, typename Policies, __size_type DEGREE>
CnRect2Set<MatrixType,Policies,DEGREE>::CnRect2Set(const VectorType& x, const MatrixType& C, const VectorType& r0, const VectorType& r, size_type degree, ScalarType t)
  : SetType(x+C*r0+r,degree,t),
    m_x(x.dimension(),degree),
    m_r(x.dimension(),degree),
    m_r0(x.dimension(),degree),
    phi(x.dimension(),degree),
    rem(x.dimension(),degree),
    enc(x.dimension(),degree),
    m_B(x.dimension(),x.dimension()),
    m_invB(x.dimension(),x.dimension()),
    m_C(x.dimension(),x.dimension()),
    m_Bjac(x.dimension(),x.dimension()),
    m_invBjac(x.dimension(),x.dimension()),
    m_Cjac(x.dimension(),x.dimension())
{
  m_x() = x;
  m_r0() = r0;
  m_r() = r;
  m_C = C;
  m_B.setToIdentity();
  m_invB.setToIdentity();
  m_Cjac.setToIdentity();
  m_Bjac.setToIdentity();
  m_invBjac.setToIdentity();
  m_x.setMatrix(m_Cjac);
  this->m_currentSet.setMatrix(m_Cjac);
  this->m_lastEnclosure.setMatrix(m_Cjac);
}

template<typename MatrixType, typename Policies, __size_type DEGREE>
CnRect2Set<MatrixType,Policies,DEGREE>::CnRect2Set(const VectorType& x, const MatrixType& C, const VectorType& r0, const MatrixType& B, const VectorType& r, size_type degree, ScalarType t)
  : SetType(x+C*r0+B*r,degree,t),
    m_x(x.dimension(),degree),
    m_r(x.dimension(),degree),
    m_r0(x.dimension(),degree),
    phi(x.dimension(),degree),
    rem(x.dimension(),degree),
    enc(x.dimension(),degree),
    m_B(x.dimension(),x.dimension()),
    m_invB(x.dimension(),x.dimension()),
    m_C(x.dimension(),x.dimension()),
    m_Bjac(x.dimension(),x.dimension()),
    m_invBjac(x.dimension(),x.dimension()),
    m_Cjac(x.dimension(),x.dimension())
{
  m_x() = x;
  m_r0() = r0;
  m_r() = r;
  m_C = C;
  m_B = B;
  m_invB = capd::matrixAlgorithms::gaussInverseMatrix(B);
  m_Cjac.setToIdentity();
  m_Bjac.setToIdentity();
  m_invBjac.setToIdentity();
  m_x.setMatrix(m_Cjac);
  this->m_currentSet.setMatrix(m_Cjac);
  this->m_lastEnclosure.setMatrix(m_Cjac);
}

// ----------------------------------------------------------------------------

template<typename MatrixType, typename Policies, __size_type DEGREE>
CnRect2Set<MatrixType,Policies,DEGREE>& CnRect2Set<MatrixType,Policies,DEGREE>::operator=(const VectorType& v)
{
  if(v.dimension()!=this->dimension())
    throw std::runtime_error("CnRect2Set::operator=(Vector) - incorrect dimensions");
  VectorType temp(this->dimension());
  VectorType y = v;
  split(y,temp);
  m_x.clear();
  m_r.clear();
  m_r0.clear();
  this->m_currentSet.clear();

  m_C.setToIdentity();
  m_B.setToIdentity();
  m_invB.setToIdentity();
  m_Cjac.setToIdentity();
  m_Bjac.setToIdentity();
  m_invBjac.setToIdentity();
   
  m_x() = y;
  m_r0() = temp;
  this->m_currentSet() = v;
  this->m_lastEnclosure() = v;
  if(this->degree()>0)
  {
    for(size_type i=0;i<this->dimension();++i)
      this->m_lastEnclosure(i,i) = this->m_currentSet(i,i) = TypeTraits<ScalarType>::one();
  }
  return *this;
}

// ------------------------------ move ---------------------------------------

template<typename MatrixType, typename Policies, __size_type DEGREE>
template<class DynSysT>
void CnRect2Set<MatrixType,Policies,DEGREE>::move(DynSysT& cndynsys, CnRect2Set& result) const
{
  const size_type dimension = this->dimension();
  VectorType xx(*this);
  VectorType x(m_x), s(dimension);
  MatrixType S(dimension,dimension);

  VectorType y = cndynsys.encloseCnMap(this->getCurrentTime(),x,xx,result.phi,result.rem,result.enc);

  typename JetType::RefVectorType thisX = this->m_x();
  typename JetType::RefVectorType thisR = this->m_r();
  typename JetType::RefVectorType thisR0 = this->m_r0();

  typename JetType::RefVectorType resultX = result.m_x();
  typename JetType::RefVectorType resultR = result.m_r();
  typename JetType::RefVectorType resultR0 = result.m_r0();

  typename JetType::RefVectorType thisCurrentSet = this->m_currentSet();
  typename JetType::RefVectorType resultCurrentSet = result.m_currentSet();
  typename JetType::RefVectorType nonlinearPart = result.rem();

  if(this!=&result)
    result.m_r0 = this->m_r0;

  // C^0 part
  y += VectorType(result.rem());

  MatrixType A = MatrixType(result.phi);
  result.m_C = A*m_C;
  MatrixType B = A*m_B;
  result.m_B = B;
  resultCurrentSet = y + result.m_C*thisR0 + B*thisR;
  // here we compute enclosure of the image after one iteration of the map/flow
  if(!intersection(resultCurrentSet,y + (result.m_C)*thisR0 + B*thisR,resultCurrentSet))
    throw std::runtime_error("CnRect2Set - fatal error! Intersection of two different representations of a set is empty!");

  split(result.m_C,S);
  split(y,s);
  resultX = y;
  s += S * thisR0;
  this->Policies::computeBinvB(result.m_B,result.m_invB,thisR);
  resultR = (B*result.m_invB)*thisR + result.m_invB*s;
  if(!intersection(resultCurrentSet, resultX + (result.m_C)*resultR0 + (result.m_B)*resultR, resultCurrentSet))
    throw std::runtime_error("CnRect2Set - fatal error! Intersection of two different representations of a set is empty!");
  this->Policies::reorganizeIfNeeded(result.m_B,result.m_invB,resultR,result.m_C,resultR0);

//   Phi += Rem
  capd::vectalg::addAssignObjectObject(result.phi,result.rem);

  A = MatrixType(result.phi); // in fact A += Rem
  MatrixType C = result.m_Cjac = A * m_Cjac;
  split(result.m_Cjac,S);
  B = result.m_Bjac = A*m_Bjac;
  this->Policies::computeBinvB(result.m_Bjac,result.m_invBjac,(MatrixType)this->m_r);
  MatrixType J=result.m_Bjac*B;

// C^1 -- C^r part
  // Rem is now unnecessary. We will store in Rem a nonlinear part of composition
  if(this->degree()>0)
  {
    substitutionPowerSeries(result.phi,this->m_currentSet,result.rem,true);
  }

  size_type i=1;
  typename ScalarType::BoundType maxSizeR = 0., maxSizeR0 = 0.;
  for(
      thisX.next(), thisR.next(), thisR0.next(), resultX.next(), resultR.next(), resultR0.next(),
      nonlinearPart.next(), thisCurrentSet.next(), resultCurrentSet.next();

      i < newton(dimension+this->degree(),this->degree());

      thisX.next(), thisR.next(), thisR0.next(), resultX.next(), resultR.next(), resultR0.next(),
      nonlinearPart.next(), thisCurrentSet.next(), resultCurrentSet.next(),++i
      )
  {
      VectorType temp = nonlinearPart + A*thisX;
      resultCurrentSet = temp + C*thisR0 + B*thisR;
      y = temp + S*thisR0;
      split(y,s);
      resultR = (result.m_Bjac*s)  + (J*thisR);
      resultX = y;
      maxSizeR = capd::max(maxSizeR,capd::vectalg::maxDiam(resultR).rightBound());
      maxSizeR0 = capd::max(maxSizeR0,capd::vectalg::maxDiam(resultR0).rightBound());
  }

  if(maxSizeR > 20.*maxSizeR0)
  {
    typename JetType::RefVectorType resultR = result.m_r();
    typename JetType::RefVectorType resultR0 = result.m_r0();
    B = (result.m_Bjac * result.m_Cjac);

    for(i=1, resultR.next(), resultR0.next();
        i < newton(dimension+this->degree(),this->degree());
        resultR.next(), resultR0.next(),++i
        )
    {
      resultR0 = resultR + B * resultR0;
      resultR.clear();
    }

    result.m_Cjac = result.m_Bjac;
    result.m_Bjac.setToIdentity();
  }

  result.setCurrentTime(this->getCurrentTime() + cndynsys.getStep());

  // this might be seen as unnecessary step because we have
  // enc - in Cnrect2Set
  // m_lastEnclosure inherited from CnSet
  // enc, however, might be in an inconsistent state if enclosure could not be found.
  // m_lastEnclosure always contains valid enclosure copied here after successful integration step.
  result.setLastJetEnclosure(this->enc);
}

}} // namespace capd::dynset

#endif // _CAPD_DYNSET_CNRECT2SET_HPP_
