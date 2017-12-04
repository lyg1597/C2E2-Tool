
/////////////////////////////////////////////////////////////////////////////
/// @file C1Coeff.cpp
///
/// @author Daniel Wilczak
/////////////////////////////////////////////////////////////////////////////

// Copyright (C) 2000-2005 by the CAPD Group.
//
// This file constitutes a part of the CAPD library,
// distributed under the terms of the GNU General Public License.
// Consult  http://capd.wsb-nlu.edu.pl/ for details.

#include "capd/intervals/lib.h"
#include "capd/vectalg/lib.h"
#include "capd/vectalg/Vector.hpp"
#include "capd/vectalg/Matrix.hpp"
#include "capd/diffAlgebra/C1Coeff.h"

template class capd::diffAlgebra::C1Coeff<capd::DMatrix>;
template class capd::diffAlgebra::C1Coeff<capd::IMatrix>;
