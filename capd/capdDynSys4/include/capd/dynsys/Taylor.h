/// @addtogroup dynsys
/// @{

/////////////////////////////////////////////////////////////////////////////
/// @file Taylor.h
///
/// @author Daniel Wilczak
/////////////////////////////////////////////////////////////////////////////

// Copyright (C) 2000-2012 by the CAPD Group.
//
// This file constitutes a part of the CAPD library,
// distributed under the terms of the GNU General Public License.
// Consult  http://capd.ii.uj.edu.pl/ for details.

#ifndef _CAPD_DYNSYS_TAYLOR_H_
#define _CAPD_DYNSYS_TAYLOR_H_

#include <string>
#include <stdexcept>
#include "capd/dynset/C0Set.h"
#include "capd/dynset/C1Set.h"
#include "capd/dynsys/C1DynSys.h"
#include "capd/dynsys/BasicTaylor.h"

namespace capd{
namespace dynsys{

template <
  typename MapT,
  typename StepControlT = capd::dynsys::IEncFoundStepControl,
  typename CurveT = capd::diffAlgebra::Curve< capd::diffAlgebra::BasicCurve<typename MapT::MatrixType> >
>
class Taylor: public virtual C1DynSys<typename MapT::MatrixType>, public virtual BasicTaylor<MapT,StepControlT,CurveT>
{
public:
  typedef MapT MapType;
  typedef StepControlT StepControlType;
  typedef typename MapT::FunctionType FunctionType;
  typedef typename MapType::MatrixType MatrixType;
  typedef typename MatrixType::RowVectorType VectorType;
  typedef typename MatrixType::ScalarType ScalarType;
  typedef BasicTaylor<MapT,StepControlT,CurveT> BaseTaylor;
  typedef typename MatrixType::size_type size_type;

  Taylor(MapType& vField, size_type order, const StepControlT& stepControl = StepControlT());
  void setOrder(size_type order); ///< Sets the order of the Taylor method

  // implementation of DynSys interface
  VectorType Phi(const ScalarType& t,const VectorType& iv);
  MatrixType JacPhi(const ScalarType& t,const VectorType& iv);
  VectorType enclosure(const ScalarType& t, const VectorType& x);
  VectorType Remainder(const ScalarType& t, const VectorType& iv, VectorType& o_enc);
  void encloseC0Map(
      const ScalarType& t,
      const VectorType& x,
      const VectorType& xx,
      VectorType& o_phi,
      VectorType& o_rem,
      VectorType& o_enc,
      MatrixType& o_jacPhi
  );

  // implementation of C1DynSys interface
  MatrixType jacEnclosure(const ScalarType& t, const VectorType& enc);
  MatrixType jacEnclosure(const ScalarType& t, const VectorType& enc, ScalarType* logNormOfDerivative);

  void JacRemainder(
         const ScalarType& t,
         const VectorType &vecEnclosure,
         const MatrixType &jacEnclosure,
         VectorType &Remainder,
         MatrixType &jRemainder
      ) ;

  void encloseC1Map(
      const ScalarType& t,
      const VectorType& x,
      const VectorType& xx,
      VectorType& o_phi,
      VectorType& o_rem,
      VectorType& o_enc,
      MatrixType& o_jacPhi,
      MatrixType& o_jacRem,
      MatrixType& o_jacEnc
  );

  /// it computes image of the set (in given representation) using set.move function, see capd/dynsys/Move.h for details
  template <typename SetType>
  void operator()(SetType& set){
    this->saveCurrentSet(set);
	  C1SetMove<Taylor,SetType>::move(set,*this);
  }

  // the following methods provide an interface for generic algorithms based on an abstract solver
  void computeRemainderCoefficients(const VectorType& x);
  void computeRemainderCoefficients(const VectorType& x, const MatrixType& M);
  void computeRemainderCoefficients(ScalarType t, const VectorType& x);
  void computeRemainderCoefficients(ScalarType t, const VectorType& x, const MatrixType& M);

