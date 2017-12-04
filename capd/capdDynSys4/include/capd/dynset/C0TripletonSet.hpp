/////////////////////////////////////////////////////////////////////////////
/// @file C0TripletonSet.hpp
///
/// @author Daniel Wilczak
/////////////////////////////////////////////////////////////////////////////

// Copyright (C) 2000-2012 by the CAPD Group.
//
// This file constitutes a part of the CAPD library,
// distributed under the terms of the GNU General Public License.
// Consult  http://capd.ii.uj.edu.pl/ for details.

#ifndef _CAPD_DYNSET_C0TRIPLETONSET_HPP_
#define _CAPD_DYNSET_C0TRIPLETONSET_HPP_

#include <stdexcept>
#include "capd/vectalg/iobject.hpp"
#include "capd/dynset/C0TripletonSet.h"
#include "capd/matrixAlgorithms/floatMatrixAlgorithms.hpp"

namespace capd {
namespace dynset {

template<typename MatrixType, typename Policies>
C0TripletonSet<MatrixType,Policies>::C0TripletonSet(const VectorType& x, ScalarType t)
  : SetType(x,VectorType(x.dimension()),t),
    m_x(x.dimension()),
    m_C(x.dimension(),x.dimension()),
    m_B(x.dimension(),x.dimension()),
    m_Q(x.dimension(),x.dimension()),
    m_r0(x.dimension()),
    m_r(x.dimension()),
    m_q(x.dimension()),
    m_invB(x.dimension(),x.dimension()),
    m_invQ(x.dimension(),x.dimension())
{
  split(x,m_x,m_r0);
  for(size_type i=1;i<=x.dimension();++i)
    m_invQ(i,i) = m_invB(i,i) = m_C(i,i) = m_Q(i,i) = m_B(i,i) = typename ScalarType::BoundType(1.);
}

template<typename MatrixType, typename Policies>
C0TripletonSet<MatrixType,Policies>::C0TripletonSet(const VectorType& x, const VectorType& r, ScalarType t)
  : SetType(x+r,VectorType(x.dimension()),t),
    m_x(x.dimension()),
    m_C(x.dimension(),x.dimension()),
    m_B(x.dimension(),x.dimension()),
    m_Q(x.dimension(),x.dimension()),
    m_r0(r),
    m_r(x.dimension()),
    m_q(x.dimension()),
    m_invB(x.dimension(),x.dimension()),
    m_invQ(x.dimension(),x.dimension())
{
  split(x+r,m_x,m_r0);
  for(size_type i=1;i<=x.dimension();++i)
    m_invQ(i,i) = m_invB(i,i) = m_C(i,i) = m_Q(i,i) = m_B(i,i) = typename ScalarType::BoundType(1.);
}

template<typename MatrixType, typename Policies>
C0TripletonSet<MatrixType,Policies>::C0TripletonSet(const VectorType& x, const MatrixType& C, const VectorType& r0, ScalarType t)
  : SetType(x+C*r0,VectorType(x.dimension()),t),
    m_x(x.dimension()),
    m_C(C),
    m_B(x.dimension(),x.dimension()),
    m_Q(x.dimension(),x.dimension()),
    m_r0(r0),
    m_r(x.dimension()),
    m_q(x.dimension()),
    m_invB(x.dimension(),x.dimension()),
    m_invQ(x.dimension(),x.dimension())
{
  split(x,m_x,m_r);
  m_q = m_r;
  for(size_type i=1;i<=x.dimension();++i)
    m_invQ(i,i) = m_invB(i,i) = m_Q(i,i) = m_B(i,i) = typename ScalarType::BoundType(1.);
}

template<typename MatrixType, typename Policies>
C0TripletonSet<MatrixType,Policies>::C0TripletonSet(const VectorType& x, const MatrixType& C, const VectorType& r0, const MatrixType& B, const VectorType& r, ScalarType t)
  : SetType(x+C*r0+r,VectorType(x.dimension()),t),
    m_x(x.dimension()),
    m_C(C),
    m_B(B),
    m_Q(B),
    m_r0(r0),
    m_r(x.dimension()),
    m_q(x.dimension()),
    m_invB(x.dimension(),x.dimension()),
    m_invQ(x.dimension(),x.dimension())
{
  split(x+r,m_x,m_r);
  m_invQ = m_invB = capd::matrixAlgorithms::krawczykInverse(B);
}

template<typename MatrixType, typename Policies>
C0TripletonSet<MatrixType,Policies>::C0TripletonSet(const VectorType& x, const MatrixType& C, const VectorType& r0, const VectorType& r, ScalarType t)
  : SetType(x+C*r0+r,VectorType(x.dimension()),t),
    m_x(x.dimension()),
    m_C(C),
    m_B(x.dimension(),x.dimension()),
    m_Q(x.dimension(),x.dimension()),
    m_r0(r0),
    m_r(x.dimension()),
    m_q(x.dimension()),
    m_invB(x.dimension(),x.dimension()),
    m_invQ(x.dimension(),x.dimension())
{
  split(x+r,m_x,m_r);
  m_q = m_r;
  for(size_type i=1;i<=x.dimension();++i)
    m_invQ(i,i) = m_invB(i,i) = m_Q(i,i) = m_B(i,i) = typename ScalarType::BoundType(1.);
}

template <typename MatrixType, typename Policies>
void C0TripletonSet<MatrixType, Policies>::move(DynSysType & dynsys) {
  move(dynsys, *this);
}

template <typename MatrixType, typename Policies>
void C0TripletonSet<MatrixType, Policies>::move(DynSysType & dynsys, C0TripletonSet& result) const
{
  VectorType y(m_x.dimension()), rem(m_x.dimension()), enc(m_x.dimension());
  MatrixType jacPhi(m_x.dimension(), m_x.dimension());

  // the following function can throw an exception leaving output parameters in an inconsistent state
  // do not overwrite parameters of the set until we are sure that they are computed correctly
  dynsys.encloseC0Map(this->getCurrentTime(),m_x,this->m_currentSet, y, rem, enc, jacPhi);

  this->move(y,this->m_currentSet-this->m_x,jacPhi,rem,result);

  result.setCurrentTime(this->getCurrentTime()+dynsys.getStep());
  result.setLastEnclosure(enc);
  this->reorganizeIfNeeded(result);
}

// -------------------------------------------------------------

template <typename MatrixType, typename Policies>
void C0TripletonSet<MatrixType, Policies>::move(
      const VectorType& y, const VectorType& deltaX, const MatrixType& jacPhi, const VectorType& rem,
      C0TripletonSet& result
    ) const
{
  int d = m_x.dimension();
  MatrixType B(d,d), Q(d,d), deltaC(d,d);
  VectorType deltaY(d);

  // first enclosure of the image is given by simply interval evaluation
  result.m_x = y+rem;
  result.m_currentSet = result.m_x + jacPhi*deltaX;

  // another enclosure - intersect it with previous enclosure
  result.m_C = jacPhi * m_C;
  B = jacPhi * m_B;
  Q = result.m_Q = jacPhi * m_Q;

  // first we enclose intersection(B*r,Q*qr)
  VectorType qr = intersection(B*m_r,Q*m_q);

  // another enclosure is phi + C*r0 + qr
  if(!intersection(result.m_currentSet,result.m_x + result.m_C*m_r0 + qr, result.m_currentSet))
    throw std::logic_error("C0TripletonSet: empty intersection of two representations");

  // here we compute representation for the new set
  // we will reuse 'rem' for storing of local errors. Old value of 'rem' is unnecessary now.
  split(result.m_C,deltaC);
  split(result.m_x,deltaY);
  deltaY += deltaC*m_r0;
  if(!subset(result.m_x,result.m_currentSet))
    throw std::runtime_error("C0TripletonSet: computed center m_x does not belong to the set!");

  // propagate by QR factorization
  result.m_Q = Q;
  this->Policies::computeBinvB(result.m_Q,result.m_invQ,this->m_q);
  result.m_q = (result.m_invQ*Q)*m_q + result.m_invQ*deltaY;

  // propagate by inverse matrix
  try{
    // Q is unnecessary now
    result.m_B = midMatrix(B);
    result.m_invB = capd::matrixAlgorithms::krawczykInverse(result.m_B);
    result.m_r = (result.m_invB*B)*m_r + result.m_invB*deltaY;

  }catch(...)
  {
    result.m_B = result.m_Q;
    result.m_r = result.m_q;
    result.m_invB = result.m_invQ;
  }

  // eventually we compute new representation of r
  if(&result != this)
    result.m_r0 = m_r0;
}

template <typename MatrixType, typename Policies>
std::string C0TripletonSet<MatrixType, Policies>::show(void) const {
  std::ostringstream descriptor;
  descriptor << name() << ":\n";
  descriptor << " x=" << m_x << "\n B=" << m_B << "\n r=" << m_r << " ";
  descriptor << " m_Q = " << m_Q << "\n" << "m_q=" << m_q << " "
             << "\n C="  << m_C
             << "\n r0=" << m_r0 << " ";
  return descriptor.str();
}

}} // namespace capd::dynset

#endif // _CAPD_DYNSET_C0TRIPLETONSET_HPP_

