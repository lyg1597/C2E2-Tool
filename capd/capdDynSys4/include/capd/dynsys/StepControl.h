/// @addtogroup dynsys
/// @{

/////////////////////////////////////////////////////////////////////////////
/// @file StepControl.h
///
/// @author Tomasz Kapela, Daniel Wilczak
/////////////////////////////////////////////////////////////////////////////

// Copyright (C) 2000-2012 by the CAPD Group.
//
// This file constitutes a part of the CAPD library,
// distributed under the terms of the GNU General Public License.
// Consult  http://capd.ii.uj.edu.pl/ for details.

#ifndef _CAPD_DYNSYS_STEP_CONTROL_H_
#define _CAPD_DYNSYS_STEP_CONTROL_H_

#include <cmath>
#include "capd/auxil/logger.h"
#include "capd/auxil/minmax.h"
#include "capd/basicalg/TypeTraits.h"
#include "capd/rounding/DoubleRounding.h"
#include "capd/basicalg/power.h"
#include "capd/vectalg/Norm.hpp"
#include "capd/vectalg/iobject.hpp"

namespace capd {
namespace dynsys {

/// the following function clears the last bits of mantissa
///
template <class Real>
Real clearMantissaBits(Real step, int maxValue = 32) {
  //capd::rounding::DoubleRounding::roundNearest();
  // fill the mantissa by zeroes
  double p = toDouble(step);
  int exponent;
  p = frexp(p,&exponent);
  p*= maxValue;
  p = static_cast<int>(p);
  p /= maxValue;
  step  = ldexp(p,exponent);
  return step;
}

// ---------------------------------------------------------------------
/// This class is a common interface for StepControl
/// used in PoincareMap and TimeMap.
/// Both classes inherit this interface
template <class StepControlT, class Scalar>
class StepControlInterface {
public:
  typedef StepControlT StepControlType;
  StepControlInterface()
    : m_onOffStepControl(true),
      m_absoluteTolerance(1.e-18),
      m_relativeTolerance(1.e-18),
      m_maxStep(1.e+100)
  {}

  StepControlInterface(const StepControlType& stepControl) :
    m_stepControl(stepControl),
    m_onOffStepControl(true),
    m_absoluteTolerance(1.e-18),
    m_relativeTolerance(1.e-18),
    m_maxStep(1.e+100)
  {}

  void turnOnStepControl() {
    m_onOffStepControl = true;
  }

  void turnOffStepControl() {
    m_onOffStepControl = false;
  }

  void onOffStepControl(bool _onOffStepControl)
  {
    this->m_onOffStepControl = _onOffStepControl;
  }

  const StepControlType& getStepControl() const {
    return m_stepControl;
  }

  void setStepControl(const StepControlType& stepControl) {
    m_stepControl = stepControl;
  }

  bool isStepChangeAllowed() const
  {
    return m_onOffStepControl;
  }

  void setAbsoluteTolerance(double tol){
    m_absoluteTolerance = fabs(tol);
  }

  void setRelativeTolerance(double tol){
    m_relativeTolerance = fabs(tol);
  }

  double getAbsoluteTolerance() const{
    return m_absoluteTolerance;
  }

  double getRelativeTolerance() const{
    return m_relativeTolerance;
  }

  Scalar getMaxStep() const{
    return m_maxStep;
  }

  void setMaxStep(Scalar maxStep){
    m_maxStep = maxStep;
  }

protected:
  StepControlType m_stepControl;
  bool m_onOffStepControl;
  double m_absoluteTolerance, m_relativeTolerance;
  Scalar m_maxStep;
};


// ---------------------------------------------------------------------
template <class StepControlT>
class NoStepControlInterface{
public:
  typedef StepControlT StepControlType;
  NoStepControlInterface(){
  }
  NoStepControlInterface(const StepControlType& stepControl) :
    m_stepControl(stepControl){
  }

  void turnOnStepControl() {
  }

  void turnOffStepControl() {
  }

  const StepControlType& getStepControl() const {
    return m_stepControl;
  }

  void setStepControl(const StepControlType& stepControl) {
    m_stepControl = stepControl;
  }

  bool isStepChangeAllowed() const
  {
    return false;
  }

  ///
  void onOffStepControl(bool _onOffStepControl){
  }

  void setAbsoluteTolerance(double tol){
  }

  void setRelativeTolerance(double tol){
  }
protected:
  StepControlType m_stepControl;
};

// ---------------------------------------------------------------------
/// This class provides an empty time step control for the solutions to ODEs.
/// It contains an interface for other implementations of TSC.
class NoStepControl {
public:

