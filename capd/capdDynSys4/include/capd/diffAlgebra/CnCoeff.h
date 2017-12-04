/// @addtogroup diffAlgebra
/// @{

/////////////////////////////////////////////////////////////////////////////
/// @file CnCoeff.h
///
/// @author Daniel Wilczak
/////////////////////////////////////////////////////////////////////////////

// Copyright (C) 2000-2012 by the CAPD Group.
//
// This file constitutes a part of the CAPD library, 
// distributed under the terms of the GNU General Public License.
// Consult  http://capd.ii.uj.edu.pl/ for details.

#include <vector>
#include "capd/diffAlgebra/TimeRange.h"
#include "capd/diffAlgebra/Jet.h"
#include "capd/diffAlgebra/CoeffTraits.h"

#ifndef _CAPD_DIFFALGEBRA_CNCOEFF_H_
#define _CAPD_DIFFALGEBRA_CNCOEFF_H_

namespace capd{
namespace diffAlgebra{

/**
 * The class represent a jet of solution to a nonautomnomous ODE.
 * This is a truncated power series of degree D of a function
 * \f t->R^n \f
 * at given time
 *
 * The base class Jet is used to store coefficients of the jet.
 * The base class TimeRange is used to store current time.
*/

template<typename MatrixT, __size_type DEGREE>
class CnCoeff : public capd::diffAlgebra::TimeRange<typename MatrixT::ScalarType>,
                public Jet<MatrixT,DEGREE>
{
public:
  typedef CnCoeff SetType;
  typedef MatrixT MatrixType;
  typedef Jet<MatrixT,DEGREE> JetType;

  typedef typename MatrixType::ScalarType ScalarType;
  typedef typename MatrixType::RefColumnVectorType RefVectorType;
  typedef typename MatrixType::RowVectorType VectorType;
  typedef typename MatrixType::ColumnVectorType ImageVectorType;
  typedef typename VectorType::size_type size_type;

  typedef typename JetType::iterator iterator;
  typedef typename JetType::const_iterator const_iterator;
  typedef typename JetType::Multipointer Multipointer;
  typedef typename JetType::Multiindex Multiindex;
   
  CnCoeff();
  CnCoeff(size_type n, size_type d);
  CnCoeff(size_type m, size_type n, size_type d);
   
  using JetType::operator();
  using JetType::clear;
  using JetType::setMatrix;
  using JetType::toString;
  using JetType::save;
  using JetType::load;
  using JetType::begin;
  using JetType::end;
  using JetType::dimension;
  using JetType::imageDimension;
  using JetType::degree;
  using JetType::resize;

  // this operator returns a value of function, i.e. 0-order derivatives
  operator ImageVectorType() const;
// this operator returns a value of function, i.e. 0-order derivatives
  RefVectorType operator()(void) const;
// this operator returns first order derivatives as a matrix
  operator MatrixType() const;

  template<class DynSysT>
  void move(DynSysT& dynsys){
	   dynsys(*this);
  }
   
protected:
  using JetType::m_N;
  using JetType::m_M;
}; // the end of class CnCoeff

// --------------------- inline definitions ------------------------

template<typename MatrixT, __size_type DEGREE>
inline CnCoeff<MatrixT,DEGREE>::CnCoeff()
    : capd::diffAlgebra::TimeRange<ScalarType>(TypeTraits<ScalarType>::zero() ),
      JetType(1,1,1)
{}

// -----------------------------------------------------------------

template<typename MatrixT, __size_type DEGREE>
inline CnCoeff<MatrixT,DEGREE>::CnCoeff(size_type dim, size_type degree)
  : capd::diffAlgebra::TimeRange<ScalarType>(TypeTraits<ScalarType>::zero() ),
    JetType(dim, dim, degree)
{}

// -----------------------------------------------------------------

template<typename MatrixT, __size_type DEGREE>
inline CnCoeff<MatrixT,DEGREE>::CnCoeff(size_type m, size_type n, size_type degree)
  : capd::diffAlgebra::TimeRange<ScalarType>(TypeTraits<ScalarType>::zero() ),
    JetType(m, n, degree)
{}

// -----------------------------------------------------------------

template<typename MatrixT, __size_type DEGREE>
inline typename CnCoeff<MatrixT,DEGREE>::RefVectorType
   CnCoeff<MatrixT,DEGREE>::operator()(void) const
{
  return RefVectorType(begin(),newton(this->dimension()+this->degree(),this->dimension()),this->imageDimension());
}

template<typename MatrixT, __size_type DEGREE>
struct CoeffTraits<CnCoeff<MatrixT,DEGREE> >{
	const static bool isC0Jet=false;
	const static bool isC1Jet=false;
	const static bool isC2Jet=false;
	const static bool isCnJet=true;
};
}} // namespace capd::diffAlgebra

#endif // _CAPD_DIFFALGEBRA_CNCOEFF_H_

/// @}
