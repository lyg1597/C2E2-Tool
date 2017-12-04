//////////////////////////////////////////////////////////////////////////////
//   Package:          CAPD

/////////////////////////////////////////////////////////////////////////////
//
/// @file dynsys/typedefs.h
///
/// @author Daniel Wilczak   @date 2013-01-09
//
/////////////////////////////////////////////////////////////////////////////

// This file constitutes a part of the CAPD library,
// distributed under the terms of the GNU General Public License.
// Consult  http://capd.ii.uj.edu.pl/ for details.

namespace CAPD_USER_NAMESPACE{

typedef capd::dynsys::DynSys<CAPD_USER_NAMESPACE::IMatrix> IDynSys;
typedef capd::dynsys::TaylorHOE<CAPD_USER_NAMESPACE::IMap> ITaylor;
typedef capd::dynsys::C2Taylor<CAPD_USER_NAMESPACE::IMap> IC2Taylor;
typedef capd::dynsys::CnTaylor<CAPD_USER_NAMESPACE::IMap> ICnTaylor;
typedef capd::dynsys::TaylorException<CAPD_USER_NAMESPACE::IVector> ITaylorException;

// classes for nonrigorous computations

typedef capd::dynsys::BasicTaylor<CAPD_USER_NAMESPACE::DMap> DTaylor;
typedef capd::dynsys::BasicC2Taylor<CAPD_USER_NAMESPACE::DMap> DC2Taylor;
typedef capd::dynsys::BasicCnTaylor<CAPD_USER_NAMESPACE::DMap> DCnTaylor;

} // end of namespace capd

