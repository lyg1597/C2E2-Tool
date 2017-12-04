/// @addtogroup poincare
/// @{

/////////////////////////////////////////////////////////////////////////////
/// @file BasicPoincareMap_template.h
///
/// @author Daniel Wilczak
/// @author Tomasz Kapela
/////////////////////////////////////////////////////////////////////////////

// Copyright (C) 2008 by the CAPD Group.
//
// Distributed under the terms of the GNU General Public License.
// Consult  http://capd.ii.uj.edu.pl/ for details.

#ifndef _CAPD_POINCARE_BASIC_POINCARE_MAP_TEMPLATE_H_
#define _CAPD_POINCARE_BASIC_POINCARE_MAP_TEMPLATE_H_

#include "capd/poincare/BasicPoincareMap.h"

namespace capd{
namespace poincare{

// -----------------------------------------------------------------------------------------

/**
 * Nonrigorous Poincare map
 *
 * A point just after the section on the nonrigorous trajectory is returned
 * The result contains also approximate values of the partial derivatives
 * of the flow
 */

template <typename SolverT, typename FunT>
template <typename CnCoeffType>
CnCoeffType& BasicPoincareMap<SolverT, FunT>::operator()(CnCoeffType& coeff)
{
  SaveStepControl<SolverT> ssc(this->m_solver);
  this->reachSection(coeff, 1);
  ScalarType lastStep = this->m_solver.getStep();
  ScalarType t0 = findCrossingTime(coeff.getCurrentTime(),this->m_solver.getCurve());
  this->m_solver.getCurve().eval(t0,coeff);
  return coeff;
}

//////////////////////////////////////////////////////////////////////////////////
///
/// This function moves theSet with the given flow and stops just before the section
/// (for n-th iterate of Poincare Map).
///
/// @return set on the section or just after the section
///        (suitable for a computation of next Poincare Map iteration).
///
/// Parameter theSet contains on return the set just before the section.
///
/// Common function for different set types.
//////////////////////////////////////////////////////////////////////////////////
template <typename SolverT, typename FunT>
template<typename T>
T BasicPoincareMap<SolverT, FunT>::reachSection(
  T& theSet,    ///< @param[in,out] theSet  on input: initial set position, on return: the set just before the section.
  int n,        ///<                number of iterates
  ScalarType * lastStep   ///< if given
){
  this->m_solver.getStepControl().init(this->m_solver,theSet.getCurrentTime(),theSet);

  T v = theSet;

  // --------- leave the section --------------------------------
  // We do at least one step to avoid situation where initial point
  // is just before section due to floating point errors

  this->m_solver(v);
  ScalarType sign = m_section(v);

  while( (sign==0.0)                              // We are on the section
      || !((m_crossingDirection == Both)          // section sign is not correct
          || ((m_crossingDirection == MinusPlus) && (sign < 0.0))
          || ((m_crossingDirection == PlusMinus) && (sign > 0.0))
      )
  ){
    this->m_solver(v);
    sign = m_section(v);
  }

  // ------------- reach the section ----------------------------------
  //first approach to section with a relatively large time step
  T beforeSection = v;
  while(m_section(v)*sign > 0) {
    beforeSection = v;
    this->m_solver(v);
  }

  theSet = beforeSection;
  if(lastStep)
    *lastStep = this->m_solver.getStep();
  return v;
}

// ------------------------------------------------------------------------------------------
/**
 *  Computes a partial derivatives of a Poincare map to an arbitrarily order.
 *
 *  @param PhiSeries  partial derivatives of the flow to a given order
 *
 *  @remark Computations are valid only for linear sections.
 */
template <typename SolverT, typename FunT>
template<typename CnCoeffType>
CnCoeffType BasicPoincareMap<SolverT, FunT>::computePartialDerivatives(const CnCoeffType& PhiSeries)
{
  typedef typename CnCoeffType::Multipointer Multipointer;
  const size_type dim = this->getVectorField().dimension();
  const size_type degree = PhiSeries.degree();
  if(dim != PhiSeries.dimension())
      throw std::runtime_error("Incompatible dimensions in BasicPoincareMap::computePartialDerivatives");
  if(degree > this->m_solver.degree())
      throw std::runtime_error("Incompatible orders in BasicPoincareMap::computePartialDerivatives");

  CnCoeffType PSeries(dim,degree); // result
  PSeries() = PhiSeries();
  if(degree<=0)
    return PSeries;

// fieldOnPx contains a series of vectorField computed at Px
  const typename SolverT::SeriesContainerType& DtPhiSeries = m_solver.computeDaPhi(PhiSeries,degree+1);

  VectorType gradientOnPx = this->m_section.gradient(PhiSeries());
  VectorType fieldOnPx =  this->getVectorField()(PhiSeries());
  ScalarType scalarProduct = - gradientOnPx * fieldOnPx;

// first we compute separately dt/dx
  CnCoeffType TSeries(dim,degree);
  size_type i,j,k,s,p;
  for(j=0;j<dim;++j)
      for(i=0;i<dim;++i)
        TSeries(0,j) += gradientOnPx[i] * PhiSeries(i,j);
  for(j=0;j<dim;++j)
      TSeries(0,j) /= scalarProduct;

// and we compute dP/dx
  for(i=0;i<dim;++i)
      for(j=0;j<dim;++j)
        PSeries(i,j) = fieldOnPx[i]*TSeries(0,j) + PhiSeries(i,j);

// recursive computation of higher order partial derivatives
  for(p=2;p<=degree;++p)
  {
      Multipointer a = PSeries.first(p);
      do
      {
        VectorType temp = PhiSeries(a);
        VectorType temp2(dim);

        for(k=2;k<=p;++k)
        {
            const typename Multipointer::IndicesSet& is = Multipointer::generateList(p,k);
            typename Multipointer::IndicesSet::const_iterator b=is.begin(), e=is.end();
            while(b!=e)
            {
              ScalarType product=TSeries( 0,a.subMultipointer((*b)[0]) );
              for(j=1;j<k;++j)
                  product *= TSeries( 0,a.subMultipointer((*b)[j]) );

              DtPhiSeries.takeVector(Multipointer(0),k,temp2);
              temp += (product*factorial(k)) * temp2;

              for(s=0;s<k;++s)
              {
                  ScalarType product2(1.);
                  for(j=0;j<k;++j)
                    if(j!=s)
                        product2 *= TSeries( 0,a.subMultipointer((*b)[j]) );

                  DtPhiSeries.takeVector(a.subMultipointer((*b)[s]),k-1,temp2);
                  temp += product2*factorial(k-1)*temp2;
              }
              ++b;
        } // end while loop
      } // end k-loop

        TSeries(0,a) = (gradientOnPx*temp)/scalarProduct;
        PSeries(a) = temp + TSeries(0,a)*fieldOnPx;
      }while(PSeries.next(a));
  }

  return PSeries;
}


}}

#endif

