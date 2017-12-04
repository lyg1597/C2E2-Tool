//////////////////////////////////////////////////////////////////////////////
//   Package:          CAPD

/////////////////////////////////////////////////////////////////////////////
//
/// @file C1Coeff.h
///
/// @author Tomasz Kapela   @date 2010-04-20
//
/////////////////////////////////////////////////////////////////////////////

// Copyright (C) Tomasz Kapela 2010
//
// This file constitutes a part of the CAPD library,
// distributed under the terms of the GNU General Public License.
// Consult  http://capd.ii.uj.edu.pl/ for details.

#ifndef _CAPD_DIFFALGEBRA_C1COEFF_H_
#define _CAPD_DIFFALGEBRA_C1COEFF_H_

#include "capd/diffAlgebra/C0Coeff.h"

namespace capd{
namespace diffAlgebra{

template <typename MatrixT>
class C1Coeff : public capd::diffAlgebra::C0Coeff<typename MatrixT::RowVectorType>{
public:
  typedef C1Coeff SetType;
  typedef capd::diffAlgebra::C0Coeff<typename MatrixT::RowVectorType> BaseCoeff;
  typedef MatrixT MatrixType;

  typedef typename MatrixType::ScalarType ScalarType;
  typedef typename MatrixType::RefColumnVectorType RefVectorType;
  typedef typename MatrixType::RowVectorType VectorType;

  typedef typename VectorType::size_type size_type;
  typedef typename VectorType::iterator iterator;
  typedef typename VectorType::const_iterator const_iterator;

  C1Coeff(){
  }
  C1Coeff(size_type dim): BaseCoeff(dim), m_derivative(dim,dim){
  }
  C1Coeff(const VectorType & v, const MatrixType & der): BaseCoeff(v), m_derivative(der){
  }

  C1Coeff& operator=(const VectorType & v){
    BaseCoeff::operator=(v);
    return *this;
  }
  /// this operator returns first order derivatives as a matrix
  operator const MatrixType & () const{
    return m_derivative;
  }
  void clear(){
    BaseCoeff::clear();
    m_derivative.clear();
  }
  void setMatrix(const MatrixType& der){
    m_derivative = der;
  }
  void setDerivative(const MatrixType & der){
    m_derivative = der;
  }
  const MatrixType & getDerivative()const{
    return m_derivative;
  }
  MatrixType & refDerivative(){
    return m_derivative;
  }
   friend std::ostream & operator<< (std::ostream & s, const C1Coeff & coeff){
     s << static_cast<const BaseCoeff&>(coeff);
     s << "\n" << coeff.m_derivative;
     return s;
   }

   template<class DynSysT>
   void move(DynSysT& dynsys){
	   this->m_value = dynsys(this->m_currentTime,this->m_value,m_derivative,m_derivative);
   }

   static size_type degree(){
   	   return 1;
   }
protected:
   MatrixType m_derivative;
};

template<class MatrixT>
struct CoeffTraits<C1Coeff<MatrixT> >{
	const static bool isC0Jet=false;
	const static bool isC1Jet=true;
	const static bool isC2Jet=false;
	const static bool isCnJet=false;
};

}} // end of namespace capd::diffAlgebra

#endif // _CAPD_DIFFALGEBRA_C1COEFF_H_
