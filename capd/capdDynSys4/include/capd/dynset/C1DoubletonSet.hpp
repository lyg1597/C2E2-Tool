/////////////////////////////////////////////////////////////////////////////
/// @file C1DoubletonSet.hpp
///
/// @author Daniel Wilczak
/////////////////////////////////////////////////////////////////////////////

// Copyright (C) 2000-2012 by the CAPD Group.
//
// This file constitutes a part of the CAPD library,
// distributed under the terms of the GNU General Public License.
// Consult  http://capd.ii.uj.edu.pl/ for details.

#ifndef _CAPD_DYNSET_C1DOUBLETONSET_HPP_
#define _CAPD_DYNSET_C1DOUBLETONSET_HPP_

#include <stdexcept>
#include "capd/vectalg/iobject.hpp"
#include "capd/dynset/C1DoubletonSet.h"
#include "capd/geomset/CenteredDoubletonSet.hpp"
#include "capd/geomset/MatrixDoubletonSet.hpp"
#include "capd/matrixAlgorithms/floatMatrixAlgorithms.hpp"

namespace capd {
namespace dynset {

template<typename MatrixType, typename Policies>
C1DoubletonSet<MatrixType,Policies>::C1DoubletonSet(const VectorType& x, ScalarType t)
  : SetType(
      x,
      VectorType(x.dimension()),
      MatrixType::Identity(x.dimension()),
      MatrixType(x.dimension(),x.dimension()),
      t),
    C0BaseSet(x),
    C1BaseSet(x.dimension())
{}

template<typename MatrixType, typename Policies>
C1DoubletonSet<MatrixType,Policies>::C1DoubletonSet(const VectorType& x, const VectorType& r0, ScalarType t)
  : SetType(
      x+r0,
      VectorType(x.dimension()),
      MatrixType::Identity(x.dimension()),
      MatrixType(x.dimension(),x.dimension()),
      t),
    C0BaseSet(x, r0),
    C1BaseSet(x.dimension())
{}

template<typename MatrixType, typename Policies>
C1DoubletonSet<MatrixType,Policies>::C1DoubletonSet(const VectorType& x, const MatrixType& C, const VectorType& r0, ScalarType t)
  : SetType(
      x+C*r0,
      VectorType(x.dimension()),
      MatrixType::Identity(x.dimension()),
      MatrixType(x.dimension(),x.dimension()),
      t),
    C0BaseSet(x, C, r0),
    C1BaseSet(x.dimension())
{}

template<typename MatrixType, typename Policies>
C1DoubletonSet<MatrixType,Policies>::C1DoubletonSet(const VectorType& x, const MatrixType& C, const VectorType& r0, const VectorType& r, ScalarType t)
  : SetType(
      x+C*r0+r,
      VectorType(x.dimension()),
      MatrixType::Identity(x.dimension()),
      MatrixType(x.dimension(),x.dimension()),
      t),
    C0BaseSet(x, C, r0, r),
    C1BaseSet(x.dimension())
{}

template<typename MatrixType, typename Policies>
C1DoubletonSet<MatrixType,Policies>::C1DoubletonSet(
      const VectorType& x,
      const MatrixType& C, const VectorType& r0,
      const MatrixType& B, const VectorType& r,
      ScalarType t
   ) : SetType(
      x+C*r0+B*r,
      VectorType(x.dimension()),
      MatrixType::Identity(x.dimension()),
      MatrixType(x.dimension(),x.dimension()),
      t),
    C0BaseSet(x, C, r0, B, r),
    C1BaseSet(x.dimension())
{}

template<typename MatrixType, typename Policies>
C1DoubletonSet<MatrixType,Policies>::C1DoubletonSet(const C0BaseSet & c0part, const C1BaseSet& c1part, ScalarType t)
    : SetType(
       VectorType(c0part),
       VectorType(c0part.dimension()),
       MatrixType(c1part),
       MatrixType(c0part.dimension(),c0part.dimension()),
       t),
   C0BaseSet(c0part),
   C1BaseSet(c1part)
{}


template <typename MatrixType, typename Policies>
void C1DoubletonSet<MatrixType, Policies>::move(DynSysType & dynsys) {
  move(dynsys, *this);
}

template <typename MatrixType, typename Policies>
void C1DoubletonSet<MatrixType, Policies>::move(DynSysType & dynsys, C1DoubletonSet& result) const
{
  // important: here we assume that both m_r and m_r0 contains zero
  // this is assured by each constructor and each step of this algorithm

  const size_type dim = m_x.dimension();
  VectorType y(dim), rem(dim), enc(dim);
  MatrixType jacPhi(dim,dim), jacEnc(dim,dim), jacRem(dim,dim);
  MatrixType B(dim,dim), Q(dim,dim);

  // the following function can throw an exception leaving output parameters in an inconsistent state
  // do not overwrite parameters of the set until we are sure that they are computed correctly
  dynsys.encloseC1Map(this->getCurrentTime(),
                    this->m_x, this->m_currentSet, // input parameters
                    y, rem, enc,                   // C^0 output
                    jacPhi, jacRem, jacEnc         // C^1 output
                    );


  // ---------- C^0 - part -----------------

  // first enclosure of the image is given by simply interval evaluation
  y +=rem;
  result.m_currentSet = y + jacPhi*(this->m_currentSet-this->m_x);

  // another enclosure - intersect it with previous enclosure
  result.m_x = y;
  result.m_C = jacPhi * m_C;
  B = result.m_B = jacPhi * m_B;

  if(!intersection(result.m_currentSet,result.m_x + result.m_C * this->m_r0 + result.m_B*this->m_r,result.m_currentSet))
      throw std::logic_error("C1DoubletonSet: intersection of two enclosures of the same object is empty!");


  // here we compute representation for the new set
  // we will reuse 'rem' for storing of local errors. Old value of 'rem' is unnecessary now.
  split(result.m_C, Q);
  split(result.m_x, rem);
  rem += Q*m_r0;
  if(!subset(result.m_x,result.m_currentSet))
    throw std::runtime_error("C1DoubletonSet - fatal error! Intersection of two different representations of a set is empty!");
  // we assume that Policies provides algorithms for computation
  // of B, its inverse invB and updates
  this->Policies::computeBinvB(result.m_B,result.m_invB,this->m_r);

  // eventually we compute new representation of r
  result.m_r = (result.m_invB * B) * m_r + result.m_invB * rem;

  // ---------- C^1 - part -----------------

  // first enclosure of the image is given by simply interval evaluation
  jacPhi += jacRem;
  result.m_currentMatrix = jacPhi*this->m_currentMatrix;

  // another enclosure - intersect it with previous enclosure
  result.m_D = jacPhi*this->m_D;
  B = result.m_Bjac = jacPhi*this->m_Bjac;
  result.m_Cjac = jacPhi*this->m_Cjac;

  if(!capd::vectalg::intersection(result.m_currentMatrix,result.m_D+result.m_Cjac*m_R0 + result.m_Bjac*m_R,result.m_currentMatrix))
    throw std::runtime_error("C1DoubletonSet - fatal error! Intersection of two different representations of matrix is empty!");

  // here we compute representation for the new set
  // jacRem is unnecessary now
  split(result.m_D, jacRem);
  split(result.m_Cjac, Q);
  jacRem += Q * m_R0;

  // we assume that Policies provides algorithms for computation
  // of B, its inverse invB and updates
  this->Policies::computeBinvB(result.m_Bjac,result.m_invBjac,this->m_R);

  // eventually we compute new representation of r
  result.m_R = (result.m_invBjac * B) * m_R + result.m_invBjac * jacRem;

  if(&result != this){
    result.m_r0 = m_r0;
    result.m_R0 = m_R0;
  }
  result.setCurrentTime(this->getCurrentTime()+dynsys.getStep());
  result.setLastEnclosure(enc);
  result.setLastMatrixEnclosure(jacEnc);

  this->Policies::reorganizeIfNeeded(result.m_B,result.m_invB,result.m_r,result.m_C,result.m_r0);
  this->Policies::reorganizeC1IfNeeded(result.m_Bjac,result.m_invBjac,result.m_R,result.m_Cjac,result.m_R0);
}

template <typename MatrixType, typename Policies>
std::string C1DoubletonSet<MatrixType, Policies>::show(void) const {
  std::ostringstream descriptor;
  descriptor << name()
             << C0BaseSet::toString()
             << C1BaseSet::toString();
  return descriptor.str();
}

}} // namespace capd::dynset

#endif // _CAPD_DYNSET_C1DOUBLETONSET_HPP_

