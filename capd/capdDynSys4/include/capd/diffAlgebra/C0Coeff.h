//////////////////////////////////////////////////////////////////////////////
//   Package:          CAPD

/////////////////////////////////////////////////////////////////////////////
//
/// @file C0Coeff.h
///
/// @author Tomasz Kapela   @date 2010-04-20
//
/////////////////////////////////////////////////////////////////////////////

// Copyright (C) Tomasz Kapela 2010
//
// This file constitutes a part of the CAPD library,
// distributed under the terms of the GNU General Public License.
// Consult  http://capd.ii.uj.edu.pl/ for details.

#ifndef _CAPD_DIFFALGEBRA_C0COEFF_H_
#define _CAPD_DIFFALGEBRA_C0COEFF_H_

#include "capd/diffAlgebra/TimeRange.h"
#include "capd/diffAlgebra/CoeffTraits.h"

namespace capd{
namespace diffAlgebra{

template <typename VectorT>
class C0Coeff : public capd::diffAlgebra::TimeRange<typename VectorT::ScalarType>{
public:
  typedef C0Coeff SetType;
  typedef VectorT VectorType;
  typedef typename VectorType::ScalarType ScalarType;
  typedef typename VectorType::size_type size_type;
  typedef typename VectorType::iterator iterator;
  typedef typename VectorType::const_iterator const_iterator;

  C0Coeff() : capd::diffAlgebra::TimeRange<ScalarType>(TypeTraits<ScalarType>::zero() )
  {}

  C0Coeff(size_type dim)
    : capd::diffAlgebra::TimeRange<ScalarType>(TypeTraits<ScalarType>::zero() ),
     m_value(dim)
  {}

  C0Coeff(const VectorType & v)
    : capd::diffAlgebra::TimeRange<ScalarType>(TypeTraits<ScalarType>::zero() ),
     m_value(v)
  {}

  /// this operator returns a value of function, i.e. 0-order derivatives
   operator const VectorType & () const{
     return m_value;
   }
   /// this operator returns a value of function, i.e. 0-order derivatives
   const VectorType & operator()(void) const{
     return m_value;
   }
   C0Coeff& operator=(const VectorType & v){
     m_value = v;
     return *this;
   }
   void clear(){
     m_value.clear();
   }
   VectorType& refValue(){
     return m_value;
   }
   friend std::ostream & operator<< (std::ostream & s, const C0Coeff & coeff){
     s << coeff.getCurrenttime() << "\n" << coeff.m_value;
     return s;
   }
   size_type dimension() const{
	   return m_value.dimension();
   }

   template<class DynSysT>
   void move(DynSysT& dynsys){
	   m_value = dynsys(this->m_currentTime,m_value);
   }

   static size_type degree(){
	   return 0;
   }
protected:
   VectorType m_value;
};

template<class VectorT>
struct CoeffTraits<C0Coeff<VectorT> >{
	const static bool isC0Jet=true;
	const static bool isC1Jet=false;
	const static bool isC2Jet=false;
	const static bool isCnJet=false;
};

}} // end of namespace capd::diffAlgebra

#endif // _CAPD_DIFFALGEBRA_C0COEFF_H_
