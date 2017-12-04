/// @addtogroup dynsys
/// @{

/////////////////////////////////////////////////////////////////////////////
/// @file highOrderEnclosure.h
///
/// @author Daniel Wilczak
/////////////////////////////////////////////////////////////////////////////

// Copyright (C) 2000-2012 by the CAPD Group.
//
// This file constitutes a part of the CAPD library,
// distributed under the terms of the GNU General Public License.
// Consult  http://capd.ii.uj.edu.pl/ for details.

#ifndef _CAPD_DYNSYS_HIGH_ORDER_ENCLOSURE_H_
#define _CAPD_DYNSYS_HIGH_ORDER_ENCLOSURE_H_

#include "capd/intervals/Interval.hpp"
#include "capd/dynsys/enclosure.hpp"
#include "capd/diffAlgebra/C1Coeff.h"
#include "capd/dynsys/TaylorException.h"
#include "capd/dynsys/StepControl.h"

namespace capd{
namespace dynsys{

template<class DS>
void highOrderEnclosureError(DS & ds, const char message[])
{
  typename DS::VectorType problematicInitialCondition(ds.dimension());
  for(typename DS::size_type i=0;i<ds.dimension();++i)
    problematicInitialCondition[i] = ds.coefficient(i,0);
  throw TaylorException<typename DS::VectorType>(message,problematicInitialCondition,ds.getStep());
}

/**
 * This function computes enclosure for the Taylor series up to some order of \varphi(t,x).
 * Then the coefficients for the remainder part are recomputed if they at least one of them is equal to zero.
 * This means that this is the first step of integration.
 */

//#define ENC_POLY

#ifdef ENC_POLY

template<class DS>
bool encloseRangeOfPolynomial(DS& ds, typename DS::VectorType& phi, const typename DS::ScalarType& h)
{
  typedef typename DS::size_type size_type;
  const size_type order = ds.getOrder();
  const size_type dimension = ds.dimension();
  bool recomputeRemainder = false;
  for(size_type i = 0; i<dimension;++i)
  {
    phi[i] = ds.coefficient(i,order);
    for(int j=order-1;j>=0;--j)
      phi[i] = h*phi[i] + ds.coefficient(i,j);
    if(ds.remainderCoefficient(i,order+1).leftBound() != 0. or ds.remainderCoefficient(i,order+1).rightBound()!=0.)
      {}
    else
      recomputeRemainder = true;
  }
  return recomputeRemainder;
}

#else

template<class DS>
bool encloseRangeOfPolynomial(DS& ds, typename DS::VectorType& phi, const typename DS::ScalarType& h)
{
  typedef typename DS::size_type size_type;
  const size_type order = ds.getOrder();
  const size_type dimension = ds.dimension();
  // first enclose derivative
  typename DS::VectorType phiPrim(dimension);
  for(size_type i = 0; i<dimension;++i)
  {
    phiPrim[i] = order*ds.coefficient(i,order);
    for(int j=order-1;j>0;--j)
      phiPrim[i] = h*phiPrim[i] + j*ds.coefficient(i,j);
  }

  bool recomputeRemainder = false;
  for(size_type i = 0; i<dimension;++i)
  {
    typename DS::ScalarType hh;
    if(isSingular(phiPrim[i]))
    {
      hh = h;
    } else{
      hh = h.right();
    }
    phi[i] = ds.coefficient(i,order);
    for(int j=order-1;j>=0;--j)
      phi[i] = hh*phi[i] + ds.coefficient(i,j);
    if(phi[i].leftBound()>=ds.remainderCoefficient(i,0).leftBound() and phi[i].rightBound()<=ds.remainderCoefficient(i,0).rightBound())
      {}
    else
      recomputeRemainder = true;
    phi[i] = intervalHull(phi[i],ds.coefficient(i,0));
  }
  return recomputeRemainder;
}

#endif

/**
 * This function computes enclosure for the Taylor series up to some order of \varphi(t,x).
 * Then the coefficients for the remainder part are recomputed if they at least one of them is equal to zero.
 * This usually means that this is the first step of integration.
 */
template<class DS>
bool encloseRangeOfPolynomial(
    DS& ds,
    capd::diffAlgebra::C1Coeff<typename DS::MatrixType>& phi,
    const typename DS::ScalarType& h
    )
{
  bool recomputeRemainder = encloseRangeOfPolynomial(ds,phi.refValue(),h);
  typedef typename DS::size_type size_type;
  const size_type order = ds.getOrder();
  const size_type dimension = ds.dimension();

  for(size_type i=0; i<dimension;++i)
  {
    for(size_type k=0;k<dimension;++k)
    {
      phi.refDerivative()(i+1,k+1) = ds.coefficient(i,k,order);
      for(int j=order-1;j>=0;--j)
        phi.refDerivative()(i+1,k+1) = h*phi.refDerivative()(i+1,k+1) + ds.coefficient(i,k,j);
      if(phi.refDerivative()(i+1,k+1).leftBound()>=ds.remainderCoefficient(i,k,0).leftBound() and phi.refDerivative()(i+1,k+1).rightBound()<=ds.remainderCoefficient(i,k,0).rightBound())
        {}
      else
        recomputeRemainder = true;
    }
  }
  return recomputeRemainder;
}


/**
 * Two overloaded functions that recompute remainder coefficients for C^0 and C^1 computations
 */
template<class DS>
inline void computeRemainderCoefficients(const typename DS::ScalarType& t, DS& ds, typename DS::VectorType& x)
{
  ds.setCurrentTime(t);
  ds.computeRemainderCoefficients(x);
}

template<class DS>
inline void computeRemainderCoefficients(const typename DS::ScalarType& t, DS& ds, capd::diffAlgebra::C1Coeff<typename DS::MatrixType>& x)
{
  ds.setCurrentTime(t);
  ds.computeRemainderCoefficients(x.refValue(),x.refDerivative());
}

/**
 * Two overloaded functions that predict next enclosure
 */
template<class DS>
inline void predictNextEnclosure(
      DS& ds,
      const typename DS::ScalarType& stepToOrder,
      const typename DS::VectorType& phi,
      typename DS::VectorType& out_enc,
      typename DS::VectorType& out_remEnc
      )
{
  const static typename DS::ScalarType::BoundType epsilon = 1.e-300;
  const static typename DS::ScalarType mulFactor(-2.,2.);
  for(typename DS::size_type i=0;i<ds.dimension();++i)
  {
    out_remEnc[i] = mulFactor*(stepToOrder * ds.remainderCoefficient(i,ds.getOrder()+1)+epsilon);
    //out_remEnc[i] = mulFactor*h*ds.getStepControl().m_epsilon*(1+size(x));
    out_enc[i] = phi[i] + out_remEnc[i];
  }
}

template<class DS>
inline void predictNextEnclosure(
      DS& ds,
      const typename DS::ScalarType& stepToOrder,
      const capd::diffAlgebra::C1Coeff<typename DS::MatrixType>& phi,
      capd::diffAlgebra::C1Coeff<typename DS::MatrixType>& out_enc,
      capd::diffAlgebra::C1Coeff<typename DS::MatrixType>& out_remEnc
      )
{
  const static typename DS::ScalarType::BoundType epsilon = 1.e-300;
  const static typename DS::ScalarType mulFactor(-2.,2.);
  typedef typename DS::size_type size_type;

  predictNextEnclosure(ds,stepToOrder,phi(),out_enc.refValue(),out_remEnc.refValue());

  for(size_type i=0;i<ds.dimension();++i)
  {
    for(size_type j=0;j<ds.dimension();++j)
    {
      out_remEnc.refDerivative()(i+1,j+1) = mulFactor*(stepToOrder * ds.remainderCoefficient(i,j,ds.getOrder()+1)+epsilon);
      //u[i] = mulFactor*h*ds.getStepControl().m_epsilon*(1+size(x));
      out_enc.refDerivative()(i+1,j+1) = phi.getDerivative()(i+1,j+1) + out_remEnc.getDerivative()(i+1,j+1);
    }
  }
}

/**
 * Two overloaded functions that predict next enclosure
 */

template<class DS>
inline bool checkInclusion(
      DS& ds,
      const typename DS::VectorType& remEnc,
      const typename DS::ScalarType& stepToOrder,
      typename DS::VectorType& out_rem,
      typename DS::ScalarType& out_factor
      )
{
  bool isSubset = true;
  for(typename DS::size_type i=0;i<ds.dimension();++i)
  {
    out_rem[i] = stepToOrder * ds.remainderCoefficient(i,ds.getOrder()+1);
    typename DS::ScalarType v = capd::max(capd::abs(out_rem[i].leftBound()),capd::abs(out_rem[i].rightBound()));
    if(!isSingular(v))
      out_factor = capd::min(out_factor,remEnc[i].right()/v);
    if(!subsetInterior(out_rem[i],remEnc[i]))
      isSubset = false;
  }
  return isSubset;
}

template<class DS>
inline bool checkInclusion(
      DS& ds,
      const capd::diffAlgebra::C1Coeff<typename DS::MatrixType>& remEnc,
      const typename DS::ScalarType& stepToOrder,
      capd::diffAlgebra::C1Coeff<typename DS::MatrixType>& out_rem,
      typename DS::ScalarType& out_factor
      )
{
  typedef typename DS::size_type size_type;
  bool isSubset = checkInclusion(ds,remEnc(),stepToOrder,out_rem.refValue(),out_factor);
  for(size_type i=0;i<ds.dimension();++i)
  {
    for(size_type j=0;j<ds.dimension();++j)
    {
      out_rem.refDerivative()(i+1,j+1) = stepToOrder * ds.remainderCoefficient(i,j,ds.getOrder()+1);
      typename DS::ScalarType v = capd::max(capd::abs(out_rem.refDerivative()(i+1,j+1).leftBound()),capd::abs(out_rem.refDerivative()(i+1,j+1).rightBound()));
      if(!isSingular(v))
        out_factor = capd::min(out_factor,remEnc.getDerivative()(i+1,j+1).right()/v);
      if(!subsetInterior(out_rem.refDerivative()(i+1,j+1),remEnc.getDerivative()(i+1,j+1)))
        isSubset = false;
    }
  }
  return isSubset;
}

/**
 * Two overloaded functions that predict next enclosure
 */
template<class VectorType>
inline void refineEnclosure( const VectorType& phi, const VectorType& rem, VectorType& out_remEnc, VectorType& out_enc )
{
  for(typename VectorType::size_type i=0;i< phi.dimension();++i){

    if(rem[i].subsetInterior(out_remEnc[i]))
      continue;
    out_remEnc[i] *= typename TypeTraits<typename VectorType::ScalarType>::Real(1.5);
    out_enc[i] = phi[i] + out_remEnc[i];
  }
}

template<class MatrixType>
inline void refineEnclosure( 
    const capd::diffAlgebra::C1Coeff<MatrixType>& phi,
    const capd::diffAlgebra::C1Coeff<MatrixType>& rem,
    capd::diffAlgebra::C1Coeff<MatrixType>& out_remEnc,
    capd::diffAlgebra::C1Coeff<MatrixType>& out_enc
  )
{
  refineEnclosure(phi(),rem(),out_remEnc.refValue(),out_enc.refValue());
  for(typename MatrixType::size_type i=1;i<=phi().dimension();++i)
  {
    for(typename MatrixType::size_type j=1;j<=phi().dimension();++j)
    {
      if(rem.getDerivative()(i,j).subsetInterior(out_remEnc.getDerivative()(i,j)))
        continue;
      out_remEnc.refDerivative()(i,j) *= typename TypeTraits<typename MatrixType::ScalarType>::Real(1.5);
      out_enc.refDerivative()(i,j) = phi.getDerivative()(i,j) + out_remEnc.getDerivative()(i,j);
    }
  }
}

/**
  * Computes enclosure of solution of ODE during one time step  i.e \f$ \varphi([0,h],x) \f$
  * for some \f$ h\in[0,hTrial] \f$, where \f$ hTrial \f$ is the predicted step
  * or for \f$ h=hTrial \f$ if changing of the time step is not allowed.
  *
  * If CoeffType is a C1Coff structure then the function computes enclosure
  * and the Lagrange remainder for the solutions to variational equations as well.
  *
  * @param ds             dynamical system
  * @param out_remainder  when success, contains the Lagrange remainder computed at enclosure
  * @return  enclosure of solution of ODE
  */
template<class DS, class CoeffType>
CoeffType highOrderEnclosure(const typename DS::ScalarType& t, DS& ds, CoeffType& out_remainder)
{
  typedef typename DS::ScalarType ScalarType;
  typedef typename ScalarType::BoundType Real;
  const typename DS::size_type dimension = ds.dimension();

  const static ScalarType I(TypeTraits<Real>::zero(),TypeTraits<Real>::one());
  ScalarType h = I*ds.getStep();
  ScalarType stepToOrder = power(h,ds.getOrder()+1);

  CoeffType phi(dimension), enc(dimension), remEnc(dimension);
  if(encloseRangeOfPolynomial(ds,phi,h))
    computeRemainderCoefficients(t+h,ds,phi);

  // Now we are predicting the enclosure based on the remainder from the previous approved step
  // (enc and remEnc are returned by this function)
  predictNextEnclosure(ds,stepToOrder,phi,enc,remEnc);
  // and compute remainder coefficients on predicted enclosure.
  computeRemainderCoefficients(t+h,ds,enc);

  ScalarType factor(1.);
  // here we check if computed remainder coefficients are subsets of predicted enclosure of remainder coefficients
  // (factor can be decreased in this function)
  // out_remainder is returned by this function
  if (checkInclusion(ds,remEnc,stepToOrder,out_remainder,factor) )
    return enc;

  // we do not have inclusion, but maybe we can adjust the time step so that it will hold

  if(ds.isStepChangeAllowed())
  {
    if(isSingular(factor)
        or isInf(factor.leftBound()) or isInf(factor.rightBound())
        or isNaN(factor.leftBound()) or isNaN(factor.rightBound())
    ) highOrderEnclosureError(ds,"High Order Enclosure Error: cannot adjust time step. Cannot integrate.");

    factor = clearMantissaBits(exp(log(factor)/(ds.getOrder()+1)).leftBound());
    typename capd::TypeTraits<ScalarType>::Real newStep = leftBound(ds.getStep()*factor);

    if(capd::abs(newStep) < ds.getStepControl().getMinStepAllowed())
      highOrderEnclosureError(ds,"High Order Enclosure Error: minimal time step reached. Cannot integrate.");

    // success - we have enclosure with new (smaller) time step
    ds.adjustTimeStep(ScalarType(newStep));
    return enc;
  }

  // time step change is not allowed. We have to refine enclosure and recompute remainder
  int maxAttempts = 30;
  while(maxAttempts)
  {
    // In this function remEnc is enlarged and enc is set to phi + remEnc
    refineEnclosure(phi,out_remainder,remEnc,enc);
    computeRemainderCoefficients(t+h,ds,enc);

    // check the inclusion
    ScalarType factor(1.);
    if (checkInclusion(ds,remEnc,stepToOrder,out_remainder,factor) )
      return enc;
    maxAttempts--;
  } // endwhile

  highOrderEnclosureError(ds,"High Order Enclosure Error: cannot find enclosure guaranteeing bounds, loop limit exceeded");
  return out_remainder; // just to do not have warning
}

}} // namespace capd::dynsys

#endif
