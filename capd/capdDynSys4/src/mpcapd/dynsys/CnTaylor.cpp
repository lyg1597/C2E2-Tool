
/////////////////////////////////////////////////////////////////////////////
/// @file CnTaylor.cpp
///
/// @author Daniel Wilczak
/////////////////////////////////////////////////////////////////////////////

// Copyright (C) 2000-2005 by the CAPD Group.
//
// This file constitutes a part of the CAPD library, 
// distributed under the terms of the GNU General Public License.
// Consult  http://capd.wsb-nlu.edu.pl/ for details. 

/* Author: Daniel Wilczak 2006 */

#include "capd/vectalg/mplib.h"
#include "capd/map/mplib.h"
#include "capd/dynsys/BasicCnTaylor.hpp"
#include "capd/dynsys/CnTaylor.hpp"
#include "capd/map/Map.hpp"

#ifdef __HAVE_MPFR__
  template class capd::dynsys::BasicCnTaylor<capd::MpMap, capd::dynsys::MpLastTermsStepControl<capd::MpFloat> >;

  template class capd::dynsys::BasicCnTaylor<capd::MpIMap,capd::dynsys::IMpLastTermsStepControl<capd::MpInterval> >;
  template class capd::dynsys::CnTaylor<capd::MpIMap,capd::dynsys::IMpLastTermsStepControl<capd::MpInterval>  >;

  template class capd::dynsys::BasicCnTaylor<capd::MpIMap,capd::dynsys::IEncFoundStepControl>;
  template class capd::dynsys::CnTaylor<capd::MpIMap,capd::dynsys::IEncFoundStepControl>;
#endif


