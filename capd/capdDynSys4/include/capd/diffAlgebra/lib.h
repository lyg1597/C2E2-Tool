//////////////////////////////////////////////////////////////////////////////
//   Package:          CAPD

/////////////////////////////////////////////////////////////////////////////
//
/// @file map/lib.h
///
/// @author Daniel Wilczak
//
/////////////////////////////////////////////////////////////////////////////


// This file constitutes a part of the CAPD library,
// distributed under the terms of the GNU General Public License.
// Consult  http://capd.ii.uj.edu.pl/ for details.

#ifndef _CAPD_DIFFALGEBRA_LIB_H_
#define _CAPD_DIFFALGEBRA_LIB_H_

#include "capd/vectalg/lib.h"
#include "capd/diffAlgebra/C0Coeff.h"
#include "capd/diffAlgebra/C1Coeff.h"
#include "capd/diffAlgebra/C2Coeff.h"
#include "capd/diffAlgebra/CnCoeff.h"

namespace capd{

typedef capd::diffAlgebra::C0Coeff<capd::IVector> IC0Coeff;
typedef capd::diffAlgebra::C0Coeff<capd::DVector> DC0Coeff;

typedef capd::diffAlgebra::C1Coeff<capd::IMatrix> IC1Coeff;
typedef capd::diffAlgebra::C1Coeff<capd::DMatrix> DC1Coeff;

typedef capd::diffAlgebra::C2Coeff<capd::IMatrix> IC2Coeff;
typedef capd::diffAlgebra::C2Coeff<capd::DMatrix> DC2Coeff;

typedef capd::diffAlgebra::CnCoeff<capd::IMatrix, 0> ICnCoeff;
typedef capd::diffAlgebra::CnCoeff<capd::DMatrix, 0> DCnCoeff;

typedef capd::diffAlgebra::Hessian<double,0,0> DHessian;
typedef capd::diffAlgebra::Hessian<interval,0,0> IHessian;

typedef capd::diffAlgebra::Jet<capd::DMatrix,0> DJet;
typedef capd::diffAlgebra::Jet<capd::IMatrix,0> IJet;

} // end of namespace capd

#endif // _CAPD_DIFFALGEBRA_LIB_H_
