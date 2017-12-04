
/////////////////////////////////////////////////////////////////////////////
/// @file PoincareMap.cpp
///
/// @author Daniel Wilczak
/////////////////////////////////////////////////////////////////////////////

// Copyright (C) 2000-2013 by the CAPD Group.
//
// This file constitutes a part of the CAPD library, 
// distributed under the terms of the GNU General Public License.
// Consult  http://capd.ii.uj.edu.pl/ for details.


#include "capd/intervals/lib.h"
#include "capd/vectalg/lib.h"
#include "capd/map/lib.h"
#include "capd/dynsys/lib.h"
#include "capd/poincare/PoincareMap.hpp"
#include "capd/dynsys/FadMap.h"
#include "capd/dynsys/BasicFadTaylor.hpp"
#include "capd/dynsys/FadTaylor.hpp"
#include "capd/dynsys/FadTaylorHOE.hpp"

using namespace capd;

template class poincare::BasicPoincareMap<DTaylor>;
template class poincare::BasicPoincareMap<DC2Taylor>;
template class poincare::BasicPoincareMap<DCnTaylor>;
template class poincare::BasicPoincareMap< dynsys::BasicFadTaylor<dynsys::LorenzFadMap<double,0>,dynsys::DLastTermsStepControl > >;

template class poincare::PoincareMap<ITaylor>;
template class poincare::PoincareMap<IC2Taylor>;
template class poincare::PoincareMap<ICnTaylor>;

template class poincare::PoincareMap< dynsys::FadTaylor<dynsys::LorenzFadMap<DInterval,0>,dynsys::ILastTermsStepControl > >;
template class poincare::PoincareMap< dynsys::FadTaylorHOE<dynsys::LorenzFadMap<DInterval,0>,dynsys::ILastTermsStepControl > >;
