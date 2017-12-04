/// @addtogroup diffAlgebra
/// @{

/////////////////////////////////////////////////////////////////////////////
/// @file CnCoeff.hpp
///
/// @author Daniel Wilczak
/////////////////////////////////////////////////////////////////////////////

// Copyright (C) 2000-2005 by the CAPD Group.
//
// This file constitutes a part of the CAPD library, 
// distributed under the terms of the GNU General Public License.
// Consult  http://capd.ii.uj.edu.pl/ for details.

#ifndef _CAPD_DIFFALGEBRA_CNCOEFF_HPP_
#define _CAPD_DIFFALGEBRA_CNCOEFF_HPP_

#include <algorithm>
#include <sstream>
#include "capd/diffAlgebra/CnCoeff.h"
#include "capd/diffAlgebra/Jet.hpp"
#include "capd/basicalg/TypeTraits.h"

namespace capd{
namespace diffAlgebra{

// -----------------------------------------------------------------

template<typename MatrixT, __size_type DEGREE>
CnCoeff<MatrixT,DEGREE>::operator typename CnCoeff<MatrixT,DEGREE>::ImageVectorType() const
{
  ImageVectorType result(this->imageDimension());
  typename ImageVectorType::iterator b=result.begin(), e=result.end();
  const_iterator i=begin();
  size_type stride = newton(this->dimension()+this->degree(),this->dimension());
  while(b!=e)
  {
    *b = *i;
    ++b;
    i+=stride;
  }
  return result;
}

// -----------------------------------------------------------------

template<typename MatrixT, __size_type DEGREE>
CnCoeff<MatrixT,DEGREE>::operator typename CnCoeff<MatrixT,DEGREE>::MatrixType() const
{
  MatrixType result(this->imageDimension(),this->dimension());
  typename MatrixType::iterator b=result.begin(), e=result.end();
  size_type i=0;
  while(b!=e)
  {
    const_iterator b1=begin(i,1), e1=end(i,1);
    while(b1!=e1)
    {
      *b = *b1;
      ++b;
      ++b1;
    }
    ++i;
  }
  return result;
}

}} // the end of the namespace capd::diffAlgebra

// -----------------------------------------------------------------

template<typename MatrixT, capd::vectalg::__size_type DEGREE>
capd::diffAlgebra::CnCoeff<MatrixT,DEGREE>
operator+(const capd::diffAlgebra::CnCoeff<MatrixT,DEGREE>& x, const capd::diffAlgebra::CnCoeff<MatrixT,DEGREE>& y)
{
  if(x.dimension()!=y.dimension() or x.imageDimension()!=y.imageDimension())
    throw std::runtime_error("CnCoeff operator+: incompatible dimensions of power series");
  capd::diffAlgebra::CnCoeff<MatrixT,DEGREE> result(x);
  typename capd::diffAlgebra::CnCoeff<MatrixT,DEGREE>::iterator i = result.begin();
  typename capd::diffAlgebra::CnCoeff<MatrixT,DEGREE>::const_iterator b = y.begin(), e=y.end();
  while(b!=e)
  {
    (*i) += (*b);
    ++b;
    ++i;
  }
  return result;
}

// -----------------------------------------------------------------

template<typename MatrixT, capd::vectalg::__size_type DEGREE>
capd::diffAlgebra::CnCoeff<MatrixT,DEGREE>
operator-(const capd::diffAlgebra::CnCoeff<MatrixT,DEGREE>& x, const capd::diffAlgebra::CnCoeff<MatrixT,DEGREE>& y)
{
  if(x.dimension()!=y.dimension() or x.imageDimension()!=y.imageDimension())
    throw std::runtime_error("CnCoeff operator-: incompatible dimensions of power series");
  capd::diffAlgebra::CnCoeff<MatrixT,DEGREE> result(x);
  typename capd::diffAlgebra::CnCoeff<MatrixT,DEGREE>::iterator i = result.begin();
  typename capd::diffAlgebra::CnCoeff<MatrixT,DEGREE>::const_iterator b = y.begin(), e=y.end();
  while(b!=e)
  {
    (*i) -= (*b);
    ++b;
    ++i;
  }
  return result;
}

