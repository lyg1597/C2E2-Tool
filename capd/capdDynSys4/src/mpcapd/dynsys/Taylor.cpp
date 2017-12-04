
/////////////////////////////////////////////////////////////////////////////
/// @file Taylor.cpp
///
/// @author Daniel Wilczak
/////////////////////////////////////////////////////////////////////////////

// Copyright (C) 2000-2005 by the CAPD Group.
//
// This file constitutes a part of the CAPD library,
// distributed under the terms of the GNU General Public License.
// Consult  http://capd.wsb-nlu.edu.pl/ for details.

#include "capd/vectalg/mplib.h"
#include "capd/map/mplib.h"
#include "capd/dynsys/BasicTaylor.hpp"
#include "capd/dynsys/Taylor.hpp"

#ifdef __HAVE_MPFR__

  template class capd::dynsys::Taylor<capd::MpIMap, capd::dynsys::IMpLastTermsStepControl<capd::MpInterval> >;
  template class capd::dynsys::Taylor<capd::MpIMap, capd::dynsys::IEncFoundStepControl>;

  template class capd::dynsys::BasicTaylor<capd::MpIMap, capd::dynsys::IMpLastTermsStepControl<capd::MpInterval> >;
  template class capd::dynsys::BasicTaylor<capd::MpIMap, capd::dynsys::IEncFoundStepControl>;

  template class capd::dynsys::BasicTaylor<capd::MpMap, capd::dynsys::MpLastTermsStepControl<capd::MpFloat> >;
#endif
