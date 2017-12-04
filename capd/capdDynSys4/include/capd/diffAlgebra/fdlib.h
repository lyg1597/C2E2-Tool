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

#include "capd/diffAlgebra/C0Coeff.h"
#include "capd/diffAlgebra/C1Coeff.h"
#include "capd/diffAlgebra/C2Coeff.h"
#include "capd/diffAlgebra/CnCoeff.h"

namespace CAPD_USER_NAMESPACE{

typedef capd::diffAlgebra::C0Coeff<CAPD_USER_NAMESPACE::IVector> IC0Coeff;
typedef capd::diffAlgebra::C0Coeff<CAPD_USER_NAMESPACE::DVector> DC0Coeff;

typedef capd::diffAlgebra::C1Coeff<CAPD_USER_NAMESPACE::IMatrix> IC1Coeff;
typedef capd::diffAlgebra::C1Coeff<CAPD_USER_NAMESPACE::DMatrix> DC1Coeff;

typedef capd::diffAlgebra::C2Coeff<CAPD_USER_NAMESPACE::IMatrix> IC2Coeff;
typedef capd::diffAlgebra::C2Coeff<CAPD_USER_NAMESPACE::DMatrix> DC2Coeff;

typedef capd::diffAlgebra::CnCoeff<CAPD_USER_NAMESPACE::IMatrix, 0> ICnCoeff;
typedef capd::diffAlgebra::CnCoeff<CAPD_USER_NAMESPACE::DMatrix, 0> DCnCoeff;

typedef capd::diffAlgebra::Hessian<CAPD_USER_NAMESPACE::DVector::ScalarType,CAPD_USER_NAMESPACE::DVector::csDim,CAPD_USER_NAMESPACE::DMatrix::ColumnVectorType::csDim> DHessian;
typedef capd::diffAlgebra::Hessian<CAPD_USER_NAMESPACE::IVector::ScalarType,CAPD_USER_NAMESPACE::IVector::csDim,CAPD_USER_NAMESPACE::IMatrix::ColumnVectorType::csDim> IHessian;

typedef capd::diffAlgebra::Jet<CAPD_USER_NAMESPACE::DMatrix,0> DJet;
typedef capd::diffAlgebra::Jet<CAPD_USER_NAMESPACE::IMatrix,0> IJet;

} // end of namespace CAPD_USER_NAMESPACE

