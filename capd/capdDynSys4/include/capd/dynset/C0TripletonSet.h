/////////////////////////////////////////////////////////////////////////////
/// @file C0Tripleton.h
///
/// @author Daniel Wilczak
/////////////////////////////////////////////////////////////////////////////

// Copyright (C) 2000-2013 by the CAPD Group.
//
// This file constitutes a part of the CAPD library,
// distributed under the terms of the GNU General Public License.
// Consult  http://capd.ii.uj.edu.pl/ for details.

#ifndef _CAPD_DYNSET_C0TRIPLETONSET_H_
#define _CAPD_DYNSET_C0TRIPLETONSET_H_

#include "capd/dynset/C0Set.h"

namespace capd{
namespace dynset{
/**
 * Class \b C0TripletonSet \b represents a subset of R^n in the following form
 *
 * x + C*r0 + intersection(B*r , Q*q )
 *
 * where
 *  x is a point vector
 *  C,B,Q are point matrices, where B and Q are invertible and Q is close to orthogonal
 *  r0,r are interval vectors
 *
 *  Moreover it stores rigorous inverse matrices of B and Q
 *
 */

template<typename MatrixT, typename Policies>
class C0TripletonSet : public C0Set<MatrixT>, public Policies
{
public:
  typedef MatrixT MatrixType;
  typedef typename MatrixT::RowVectorType VectorType;
  typedef typename MatrixType::ScalarType ScalarType;
  typedef typename MatrixType::size_type size_type;
  typedef C0Set<MatrixT> SetType;
  typedef typename C0Set<MatrixT>::DynSysType DynSysType;
  typedef C0TripletonSet BaseSet;

  C0TripletonSet(const VectorType& x, ScalarType t = TypeTraits<ScalarType>::zero());
  C0TripletonSet(const VectorType& x, const VectorType& r0, ScalarType t = TypeTraits<ScalarType>::zero());
  C0TripletonSet(const VectorType& x, const MatrixType& C, const VectorType& r0, ScalarType t = TypeTraits<ScalarType>::zero());
  C0TripletonSet(const VectorType& x, const MatrixType& C, const VectorType& r0, const VectorType& r, ScalarType t = TypeTraits<ScalarType>::zero());
  C0TripletonSet(const VectorType& x, const MatrixType& C,
      const VectorType& r0, const MatrixType& B,
      const VectorType& r,
      ScalarType t = TypeTraits<ScalarType>::zero()
    );
  using SetType::m_currentSet;

  /// computes image of the set after one step/iterate of the dynamical system
  void move(DynSysType & dynsys);

  // auxiliary function if we do not need to allocate memory for temporary matrices and vectors
  /// this computes next representation of the set given computed one-step enclosure of the form y + jacPhi*deltaX + rem
  void move(const VectorType& y, const VectorType& deltaX, const MatrixType& jacPhi, const VectorType& rem, C0TripletonSet& result) const;

  /// computes image of the set after one step/iterate of the dynamical system and stores it in result
  void move(DynSysType & dynsys, C0TripletonSet& result) const;

  bool reorganizeIfNeeded(C0TripletonSet& result) const
  {
    if(this->Policies::reorganizeIfNeeded(result.m_Q,result.m_invQ,result.m_q,result.m_C,result.m_r0)){
      result.m_B = result.m_Q;
      result.m_r = result.m_q;
      return true;
    }
    return false;
  }

  using SetType::operator VectorType;

  std::string show() const;
  std::string name() const { return "C0TripletonSet"; }

  // representation of set
  VectorType m_x;
  MatrixType m_C, m_B, m_Q;
  VectorType m_r0, m_r, m_q;

  // computed rigorous inverses of m_B and m_Q
  MatrixType m_invB, m_invQ;
};

/// @}

}} //namespace capd::dynset

#endif // _CAPD_DYNSET_C0TRIPLETONSET_H_
