/// @addtogroup dynsys
/// @{

/////////////////////////////////////////////////////////////////////////////
/// @file TaylorHOE.hpp
///
/// @author Daniel Wilczak
/////////////////////////////////////////////////////////////////////////////

// Copyright (C) 2000-2013 by the CAPD Group.
//
// This file constitutes a part of the CAPD library,
// distributed under the terms of the GNU General Public License.
// Consult  http://capd.ii.uj.edu.pl/ for details.

/* Author: Daniel Wilczak 2010-2013 */

#ifndef _CAPD_DYNSYS_TAYLORHOE_HPP_
#define _CAPD_DYNSYS_TAYLORHOE_HPP_

#include <sstream>
#include <string>
#include <stdexcept>

#include "capd/dynsys/highOrderEnclosure.h"
#include "capd/dynsys/Taylor.hpp"
#include "capd/dynsys/TaylorHOE.h"
#include "capd/vectalg/algebraicOperations.hpp"

namespace capd{
namespace dynsys{

//###########################################################//

template<typename MapType, typename StepControlType,typename CurveType>
TaylorHOE<MapType,StepControlType,CurveType>::TaylorHOE(MapType& vectorField, size_type order, const StepControlType& stepControl)
  : BasicTaylor<MapType,StepControlType,CurveType>(vectorField,order,stepControl),
    BaseTaylor(vectorField,order,stepControl)
{}

//###########################################################//

template<typename MapType, typename StepControlType,typename CurveType>
typename TaylorHOE<MapType,StepControlType,CurveType>::VectorType
TaylorHOE<MapType,StepControlType,CurveType>::enclosure(const ScalarType& t, const VectorType& x, VectorType& out_Remainder)
///<the function finds an enclosure for \varphi([0,step],x)
{
  bool stepChangeAllowance = this->isStepChangeAllowed();
  this->turnOffStepControl();
  this->setCurrentTime(t);
  this->computeCoefficients(x,this->getOrder());
  VectorType result = capd::dynsys::highOrderEnclosure(t,*this,out_Remainder);
  this->onOffStepControl(stepChangeAllowance);
  return result;
}


//###########################################################//

template<typename MapType, typename StepControlType,typename CurveType>
void TaylorHOE<MapType,StepControlType,CurveType>::c1Enclosure(const ScalarType& t, const VectorType& x, VectorType& o_enc, MatrixType& o_jacEnc)
///<the function finds an enclosure for \varphi([0,step],x)
{
  bool stepChangeAllowance = this->isStepChangeAllowed();
  this->turnOffStepControl();
  this->setCurrentTime(t);
  this->computeCoefficients(x,this->getOrder());
  capd::diffAlgebra::C1Coeff<MatrixType> rem(this->dimension());
  capd::diffAlgebra::C1Coeff<MatrixType> enc = capd::dynsys::highOrderEnclosure(t,*this,rem);
  this->onOffStepControl(stepChangeAllowance);
  o_enc = enc.refValue();
  o_jacEnc = enc.refDerivative();
}

//###########################################################//

template<typename MapType, typename StepControlType,typename CurveType>
typename TaylorHOE<MapType,StepControlType,CurveType>::VectorType
TaylorHOE<MapType,StepControlType,CurveType>::enclosure(const ScalarType& t, const VectorType& x)
///<the function finds an enclosure for \varphi([0,step],x)
{
  VectorType remainder(x.dimension());
  return this->enclosure(t,x,remainder);
}

//###########################################################//

template<typename MapType, typename StepControlType,typename CurveType>
typename TaylorHOE<MapType,StepControlType,CurveType>::VectorType
TaylorHOE<MapType,StepControlType,CurveType>::Remainder(const ScalarType& t, const VectorType &x, VectorType& out_enc)
///<the function finds an enclosure for \varphi([0,step],x)
{
  VectorType remainder(x.dimension());
  out_enc = this->enclosure(t,x,remainder);
  return remainder;
}

//###########################################################//

template<typename MapType, typename StepControlType,typename CurveType>
void TaylorHOE<MapType,StepControlType,CurveType>::encloseC0Map(
      const ScalarType& t,
      const VectorType& x,
      const VectorType& xx,
      VectorType& o_phi,
      VectorType& o_rem,
      VectorType& o_enc,
      MatrixType& o_jacPhi
  )
{
  int order = this->getOrder();
  MatrixType* matrixCoeff = this->getMatrixCoefficients();
  VectorType* center = this->getCoefficientsAtCenter();

  // here we compute all the coefficients for phi(t) and DPhi(t)
  this->setCurrentTime(t);
  this->computeCoefficientsAtCenter(x,order);
  this->computeCoefficients(xx,order);
  this->computeTimeStep(t,xx);

  // in the following function the time step can be adjusted
  o_enc = capd::dynsys::highOrderEnclosure(t,*this,o_rem);

  o_phi = center[order];
  o_jacPhi = matrixCoeff[order];
  for(int i=order-1;i>=0;--i)
  {
    capd::vectalg::multiplyAssignObjectScalarAddObject(o_jacPhi,this->getStep(),matrixCoeff[i]);
    capd::vectalg::multiplyAssignObjectScalarAddObject(o_phi,this->getStep(),center[i]);
  }

}
//###########################################################//


template<typename MapType, typename StepControlType,typename CurveType>
void TaylorHOE<MapType,StepControlType,CurveType>::encloseC1Map(
      const ScalarType& t,
      const VectorType& x,
      const VectorType& xx,
      VectorType& o_phi,
      VectorType& o_rem,
      VectorType& o_enc,
      MatrixType& o_jacPhi,
      MatrixType& o_jacRem,
      MatrixType& o_jacEnc
  )
{
  int order = this->getOrder();
  VectorType* center = this->getCoefficientsAtCenter();
  MatrixType* matrixCoeff = this->getMatrixCoefficients();

  // here we compute all the coefficients for phi(t) and DPhi(t)
  this->setCurrentTime(t);
  this->computeCoefficientsAtCenter(x,order);
  this->computeCoefficients(xx,order);
  this->computeTimeStep(t,xx);

  // in the following function the time step can be adjusted
  capd::diffAlgebra::C1Coeff<MatrixType> rem(this->dimension());
  capd::diffAlgebra::C1Coeff<MatrixType> enc = capd::dynsys::highOrderEnclosure(t,*this,rem);

  o_phi = center[order];
  o_jacPhi = matrixCoeff[order];
  for(int i=order-1;i>=0;--i)
  {
    capd::vectalg::multiplyAssignObjectScalarAddObject(o_jacPhi,this->getStep(),matrixCoeff[i]);
    capd::vectalg::multiplyAssignObjectScalarAddObject(o_phi,this->getStep(),center[i]);
  }

  o_rem = rem.refValue();
  o_jacRem = rem.refDerivative();
  o_enc = enc.refValue();
  o_jacEnc = enc.refDerivative();
}

}} //namespace capd::dynsys

#endif // _CAPD_DYNSYS_TAYLORHOE_HPP_

/// @}
