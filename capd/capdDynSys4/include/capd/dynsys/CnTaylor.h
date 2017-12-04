/// @addtogroup dynsys
/// @{

/////////////////////////////////////////////////////////////////////////////
/// @file CnTaylor.h
///
/// @author Daniel Wilczak
/////////////////////////////////////////////////////////////////////////////

// Copyright (C) 2000-2012 by the CAPD Group.
//
// This file constitutes a part of the CAPD library,
// distributed under the terms of the GNU General Public License.
// Consult  http://capd.ii.uj.edu.pl/ for details.

#ifndef _CAPD_DYNSYS_CNTAYLOR_H_
#define _CAPD_DYNSYS_CNTAYLOR_H_

#include <string>
#include <vector>
#include "capd/dynset/C1Set.h"
#include "capd/dynset/C2Set.h"
#include "capd/dynset/CnSet.h"
#include "capd/vectalg/Norm.h"
#include "capd/dynsys/CnDynSys.h"
#include "capd/dynsys/BasicCnTaylor.h"

namespace capd{
namespace dynsys{


template <
  typename MapT,
//  typename StepControlT = capd::dynsys::IEncFoundStepControl,
  typename StepControlT = capd::dynsys::ILastTermsStepControl,
  typename CurveT = capd::diffAlgebra::CnCurve< capd::diffAlgebra::BasicCnCurve< typename MapT::MatrixType > >
  >
class CnTaylor : public CnDynSys<typename MapT::MatrixType>, public BasicCnTaylor<MapT,StepControlT,CurveT>
{
public:
  typedef MapT MapType;
  typedef BasicCnTaylor<MapT,StepControlT,CurveT> BaseTaylor;
  typedef StepControlT StepControlType;
  typedef typename MapType::ScalarType ScalarType;
  typedef typename MapType::VectorType VectorType;
  typedef typename MapType::MatrixType MatrixType;
  typedef typename MapType::HessianType HessianType;
  typedef typename MapType::FunctionType FunctionType;
  typedef typename BaseTaylor::CnCoeffType CnCoeffType;
  typedef typename MatrixType::size_type size_type;

//  typedef typename SeriesContainerType::Multipointer Multipointer;
//  typedef typename SeriesContainerType::Multiindex Multiindex;
  typedef CurveT SolutionCurve;

  /// it computes image of the set (in given representation) using set.move function, see capd/dynsys/Move.h for details
  template <typename SetType>
  void operator()(SetType& set){
    this->saveCurrentSet(set);
    CnSetMove<CnTaylor,SetType>::move(set,*this);
  }

  CnTaylor(MapType& vectorField, size_type order, const StepControlT& stepControl=StepControlT());

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
         const VectorType& vecEnclosure,
         const MatrixType& jacEnclosure,
         VectorType& Remainder,
         MatrixType& jacRemainder
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

  // methods for C^2
  void encloseC2Map(
      const ScalarType& t,
      const VectorType& x,
      const VectorType& xx,
      VectorType& o_phi,
      VectorType& o_rem,
      VectorType& o_enc,
      MatrixType& o_jacPhi,
      MatrixType& o_jacRem,
      MatrixType& o_jacEnc,
      HessianType& o_hessianPhi,
      HessianType& o_hessianRem,
      HessianType& o_hessianEnc
  );

  void c2Enclosure(const VectorType& enc, MatrixType& jacEnc, HessianType& o_hessianEnc);
  void c2Remainder(
        const VectorType& Enc,
        const MatrixType& jacEnc,
        const HessianType& hessianEnc,
        VectorType& o_Rem,
        MatrixType& o_jacRem,
        HessianType& o_hessianRem
    );

  template<class JetT>
  VectorType encloseCnMap(
        const ScalarType& t,
        const VectorType& x,
        const VectorType& xx,
        JetT& phi,
        JetT& rem,
        JetT& enc
      );

  template<class JetT>
  VectorType cnEnclosure(const ScalarType& t, const VectorType& x, JetT& result);

  template<class JetT>
  void cnRemainder(const JetT& enc, JetT& result) ;

