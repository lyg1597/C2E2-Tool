/// @addtogroup poincare
/// @{

/////////////////////////////////////////////////////////////////////////////
/// @file  PoincareMap.hpp
///
/// @author Daniel Wilczak
/// @author Tomasz Kapela
/////////////////////////////////////////////////////////////////////////////

// Copyright (C) 2008 by the CAPD Group.
//
// Distributed under the terms of the GNU General Public License.
// Consult  http://capd.ii.uj.edu.pl/ for details.

#ifndef _CAPD_POINCARE_POINCARE_MAP_HPP_
#define _CAPD_POINCARE_POINCARE_MAP_HPP_

#include <cassert>
#include <exception>
#include "capd/poincare/PoincareMap.h"
#include "capd/poincare/BasicPoincareMap.hpp"

namespace capd{
namespace poincare{

/**
 *   Constructs PoincareMap for given dynamical system and section
 *
 *  @param ds        dynamical system
 *  @param section   Poincare section
 *  @param crossing  section crossing direction
 *  @param factor    time step correction factor during section crossing (should be in interval (0, 1))
**/
template <typename SolverT, typename FunT>
PoincareMap<SolverT,FunT>::PoincareMap(
       Solver  & ds,
       FunctionType & section,
       typename BasicPoincareMap<SolverT,FunT>::CrossingDirection crossing,
       const BoundType & errorTolerance
  ) : BasicPoincareMap<SolverT,FunT>(ds, section, crossing, errorTolerance),
  timeStepCorrectionFactor(0.9),
  maxCorrectionAttempts(10),
  minCrossingTimeStep( capd::TypeTraits< ScalarType >::epsilon()*4),
  sectionCoordinateSystem(MatrixType::Identity(ds.dimension()))
{}

/*__________________________________________________________________________*/

template <typename SolverT, typename FunT>
void PoincareMap<SolverT,FunT>::updateJacEnclosure(C1Set& S)
{
  if(derivativeOfFlow!=NULL)
  {
    const typename Solver::SolutionCurve& c = this->m_solver.getCurve();
    ScalarType domain(c.getLeftDomain(),c.getRightDomain());
    MatrixType jacEnc = c[domain];
    if(!intersection(jacEnc,S.getLastMatrixEnclosure(),jacEnc))
      throw std::logic_error("PoincareMap::updateJacEnclosure - intersection of two methods for enclosing derivative is empty!");
    MatrixType oneStepBound = jacEnc*MatrixType(S);
    intervalHull(*derivativeOfFlow,oneStepBound,*derivativeOfFlow);
  }
  this->updateTime(S);
}

/*__________________________________________________________________________*/

template <typename SolverT, typename FunT>
void PoincareMap<SolverT,FunT>::updateJacEnclosure(C2Set& S)
{
  if(this->derivativeOfFlow!=NULL)
  {
    MatrixType currentDerivative = MatrixType(S);
    const typename Solver::SolutionCurve& curve = this->m_solver.getCurve();
    ScalarType domain(curve.getLeftDomain(),curve.getRightDomain());
    MatrixType jacEnc = curve[domain];
    if(!intersection(jacEnc,S.getLastMatrixEnclosure(),jacEnc))
      throw std::logic_error("PoincareMap::updateJacEnclosure - intersection of two methods for enclosing derivative is empty!");
    intervalHull(*derivativeOfFlow,jacEnc*currentDerivative,*derivativeOfFlow);

    // computation of the hessian of Poincare map
    if(hessianOfFlow!=NULL)
    {
      typename C2Set::HessianType oneStepHessian(this->getVectorField().dimension());
      typename C2Set::HessianType EH = curve.hessian(domain);
      if(!intersection(EH,S.getLastHessianEnclosure(),EH))
          throw std::logic_error("PoincareMap::updateJacEnclosure - intersection of two methods for enclosing hessian is empty!");
      oneStepHessian = jacEnc*(typename C2Set::HessianType)S + EH*currentDerivative;
      intervalHull(*hessianOfFlow,oneStepHessian,*hessianOfFlow);
    } // end if(hessianOfFlow!=NULL)
  } // end of if(derivativeOfFlow!=NULL)
  this->updateTime(S);
}

/*__________________________________________________________________________*/

template <typename SolverT, typename FunT>
void PoincareMap<SolverT,FunT>::saveJacEnclosure(C2Set& S)
{
  this->saveTime(S);
  if(derivativeOfFlow!=NULL)
    *derivativeOfFlow = MatrixType(S);

  if(hessianOfFlow!=NULL)
	*hessianOfFlow = (HessianType)S;
}

/*__________________________________________________________________________*/

template <typename SolverT, typename FunT>
void PoincareMap<SolverT,FunT>::saveJacEnclosure(C1Set& S)
{
  this->saveTime(S);
  if(derivativeOfFlow!=NULL)
    *derivativeOfFlow = MatrixType(S);
}

}} // namespace capd::poincare

#endif // _CAPD_POINCARE_POINCARE_MAP_HPP_

/// @}
