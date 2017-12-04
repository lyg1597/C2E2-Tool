/// @addtogroup dynsys
/// @{

/////////////////////////////////////////////////////////////////////////////
/// @file C2Taylor.hpp
///
/// @author Daniel Wilczak
/////////////////////////////////////////////////////////////////////////////

// Copyright (C) 2000-2005 by the CAPD Group.
//
// This file constitutes a part of the CAPD library,
// distributed under the terms of the GNU General Public License.
// Consult  http://capd.ii.uj.edu.pl/ for details.

#ifndef _CAPD_DYNSYS_C2TAYLOR_HPP_
#define _CAPD_DYNSYS_C2TAYLOR_HPP_

#include <string>
#include <stdexcept>

#include "capd/dynsys/C2Taylor.h"
#include "capd/dynsys/Taylor.hpp"
#include "capd/dynsys/BasicC2Taylor.hpp"
#include "capd/vectalg/Norm.hpp"

namespace capd{
namespace dynsys{

//###########################################################//

template<typename MapType,typename StepControlT, typename CurveT>
C2Taylor<MapType,StepControlT,CurveT>::C2Taylor(MapType& vectorField,size_type order)
  : BasicTaylor<MapType,StepControlT,CurveT>(vectorField,order),
    Taylor<MapType,StepControlT,CurveT>(vectorField,order),
    BasicC2Taylor<MapType,StepControlT,CurveT>(vectorField,order)
{}

//###########################################################//

template<typename MapType,typename StepControlT, typename CurveT>
void C2Taylor<MapType,StepControlT,CurveT>::c2Enclosure(const VectorType& enc, MatrixType& o_jacEnc, HessianType& o_hessianEnc)
{
  capd::dynsys::c2Enclosure(*(this->m_vField),this->m_step,enc,o_jacEnc,o_hessianEnc);
}

// ####################################################################

template<typename MapType,typename StepControlT, typename CurveT>
void C2Taylor<MapType,StepControlT,CurveT>::c2Remainder(
      const VectorType& Enc,
      const MatrixType& jacEnc,
      const HessianType& hessianEnc,
      VectorType& o_Rem,
      MatrixType& o_jacRem,
      HessianType& o_hessianRem
  )
{
  VectorType* remCoeff = this->getRemainderCoefficients();
  MatrixType* matrixRemCoeff = this->getMatrixRemainderCoefficients();
  HessianType* hessianRemCoeff = this->getHessianRemainderCoefficients();
  remCoeff[0] = Enc;
  matrixRemCoeff[0] = jacEnc;
  hessianRemCoeff[0] = hessianEnc;
  this->m_vField->computeODECoefficients(remCoeff,matrixRemCoeff,hessianRemCoeff,this->getOrder()+1);
  ScalarType fac = power(this->m_step,this->getOrder()+1);
  o_Rem = remCoeff[this->getOrder()+1] * fac;
  o_jacRem = matrixRemCoeff[this->getOrder()+1] * fac;

  typename HessianType::iterator i1=o_hessianRem.begin(), i2=o_hessianRem.end(),
                                 j=hessianRemCoeff[this->getOrder()+1].begin();
  while(i1!=i2)
  {
    *i1 = *j * fac;
    ++i1;
    ++j;
  }
}

// ####################################################################

template<typename MapType,typename StepControlT, typename CurveT>
void C2Taylor<MapType,StepControlT,CurveT>::encloseC2Map(
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
  )
{
  VectorType* center = this->getCoefficientsAtCenter();
  VectorType* coeff = this->getCoefficients();
  MatrixType* matrixCoeff = this->getMatrixCoefficients();
  HessianType* hessianCoeff = this->getHessianCoefficients();

  int order=this->getOrder();
  coeff[0] = xx;
  matrixCoeff[0].setToIdentity();
  hessianCoeff[0].clear();
  this->m_vField->computeODECoefficients(coeff,matrixCoeff,hessianCoeff,order);
  this->computeCoefficientsAtCenter(x,order);
  this->computeTimeStep(t,xx);

  o_enc = this->enclosure(t,xx);
  this->c2Enclosure(o_enc,o_jacEnc,o_hessianEnc);
  this->c2Remainder(o_enc,o_jacEnc,o_hessianEnc,o_rem,o_jacRem,o_hessianRem);

  // summation of the Taylor series
  o_phi = center[order];
  o_jacPhi = matrixCoeff[order];
  o_hessianPhi = hessianCoeff[order];
  for(int r = order - 1; r >= 0; --r)
  {
    capd::vectalg::multiplyAssignObjectScalarAddObject(o_phi,this->m_step,center[r]);
    capd::vectalg::multiplyAssignObjectScalarAddObject(o_jacPhi,this->m_step,matrixCoeff[r]);
    capd::vectalg::multiplyAssignObjectScalarAddObject(o_hessianPhi,this->m_step,hessianCoeff[r]);
  }
}

}} // namespace capd::dynsys

#endif // _CAPD_DYNSYS_C2TAYLOR_HPP_

/// @}
