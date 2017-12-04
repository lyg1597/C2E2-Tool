/// @addtogroup poincare
/// @{

/////////////////////////////////////////////////////////////////////////////
/// @file BasicPoincareMap.h
///
/// @author Daniel Wilczak
/// @author Tomasz Kapela
/////////////////////////////////////////////////////////////////////////////

// Copyright (C) 2000-2013 by the CAPD Group.
//
// Distributed under the terms of the GNU General Public License.
// Consult  http://capd.ii.uj.edu.pl/ for details.

#ifndef _CAPD_POINCARE_BASIC_POINCARE_MAP_H_
#define _CAPD_POINCARE_BASIC_POINCARE_MAP_H_

#include <string>
#include "capd/basicalg/factrial.h"
#include "capd/basicalg/TypeTraits.h"
#include "capd/diffAlgebra/C2Coeff.h"
#include "capd/diffAlgebra/SolutionCurve.h"
#include "capd/poincare/SaveStepControl.h"

namespace capd{
namespace poincare{

/**
 * Defines constants to specify section crossing direction
 *
 * Section is given zeros of some scalar function.
 * PlusMinus means that we want to cross section comming from positive values
 * and going to negative ones (MinusPlus means opposite direction).
 * Constant None means that we do not specify direction, both are allowed.
 */
enum CrossingDirection { PlusMinus = 1, Both = 0, MinusPlus = -1};

/**
 *  BasicPoicareMap class is mainly used for non-rigorous computations of Poincare Map.
 *  For given Dynamical System and section it computes first return map
 *  and its derivatives but non-rigorously (i.e. without handling any computational errors).
 *
 *  Let \f$ \varphi(t, x): R \times R^n -> R^n \f$ be a dynamical system
 *  Let \f$ s: R^n \to R \f$ be a section function,
 *  Let \f$ S = \{x \in R^n : s(x) = 0\} \f$
 *  Let \f$ P: S \to S \f$ be a Poincare Map
 *  For given point \f$ x \in S \f$ let T(x) be first return time (in given crossing direction)
 *  i.e.  \f$ P(x) = \varphi(T(x), x) \in S \f$
 *
 *  In the following we denote by
 *    - dP the derivative of Poincare Map P  : \f$ dP(x) = \frac{\partial P(x)}{\partial x} \f$
 *    - dT the derivative of T(x) : \f$ dT(x)  = \frac{\partial T(x)}{\partial x} \f$
 *    - dF the derivative of the flow : \f$ dF(x) = \frac{\partial \varphi(T(x), x)}{\partial x} \f$
 *  Then
 *    \f$ dP = dF + \frac{\partial \varphi}{\partial t} dT \f$
 *
 *  Parameters:
 *   - SolverT    ODE solver
 *       - SolverT::MapType  vector field type
 *   - FunT  - section function type
 *       - FunT should have method gradient()
 *
 */

template <typename SolverT, typename FunT = typename SolverT::FunctionType >
class BasicPoincareMap
{
public:
  typedef SolverT Solver;                                       ///< ODE solver type
  typedef typename Solver::MapType VectorFieldType;             ///< vector field type
  typedef typename Solver::MatrixType MatrixType;               ///< matrix type
  typedef typename Solver::VectorType VectorType;               ///< vector type
  typedef typename Solver::ScalarType ScalarType;               ///< scalar entries type
  typedef typename VectorType::size_type size_type;             ///< integral type used to index containers (vectors, matrices, etc)
  typedef typename TypeTraits<ScalarType>::Real RealType;       ///< floating point type: for floating points is equal to ScalarType, for intervals is a type of their bounds.
  typedef typename Solver::SolutionCurve CurveType;
  typedef capd::diffAlgebra::SolutionCurve<CurveType> SolutionCurve;
  typedef typename CurveType::HessianType HessianType;        ///< data structure for storing Hessians
  typedef  FunT FunctionType;                                   ///< Section function type
  typedef capd::poincare::CrossingDirection CrossingDirection;  ///< specifies direction of crossing of Poincare section (plus to minus, minus to plus or both)

  /// Constructor
  BasicPoincareMap(
             Solver& solver,
             FunctionType & section,
             CrossingDirection direction = Both,
             const RealType & errorTolerance = pow(TypeTraits<RealType>::epsilon(), RealType(14.)/RealType(15.))
        );

