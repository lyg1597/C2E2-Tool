/////////////////////////////////////////////////////////////////////////////
/// @file C0DoubletonSet.hpp
///
/// @author Daniel Wilczak
/////////////////////////////////////////////////////////////////////////////

// Copyright (C) 2000-2012 by the CAPD Group.
//
// This file constitutes a part of the CAPD library,
// distributed under the terms of the GNU General Public License.
// Consult  http://capd.ii.uj.edu.pl/ for details.

#ifndef _CAPD_DYNSET_C0DOUBLETONSET_HPP_
#define _CAPD_DYNSET_C0DOUBLETONSET_HPP_

#include <stdexcept>
#include "capd/vectalg/iobject.hpp"
#include "capd/dynset/C0DoubletonSet.h"
#include "capd/geomset/CenteredDoubletonSet.hpp"
#include "capd/matrixAlgorithms/floatMatrixAlgorithms.hpp"

namespace capd {
namespace dynset {

template<typename MatrixType, typename Policies>
C0DoubletonSet<MatrixType,Policies>::C0DoubletonSet(const VectorType& x, ScalarType t)
  : SetType(x,VectorType(x.dimension()),t),
    BaseSet(x)
{}

template<typename MatrixType, typename Policies>
C0DoubletonSet<MatrixType,Policies>::C0DoubletonSet(const VectorType& x, const VectorType& r, ScalarType t)
  : SetType(x+r,VectorType(x.dimension()),t),
    BaseSet(x,r)
{}

template<typename MatrixType, typename Policies>
C0DoubletonSet<MatrixType,Policies>::C0DoubletonSet(const VectorType& x, const MatrixType& C, const VectorType& r0, ScalarType t)
  : SetType(x+C*r0,VectorType(x.dimension()),t),
    BaseSet(x,C,r0)
{}

template<typename MatrixType, typename Policies>
C0DoubletonSet<MatrixType,Policies>::C0DoubletonSet(const VectorType& x, const MatrixType& C, const VectorType& r0, const VectorType& r, ScalarType t)
  : SetType(x+C*r0+r,VectorType(x.dimension()),t),
    BaseSet(x,C,r0,r)
{}

template<typename MatrixType, typename Policies>
C0DoubletonSet<MatrixType,Policies>::C0DoubletonSet(const VectorType& x, const MatrixType& C,
    const VectorType& r0, const MatrixType& B,
    const VectorType& r, ScalarType t
)
  : SetType(x+C*r0+B*r,VectorType(x.dimension()),t),
    BaseSet(x,C,r0,B,r)
{}


template <typename MatrixType, typename Policies>
void C0DoubletonSet<MatrixType, Policies>::move(DynSysType & dynsys) {
  move(dynsys, *this);
}

template <typename MatrixType, typename Policies>
void C0DoubletonSet<MatrixType, Policies>::move(const VectorType& y, const VectorType& deltaX, const MatrixType& jacPhi, const VectorType& rem, C0DoubletonSet& result) const {
  MatrixType B(m_x.dimension(), m_x.dimension());
  MatrixType deltaC(m_x.dimension(), m_x.dimension());
  VectorType deltaY(m_x.dimension());

  // first enclosure of the image is given by simply interval evaluation
  result.m_x = y+rem;
  result.m_currentSet = result.m_x + jacPhi*deltaX;

  // another enclosure - intersect it with previous enclosure
  result.m_C = jacPhi * m_C;
  B = result.m_B = jacPhi * m_B;

  if(!intersection(result.m_currentSet,result.m_x + result.m_C * this->m_r0 + result.m_B*this->m_r,result.m_currentSet))
      throw std::logic_error("C0DoubletonSet: intersection of two enclosures of the same object is empty!");

  // here we compute representation for the new set
  // we will reuse 'rem' for storing of local errors. Old value of 'rem' is unnecessary now.
  split(result.m_C, deltaC);
  split(result.m_x, deltaY);
  deltaY += deltaC*m_r0;
  if(!subset(result.m_x,result.m_currentSet))
    throw std::runtime_error("C0DoubletonSet: computed center m_x does not belong to the set!");
  // we assume that Policies provides algorithms for computation
  // of B, its inverse invB and updates
  this->Policies::computeBinvB(result.m_B,result.m_invB,this->m_r);

  // eventually we compute new representation of r
  result.m_r = (result.m_invB * B) * m_r + result.m_invB * deltaY;
  if(&result != this)
    result.m_r0 = m_r0;
}

template <typename MatrixType, typename Policies>
void C0DoubletonSet<MatrixType, Policies>::move(DynSysType & dynsys, C0DoubletonSet& result) const {
  // important: here we assume that both m_r and m_r0 contains zero
  // this is assured by each constructor and each step of this algorithm

  // important: *this and result usually are the same object.
  // We cannot assume that members of *this and result are different objects.

  VectorType y(m_x.dimension()), rem(m_x.dimension()), enc(m_x.dimension());
  MatrixType jacPhi(m_x.dimension(), m_x.dimension());

  // the following function can throw an exception leaving output parameters in an inconsistent state
  // do not overwrite parameters of the set until we are sure that they are computed correctly
  dynsys.encloseC0Map(this->getCurrentTime(),m_x,this->m_currentSet, y, rem, enc, jacPhi);

  this->move(y,this->m_currentSet-this->m_x,jacPhi,rem,result);

  result.setCurrentTime(this->getCurrentTime()+dynsys.getStep());
  result.setLastEnclosure(enc);
  this->Policies::reorganizeIfNeeded(result.m_B,result.m_invB,result.m_r,result.m_C,result.m_r0);
}

template <typename MatrixType, typename Policies>
std::string C0DoubletonSet<MatrixType, Policies>::show(void) const {
  std::ostringstream descriptor;
  descriptor << name() << ":\n" << BaseSet::toString();
  return descriptor.str();
}

}} // namespace capd::dynset

#endif // _CAPD_DYNSET_C0DOUBLETONSET_HPP_