  template <class DS, class SetType>
  typename DS::ScalarType computeNextTimeStep(DS& dynamicalSystem, typename DS::ScalarType const& t, const SetType&, const typename DS::ScalarType& maxStep) const{
    return capd::min(dynamicalSystem.getStep(),maxStep);
  }

  template <class DS,class SetType>
  typename DS::ScalarType getFirstTimeStep(DS& dynamicalSystem, const SetType&, typename DS::ScalarType const& t, const typename DS::ScalarType& maxStep) const {
    return capd::min(dynamicalSystem.getStep(),maxStep);
  }

  double getMinStepAllowed() const{ // here it does not matter what is the minimal time step
    return 1e-20;
  }
  template <class DS,class SetType>
  void init(DS& dynamicalSystem, typename DS::ScalarType const& t, const SetType&) const {
  }
};

// ---------------------------------------------------------------------
template <>
class StepControlInterface<NoStepControl,double> : public NoStepControlInterface<NoStepControl>{
public:
  typedef NoStepControl StepControlType;
  StepControlInterface(){
  }
  StepControlInterface(const StepControlType& stepControl) :
    NoStepControlInterface<NoStepControl>(stepControl){
  }

};

// ---------------------------------------------------------------------

///
template <typename ScalarType>
class FixedStepControl {
public:
  explicit FixedStepControl(const ScalarType & timeStep = ScalarType(1./1024.)) : m_timeStep(timeStep){
  }
  template <class DS, class SetType>
  typename DS::ScalarType computeNextTimeStep(DS& dynamicalSystem, typename DS::ScalarType const& t, const SetType&, const typename DS::ScalarType& maxStep) const{
    return capd::min(m_timeStep, maxStep);
  }

  template <class DS,class SetType>
  void init(DS& dynamicalSystem, typename DS::ScalarType const& t, const SetType&) const {
  }

  double getMinStepAllowed() const { // here it does not matter what is the minimal time step
    return 1e-20;
  }

private:
  ScalarType m_timeStep;
};

///////////////////////////////////////////////////////////////////////////////////
///
/// Computes next time step using already computed Taylor series of the solution.
///
/// We choose time step so that expected error of the next step 
/// of the ODE integration is less than epsilon.
///
/// @remark For non rigorous computations it is good to take more that one term
///         to prevent big time steps when the last term happens to be close to zero 
///
///////////////////////////////////////////////////////////////////////////////////
template<class Solver>
typename Solver::ScalarType
computeNextStep(
    Solver& solver,                               ///< ODE solver containing Taylor series of the solution for current step
    int numberOfTerms,                            ///< number of terms in Taylor series to use in predictions
    const typename TypeTraits<typename Solver::ScalarType>::Real& epsilon,     ///< expected error for one step of integration
    const typename TypeTraits<typename Solver::ScalarType>::Real& minTimeStep, ///< minimal time step allowed
    int degree
) {
  typedef typename Solver::ScalarType ScalarType;
  typedef typename TypeTraits<ScalarType>::Real Real;
  int order = solver.getOrder();
  Real optStep = 1.5*rightBound(solver.getMaxStep());

  for(int i = order; (i > order - numberOfTerms) && (i > 0); i--) {
    Real coeffNorm = rightBound(solver.getCoeffNorm(i,degree));
    capd::rounding::DoubleRounding::roundNearest();
    if(isSingular(coeffNorm)) continue;
    Real errorTerm = epsilon/ coeffNorm;
    Real step = exp(log(errorTerm)/i);
    optStep = capd::min(optStep, step);
  }

  optStep = capd::max(optStep, minTimeStep);
  optStep = clearMantissaBits(optStep);

  // note: solver.getMaxStep() might be a desired nondegenerated interval! Do not change it to leftBound.
  return optStep < solver.getMaxStep() ? ScalarType(optStep) :  solver.getMaxStep();
}

class ILastTermsStepControl {
public:
  typedef double Real;

  explicit ILastTermsStepControl(
      int _terms = 1,
      Real _minStep = 1. / 1048576
    ) : m_numberOfTerms(_terms),
        m_minTimeStep(_minStep)
  {}

  template <class Solver, class SetType>
  typename Solver::ScalarType computeNextTimeStep(Solver& solver, typename Solver::ScalarType const& t, const SetType& s) const {
    typedef typename Solver::VectorType VectorType;
    typedef typename Solver::MatrixType MatrixType;

    return computeNextStep(
        solver,
        m_numberOfTerms,
        capd::min(solver.getAbsoluteTolerance(),solver.getRelativeTolerance()/(1.+solver.getRelativeTolerance())*toDouble(rightBound(solver.getCoeffNorm(0,s.degree())))),
        m_minTimeStep,
        s.degree()
     );
  }

