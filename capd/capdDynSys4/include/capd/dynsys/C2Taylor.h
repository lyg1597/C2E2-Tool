/// @addtogroup dynsys
/// @{

/////////////////////////////////////////////////////////////////////////////
/// @file C2Taylor.h
///
/// @author Daniel Wilczak
/////////////////////////////////////////////////////////////////////////////

// Copyright (C) 2000-2012 by the CAPD Group.
//
// This file constitutes a part of the CAPD library,
// distributed under the terms of the GNU General Public License.
// Consult  http://capd.ii.uj.edu.pl/ for details.

#ifndef _CAPD_DYNSYS_C2TAYLOR_H_
#define _CAPD_DYNSYS_C2TAYLOR_H_

#include <string>
#include <vector>
#include "capd/vectalg/Norm.h"
#include "capd/dynset/C2Set.h"
#include "capd/dynsys/Taylor.h"
#include "capd/dynsys/C2DynSys.h"
#include "capd/dynsys/BasicC2Taylor.h"

namespace capd{
namespace dynsys{

template <
  typename MapT,
  //typename StepControlT = capd::dynsys::ILastTermsStepControl,
    typename StepControlT = capd::dynsys::IEncFoundStepControl,
  typename CurveT = capd::diffAlgebra::C2Curve< capd::diffAlgebra::BasicC2Curve<typename MapT::MatrixType> >
>
class C2Taylor : public Taylor<MapT,StepControlT,CurveT>,
                 public BasicC2Taylor<MapT,StepControlT,CurveT>,
                 public C2DynSys<typename MapT::MatrixType>
{
public:
  typedef MapT MapType;
  typedef StepControlT StepControlType;
  typedef typename MapT::FunctionType FunctionType;
  typedef typename MapType::MatrixType MatrixType;
  typedef typename MatrixType::RowVectorType VectorType;
  typedef typename MatrixType::ScalarType ScalarType;
  typedef typename MapType::HessianType HessianType;
  typedef BasicC2Taylor<MapT,StepControlT,CurveT> BaseC2Taylor;
  typedef Taylor<MapT,StepControlT,CurveT> BaseTaylor;
  typedef CurveT SolutionCurve;
  typedef typename MatrixType::size_type size_type;

  C2Taylor(MapType& vectorField, size_type order);

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

  using BaseC2Taylor::getVectorField;
  using BaseC2Taylor::setOrder;
  using BaseC2Taylor::getOrder;
  using BaseC2Taylor::getStep;
  using BaseC2Taylor::setStep;
  using BaseC2Taylor::dimension;

  /// it computes image of the set (in given representation) using set.move function, see capd/dynsys/Move.h for details
  template <typename SetType>
  void operator()(SetType& set){
    this->saveCurrentSet(set);
	  C2SetMove<C2Taylor,SetType>::move(set,*this);
  }

  void setCurrentTime(const ScalarType& a_time) const
  {
    BaseTaylor::setCurrentTime(a_time);
  }

  const ScalarType& getCurrentTime(const ScalarType& a_time) const
  {
    return BaseTaylor::getCurrentTime();
  }

protected:
  using BaseTaylor::saveCurrentSet;
  void saveCurrentSet(capd::dynset::C2Set<MatrixType>& set){
    this->setInitMatrix((MatrixType)set);
    this->setInitHessian((HessianType)set);
  }

  void operator=(const C2Taylor& ) {}
  C2Taylor(const C2Taylor& t) : BasicTaylor<MapT,StepControlT,CurveT>(t), BaseTaylor(t), BaseC2Taylor(t) {}

  using BaseTaylor::m_step;
  using BaseTaylor::m_vField;
}; // the end of class C2Taylor

}} // the end of the namespace capd::dynsys

#endif // _CAPD_DYNSYS_C2TAYLOR_H_

/// @}
