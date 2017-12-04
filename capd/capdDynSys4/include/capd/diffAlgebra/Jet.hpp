/// @addtogroup diffAlgebra
/// @{

/////////////////////////////////////////////////////////////////////////////
/// @file Jet.hpp
///
/// @author Daniel Wilczak
/////////////////////////////////////////////////////////////////////////////

// Copyright (C) 2000-2012 by the CAPD Group.
//
// This file constitutes a part of the CAPD library,
// distributed under the terms of the GNU General Public License.
// Consult  http://capd.ii.uj.edu.pl/ for details.

#ifndef _CAPD_DIFFALGEBRA_JET_HPP_
#define _CAPD_DIFFALGEBRA_JET_HPP_

#include <algorithm>
#include <sstream>
#include "capd/diffAlgebra/Jet.h"
#include "capd/basicalg/TypeTraits.h"

namespace capd{
namespace diffAlgebra{

// -----------------------------------------------------------------

template<typename MatrixT, __size_type DEGREE>
Jet<MatrixT,DEGREE>::operator typename Jet<MatrixT,DEGREE>::ImageVectorType() const
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
Jet<MatrixT,DEGREE>::operator typename Jet<MatrixT,DEGREE>::MatrixType() const
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

// -----------------------------------------------------------------

template<typename MatrixT, __size_type DEGREE>
void Jet<MatrixT,DEGREE>::setMatrix(const MatrixType& m)
{
  typename MatrixType::const_iterator b=m.begin(), e=m.end();
  size_type i=0;
  while(b!=e)
  {
    iterator b1=begin(i,1), e1=end(i,1);
    while(b1!=e1)
    {
      *b1 = *b;
      ++b;
      ++b1;
    }
    ++i;
  }
}

// -----------------------------------------------------------------

template<typename MatrixT, __size_type DEGREE>
typename Jet<MatrixT,DEGREE>::ImageVectorType Jet<MatrixT,DEGREE>::operator()(const VectorType& v) const
{
  ImageVectorType result = ImageVectorType(*this);

  for(size_type i=1;i<=this->degree();++i)
  {
    Multipointer mp = this->first(i);
    do{
      typename ImageVectorType::ScalarType temp = power(v,mp);
      result += temp* (*this)(mp);
    }while(this->hasNext(mp));
  }
  return result;
}

// ----------------------------------------------------------

template<typename MatrixT, __size_type DEGREE>
std::ostream & print(
    std::ostream & str, const Jet<MatrixT,DEGREE> & coeff,
    int minDegree /*= 0*/, int maxDegree /*= -1*/,
    int firstFun /*= 0*/, int lastFun /*= -1*/,
    int firstVariable /*= 0*/
){
  typedef typename Jet<MatrixT,DEGREE>::Multiindex Multiindex;
  typedef typename Jet<MatrixT,DEGREE>::Multipointer Multipointer;
  if(lastFun<0) lastFun=coeff.imageDimension();
  if(maxDegree<0) maxDegree=coeff.degree();
  for(int r=minDegree; r<= maxDegree; ++r){
    if(r)
      str << "\nTaylor coefficients of order " << r << " :";
    else
      str << "\nvalue :";
    Multiindex index(coeff.dimension());
    index[firstVariable] = r;
    Multipointer mp(index);
    do{
      str << "\n   "<< Multiindex(coeff.dimension(), mp) << "  : ";
      capd::vectalg::printVector(str, coeff(mp), firstFun, lastFun) ;
    }while(coeff.hasNext(mp));
  }
  str << std::endl;
  return str;
}

/**
 * Return derivative information in human readable form
 *
 * You can specify which derivatives you want,
 * It will return all derivatives
 * \f$ df_i/dx^a \f$
 * where
 *   i = firstFun,..., lastFun
 *   |a| = minDegree, ..., maxDegree
 *
 * @param firstFun index of the first function
 * @param lastFun index of the last function
 * @param minDegree minimal degree displayed
 * @param maxDegree maximal degree displayed
 * @return string containing derivatives information
 */
template<typename MatrixT, __size_type DEGREE>
std::string Jet<MatrixT,DEGREE>::toString(
       int firstFun /*= 0*/, int lastFun /*= -1*/,
       int firstVariable /*= 0*/,
       int minDegree /*= 0*/, int maxDegree /*= -1*/,
       int precision /* =15*/) const {
  std::ostringstream str;
  print(str, *this, minDegree, maxDegree, firstFun, lastFun, firstVariable);
  return str.str();
}

/**
 *  saves data to stream out with given precision
 *
 * - the default precision allows to load data without lose of precision,
 * - if given precision \b prec is less or equal to 0 the current precision is used
 *
 * Format:
 * \code
 *   degree (order of the highest derivative)
 *   dimension
 *   imageDimension
 *
 *   coefficient in the order as they are stored in the memory
 *   each line contains one coefficient
 * \endcode
 */
template<typename MatrixT, __size_type DEGREE>
std::ostream &  Jet<MatrixT,DEGREE>::save(
      std::ostream & out,
      std::streamsize prec /* = capd::TypeTraits<ScalarType>::numberOfDigits() + 1 */
  ) const {
    std::streamsize oldPrecision = out.precision();
    if(prec > 0){
      out.precision(prec);
    }
    out << this->degree() << "\n" << this->dimension() << "\n" << this->imageDimension()
        << "\n\n";
     const_iterator it = this->begin(), end_it = this->end();
     while(it != end_it) {
       out << *it << "\n";
       it++;
     }
     if(prec > 0){
       out.precision(oldPrecision);
     }
     out << std::endl;
     return out;
  }

/// loads cn data from given stream
/// (expected format is exactly this provided by save)
template<typename MatrixT, __size_type DEGREE>
std::istream & Jet<MatrixT,DEGREE>::load(std::istream & in){

  size_type degree, dimension, imageDimension;
  in >> degree >> dimension >> imageDimension;

  if((this->degree() != degree) || (this->dimension() != dimension) || (this->imageDimension() != imageDimension)) {
    *this = Jet(imageDimension,dimension,degree);
  }
  iterator it = this->begin();
  while(it != this->end()) {
    in >> *it;
    it++;
    if(!in) {
      throw std::ios_base::failure(" Error when loading Jet from stream ");
    }
  }
  return in;
}

}} // the end of the namespace capd::diffAlgebra

