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

#ifndef _CAPD_DYNSYS_FADTAYLORHOE_H_
#define _CAPD_DYNSYS_FADTAYLORHOE_H_

#include "capd/dynsys/FadTaylor.h"

namespace capd{
namespace dynsys{

template<class FadMapT, typename StepControlT = capd::dynsys::ILastTermsStepControl >
class FadTaylorHOE : public FadTaylor<FadMapT,StepControlT>
{
public:
  typedef FadMapT MapType;
  typedef FadTaylor<FadMapT,StepControlT> BaseTaylor;
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

  FadTaylorHOE(MapType& f, size_type _order, const StepControlT& _stepControl=StepControlT());

  /// Main method for simultaneous validation of the existence of solutions and computation of Lagrange remainder.
  VectorType enclosure(const ScalarType& t, const VectorType& x, VectorType& out_Remainder);

  // Overridden methods from DynSys
  VectorType Remainder(const ScalarType& t, const VectorType &iv, VectorType &out_enc);
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
	  C1SetMove<FadTaylorHOE,SetType>::move(set,*this);
  }

}; // the end of class FadTaylorHOE

}} // the end of the namespace capd::dynsys

#endif // _CAPD_DYNSYS_FADTAYLORHOE_H_

/// @}
