/////////////////////////////////////////////////////////////////////////////
/// @file CnRect2Set.h
///
/// @author Daniel Wilczak
/////////////////////////////////////////////////////////////////////////////

// Copyright (C) 2000-2012 by the CAPD Group.
//
// This file constitutes a part of the CAPD library, 
// distributed under the terms of the GNU General Public License.
// Consult  http://capd.ii.uj.edu.pl/ for details. 

#ifndef _CAPD_DYNSET_CNRECT2SET_H_
#define _CAPD_DYNSET_CNRECT2SET_H_

#include <stdexcept>
#include "capd/dynset/CnSet.h"

namespace capd{
namespace dynset{
/// @addtogroup dynset
/// @{

/**
 * Set that stores all derivatives to given order in doubleton form with reorganization moved by QR decomposition method.
 */
template<typename MatrixT, typename Policies = dynset::DefaultPolicy, __size_type DEGREE=0>
class CnRect2Set : public Policies, public CnSet<MatrixT,DEGREE>{
public:
  typedef MatrixT MatrixType;
  typedef CnSet<MatrixT,DEGREE> SetType;
  typedef typename SetType::VectorType VectorType;
  typedef typename SetType::ScalarType ScalarType;
  typedef typename SetType::RefVectorType RefVectorType;
  typedef typename SetType::JetType JetType;
  typedef typename SetType::Multipointer Multipointer;
  typedef typename SetType::Multiindex Multiindex;
  typedef typename MatrixType::size_type size_type;

  // constructors
  CnRect2Set(const VectorType& x, size_type degree, ScalarType t = TypeTraits<ScalarType>::zero());
  CnRect2Set(const VectorType& x, const VectorType& r0, size_type degree, ScalarType t = TypeTraits<ScalarType>::zero());
  CnRect2Set(const VectorType& x, const MatrixType& C, const VectorType& r0, size_type degree, ScalarType t = TypeTraits<ScalarType>::zero());
  CnRect2Set(const VectorType& x, const MatrixType& C, const VectorType& r0, const VectorType& r, size_type degree, ScalarType t = TypeTraits<ScalarType>::zero());
  CnRect2Set(const VectorType& x, const MatrixType& C, const VectorType& r0, const MatrixType& B, const VectorType& r, size_type degree, ScalarType t = TypeTraits<ScalarType>::zero());

  CnRect2Set& operator=(const VectorType&);
  template<class DynSysT>
  void move(DynSysT& cndynsys) { this->move(cndynsys,*this); }

  template<class DynSysT>
  void move(DynSysT& cndynsys, CnRect2Set& result) const;
   
  using SetType::operator VectorType;
  using SetType::operator MatrixType;

private:
  JetType m_x,m_r,m_r0;
  JetType phi, rem, enc;
  MatrixType m_B, m_invB, m_C;
  MatrixType m_Bjac, m_invBjac, m_Cjac;
}; // end of class CnRect2Set

/// @}
}} // the end of the namespace capd::dynset

#endif // _CAPD_DYNSET_CNRECT2SET_H_
