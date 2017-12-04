/// @addtogroup poincare
/// @{

/////////////////////////////////////////////////////////////////////////////
/// @file BasicPoincareMap_inline.h
///
/// @author Daniel Wilczak
/// @author Tomasz Kapela
/////////////////////////////////////////////////////////////////////////////

// Copyright (C) 2008 by the CAPD Group.
//
// Distributed under the terms of the GNU General Public License.
// Consult  http://capd.ii.uj.edu.pl/ for details.

#ifndef _CAPD_POINCARE_BASIC_POINCARE_MAP_INLINE_H_
#define _CAPD_POINCARE_BASIC_POINCARE_MAP_INLINE_H_

#include "capd/poincare/BasicPoincareMap.h"

namespace capd{
namespace poincare{

// -----------------------------------------------------------------------------------------


template <typename SolverT, typename FunT>
inline const typename BasicPoincareMap<SolverT, FunT>::Solver&
BasicPoincareMap<SolverT, FunT>::getDynamicalSystem() const
{
  return m_solver;
}
// -----------------------------------------------------------------------------------------

template <typename SolverT, typename FunT>
inline typename BasicPoincareMap<SolverT, FunT>::Solver&
BasicPoincareMap<SolverT, FunT>::getDynamicalSystem()
{
  return m_solver;
}
// -----------------------------------------------------------------------------------------

template <typename SolverT, typename FunT>
inline typename BasicPoincareMap<SolverT, FunT>::VectorFieldType const&
BasicPoincareMap<SolverT, FunT>::getVectorField() const
{
  return m_solver.getVectorField();
}

// -----------------------------------------------------------------------------------------

template <typename SolverT, typename FunT>
inline typename BasicPoincareMap<SolverT, FunT>::VectorFieldType&
BasicPoincareMap<SolverT, FunT>::getVectorField()
{
  return m_solver.getVectorField();
}

// -----------------------------------------------------------------------------------------

template <typename SolverT, typename FunT>
inline typename BasicPoincareMap<SolverT, FunT>::FunctionType const&
BasicPoincareMap<SolverT, FunT>::getSection() const
{
  return m_section;
}

// -----------------------------------------------------------------------------------------

template <typename SolverT, typename FunT>
inline typename BasicPoincareMap<SolverT, FunT>::FunctionType&
BasicPoincareMap<SolverT, FunT>::getSection()
{
  return m_section;
}

// -----------------------------------------------------------------------------------------

template <typename SolverT, typename FunT>
inline typename BasicPoincareMap<SolverT, FunT>::size_type
BasicPoincareMap<SolverT, FunT>::getOrder() const
{
  return m_solver.getOrder();
}

// -----------------------------------------------------------------------------------------

template <typename SolverT, typename FunT>
inline typename BasicPoincareMap<SolverT, FunT>::ScalarType
BasicPoincareMap<SolverT, FunT>::getStep() const
{
  return m_solver.getStep();
}

// -----------------------------------------------------------------------------------------

template <typename SolverT, typename FunT>
inline void BasicPoincareMap<SolverT, FunT>::setOrder(size_type newOrder)
{
  m_solver.setOrder(newOrder);
}

// -----------------------------------------------------------------------------------------

template <typename SolverT, typename FunT>
inline void BasicPoincareMap<SolverT, FunT>::setStep(const ScalarType& newStep)
{
  m_solver.setStep(newStep);
}

// -----------------------------------------------------------------------------------------

template <typename SolverT, typename FunT>
inline void BasicPoincareMap<SolverT, FunT>::setFactor(double newFactor)
{
  m_sectionFactor = newFactor;
}

// -----------------------------------------------------------------------------------------

template <typename SolverT, typename FunT>
inline void BasicPoincareMap<SolverT, FunT>::setSection(const FunctionType& newSection)
{
  m_section = newSection;
}

}}

#endif
