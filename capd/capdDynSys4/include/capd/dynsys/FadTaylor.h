/// @addtogroup dynsys
/// @{

/////////////////////////////////////////////////////////////////////////////
/// @file FadTaylor.h
///
/// @author Daniel Wilczak
/////////////////////////////////////////////////////////////////////////////

// Copyright (C) 2008 by the CAPD Group.
//
// Distributed under the terms of the GNU General Public License.
// Consult  http://capd.ii.uj.edu.pl/ for details.

#ifndef _CAPD_DYNSYS_FADTAYLOR_H_
#define _CAPD_DYNSYS_FADTAYLOR_H_

#include "capd/vectalg/Norm.h"
#include "capd/dynsys/C1DynSys.h"
#include "capd/dynset/C0Set.h"
#include "capd/dynset/C1Set.h"
#include "capd/dynsys/BasicFadTaylor.h"

namespace capd{
namespace dynsys{

template<class FadMapT, typename StepControlT = capd::dynsys::IEncFoundStepControl >
class FadTaylor : public capd::dynsys::C1DynSys<typename FadMapT::MatrixType>, public BasicFadTaylor<FadMapT,StepControlT>{
public:
  typedef FadMapT MapType;
  typedef BasicFadTaylor<FadMapT,StepControlT> BaseTaylor;
  typedef StepControlT StepControlType;
  typedef typename BaseTaylor::ScalarType ScalarType;
  typedef typename BaseTaylor::FScalar FScalar;
  typedef typename BaseTaylor::TFScalar TFScalar;
  typedef typename BaseTaylor::MatrixType MatrixType;
  typedef typename BaseTaylor::VectorType VectorType;
  typedef typename BaseTaylor::FVector FVector;
  typedef typename BaseTaylor::TFVector TFVector;
  typedef typename BaseTaylor::FunctionType FunctionType;
  typedef typename MatrixType::size_type size_type;

  typedef capd::vectalg::Norm<VectorType,MatrixType> NormType;

  FadTaylor(MapType& f, size_type _order, const StepControlT& _stepControl=StepControlT());

  // methods for C^0 computations
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

  // methods for C^1 computations
  MatrixType jacEnclosure(const ScalarType& t, const VectorType& enc);
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

  /// it computes image of the set (in given representation) using set.move function, see DynSys.h for details
  template <typename SetType>
  void operator()(SetType& set){
    this->saveCurrentSet(set);
	  C1SetMove<FadTaylor,SetType>::move(set,*this);
  }

  ScalarType getCoeffNorm(size_type i, size_type degree) const;
  ScalarType getStep() const{
    return BaseTaylor::getStep();
  }

  // the following methods provide an interface for generic algorithms based on an abstract solver
  void computeRemainderCoefficients(const VectorType& x);
  void computeRemainderCoefficients(const VectorType& x, const MatrixType& M);
  void computeRemainderCoefficients(ScalarType t, const VectorType& x);
  void computeRemainderCoefficients(ScalarType t, const VectorType& x, const MatrixType& M);

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

}; // the end of class FadTaylor

//###########################################################//

template<class FadMapT, typename StepControlT>
inline void FadTaylor<FadMapT,StepControlT>::computeRemainderCoefficients(const VectorType& x)
{
  this->setInitialCondition(x,this->m_rem);
  this->computeCoeff(this->m_rem,this->m_remOut,this->getOrder()+1);
}

//###########################################################//

template<class FadMapT, typename StepControlT>
inline void FadTaylor<FadMapT,StepControlT>::computeRemainderCoefficients(ScalarType t, const VectorType& x)
{
  this->setCurrentTime(t);
  this->computeRemainderCoefficients(x);
}

//###########################################################//

template<class FadMapT, typename StepControlT>
inline void FadTaylor<FadMapT,StepControlT>::computeRemainderCoefficients(const VectorType& x, const MatrixType& M)
{
  this->setInitialCondition(x,M,this->m_jacRem);
  this->computeCoeff(this->m_jacRem,this->m_jacRemOut,this->getOrder()+1);

  for(size_type i=0;i<this->dimension();++i)
    for(size_type j=0;j<=this->getOrder()+1;++j)
      this->remainderCoefficient(i,j) = this->m_jacRem[i][j].x();
}

//###########################################################//

template<class FadMapT, typename StepControlT>
inline void FadTaylor<FadMapT,StepControlT>::computeRemainderCoefficients(ScalarType t, const VectorType& x, const MatrixType& M)
{
  this->setCurrentTime(t);
  this->computeRemainderCoefficients(x,M);
}

}} // the end of the namespace capd::dynsys

#endif // _CAPD_DYNSYS_FADTAYLOR_H_

/// @}
