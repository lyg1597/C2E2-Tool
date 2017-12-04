/////////////////////////////////////////////////////////////////////////////
/// @file CnDoubletonSet.h
///
/// @author Daniel Wilczak
/////////////////////////////////////////////////////////////////////////////

// Copyright (C) 2000-2012 by the CAPD Group.
//
// This file constitutes a part of the CAPD library, 
// distributed under the terms of the GNU General Public License.
// Consult  http://capd.ii.uj.edu.pl/ for details. 

#ifndef _CAPD_DYNSET_CNDOUBLETONSET_H_
#define _CAPD_DYNSET_CNDOUBLETONSET_H_

#include <stdexcept>
#include <vector>
#include <list>
#include "capd/dynset/CnSet.h"
#include "capd/diffAlgebra/Jet.h"

namespace capd{
namespace dynset{
/// @addtogroup dynset
/// @{

/**
 * This set stores vector of derivatives with respect to a multiindex alpha as a doubleton.
 * The evolution is partially defined in policies.
 */
template<typename MatrixT, typename Policies , __size_type DEGREE=0>
class CnDoubletonSet : public Policies, public CnSet<MatrixT,DEGREE>{
public:
  typedef MatrixT MatrixType;
  typedef CnSet<MatrixT,DEGREE> SetType;
  typedef typename MatrixType::size_type size_type;
  typedef typename SetType::VectorType VectorType;
  typedef typename SetType::ScalarType ScalarType;
  typedef typename SetType::RefVectorType RefVectorType;
  typedef typename SetType::JetType JetType;
  typedef typename SetType::Multipointer Multipointer;
  typedef typename SetType::Multiindex Multiindex;
  typedef capd::diffAlgebra::CnContainer<MatrixType,0,0,0> CoordSystemContainer;

  // constructors
  CnDoubletonSet(const VectorType& x, size_type degree, ScalarType t = TypeTraits<ScalarType>::zero());
/*
  CnDoubletonSet(const VectorType& x, const VectorType& r0, unsigned degree, ScalarType t = TypeTraits<ScalarType>::zero());
  CnDoubletonSet(const VectorType& x, const MatrixType& C, const VectorType& r0, unsigned degree, ScalarType t = TypeTraits<ScalarType>::zero());
  CnDoubletonSet(const VectorType& x, const MatrixType& C, const VectorType& r0, const VectorType& r, unsigned degree, ScalarType t = TypeTraits<ScalarType>::zero());
  CnDoubletonSet(const VectorType& x, const MatrixType& C, const VectorType& r0, const MatrixType& B, const VectorType& r, unsigned degree, ScalarType t = TypeTraits<ScalarType>::zero());

  static CnDoubletonSet create(const VectorType& x, unsigned degree, ScalarType t = TypeTraits<ScalarType>::zero());
  static CnDoubletonSet create(const VectorType& x, const VectorType& r0, unsigned degree, ScalarType t = TypeTraits<ScalarType>::zero());
  static CnDoubletonSet create(const VectorType& x, const MatrixType& C, const VectorType& r0, unsigned degree, ScalarType t = TypeTraits<ScalarType>::zero());
  static CnDoubletonSet create(const VectorType& x, const MatrixType& C, const VectorType& r0, const VectorType& r, unsigned degree, ScalarType t = TypeTraits<ScalarType>::zero());
  static CnDoubletonSet create(const VectorType& x, const MatrixType& C, const VectorType& r0, const MatrixType& B, const VectorType& r, unsigned degree, ScalarType t = TypeTraits<ScalarType>::zero());

  CnDoubletonSet& operator=(const VectorType&);
*/
  template<class DynSysT>
  void move(DynSysT& cndynsys) { this->move(cndynsys,*this); }

  template<class DynSysT>
  void move(DynSysT& cndynsys, CnDoubletonSet& result) const;

  using SetType::operator VectorType;
  using SetType::operator MatrixType;

private:
  JetType m_x, m_r, m_r0;
  JetType phi, rem, enc;
  CoordSystemContainer m_C, m_B, m_invB;

}; // end of class CnDoubletonSet

/// @}
}} // the end of the namespace capd::dynset

#endif // _CAPD_DYNSET_CNDOUBLETONSET_H_
