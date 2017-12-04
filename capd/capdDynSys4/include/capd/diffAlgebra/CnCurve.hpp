/////////////////////////////////////////////////////////////////////////////
/// @file CnCurve.hpp
///
/// @author Daniel Wilczak
/////////////////////////////////////////////////////////////////////////////

// Copyright (C) 2000-2012 by the CAPD Group.
//
// This file constitutes a part of the CAPD library,
// distributed under the terms of the GNU General Public License.
// Consult  http://capd.ii.uj.edu.pl/ for details.

#ifndef _CAPD_DIFFALGEBRA_CNCURVE_HPP_
#define _CAPD_DIFFALGEBRA_CNCURVE_HPP_

#include <stdexcept>
#include "capd/diffAlgebra/CnCurve.h"
#include "capd/diffAlgebra/Curve.hpp"
#include "capd/diffAlgebra/BasicCnCurve.hpp"

namespace capd{
namespace diffAlgebra{

template<class CurveT, class HessianT>
void oneStepHessian(typename CurveT::ScalarType h, const CurveT& curve, HessianT& result)
{
  typedef typename CurveT::size_type size_type;
  const size_type dim=curve.dimension();
  for(size_type i=0;i<dim;++i)
    for(size_type j=0;j<dim;++j)
      for(size_type c=j;c<dim;++c){
        int p = curve.getOrder();
        result(i,j,c) = curve.coefficient(i,j,c,p);
        for(--p;p>=0;--p)
          result(i,j,c) = result(i,j,c)*h + curve.coefficient(i,j,c,p);
      }
}

template<class CurveT, class JetType>
void oneStepJet(typename CurveT::ScalarType h, const CurveT& curve, JetType& v) {
  typedef typename CurveT::size_type size_type;
  const size_type dim=curve.dimension();
  for(size_type i=0;i<dim;++i)
  {
    const typename CurveT::ScalarType* p = &curve.coefficient(i,curve.getOrder());
    typename JetType::iterator b = v.begin(i), e = v.end(i);
    for(;b!=e;++b,++p)
      *b = *p;
    for(int r = curve.getOrder()-1;r>=0;--r)
    {
      const typename CurveT::ScalarType* p = &curve.coefficient(i,r);
      typename JetType::iterator b = v.begin(i), e = v.end(i);
      for(;b!=e;++b,++p)
        *b = (*b)*h + (*p);
    }
  }
}

template <class BaseCurveT, bool isInterval >
typename CnCurve<BaseCurveT,isInterval>::HessianType CnCurve<BaseCurveT,isInterval>::hessian(const ScalarType& h) const
{
  if((h>=this->m_left) and (h<=this->m_right))
  {
    HessianType result(this->dimension());
    oneStepHessian(h,*this,result);
    return result;
  }
  throw std::domain_error("capd::diffAlgebra::CnCurve::hessian(t) error - argument t is out of domain");
}

template <class BaseCurveT>
typename CnCurve<BaseCurveT,true>::HessianType CnCurve<BaseCurveT,true>::hessian(const ScalarType& h) const
{
  const size_type dim=this->dimension();
  if((h>=this->m_left) and (h<=this->m_right))
  {
    HessianType result(this->dimension());
    oneStepHessian(h,*this,result);
    ScalarType c = power(h,this->getOrder()+1);
    for(size_type i=0;i<dim;++i)
      for(size_type j=0;j<dim;++j)
        for(size_type c=j;c<dim;++c)
          result(i,j,c) += c*this->remainderCoefficient(i,j,c,this->getOrder()+1);

    return result;
  }
  throw std::domain_error("capd::diffAlgebra::CnCurve::hessian(t) error - argument t is out of domain");
}

///@}
}} // namespace capd::diffAlgebra

#endif // _CAPD_DIFFALGEBRA_CNCURVE_H_
