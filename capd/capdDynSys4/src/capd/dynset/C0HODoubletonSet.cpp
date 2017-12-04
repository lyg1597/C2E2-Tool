
/////////////////////////////////////////////////////////////////////////////
/// @file C0HODoubletonSet.cpp
///
/// @author Daniel Wilczak
/////////////////////////////////////////////////////////////////////////////

// Copyright (C) 2000-2013 by the CAPD Group.
//
// This file constitutes a part of the CAPD library,
// distributed under the terms of the GNU General Public License.
// Consult  http://capd.ii.uj.edu.pl/ for details.

#include "capd/vectalg/lib.h"
#include "capd/dynset/lib.h"
#include "capd/dynset/C0HODoubletonSet.hpp"

template class capd::dynset::C0HODoubletonSet< capd::IMatrix, capd::C0Intv2Policies >;
template class capd::dynset::C0HODoubletonSet< capd::IMatrix, capd::C0Pped2Policies >;
template class capd::dynset::C0HODoubletonSet< capd::IMatrix, capd::C0Rect2Policies >;
