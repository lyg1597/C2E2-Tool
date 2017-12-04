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

#ifndef _CAPD_DYNSET_C0HOSET_HPP_
#define _CAPD_DYNSET_C0HOSET_HPP_

#include "capd/dynset/C0HOSet.h"

namespace capd{
namespace dynset{
// @addtogroup capd
/// @{

template <class BaseSetT>
C0HOSet<BaseSetT>::C0HOSet(BaseSet & set)
  : BaseSet(set), predictor(set)
{}

template <class BaseSetT>
C0HOSet<BaseSetT>::C0HOSet(const VectorType& x, ScalarType t)
  : BaseSet(x,t), predictor(x,t)
{}

template <class BaseSetT>
C0HOSet<BaseSetT>::C0HOSet(const VectorType& x, const VectorType& r0, ScalarType t)
  : BaseSet(x,r0,t), predictor(x,r0,t)
{}

template <class BaseSetT>
C0HOSet<BaseSetT>::C0HOSet(
      const VectorType& x, const MatrixType& C, const VectorType& r0, ScalarType t
    ) : BaseSet(x,C,r0,t), predictor(x,C,r0,t)
{}

template <class BaseSetT>
C0HOSet<BaseSetT>::C0HOSet(
      const VectorType& x, const MatrixType& C, const VectorType& r0, const VectorType& r, ScalarType t
    ) : BaseSet(x,C,r0,r,t), predictor(x,C,r0,r,t)
{}

template <class BaseSetT>
C0HOSet<BaseSetT>::C0HOSet(const VectorType& x, const MatrixType& C,
    const VectorType& r0, const MatrixType& B,
    const VectorType& r,
    ScalarType t
    ) : BaseSet(x,C,r0,B,r,t), predictor(x,C,r0,B,r,t)
{}


/// @}
}} // namespace capd::dynset

#endif // _CAPD_DYNSET_C0HOSET_HPP_

