
/////////////////////////////////////////////////////////////////////////////
/// @file TimeMap.cpp
///
/// @author Tomasz Kapela
/////////////////////////////////////////////////////////////////////////////

// Copyright (C) 2000-2013 by the CAPD Group.
//
// This file constitutes a part of the CAPD library, 
// distributed under the terms of the GNU General Public License.
// Consult  http://capd.ii.uj.edu.pl/ for details.

#include "capd/map/lib.h"
#include "capd/dynsys/lib.h"
#include "capd/poincare/TimeMap.hpp"
#include "capd/dynsys/FadMap.h"
#include "capd/dynsys/BasicFadTaylor.hpp"
#include "capd/dynsys/FadTaylor.hpp"
#include "capd/dynsys/FadTaylorHOE.hpp"

using namespace capd;

template class poincare::TimeMap<ITaylor>;
template class poincare::TimeMap<IC2Taylor>;
template class poincare::TimeMap<ICnTaylor>;

template class poincare::TimeMap<DTaylor>;
template class poincare::TimeMap<DC2Taylor>;
template class poincare::TimeMap<DCnTaylor>;

template class poincare::TimeMap< dynsys::BasicFadTaylor<dynsys::LorenzFadMap<double,0>,dynsys::DLastTermsStepControl > >;
template class poincare::TimeMap< dynsys::FadTaylor<dynsys::LorenzFadMap<DInterval,0>,dynsys::ILastTermsStepControl > >;
template class poincare::TimeMap< dynsys::FadTaylorHOE<dynsys::LorenzFadMap<DInterval,0>,dynsys::ILastTermsStepControl > >;