// -----------------------------------------------------------------

template<typename MatrixT, capd::vectalg::__size_type DEGREE>
capd::diffAlgebra::Jet<MatrixT,DEGREE>
operator+(const capd::diffAlgebra::Jet<MatrixT,DEGREE>& x, const capd::diffAlgebra::Jet<MatrixT,DEGREE>& y)
{
  if(x.dimension()!=y.dimension() or x.imageDimension()!=y.imageDimension())
    throw std::runtime_error("Jet operator+: incompatible dimensions of power series");
  capd::diffAlgebra::Jet<MatrixT,DEGREE> result(x);
  typename capd::diffAlgebra::Jet<MatrixT,DEGREE>::iterator i = result.begin();
  typename capd::diffAlgebra::Jet<MatrixT,DEGREE>::const_iterator b = y.begin(), e=y.end();
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
capd::diffAlgebra::Jet<MatrixT,DEGREE>
operator-(const capd::diffAlgebra::Jet<MatrixT,DEGREE>& x, const capd::diffAlgebra::Jet<MatrixT,DEGREE>& y)
{
  if(x.dimension()!=y.dimension() or x.imageDimension()!=y.imageDimension())
    throw std::runtime_error("Jet operator-: incompatible dimensions of power series");
  capd::diffAlgebra::Jet<MatrixT,DEGREE> result(x);
  typename capd::diffAlgebra::Jet<MatrixT,DEGREE>::iterator i = result.begin();
  typename capd::diffAlgebra::Jet<MatrixT,DEGREE>::const_iterator b = y.begin(), e=y.end();
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
      const capd::diffAlgebra::Jet<MatrixT,DEGREE>& second,
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
    Scalar temp = second(*ib,delta)* delta.factorial();
    ++bt;
    ++ib;
    while(bt!=et)
    {
      Multipointer delta = a.subMultipointer(*bt);
      temp *= second(*ib,delta) * delta.factorial();
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
      const capd::diffAlgebra::Jet<MatrixT,DEGREE>& first,
      const capd::diffAlgebra::Jet<MatrixT,DEGREE>& second,
      capd::diffAlgebra::Jet<MatrixT,DEGREE>& result,
      const capd::vectalg::Multipointer& a,
      bool nonlinearOnly
   )
{
  typedef typename MatrixT::ScalarType Scalar;
  typedef capd::vectalg::Multiindex Multiindex;
  typedef capd::vectalg::Multipointer Multipointer;
  typename Multiindex::IndicesSet listIndices;

  Multiindex::generateList(result.dimension(),result.degree(),listIndices);
  int p = a.module();

  result(a).clear();
  int mink = nonlinearOnly ? 2 : 1;
  for(int k=mink;k<=p;++k)
  {
    typename Multiindex::MultiindexVector::iterator b = listIndices[k-1].begin(), e=listIndices[k-1].end();
    while(b!=e)
    {
      Multipointer mp(b->dimension(),b->begin());
      std::sort(mp.begin(),mp.end());
      Scalar product = computeProduct(second,*b,a,p,k);
      result(a) += first(mp) * product * mp.factorial();
      ++b;
    }
  }
  result(a)/=a.factorial();
}

// -----------------------------------------------------------------

template<typename MatrixT, capd::vectalg::__size_type DEGREE>
void substitutionPowerSeries(
      const capd::diffAlgebra::Jet<MatrixT,DEGREE>& first,
      const capd::diffAlgebra::Jet<MatrixT,DEGREE>& second,
      capd::diffAlgebra::Jet<MatrixT,DEGREE>& result, bool nonlinearOnly
   )
// the function computes substitution of two power series truncated to the same order as 'first' and 'second'
// as a result we obtain an expansion of first(second)
{
  typedef typename MatrixT::ScalarType Scalar;
  typedef capd::vectalg::Multiindex Multiindex;
  typedef capd::vectalg::Multipointer Multipointer;

  for(unsigned i=1;i<=first.degree();++i)
  {
    Multipointer a = first.first(i);
    do{
      computeComposition(first,second,result,a,nonlinearOnly);
    }while(first.hasNext(a));
  }

  if(!nonlinearOnly)
    result() = first();
}

#endif // _CAPD_DIFFALGEBRA_JET_HPP_

/// @}
