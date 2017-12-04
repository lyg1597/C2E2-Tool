
/////////////////////////////////////////////////////////////////////////////
/// @file CnCoeff.cpp
///
/// @author Daniel Wilczak
/////////////////////////////////////////////////////////////////////////////

// Copyright (C) 2000-2005 by the CAPD Group.
//
// This file constitutes a part of the CAPD library, 
// distributed under the terms of the GNU General Public License.
// Consult  http://capd.wsb-nlu.edu.pl/ for details. 

#include "capd/vectalg/lib.h"
#include "capd/diffAlgebra/CnCoeff.hpp"
#include "capd/vectalg/Container.hpp"

template class capd::diffAlgebra::CnContainer<double,0,0,0>;
template class capd::diffAlgebra::CnContainer<long double,0,0,0>;
template class capd::diffAlgebra::CnContainer<capd::DInterval,0,0,0>;

template class capd::diffAlgebra::CnContainer<double,3,3,3>;
template class capd::diffAlgebra::CnContainer<long double,3,3,3>;
template class capd::diffAlgebra::CnContainer<capd::DInterval,3,3,3>;

template class capd::diffAlgebra::CnCoeff< capd::DMatrix, 0 >;
template class capd::diffAlgebra::CnCoeff< capd::IMatrix, 0 >;

template class capd::diffAlgebra::Jet< capd::DMatrix, 0 >;
template class capd::diffAlgebra::Jet< capd::IMatrix, 0 >;