  template <class Solver, class SetType>
  void init(Solver& solver, typename Solver::ScalarType const& t, const SetType& s) const {
    typedef typename Solver::ScalarType ScalarType;
    typedef typename Solver::VectorType VectorType;
    typedef typename Solver::MatrixType MatrixType;
    typedef typename TypeTraits<ScalarType>::Real Float;

    VectorType x(s);

    ScalarType currentTime = s.getCurrentTime();
    MatrixType df = solver.getVectorField().derivative(currentTime,x);
    capd::vectalg::EuclNorm<VectorType,MatrixType> N;
    Float lipConstant = rightBound(N(df));
    lipConstant = isSingular(lipConstant) ? Float(1.) : Float(1.)/lipConstant;
    Float h = capd::min(lipConstant,Float(1.));
    h = capd::min(h,solver.getMaxStep().leftBound());
    ScalarType I(Float(0.),h);
    x = x+I*solver.getVectorField()(currentTime+I,x);
    solver.computeRemainderCoefficients(currentTime+I,x);
  }

  Real getMinStepAllowed() const
  {
    return m_minTimeStep;
  }

  int m_numberOfTerms;
  Real m_minTimeStep;
};

// ---------------------------------------------------------------------

class DLastTermsStepControl {
public:
  typedef double Real;

  explicit DLastTermsStepControl(int _terms = 2, Real _tolerance = power(10, -TypeTraits<Real>::numberOfDigits()-3), Real _minStep = 1. / 1048576.) :
    m_numberOfTerms(_terms), m_epsilon(_tolerance), m_minTimeStep(_minStep) {
  }

  template <class Solver, class SetType>
  typename Solver::ScalarType computeNextTimeStep(Solver& solver, typename Solver::ScalarType const& t, const SetType& s) const {
    double tol = capd::min(solver.getAbsoluteTolerance(),solver.getRelativeTolerance()/(1.+solver.getRelativeTolerance())*toDouble(rightBound(solver.getCoeffNorm(0,s.degree()))));

    return computeNextStep(solver,m_numberOfTerms, tol, m_minTimeStep,s.degree());
  }

  template <class Solver,class SetType>
  void init(Solver& solver, typename Solver::ScalarType const& t, const SetType&) const {
    solver.clearCoefficients();
  }

  Real getMinStepAllowed() const
  {
    return m_minTimeStep;
  }

  int m_numberOfTerms;
  Real m_epsilon;
  Real m_minTimeStep;
};

//////////////////////////////////////////////////////////////////////////////////
///  MpLastTermsStepControl
/// 
///  Time stepper for non-rigorous computations with multiple precision
///
//////////////////////////////////////////////////////////////////////////////////
template <typename T>
class MpLastTermsStepControl {
public:
  typedef T Real;
  explicit MpLastTermsStepControl(
  	int _terms = 3,                                                      ///< number of terms to use in the estimations
  	Real _tolerance = power(10, -TypeTraits<Real>::numberOfDigits()-4),  /// expected error of the one step of integration
  	Real _minStep = 1. / 1048576.                                        ///< minimal allowed time step
  ) 
  : m_numberOfTerms(_terms), m_epsilon(_tolerance), m_minTimeStep(_minStep) {
  }

  template <class DS, class SetType>
  typename DS::ScalarType computeNextTimeStep(DS& dynamicalSystem, typename DS::ScalarType const& t, const SetType&) const {
    typename DS::ScalarType h =  computeNextStep(dynamicalSystem,m_numberOfTerms, m_epsilon, m_minTimeStep,0);
    return h;
  }

  template <class DS,class SetType>
  void init(DS& dynamicalSystem, typename DS::ScalarType const& t, const SetType&) const {
  }

  Real getMinStepAllowed() const {
    return m_minTimeStep;
  }
protected:
  template <class DS>
  Real getLipschitzEstimates(DS& dynamicalSystem, typename DS::ScalarType const& t, const typename DS::VectorType & x,  const typename DS::ScalarType& maxStep) const {

    typedef typename DS::VectorType VectorType;
    typedef typename DS::MatrixType MatrixType;

    MatrixType df = dynamicalSystem.getVectorField().derivative(t,x);
    capd::vectalg::EuclNorm<VectorType,MatrixType> norm;
    Real lipConstant = rightBound(norm(df));
    Real h = capd::min(Real(1.0)/lipConstant, leftBound(maxStep));
    return h;
  }