  /// Computes Poincare Map and derivatives of the flow to given order
  template <typename CoeffType>
  CoeffType& operator()(CoeffType& result);

  /// Computes value of Poincare Map.
  /// If in_out_time is not null, then:
  /// - the value is t0 = *in_out_time is used to initialize current time (for nonautonomous systems)
  /// - on leave the value *in_out_time contains the time for which \f$ \phi(v,T) $ \f is on the section.
  /// Thus, for autonomous systems this is exactly return time, while for nonautonomous systems T-t0 is time of integration to reach the section.
  VectorType operator()(VectorType v);
  VectorType operator()(const VectorType& v, ScalarType& in_out_time);

  /// Computes value of Poincare Map and point after the section suitable for successive iterations of Poincare Map
  VectorType operator()(const VectorType& v, VectorType& afterSection);
  /// Computes value of Poincare Map and point after the section suitable for successive iterations of Poincare Map.
  /// The last argument contains an approximate return time.
  VectorType operator()(const VectorType& v, VectorType& afterSection, ScalarType& in_out_time);

  /// Computes value of Poincare Map and derivatives of the flow
  VectorType operator()(const VectorType& v, MatrixType& dF);
  VectorType operator()(const VectorType& v, MatrixType& dF, ScalarType& in_out_time);

  /// Computes value of Poincare Map and derivatives of the flow
  VectorType operator()(const VectorType& v, MatrixType& dF, HessianType& h);
  VectorType operator()(const VectorType& v, MatrixType& dF, HessianType& h, ScalarType& in_out_time);

  /// Computes gradient of return time
  /// @param[in] Px - value of Poincare map
  /// @param[in] derivativeOfFlow - solution to first variational equation computed at return time
  /// @param[in] returnTime - return time
  /// @note returnTime must be specified for nonautonomous flows only. Otherwise, default value 0 is valid.
  VectorType computeDT(const VectorType& Px, const MatrixType& derivativeOfFlow, ScalarType returnTime = TypeTraits<ScalarType>::zero());

  /// Simultaneous computation of gradient of return time and derivative of Poincare Map dP.
  /// @param[in] Px - value of Poincare map
  /// @param[in] derivativeOfFlow - solution to first variational equation computed at return time
  /// @param[out] dT - computed gradient of return time
  /// @param[in] returnTime - enclosure of return time
  /// @note returnTime must be specified for nonautonomous flows only. Otherwise, default value 0 is valid.
  MatrixType computeDP(const VectorType& Px, const MatrixType& derivativeOfFlow, VectorType& dT, ScalarType returnTime = TypeTraits<ScalarType>::zero());

  /// Computes derivative of Poincare Map dP.
  /// @param[in] Px - value of Poincare map
  /// @param[in] derivativeOfFlow - solution to first variational equation computed at return time
  /// @note returnTime must be specified for nonautonomous flows only. Otherwise, default value 0 is valid.
  MatrixType computeDP(const VectorType& Px, const MatrixType& derivativeOfFlow, ScalarType returnTime = TypeTraits<ScalarType>::zero());

  /// Simultaneous computation of first and second Taylor coefficients of return time and Poincare map.
  /// @param[in] Px - value of Poincare map
  /// @param[in] derivativeOfFlow - solution to first variational equation computed at return time
  /// @param[in] hessianOfFlow - solution to first variational equation computed at return time
  /// @param[out] DP - computed derivative of Poincare map
  /// @param[out] D2P - computed second order Taylor coefficients of Poincare map
  /// @param[out] dT - computed gradient of return time
  /// @param[out] d2T - computed second order Taylor coefficients of return time
  /// @param[in] returnTime - return time to the section
  /// @note all input and output parameters are Taylor coefficients, not derivatives!
  /// @note returnTime must be specified for nonautonomous flows only. Otherwise, default value 0 is valid.
  void computeDP(
           const VectorType& Px,
           const MatrixType& derivativeOfFlow,
           const HessianType& hessianOfFlow,
           MatrixType& DP,
           HessianType& D2P,
           VectorType& dT,
           MatrixType& d2T,
           ScalarType returnTime = TypeTraits<ScalarType>::zero()
      );

