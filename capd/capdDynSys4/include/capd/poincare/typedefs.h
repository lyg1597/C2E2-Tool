//////////////////////////////////////////////////////////////////////////////
//   Package:          CAPD

/////////////////////////////////////////////////////////////////////////////
//
/// @file poincare/typedefs.h
///
/// @author Daniel Wilczak   @date 2013-01-09
//
/////////////////////////////////////////////////////////////////////////////

// This file constitutes a part of the CAPD library,
// distributed under the terms of the GNU General Public License.
// Consult  http://capd.ii.uj.edu.pl/ for details.

namespace CAPD_USER_NAMESPACE{

// classes for nonrigorous computations
typedef capd::poincare::BasicPoincareMap<CAPD_USER_NAMESPACE::DTaylor> DPoincareMap;
typedef capd::poincare::BasicPoincareMap<CAPD_USER_NAMESPACE::DC2Taylor> DC2PoincareMap;
typedef capd::poincare::BasicPoincareMap<CAPD_USER_NAMESPACE::DCnTaylor> DCnPoincareMap;

typedef capd::poincare::PoincareMap<CAPD_USER_NAMESPACE::ITaylor> IPoincareMap;
typedef capd::poincare::PoincareMap<CAPD_USER_NAMESPACE::IC2Taylor> IC2PoincareMap;
typedef capd::poincare::PoincareMap<CAPD_USER_NAMESPACE::ICnTaylor> ICnPoincareMap;


typedef capd::poincare::TimeMap<CAPD_USER_NAMESPACE::DTaylor> DTimeMap;
typedef capd::poincare::TimeMap<CAPD_USER_NAMESPACE::DC2Taylor> DC2TimeMap;
typedef capd::poincare::TimeMap<CAPD_USER_NAMESPACE::DCnTaylor> DCnTimeMap;

typedef capd::poincare::TimeMap<CAPD_USER_NAMESPACE::ITaylor> ITimeMap;
typedef capd::poincare::TimeMap<CAPD_USER_NAMESPACE::IC2Taylor> IC2TimeMap;
typedef capd::poincare::TimeMap<CAPD_USER_NAMESPACE::ICnTaylor> ICnTimeMap;

} // end of namespace capd
