/////////////////////////////////////////////////////////////////////////////
/// @file CnDoubletonSet.hpp
///
/// @author Daniel Wilczak
/////////////////////////////////////////////////////////////////////////////

// Copyright (C) 2000-2012 by the CAPD Group.
//
// This file constitutes a part of the CAPD library, 
// distributed under the terms of the GNU General Public License.
// Consult  http://capd.ii.uj.edu.pl/ for details. 

#ifndef _CAPD_DYNSET_CNDOUBLETON_HPP_
#define _CAPD_DYNSET_CNDOUBLETON_HPP_

#include "capd/dynset/CnDoubletonSet.h"
#include "capd/matrixAlgorithms/floatMatrixAlgorithms.hpp"

namespace capd{
namespace dynset{

// ----------------------------- CONSTRUCTORS --------------------------------

template<typename MatrixT, typename Policies, __size_type DEGREE>
CnDoubletonSet<MatrixT,Policies,DEGREE>::CnDoubletonSet(const VectorType& x, size_type degree, ScalarType t)
  : SetType(x,degree,t),
    m_x(x.dimension(),degree),
    m_r(x.dimension(),degree),
    m_r0(x.dimension(),degree),
    phi(x.dimension(),degree),
    rem(x.dimension(),degree),
    enc(x.dimension(),degree),
    m_C(1,x.dimension(),degree,MatrixT::Identity(x.dimension())),
    m_B(1,x.dimension(),degree,MatrixT::Identity(x.dimension())),
    m_invB(1,x.dimension(),degree,MatrixT::Identity(x.dimension()))
{
  this->m_x() = x;
  typename MatrixT::RefColumnVectorType refX = this->m_x();
  typename MatrixT::RefColumnVectorType refR0 = this->m_r0();
  split(refX,refR0);

  this->m_x.setMatrix(this->m_C(0));
  this->m_currentSet.setMatrix(this->m_C(0));
  this->m_lastEnclosure.setMatrix(this->m_C(0));
}


// ------------------------------ move ---------------------------------------

template<typename MatrixT, typename Policies, __size_type DEGREE>
template<typename DynSysT>
void CnDoubletonSet<MatrixT,Policies,DEGREE>::move(DynSysT& cndynsys, CnDoubletonSet& result) const
{
  const size_type dimension = this->dimension();
  VectorType xx(*this);
  VectorType x(this->m_x), s(dimension);
  MatrixType B(dimension,dimension);
  MatrixType J(dimension,dimension);
  MatrixType Q(dimension,dimension);
  VectorType y = cndynsys.encloseCnMap(this->getCurrentTime(),x,xx,result.phi,result.rem,result.enc);

  typename CoordSystemContainer::const_iterator thisB = this->m_B.begin();
  typename CoordSystemContainer::const_iterator thisInvB = this->m_invB.begin();
  typename CoordSystemContainer::const_iterator thisC = this->m_C.begin();

  typename CoordSystemContainer::iterator resultB = result.m_B.begin();
  typename CoordSystemContainer::iterator resultInvB = result.m_invB.begin();
  typename CoordSystemContainer::iterator resultC = result.m_C.begin();

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
  MatrixType A = MatrixType(result.phi);
  y = y + nonlinearPart;
  resultCurrentSet = y + A*(xx-thisX);

  *resultC = A * (*thisC);
  B = *resultB = A * (*thisB);

  // here we compute enclosure of the image after one iteration of the map/flow
  if(!intersection(resultCurrentSet,y + (*resultC)*thisR0 + B*thisR,resultCurrentSet))
    throw std::runtime_error("CnDoubletonSet - fatal error! Intersection of two different representations of a set is empty!");

  // here we compute representation for the new set
  // xx is unnecessary now
  split(*resultC, Q);
  split(y, xx);
  resultX = y;
  xx += Q*thisR0;
  if(!subset(resultX,resultCurrentSet))
    throw std::runtime_error("DoubletonSet - fatal error! Intersection of two different representations of a set is empty!");

  // we assume that Policies provides algorithms for computation
  // of B, its inverse invB
  this->Policies::computeBinvB(*resultB,*resultInvB,thisR);

  // eventually we compute new representation of r
  resultR = (*resultInvB*B)*thisR + (*resultInvB)*xx;
  if(!intersection(resultCurrentSet,resultX + (*resultC)*resultR0 + (*resultB)*resultR,resultCurrentSet))
    throw std::runtime_error("CnDoubletonSet - fatal error! Intersection of two different representations of a set is empty!");
  this->Policies::reorganizeIfNeeded(*resultB,*resultInvB,resultR,*resultC,resultR0);

  // C^1 -- C^r part
  capd::vectalg::addAssignObjectObject(result.phi,result.rem);
  A = MatrixType(result.phi);
  if(this->degree()>0)
    substitutionPowerSeries(result.phi,this->m_currentSet,result.rem,true);

  for(
      thisB++, thisInvB++, thisC++, resultB++, resultInvB++, resultC++,
      thisX.next(), thisR.next(), thisR0.next(), resultX.next(), resultR.next(), resultR0.next(),
      nonlinearPart.next(), thisCurrentSet.next(), resultCurrentSet.next();

      thisB!=this->m_B.end();

      thisB++, thisInvB++, thisC++, resultB++, resultInvB++, resultC++,
      thisX.next(), thisR.next(), thisR0.next(), resultX.next(), resultR.next(), resultR0.next(),
      nonlinearPart.next(), thisCurrentSet.next(), resultCurrentSet.next()
      )
  {
    y = A*thisX + nonlinearPart;
    B = *resultB = A*(*thisB);
    *resultC = A*(*thisC);
    resultCurrentSet = A*thisCurrentSet + nonlinearPart;
    intersection(y + B*thisR + (*resultC)*thisR0,resultCurrentSet,resultCurrentSet);

    split(*resultC, Q);
    resultX = y + Q*thisR0;
    split(resultX, xx);

    // we assume that Policies provides algorithms for computation
    // of B, its inverse invB
    this->Policies::computeBinvB(*resultB,*resultInvB,thisR);

    // eventually we compute new representation of r
    resultR = (*resultInvB*B)*thisR + (*resultInvB)*xx;
    this->Policies::reorganizeIfNeeded(*resultB,*resultInvB,resultR,*resultC,resultR0);
  }

  result.setCurrentTime(this->getCurrentTime()+cndynsys.getStep());

  // this might be seen as unnecessary step because we have
  // enc - in Cnrect2Set
  // m_lastEnclosure inherited from CnSet
  // enc, however, might be in an inconsistent state if enclosure could not be found.
  // m_lastEnclosure allways contains valid enclosure copied here after successful integration step.
  result.setLastJetEnclosure(this->enc);
}

}} // namespace capd::dynset

#endif // _CAPD_DYNSET_CNDOUBLETON_HPP_