  /// Recomputes Taylor expansion of the flow into Taylor expansion of Poincare map.
  /// @note Valid only for linear sections!
  template<typename CnCoeffType>
  CnCoeffType computePartialDerivatives(const CnCoeffType& DPhi);

  const Solver & getSolver() const;    ///< Returns read-only reference to solver used to integrate the system
  Solver & getSolver();                ///< Returns reference to solver used to integrate the system

  /// @deprecated
  const Solver& getDynamicalSystem() const;    ///< Returns read-only reference to solver used to integrate the system
  /// @deprecated
  Solver & getDynamicalSystem();                ///< Returns reference to solver used to integrate the system

  VectorFieldType& getVectorField();               ///< Returns vector field
  const VectorFieldType& getVectorField() const;   ///< Returns vector field

  FunctionType& getSection();                ///< Returns section function
  const FunctionType& getSection() const;    ///< Returns section function

  size_type getOrder() const;                      ///< Returns order of numerical method used
  ScalarType getStep() const;                ///< Returns time step

  void setOrder(size_type newOrder);                      ///< Sets order of Taylor method
  void setSection(const FunctionType& newSection);  ///< Sets new section function
  void setStep(const ScalarType& newStep);          ///< Sets time step for integration of DS
  void setFactor(double newFactor);           ///

  void setCrossingDirection(CrossingDirection cs) { m_crossingDirection=cs; }

  void turnOnStepControl();                 ///< Disables automatic step control
  void turnOffStepControl();                ///< Enables automatic step control. Step control strategy is built in the solver.
  void onOffStepControl(bool sc);           ///< Disables or enables automatic step control


  /// Sets maximal return time to Poincare section.
  /// If the trajectory does not reach the section during time [0,maxReturnTime] then an exception is thrown.
  /// This prevents looping of the procedure computing Poincare map in the case when the trajectory is captured by an attractor (like sink or attracting periodic orbit)
  /// and never intersect Poincare section.
  ///
  /// Default value is maxReturnTime = 1000.
  void setMaxReturnTime(double maxReturnTime);

  /// Sets threshold value of \f$ L_1\f$ norm that is considered as a blow up of the solution.
  /// A trajectory may escape to infinity in finite time without crossing Poincare section.
  /// If the \f$ L_1 \f$ norm of a point on trajectory is bigger than this threshold value then an exception is thrown.
  ///
  /// Default value is blowUpMaxNorm = 1e+10.
  void setBlowUpMaxNorm(double blowUpMaxNorm);

protected:
  /// auxiliary protected method - computes gradient of return time
  /// @param[in] Px - value of Poincare map
  /// @param[in] derivativeOfFlow - solution to first variational equation computed at return time
  /// @param[in] gradientOnPx - gradient of function that defines Poincare section evaluated at Px
  /// @param[in] denominator - scalar product of vector field evaluated at Px and gradientOnPx
  /// @param[out] result - computed gradient of return time
  void computeDT(
        const VectorType& Px,
        const MatrixType& derivativeOfFlow,
        const VectorType& gradientOnPx,
        const ScalarType& denominator,
        VectorType& result
     );

  /// This function moves theSet with the given flow and stops just before the section (for n-th iterate of Poincare Map)
  template<typename T>
  T reachSection(T& theSet, int n = 1, ScalarType * lastStep = 0);

  /// Crosses the section and returns the value of Poincare Map.
  ScalarType findCrossingTime(ScalarType timeJustBeforeSection, const CurveType & curve);

  Solver & m_solver;     ///< dynamical system (e.g. Taylor, C2Taylor, CnTaylor, FadTaylor, etc)
  FunctionType & m_section;   ///< section function
  CrossingDirection m_crossingDirection;  ///< requested direction of section crossing

  /// sectionFactor is used in the procedures reachSection and crossSection.
	/// IN NONRIGOROUS : we want to be closer than m_sectionFactor after section crossing
	/// IN RIGOROUS VERSION: Before and after section crossing  we want the set to be closer to section
	/// than sectionFactor * sizeOfSet
	RealType m_sectionFactor;

	double m_blowUpMaxNorm;
	double m_maxReturnTime;
}; // end of template PoincareMap

}} // namespace capd::poincare

#include "capd/poincare/BasicPoincareMap_inline.h"
#include "capd/poincare/BasicPoincareMap_template.h"


#endif  /* _BasicPoincareMap_H */

/// @}