  ScalarType getStep() const{
    return BaseTaylor::getStep();
  }

  ScalarType getCoeffNorm(size_type i, size_type degree) const;

  // the following methods provide an interface for generic algorithms based on an abstract solver
  void computeRemainderCoefficients(const VectorType& x);
  void computeRemainderCoefficients(const VectorType& x, const MatrixType& M);
  void computeRemainderCoefficients(ScalarType t, const VectorType& x);
  void computeRemainderCoefficients(ScalarType t, const VectorType& x, const MatrixType& M);

  using BaseTaylor::getVectorField;
  using BaseTaylor::setOrder;
  using BaseTaylor::getOrder;
  using BaseTaylor::setStep;
  using BaseTaylor::dimension;
  using BaseTaylor::setCurrentTime;
  using BaseTaylor::getCurrentTime;
  using BaseTaylor::getCurve;

protected:
  // TimeRange is base for all types of sets and nonrigorous CxCoeff
  void saveCurrentSet(const capd::diffAlgebra::TimeRange<ScalarType>& set){
  }

  void saveCurrentSet(capd::dynset::C1Set<MatrixType>& set){
    this->setInitMatrix((MatrixType)set);
  }
  void saveCurrentSet(capd::dynset::C2Set<MatrixType>& set){
    this->setInitMatrix((MatrixType)set);
    this->setInitHessian((HessianType)set);
  }
  void saveCurrentSet(capd::dynset::CnSet<MatrixType>& set){
    this->setInitJet(set.currentSet());
  }

  // @override
  void computeTimeStep(const ScalarType& t, const VectorType& x){
    this->m_step = this->isStepChangeAllowed()
        ? this->getStepControl().computeNextTimeStep(*this,t,x)
        : capd::min(this->getStep(),this->getMaxStep());
  }
  void operator=(const CnTaylor&){}
  CnTaylor(const CnTaylor& t) : BaseTaylor(t){}

  using BaseTaylor::m_step;
  using BaseTaylor::m_vField;

}; // the end of class CnTaylor

// ---------------------------------------------------------------------------------------

template <typename MapT, typename StepControlT,typename CurveT>
inline void CnTaylor<MapT, StepControlT,CurveT>::computeRemainderCoefficients(const VectorType& x)
{
  for(size_type i=0;i<this->dimension();++i)
    this->remainderCoefficient(i,0) = x[i];
  this->m_vField->computeODECoefficients(this->getRemainderCoefficients(),0,this->getOrder()+1);
}

// ---------------------------------------------------------------------------------------

template <typename MapT, typename StepControlT,typename CurveT>
inline void CnTaylor<MapT, StepControlT,CurveT>::computeRemainderCoefficients(ScalarType t, const VectorType& x)
{
  this->setCurrentTime(t);
  this->computeRemainderCoefficients(x);
}

// ---------------------------------------------------------------------------------------

template <typename MapT, typename StepControlT,typename CurveT>
inline void CnTaylor<MapT, StepControlT,CurveT>::computeRemainderCoefficients(const VectorType& x, const MatrixType& M)
{
  for(size_type i=0;i<this->dimension();++i)
  {
    this->remainderCoefficient(i,0) = x[i];
    for(size_type j=0;j<this->dimension();++j)
      this->remainderCoefficient(i,j,0) = M(i+1,j+1);
  }
  this->m_vField->computeODECoefficients(this->getRemainderCoefficients(),1,this->getOrder()+1);
}

// ---------------------------------------------------------------------------------------

template <typename MapT, typename StepControlT,typename CurveT>
inline void CnTaylor<MapT, StepControlT,CurveT>::computeRemainderCoefficients(ScalarType t, const VectorType& x, const MatrixType& M)
{
  this->setCurrentTime(t);
  this->computeRemainderCoefficients(x,M);
}

}} // the end of the namespace capd::dynsys

#include "capd/dynsys/CnTaylor_templateMembers.h"

#endif // _CAPD_DYNSYS_CNTAYLOR_H_

/// @}
