//////////////////////////////////////////////////////////////////////////////
//   Package:          CAPD

/////////////////////////////////////////////////////////////////////////////
//
/// @file dynsys/mplib.h
///
/// @author Tomasz Kapela   @date 2010-01-22
//
/////////////////////////////////////////////////////////////////////////////

// Copyright (C) Tomasz Kapela 2010
//
// This file constitutes a part of the CAPD library,
// distributed under the terms of the GNU General Public License.
// Consult  http://capd.ii.uj.edu.pl/ for details.

#ifndef _CAPD_DYNSYS_MPLIB_H_
#define _CAPD_DYNSYS_MPLIB_H_


#include "capd/basicalg/factrial.h"
//#include "capd/map/Map.h"
//#include "capd/map/C2Map.h"
//#include "capd/map/CnMap.h"
#include "capd/dynsys/TaylorException.h"
#include "capd/dynsys/C2Taylor.h"
#include "capd/dynsys/CnTaylor.h"
#include "capd/dynsys/TaylorHOE.h"
#include "capd/vectalg/mplib.h"
#include "capd/map/mplib.h"

#ifdef __HAVE_MPFR__

namespace capd{
  typedef capd::dynsys::DynSys<MpIMatrix> MpIDynSys;
  typedef capd::dynsys::Taylor<MpIMap> MpITaylor;
  typedef capd::dynsys::C2Taylor<MpIMap> MpIC2Taylor;
  typedef capd::dynsys::CnTaylor<MpIMap> MpICnTaylor;
  typedef capd::dynsys::TaylorHOE<capd::MpIMap, capd::dynsys::IMpLastTermsStepControl<capd::MpInterval> > MpITaylorHOE;

  typedef capd::dynsys::TaylorException<MpIVector> MpITaylorException;

  // classes for nonrigorous computations
  typedef capd::dynsys::BasicTaylor<MpMap, capd::dynsys::MpLastTermsStepControl<MpFloat> > MpTaylor;
  typedef capd::dynsys::BasicC2Taylor<MpMap, capd::dynsys::MpLastTermsStepControl<MpFloat> > MpC2Taylor;
  typedef capd::dynsys::BasicCnTaylor<MpMap, capd::dynsys::MpLastTermsStepControl<MpFloat> > MpCnTaylor;
} // end of namespace capd

#endif //__HAVE_MPFR__

#endif // _CAPD_DYNSYS_MPLIB_H_
