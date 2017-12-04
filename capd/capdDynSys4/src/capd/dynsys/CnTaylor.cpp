/////////////////////////////////////////////////////////////////////////////
/// @file CnTaylor.cpp
///
/// @author Daniel Wilczak
/////////////////////////////////////////////////////////////////////////////

// Copyright (C) 2000-2013 by the CAPD Group.
//
// This file constitutes a part of the CAPD library, 
// distributed under the terms of the GNU General Public License.
// Consult  http://capd.ii.uj.edu.pl/ for details. 

#include "capd/map/lib.h"

#include "capd/dynsys/BasicCnTaylor.hpp"
#include "capd/dynsys/CnTaylor.hpp"
#include "capd/diffAlgebra/CnCoeff.hpp"
#include "capd/diffAlgebra/CnCurve.hpp"

template class capd::dynsys::BasicCnTaylor<capd::DMap>;
template class capd::dynsys::BasicCnTaylor<capd::IMap,capd::dynsys::ILastTermsStepControl>;
template class capd::dynsys::CnTaylor<capd::IMap,capd::dynsys::ILastTermsStepControl>;

template class capd::dynsys::BasicCnTaylor<capd::IMap,capd::dynsys::IEncFoundStepControl>;
template class capd::dynsys::CnTaylor<capd::IMap,capd::dynsys::IEncFoundStepControl>;
