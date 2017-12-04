/////////////////////////////////////////////////////////////////////////////
///
/// @file C0HOSet.h
///
/// @author Daniel Wilczak
/// Created on: Dec 28, 2011
/////////////////////////////////////////////////////////////////////////////

// Copyright (C) 2000-2011 by the CAPD Group.
//
// This file constitutes a part of the CAPD library,
// distributed under the terms of the GNU General Public License.
// Consult  http://capd.ii.uj.edu.pl/ for details.

#ifndef _CAPD_DYNSET_C0HOSET_H_
#define _CAPD_DYNSET_C0HOSET_H_

#include "capd/basicalg/factrial.h"

namespace capd{
namespace dynset{
// @addtogroup capd
/// @{

/**
 * This class uses representation of subset of R^n inherited from template parameter.
 *
 * The evaluation of the set by an ODE is realized by intersection of two methods:
 * the Taylor method and the Hermite-Obreshkov method.
 *
 * IMPORTANT: present implementation is valid only for orders of the Taylor method less or equal 32.
 * This is due to capacity of integer type used to store binomial coefficients.
 */
template <class BaseSetT>
class C0HOSet : public BaseSetT{

public :
  typedef BaseSetT BaseSet;
  typedef typename BaseSet::MatrixType MatrixType;
  typedef typename BaseSet::VectorType VectorType;
  typedef typename MatrixType::ScalarType ScalarType;
  typedef typename MatrixType::size_type size_type;
  typedef typename BaseSet::DynSysType DynSysType;

  C0HOSet(BaseSet & set);
  explicit C0HOSet(const VectorType& x, ScalarType t = TypeTraits<ScalarType>::zero());
  C0HOSet(const VectorType& x, const VectorType& r0, ScalarType t = TypeTraits<ScalarType>::zero());
  C0HOSet(const VectorType& x, const MatrixType& C, const VectorType& r0, ScalarType t = TypeTraits<ScalarType>::zero());
  C0HOSet(const VectorType& x, const MatrixType& C, const VectorType& r0, const VectorType& r, ScalarType t = TypeTraits<ScalarType>::zero());
  C0HOSet(const VectorType& x, const MatrixType& C,
      const VectorType& r0, const MatrixType& B,
      const VectorType& r,
      ScalarType t = TypeTraits<ScalarType>::zero()
    );

  template<class Solver>
  void move(Solver& solver){ this->move(solver,*this);  }

  template<class Solver>
  void move(Solver& solver, C0HOSet& result);

  std::string name() const { return "C0HOSet"; }
  BaseSet predictor;
};

// -------------------------------------------


template <class BaseSetT>
template<class Solver>
void C0HOSet<BaseSetT>::move(Solver& solver, C0HOSet& result)
{
  size_type dimension = this->m_x.dimension();
  // predictor step - Taylor method
  VectorType y(dimension), deltaY(dimension), rem(dimension), enc(dimension);
  MatrixType jacPhi(dimension, dimension);

  // the following function can throw an exception leaving output parameters in an inconsistent state
  // do not overwrite parameters of the set until we are sure that they are computed correctly

  VectorType deltaX = this->m_currentSet - this->m_x;
  solver.encloseC0Map(this->getCurrentTime(),this->m_x, this->m_currentSet, y, rem, enc, jacPhi);
  this->predictor.move(y,deltaX,jacPhi,rem,result.predictor);

  // corrector step
  VectorType psiPlus(dimension), psiMinus(dimension);
  MatrixType JPlus(dimension,dimension);
  MatrixType JMinus(dimension,dimension);
  MatrixType Id = MatrixType::Identity(dimension);

  size_type order = solver.getOrder();
  size_type q = order/2;
  size_type p = order - q;
  capd::dynset::computePsi(solver.getCurve(),p,q,solver.getStep(),psiPlus,JPlus);

  // we have to recompute coefficients at the image
  split((VectorType)(result.predictor),y,deltaY);
  ScalarType nextTime =  this->getCurrentTime() + solver.getStep();
  solver.computePsiCoefficients(nextTime,y,(VectorType)(result.predictor),q);
  capd::dynset::computePsi(solver.getPsiCurve(),q,p,-solver.getStep(),psiMinus,JMinus);

  // solve implicit equation
  int sign = q%2 ? -1 : 1;
  for(size_type i=0;i<dimension;++i)
    rem[i] = psiPlus[i] - psiMinus[i] + sign*rem[i]/newton(p+q,q);

  MatrixType midJMinusInverse = midMatrix(capd::matrixAlgorithms::gaussInverseMatrix(midMatrix(JMinus)));
  jacPhi = midJMinusInverse*JPlus;

  rem = (Id - midJMinusInverse*JMinus)*deltaY + midJMinusInverse*rem;
  this->BaseSet::move(y,deltaY,jacPhi,rem,(BaseSet&)result);

  if(subsetInterior(VectorType(result.predictor),result.m_currentSet))
    result.BaseSet::operator=(result.predictor);
  else if(subsetInterior(result.m_currentSet,VectorType(result.predictor))){
    typedef typename BaseSet::BaseSet BBaseSet;
    result.predictor.BBaseSet::operator=(result);
  }

  this->reorganizeIfNeeded((BaseSet&)result);
  result.setCurrentSet(intersection((VectorType)result.predictor,(VectorType)result));
  result.predictor.setCurrentSet(result.m_currentSet);
  result.setCurrentTime(nextTime);
  result.setLastEnclosure(enc);
}

/// @}
}} // namespace capd::dynset

#endif // _CAPD_DYNSET_C0HOSET_H_