// -----------------------------------------------------------------

template<typename MatrixT, capd::vectalg::__size_type DEGREE>
typename MatrixT::ScalarType computeProduct(
      const capd::diffAlgebra::CnCoeff<MatrixT,DEGREE>& second,
      const capd::vectalg::Multiindex& mi,
      const capd::vectalg::Multipointer& a,
      int p, int k
   )
{
  typedef typename MatrixT::ScalarType Scalar;
  typedef capd::vectalg::Multiindex Multiindex;
  typedef capd::vectalg::Multipointer Multipointer;

  Scalar result = capd::TypeTraits<Scalar>::zero();
  const typename Multipointer::IndicesSet& is = Multipointer::generateList(p,k);
  typename Multipointer::IndicesSet::const_iterator b=is.begin(), e=is.end();

  while(b!=e)
  {
    typename Multipointer::MultipointersVector::const_iterator bt = b->begin(), et=b->end();
    typename Multiindex::const_iterator ib= mi.begin();

    Multipointer delta = a.subMultipointer(*bt);
    Scalar temp = second(*ib,delta);
    ++bt;
    ++ib;
    while(bt!=et)
    {
      Multipointer delta = a.subMultipointer(*bt);
      temp *= second(*ib,delta);
      ++bt;
      ++ib;
    }
    ++b;
    result += temp;
  }
  return result;
}

// -----------------------------------------------------------------

template<typename MatrixT, capd::vectalg::__size_type DEGREE>
void computeComposition(
      const capd::diffAlgebra::CnCoeff<MatrixT,DEGREE>& first,
      const capd::diffAlgebra::CnCoeff<MatrixT,DEGREE>& second,
      capd::diffAlgebra::CnCoeff<MatrixT,DEGREE>& result,
      const capd::vectalg::Multipointer& a,
      bool nonlinearOnly
   )
{
  typedef typename MatrixT::ScalarType Scalar;
  typedef typename MatrixT::size_type size_type;
  typedef capd::vectalg::Multiindex Multiindex;
  typedef capd::vectalg::Multipointer Multipointer;
  typename Multiindex::IndicesSet listIndices;   

  Multiindex::generateList(result.dimension(),result.degree(),listIndices);
  size_type p = a.module();

  result(a).clear();
  size_type mink = nonlinearOnly ? 2 : 1;
  for(size_type k=mink;k<=p;++k)
  {
    typename Multiindex::MultiindexVector::iterator b = listIndices[k-1].begin(), e=listIndices[k-1].end();
    while(b!=e)
    {
      Multipointer mp(b->dimension(),b->begin());
      std::sort(mp.begin(),mp.end());
      Scalar product = computeProduct(second,*b,a,p,k);
      result(a) += first(mp) * product;
      ++b;
    }
  }
}

// -----------------------------------------------------------------

template<typename MatrixT, capd::vectalg::__size_type DEGREE>
void substitutionPowerSeries(
      const capd::diffAlgebra::CnCoeff<MatrixT,DEGREE>& first,
      const capd::diffAlgebra::CnCoeff<MatrixT,DEGREE>& second,
      capd::diffAlgebra::CnCoeff<MatrixT,DEGREE>& result, bool nonlinearOnly
   )
// the function computes substitution of two power series truncated to the same order as 'first' and 'second'
// as a result we obtain an expansion of first(second)
{
  typedef typename MatrixT::ScalarType Scalar;
  typedef capd::vectalg::Multiindex Multiindex;
  typedef capd::vectalg::Multipointer Multipointer;
  typedef typename MatrixT::size_type size_type;

  for(size_type i=1;i<=first.degree();++i)
  {
    Multipointer a = first.first(i);
    do{
      computeComposition(first,second,result,a,nonlinearOnly);
    }while(first.hasNext(a));
  }
   
  if(!nonlinearOnly)
    result() = first();
}

#endif // _CAPD_DIFFALGEBRA_CNCOEFF_HPP_

/// @}
