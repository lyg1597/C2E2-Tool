


/////////////////////////////////////////////////////////////////////////////
/// @file CnRect2.cpp
///
/// @author Daniel Wilczak
/////////////////////////////////////////////////////////////////////////////

// Copyright (C) 2006 by the CAPD Group.
//
// This file constitutes a part of the CAPD library, 
// distributed under the terms of the GNU General Public License.
// Consult  http://capd.wsb-nlu.edu.pl/ for details. 

#include "capd/vectalg/mplib.h"
#include "capd/dynset/CnDoubletonSet.hpp"

#ifdef __HAVE_MPFR__
  template class capd::dynset::CnDoubletonSet<capd::MpIMatrix >;
#endif