  int m_numberOfTerms;
  Real m_epsilon;
  Real m_minTimeStep;
};

//////////////////////////////////////////////////////////////////////////////////
///  IMpLastTermsStepControl
///
///  Time stepper for rigorous computations with multiple precision
///
//////////////////////////////////////////////////////////////////////////////////
template <typename T>
class IMpLastTermsStepControl : public MpLastTermsStepControl< typename TypeTraits<T>::Real >{
public:
  typedef T ScalarType;
  typedef typename TypeTraits<T>::Real Real;
  typedef MpLastTermsStepControl<Real> BaseType;

  explicit IMpLastTermsStepControl(
      int  numberOfTerms = 1,                                      ///< number of terms in Taylor series to use in preditions
      Real errorTolerance = TypeTraits<Real>::epsilon()/1.0e4,     ///< expected error for one step of integration
      //Real errorTolerance = power(10, -TypeTraits<Real>::numberOfDigits()-3),  ///< expected error for one step of integration
      Real minStep = 1. / 1048576.                                 ///< minimal time step allowed
  )
  :  BaseType(numberOfTerms, errorTolerance, minStep) {
  }

  /// Computes first time step
  template <class DS, class SetType>
  typename DS::ScalarType getFirstTimeStep(
      DS& dynamicalSystem,                     ///< ODE integrator
      typename DS::ScalarType const& t,        ///< current time (for nonautonomous systems)
      const SetType& s,                        ///< initial condition
      const typename DS::ScalarType & maxStep  ///< maximal allowed time step
  ) const {

    typedef typename DS::VectorType VectorType;
    typedef typename DS::ScalarType ScalarType;

    VectorType x = VectorType(s);
    Real h = this->getLipschitzEstimates(dynamicalSystem, t, x, maxStep);
    dynamicalSystem.setStep(h);
    x += ScalarType(Real(0.), h) * dynamicalSystem.getVectorField()(t,x);
    x = dynamicalSystem(t,x);
    ++this->m_numberOfTerms;
    ScalarType firstStep = this->computeNextTimeStep(dynamicalSystem, s, maxStep);
    --this->m_numberOfTerms;
    return firstStep;
  }
  using BaseType::computeNextTimeStep;
};

class IEncFoundStepControl {
public:
  typedef double Real;
  /**
   * Constructor
   * @param minStep     minimal allowed step size
   * @param stepFactor  fraction of maximal possible time step to be taken as optimal time step (optimal = stepFactor * maximal)
   */
  explicit IEncFoundStepControl(Real minStep = 1. / 1048576., Real stepFactor = 0.25) :
    m_minTimeStep(minStep), m_stepFactor(stepFactor) {
  }

  template <class DS, class SetType>
  typename DS::ScalarType computeNextTimeStep(DS& dynamicalSystem, typename DS::ScalarType const& t,const SetType& _x) const {
    typedef typename DS::ScalarType ScalarType;
    typename DS::VectorType x(_x);
    Real setSize = toDouble(maxDiam(x).rightBound());
    Real eps = dynamicalSystem.getAbsoluteTolerance() + dynamicalSystem.getRelativeTolerance()*setSize;
    double factor =  capd::min(Real(1.),dynamicalSystem.getOrder()/-log(eps));
    ScalarType optStep = dynamicalSystem.getStep()/factor * Real(1.5);
    dynamicalSystem.setStep(optStep);
    while(optStep >= m_minTimeStep) {
      try {
        dynamicalSystem.enclosure(t,x);
        // if succeed then break
        break;
      } catch(...) {
        optStep = (optStep*Real(0.8)).leftBound();
        dynamicalSystem.setStep(optStep);
      }
    }
    double result = toDouble(rightBound(optStep)) * factor;
    result = clearMantissaBits(result);
    result = capd::max(result,m_minTimeStep);

    // note: dynamicalSystem.getMaxStep() might be a desired nondegenerated interval! Do not change it to leftBound.
    return result < dynamicalSystem.getMaxStep() ? ScalarType(result) : dynamicalSystem.getMaxStep();
  }

  template <class DS,class SetType>
  void init(DS& dynamicalSystem, typename DS::ScalarType const& t, const SetType&) const {
  }

  Real getMinStepAllowed() const
  {
    return m_minTimeStep;
  }

  Real m_minTimeStep;
  Real m_stepFactor;  // what part of maximal possible step take as optimal one
};


// ---------------------------------------------------------------------

}} // namespace capd::dynsys


#endif // #define _CAPD_POINCARE_STEP_CONTROL_H_
/// @}
