
/////////////////////////////////////////////////////////////////////////////
/// @file C2Taylor.cpp
///
/// @author Daniel Wilczak
/////////////////////////////////////////////////////////////////////////////

// Copyright (C) 2000-2013 by the CAPD Group.
//
// This file constitutes a part of the CAPD library, 
// distributed under the terms of the GNU General Public License.
// Consult  http://capd.ii.uj.edu.pl/ for details. 

#include "capd/vectalg/lib.h"
#include "capd/map/lib.h"
#include "capd/dynsys/Taylor.hpp"
#include "capd/dynsys/BasicC2Taylor.hpp"
#include "capd/dynsys/C2Taylor.hpp"

template class capd::dynsys::C2Taylor<capd::IMap>;
template class capd::dynsys::BasicC2Taylor<capd::IMap,capd::dynsys::IEncFoundStepControl>;
template class capd::dynsys::BasicC2Taylor<capd::DMap>;
template class capd::dynsys::BasicTaylor<capd::DMap,capd::dynsys::DLastTermsStepControl,capd::diffAlgebra::C2Curve< capd::diffAlgebra::BasicC2Curve<capd::DMatrix> > >;
template class capd::dynsys::BasicTaylor<capd::IMap,capd::dynsys::ILastTermsStepControl,capd::diffAlgebra::C2Curve< capd::diffAlgebra::BasicC2Curve<capd::IMatrix> > >;
template class capd::dynsys::BasicTaylor<capd::IMap,capd::dynsys::IEncFoundStepControl,capd::diffAlgebra::C2Curve< capd::diffAlgebra::BasicC2Curve<capd::IMatrix> > >;
