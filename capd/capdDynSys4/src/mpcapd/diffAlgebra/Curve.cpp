
/////////////////////////////////////////////////////////////////////////////
/// @file mpcapd/diffAlgebra/Curve.cpp
///
/// @author Daniel Wilczak
/////////////////////////////////////////////////////////////////////////////

// Copyright (C) 2000-2005 by the CAPD Group.
//
// This file constitutes a part of the CAPD library, 
// distributed under the terms of the GNU General Public License.
// Consult  http://capd.ii.uj.edu.pl/ for details. 


#include "capd/vectalg/mplib.h"
#include "capd/diffAlgebra/BasicCurve.hpp"
#include "capd/diffAlgebra/FadCurve.hpp"
#include "capd/diffAlgebra/Curve.hpp"

#ifdef __HAVE_MPFR__

  template class capd::diffAlgebra::BasicCurve<capd::MpMatrix>;
  template class capd::diffAlgebra::BasicCurve<capd::MpIMatrix>;

  template class capd::diffAlgebra::FadCurve<capd::MpMatrix>;
  template class capd::diffAlgebra::FadCurve<capd::MpIMatrix>;

  template class capd::diffAlgebra::Curve< capd::diffAlgebra::BasicCurve<capd::MpMatrix> >;
  template class capd::diffAlgebra::Curve< capd::diffAlgebra::BasicCurve<capd::MpIMatrix> >;

  template class capd::diffAlgebra::Curve< capd::diffAlgebra::FadCurve<capd::MpMatrix> >;
  template class capd::diffAlgebra::Curve< capd::diffAlgebra::FadCurve<capd::MpIMatrix> >;

#endif
