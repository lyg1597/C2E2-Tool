/// @addtogroup dynsys
/// @{

/////////////////////////////////////////////////////////////////////////////
/// @file CnTaylor_enclosure.hpp
///
/// @author Daniel Wilczak
/////////////////////////////////////////////////////////////////////////////

// Copyright (C) 2000-2012 by the CAPD Group.
//
// This file constitutes a part of the CAPD library,
// distributed under the terms of the GNU General Public License.
// Consult  http://capd.ii.uj.edu.pl/ for details.

#ifndef _CAPD_DYNSYS_CNTAYLOR_ENCLOSURE_HPP_
#define _CAPD_DYNSYS_CNTAYLOR_ENCLOSURE_HPP_

#include <string>
#include <stdexcept>

#include "capd/dynsys/CnTaylor.h"
#include "capd/dynsys/TaylorException.h"
#include "capd/dynsys/enclosure.hpp"
#include "capd/vectalg/Norm.hpp"

namespace capd{
namespace dynsys{

//###########################################################//

template<typename MapType, typename StepControlType,typename CurveType>
typename CnTaylor<MapType, StepControlType,CurveType>::VectorType
CnTaylor<MapType, StepControlType,CurveType>::enclosure(const ScalarType& t, const VectorType &x)
///< the function finds an enclosure for \varphi([0,step],x)
{
  return capd::dynsys::enclosure(*this->m_vField,t,x, this->m_step);
}

//###########################################################//

template<typename MapType, typename StepControlType,typename CurveType>
typename CnTaylor<MapType, StepControlType,CurveType>::MatrixType
CnTaylor<MapType, StepControlType,CurveType>::jacEnclosure(const ScalarType& t, const VectorType &enc)
/**< the function finds enclosure for Jacobian matrix (variational part)
     source- "C^1-Lohner algorithm" by P. Zgliczynski */
{
  return capd::dynsys::jacEnclosure(*this->m_vField,t,this->m_step,enc,capd::vectalg::EuclLNorm<VectorType,MatrixType>());
}


//###########################################################//

template<typename MapType,typename StepControlT, typename CurveT>
void CnTaylor<MapType,StepControlT,CurveT>::c2Enclosure(const VectorType& enc, MatrixType& o_jacEnc, HessianType& o_hessianEnc)
{
  capd::dynsys::c2Enclosure(*(this->m_vField),this->m_step,enc,o_jacEnc,o_hessianEnc);
}

}} // namespace capd::dynsys

#endif // _CAPD_DYNSYS_CNTAYLOR_ENCLOSURE_HPP_

/// @}
