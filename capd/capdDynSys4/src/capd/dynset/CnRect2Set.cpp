
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

#include "capd/vectalg/lib.h"
#include "capd/dynset/CnDoubletonSet.hpp"

#include "capd/dynset/reorganization/FactorReorganization.h"
#include "capd/dynset/reorganization/QRReorganization.h"
#include "capd/dynset/QRPolicy.h"

using namespace capd::dynset;

typedef QRReorganization<InverseQRPolicy<> > Pped2Policies;
typedef FactorReorganization<FullQRWithPivoting<> > Rect2Policies;


template class capd::dynset::CnDoubletonSet< capd::IMatrix, Pped2Policies >;
template class capd::dynset::CnDoubletonSet< capd::IMatrix, Rect2Policies >;