  void computePsiCoefficients(ScalarType t, const VectorType& x, const VectorType& xx, size_type order);

  ScalarType getCoeffNorm(size_type, size_type degree) const;
  ScalarType getStep() const{
    return BaseTaylor::getStep();
  }

  const ScalarType& getCurrentTime() const  {
    return BaseTaylor::getCurrentTime();
  }

  void setCurrentTime(const ScalarType& a_time) const  {
    BaseTaylor::setCurrentTime(a_time);
  }

  const CurveT& getPsiCurve() const{
    return psiCurve;
  }

protected:
  // TimeRange is base for all types of sets and nonrigorous CxCoeff
  void saveCurrentSet(const capd::diffAlgebra::TimeRange<ScalarType>& set){
  }

  void saveCurrentSet(const capd::dynset::C1Set<MatrixType>& set){
    this->setInitMatrix((MatrixType)set);
  }

  // @override
  void computeTimeStep(const ScalarType& t, const VectorType& x){
    this->m_step = this->isStepChangeAllowed()
        ? this->getStepControl().computeNextTimeStep(*this,t,x)
        : capd::min(this->getStep(),this->getMaxStep());
  }

  void operator=(const Taylor& ){}
  Taylor(const Taylor& t) : BaseTaylor(t), psiCurve(0.,0.,1,1,1){}

  CurveT psiCurve; ///< an extra storage for Taylor coefficients used in implicit HO-method
};

// --------------- inline definitions -----------------



//###########################################################//

template<typename MapType, typename StepControlType,typename CurveType>
inline void Taylor<MapType,StepControlType,CurveType>::computeRemainderCoefficients(const VectorType& x)
{
  VectorType* coeff = this->getRemainderCoefficients();
  coeff[0] = x;
  this->m_vField->computeODECoefficients(coeff,this->getOrder()+1);
}

//###########################################################//

template<typename MapType, typename StepControlType,typename CurveType>
inline void Taylor<MapType,StepControlType,CurveType>::computeRemainderCoefficients(ScalarType t, const VectorType& x)
{
  this->setCurrentTime(t);
  this->computeRemainderCoefficients(x);
}

//###########################################################//

template<typename MapType, typename StepControlType,typename CurveType>
inline void Taylor<MapType,StepControlType,CurveType>::computeRemainderCoefficients(const VectorType& x, const MatrixType& M)
{
  VectorType* coeff = this->getRemainderCoefficients();
  MatrixType* matrixCoeff = this->getMatrixRemainderCoefficients();
  coeff[0] = x;
  matrixCoeff[0] = M;
  this->m_vField->computeODECoefficients(coeff,matrixCoeff,this->getOrder()+1);
}

//###########################################################//

template<typename MapType, typename StepControlType,typename CurveType>
inline void Taylor<MapType,StepControlType,CurveType>::computeRemainderCoefficients(ScalarType t, const VectorType& x, const MatrixType& M)
{
  this->setCurrentTime(t);
  this->computeRemainderCoefficients(x,M);
}

//###########################################################//

template<typename MapType, typename StepControlType,typename CurveType>
inline void Taylor<MapType,StepControlType,CurveType>::computePsiCoefficients(
    ScalarType t, const VectorType& x, const VectorType& xx, size_type order
  )
{
  this->setCurrentTime(t);
  VectorType* coeff = this->psiCurve.getCoefficientsAtCenter();
  coeff[0] = x;
  this->m_vField->computeODECoefficients(coeff,order);

  coeff = this->psiCurve.getCoefficients();
  MatrixType* matrixCoeff = this->psiCurve.getMatrixCoefficients();
  coeff[0] = xx;
  matrixCoeff[0].setToIdentity();
  this->m_vField->computeODECoefficients(coeff,matrixCoeff,order);
}

}} // namespace capd::dynsys

#endif // _CAPD_DYNSYS_TAYLOR_H_

/// @}
