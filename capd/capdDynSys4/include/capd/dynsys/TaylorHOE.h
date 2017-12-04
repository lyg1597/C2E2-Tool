/// @addtogroup dynsys
/// @{

/////////////////////////////////////////////////////////////////////////////
/// @file TaylorHOE.h
///
/// @author Daniel Wilczak
/////////////////////////////////////////////////////////////////////////////

// Copyright (C) 2000-2013 by the CAPD Group.
//
// This file constitutes a part of the CAPD library,
// distributed under the terms of the GNU General Public License.
// Consult  http://capd.ii.uj.edu.pl/ for details.

/* Author: Daniel Wilczak 2010-2012 */

#ifndef _CAPD_DYNSYS_TAYLORHOE_H_
#define _CAPD_DYNSYS_TAYLORHOE_H_

#include <string>
#include <stdexcept>
#include "capd/dynset/C0Set.h"
#include "capd/dynset/C1Set.h"
#include "capd/dynsys/Taylor.h"

namespace capd{
namespace dynsys{

template <
  typename MapT,
  typename StepControlT = capd::dynsys::ILastTermsStepControl,
  typename CurveT = capd::diffAlgebra::Curve< capd::diffAlgebra::BasicCurve<typename MapT::MatrixType> >
>
class TaylorHOE: public Taylor<MapT,StepControlT,CurveT>
{
public:
  typedef MapT MapType;
  typedef StepControlT StepControlType;
  typedef typename MapT::FunctionType FunctionType;
  typedef typename MapType::MatrixType MatrixType;
  typedef typename MatrixType::RowVectorType VectorType;
  typedef typename MatrixType::ScalarType ScalarType;
  typedef typename ScalarType::BoundType BoundType;
  typedef Taylor<MapT,StepControlT,CurveT> BaseTaylor;
  typedef typename MatrixType::size_type size_type;

  TaylorHOE(MapType& vField, size_type order, const StepControlT& stepControl = StepControlT());

  /// Main method for simultaneous validation of the existence of solutions and computation of Lagrange remainder.
  VectorType enclosure(const ScalarType& t, const VectorType& x, VectorType& out_Remainder);

  // Overridden methods from DynSys
  VectorType Remainder(const ScalarType& t, const VectorType& x, VectorType& out_enc);
  VectorType enclosure(const ScalarType& t, const VectorType& x) ;
  void encloseC0Map(
      const ScalarType& t,
      const VectorType& x,
      const VectorType& xx,
      VectorType& o_phi,
      VectorType& o_rem,
      VectorType& o_enc,
      MatrixType& o_jacPhi
  );

  // Overridden methods from C1DynSys
  /// computes simultaneously an enclosure for the solutions to ODE and associated variational equations
  void c1Enclosure(const ScalarType& t, const VectorType& x, VectorType& o_enc, MatrixType& o_jacEnc);
  void encloseC1Map(
      const ScalarType& t,
      const VectorType& x0, //< usually center of x
      const VectorType& x,
      VectorType& o_phi,
      VectorType& o_rem,
      VectorType& o_enc,
      MatrixType& o_jacPhi,
      MatrixType& o_jacRem,
      MatrixType& o_jacEnc
  );

  /// it computes image of the set (in given representation) using set.move function
  template <typename SetType>
  void operator()(SetType& set){
    this->saveCurrentSet(set);
	  C1SetMove<TaylorHOE,SetType>::move(set,*this);
  }

protected:
  void operator=(const TaylorHOE& ){}
  TaylorHOE(const TaylorHOE& t) : BasicTaylor<MapT,StepControlT>(t), BaseTaylor(t){}
};

}} // namespace capd::dynsys

#endif // _CAPD_DYNSYS_TAYLORHOE_H_

/// @}
