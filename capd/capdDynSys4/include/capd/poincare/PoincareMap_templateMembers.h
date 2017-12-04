/// @addtogroup poincare
/// @{

/////////////////////////////////////////////////////////////////////////////
/// @file  PoincareMap_templateMembers.h
///
/// @author Daniel Wilczak
/// @author Tomasz Kapela
/////////////////////////////////////////////////////////////////////////////

// Copyright (C) 200-2013 by the CAPD Group.
//
// Distributed under the terms of the GNU General Public License.
// Consult  http://capd.ii.uj.edu.pl/ for details.

#ifndef _CAPD_POINCARE_POINCARE_MAP_TEMPLATE_MEMBERS_H_
#define _CAPD_POINCARE_POINCARE_MAP_TEMPLATE_MEMBERS_H_

#include <cassert>
#include "capd/poincare/PoincareMap.h"
#include "capd/poincare/BasicPoincareMap.hpp"

namespace capd{
namespace poincare{

/*__________________________________________________________________________*/

template<typename SolverT, typename FunT>
template<capd::dynset::__size_type DEGREE>
void PoincareMap<SolverT,FunT>::updateJacEnclosure(capd::dynset::CnSet<MatrixType,DEGREE>& S)
{
  typedef typename capd::dynset::CnSet<MatrixType,DEGREE>::JetType JetType;
  if(jet!=NULL)
  {
    const size_type dim = this->getVectorField().dimension();
    JetType* partialDerivatives = static_cast<JetType*> (this->jet);
    JetType enc(dim, partialDerivatives->rank()),
            oneStepBound(dim, partialDerivatives->rank());

    this->m_solver.cnEnclosure(VectorType(S), enc, *(S.getNorm()));
    substitutionPowerSeries(enc, S.currentSet(), oneStepBound, false);

    for (size_type p = 0; p < this->getVectorField().dimension(); ++p)
    {
      typename JetType::iterator
          b = partialDerivatives->begin(p, 1),
          e = partialDerivatives->end(p, partialDerivatives->rank());
      typename JetType::iterator i = oneStepBound.begin(p, 1);
      while (b != e)
      {
        *b = intervalHull(*b, *i);
        ++b;
        ++i;
      }
    }
  }

  this->updateTime(S);
}

/*__________________________________________________________________________*/

template<typename SolverT, typename FunT>
template<capd::dynset::__size_type DEGREE>
void PoincareMap<SolverT,FunT>::saveJacEnclosure(capd::dynset::CnSet<MatrixType,DEGREE>& S)
{
  typedef typename capd::dynset::CnSet<MatrixType,DEGREE>::JetType JetType;
  if(jet!=NULL)
  {
    JetType* partialDerivatives = static_cast<JetType*>(this->jet);
    *partialDerivatives = S.currentSet();
  }
  this->saveTime(S);
}


// -------------------- protected functions ---------------------------------

/*__________________________________________________________________________*/
/// Function returns section sign. It also checks If we crossed section
/// and returned in one step. In this case it throws an exception.
/// @param[in]      theSet      the set after making a step,
/// @param[in, out] position    position before time step,
/// @param[in]      updatePosition   flag that decides if position has to be updated
/// @param[in]      bound       rough enclosure for trajectory during whole time step
/// @return    section sign
template <typename SolverT, typename FunT>
template<typename T>
typename PoincareMap<SolverT,FunT>::ScalarType
PoincareMap<SolverT,FunT>::getSign(const T & theSet,  VectorType & position, bool updatePosition, const VectorType & bound )
{
  // we check if we cross the section and than return during one step
  // i.e. if during section crossing the section gradient is orthogonal to vector field
  this->checkTransversability(theSet, bound);

  // Now we are sure that either sing is constant during the time step
  // or we crossed the section transversaly, so we can compute sign on position after the step.
  if(updatePosition){
    position = VectorType(theSet);
    return this->m_section(position);
  }
  else
    return this->m_section(VectorType(theSet));
}
/*__________________________________________________________________________*/
/// Function returns section sign. It also checks for possible crossing the section
/// and returning in one step. In this case it throws an exception.
/// @param[in]      theSet      the set after making a step,
/// @param[in, out] position    position before time step,
/// @param[in]      updatePosition   flag that decided if position has to be updated
/// @return    section sign
template <typename SolverT, typename FunT>
template<typename T>
typename PoincareMap<SolverT,FunT>::ScalarType
PoincareMap<SolverT,FunT>::getSign( const T & theSet,  VectorType & position, bool updatePosition)
{
  return this->getSign(theSet, position, updatePosition, theSet.getLastEnclosure());
//  return this->getSign(theSet, position, updatePosition, this->m_solver.enclosure(theSet.getCurrentTime(),position));
}
/*__________________________________________________________________________*/

template <typename SolverT, typename FunT>
template<typename T>
T PoincareMap<SolverT,FunT>::reachSection(T& theSet, int n)
// The common procedure to reach section for all types of the sets.
// As a result theSet is closer to section than sectionFactor * sizeOfSet
// Function returns set on the section or just after the section
{
  T setAfterSection(theSet);
  findSectionCrossing(theSet, setAfterSection, n);
  approachSection(theSet);
  return setAfterSection;
}


template <typename SolverT, typename FunT>
template<typename T>
void PoincareMap<SolverT,FunT>::findSectionCrossing(T & theSet, T & setAfterSection, int n)
// Functions reaches the section. As result:
// - theSet is just before the section,
// - setAfterSection is on the section or just after it

{
  this->m_solver.getStepControl().init(this->m_solver,theSet.getCurrentTime(),theSet);
  for(int iterate = 0; iterate < n; ++iterate){

     VectorType setPosition = VectorType(theSet);
     ScalarType sign = this->m_section(setPosition);

    //------------------------------------------------------------------------------------------------
    // LEAVING SECTION AND GOING UP TO THE POINT WHERE SECTION HAS CORRECT SIGN
    // We want to leave section and reach point where the sign of the section function,
    // according to crossingDirection, indicate that next section crossing will be in a good direction.

    while( (sign.contains(0.0)) ||                  // We are on the section
        !((this->m_crossingDirection == Both) ||         // section sign is not correct
            ((this->m_crossingDirection == MinusPlus) && (sign < 0.0)) ||
            ((this->m_crossingDirection == PlusMinus) && (sign > 0.0))
        )
    ){
      theSet.move(this->m_solver);   // we make one step to leave the section and try again
      sign = this->getSign(theSet, setPosition, true);
    }

    //------------------------------------------------
    // first return to section

    assert(!sign.contains(ScalarType(0.0)));

    sign = sign.mid();            // we save the sign of the section function, sign does not contain zero
    ScalarType check = sign;      // used for checking if we cross the section (if sign is changed)
    T temp(theSet);
    T *original = &theSet, *tempSet = &temp;

    // now we try to move the set closer to section than one current time step
    while(check*sign>0)
    {
      original->move(this->m_solver,*tempSet);
      check = this->getSign(*tempSet, setPosition, true);
      std::swap(original, tempSet);
    } // end while

    if(iterate == n-1) {           // last iterate
      setAfterSection = *original;
      // We take the last set with not changed section sign
      if(&theSet != tempSet)
        theSet = *tempSet;
    } else {                       // setting theSet to be after the section for the next iterate
      if(&theSet != original)
        theSet = *original;
    }
  }
}

// ################################################################

template <typename SolverT, typename FunT>
template<typename T>
void PoincareMap<SolverT,FunT>::approachSection(T & theSet){

  SaveStepControl<SolverT> ssc(this->m_solver);
  ScalarType step = this->m_solver.getStep(); // time step used to reach the section

  VectorType setPosition = VectorType(theSet);
  ScalarType sign = this->m_section(setPosition);
  sign = sign.mid();            // we save the sign of the section function, sign do not contain zero
  ScalarType check = sign;      // used for checking if we cross the section (if sign is changed)

//==================================================================================================
// We want to come to section closer than  sectionFactor * size of set in section gradient direction
// first we turn of step control
//  this->m_solver.turnOffStepControl();

  T temp(theSet);
  T *original = &theSet, *tempSet = &temp;

  setPosition = VectorType(theSet);
  VectorType Grad = this->m_section.gradient(setPosition);

  // maximal distance is sectionFactor * size of the set in the direction perpendicular to the section
  typename ScalarType::BoundType maxDistance = diam(abs(setPosition*Grad)).rightBound()*this->m_sectionFactor;
  int maxIterations = 30; // usually 2-3 iterations are necessary to come close enough to the section

  original = &theSet;
  tempSet = &temp;

  while(maxIterations)
  {
    ScalarType approxDistance = (abs(this->m_section(setPosition))).left();
    if(approxDistance.leftBound() < maxDistance)      // we are close enough
      break;

    VectorType setCenter = midVector(setPosition);
    VectorType velocity = this->m_solver.getVectorField()(setCenter);
    ScalarType velocityInNormalDirection = abs(velocity * Grad);
    // We want to stop at distance equal to 1/15 of maxDistance
    // (1/15 is an experimental value, chosen as well working)
    ScalarType approxTime = ((approxDistance-(maxDistance/15))/ velocityInNormalDirection).mid().left();

    //this->m_solver.setStep(approxTime);
    this->m_solver.setMaxStep(approxTime);
    original->move(this->m_solver,*tempSet);

    int correctionAttempts= maxCorrectionAttempts;
    while(true)
    {
      check = this->getSign(*tempSet, setPosition, false);

      if((!(check*sign>0)) && (correctionAttempts-- != 0))
      { // an approximated time step is to big, we touched or crossed the section
        // so we shorten time step multiplying it by some correction factor from (0,1)
        ScalarType step = this->m_solver.getStep();
        ScalarType newStep = step * timeStepCorrectionFactor;
        if(step ==newStep)
          break;
        //this->m_solver.setStep(newStep);
        this->m_solver.setMaxStep(newStep);
        original->move(this->m_solver,*tempSet);
      }
      else
        break;
    } // end while

    if(correctionAttempts!= -1) {
      std::swap(original, tempSet);
      setPosition = VectorType(*original);
      Grad = this->m_section.gradient(setPosition);
      maxIterations--;
    }else{
      break;
    }
  }
  if(&theSet != original)
    theSet = *original;
}


/////////////////////////////////////////////////////////////////////////////////////
///
/// Function used to cross the section for all types of sets
///
/// @param[in,out] theSet              set just before the section, on exit is equal to setAfterTheSection
/// @param[in]     setAfterTheSection  set on the section or just after it
/// @param[out] domain                 if success contains bound for the return time relative to one step made
/// @returns        set that contains value of Poincare map for given set theSet
template <typename DS, typename FunT>
template<typename T>
bool PoincareMap<DS,FunT>::crossSection(T theSet, ScalarType& oneStepReturnTime, VectorType& bound)
{
// theSet is very close to the section
// we compute an approximate time needed to cross the section
// if the approximate values of time step is to small to change the sign of the section function
// the 'smallTime' time is used to move the set

  // We want stop after section, closer than  maxDistance =  sectionFactor * size of set in section gradient direction
  ScalarType check = this->m_section((VectorType)theSet);
  if(check.contains(0.0)){
    throw PoincareException<T>("Poincare exception in section crossing : initial set is already on the section ", theSet, check );
  }
  ScalarType sign = mid(check);
  ScalarType timeBeforeSection = theSet.getCurrentTime();
  this->m_solver(theSet);

  const CurveType& c = this->m_solver.getCurve();
  oneStepReturnTime = ScalarType(c.getLeftDomain(),c.getRightDomain());
  bound = theSet.getLastEnclosure();
  this->checkTransversability(theSet, bound);
  check = this->m_section((VectorType)theSet);

  if(!(check*sign < 0)){ // we did not touch the section yet
    return false;
  }

  // now we resolve for the return time
  for(int i=0;i<10;++i)
  {
    ScalarType midPoint = oneStepReturnTime.mid();
    ScalarType valAtMidPoint = this->m_section(c(midPoint));
    ScalarType derAtDomain = this->m_section.gradient(bound) * this->m_solver.getVectorField()(timeBeforeSection+oneStepReturnTime,bound);
    ScalarType N = midPoint - valAtMidPoint/derAtDomain;
    ScalarType newReturnTime;
    if(!intersection(oneStepReturnTime,N,newReturnTime))
      throw std::runtime_error("PoincareMap error: empty intersection in estimation of the return time");

    bound = c(oneStepReturnTime);
    if(oneStepReturnTime == newReturnTime) {
      //std::cout << "break: " << i << std::endl;
      break;
    }

    oneStepReturnTime = newReturnTime;
  }
  return true;
}

/////////////////////////////////////////////////////////////////////////////////////
///
/// Function used to cross the section for all types of sets
///
/// @param[in,out] theSet              set just before the section, on exit is equal to setAfterTheSection
/// @param[in]     setAfterTheSection  set on the section or just after it
/// @returns        set that contains value of Poincare map for given set theSet

template <typename SolverT, typename FunT>
template<typename T>
typename PoincareMap<SolverT,FunT>::VectorType
PoincareMap<SolverT,FunT>::crossSection(T& theSet, const T & setAfterTheSection)
{
  SaveStepControl<SolverT> ssc(this->m_solver);
  // theSet is very close to the section
  // we compute an approximate time needed to cross the section
  // if the approximate values of time step is to small to change the sign of the section function
  // the 'smallTime' time is used to move the set

  // We want stop after section, closer than  maxDistance =  sectionFactor * size of set in section gradient direction
  VectorType setPosition = VectorType(theSet);
  VectorType Grad = this->m_section.gradient(setPosition);
  typename ScalarType::BoundType maxDistance = diam(abs(setPosition*Grad)).rightBound()*this->m_sectionFactor;
  typename ScalarType::BoundType approxDistance = - abs(this->m_section(setPosition)).rightBound();

  VectorType result = setPosition;

  ScalarType check = this->m_section(setPosition);
  if(check.contains(0.0))
    throw PoincareException<T>("Poincare exception in section crossing : initial set is already on the section ", theSet, check );

  ScalarType sign = mid(check);

  // direction of section crossing =  PlusMinus or MinusPlus
  // we check sign of the section instead of checking value of variable crossingDirection because it can be set to None
  typename BasicPoincareMap<SolverT, FunT>::CrossingDirection direction = ( check > 0)? PlusMinus: MinusPlus;

  T temp(theSet); // we save the set before section, used in throwing exception only
  saveJacEnclosure(theSet); // we save the \frac{d}{dx}\phi

  T* original = &theSet;
  T* tempSet = &temp;
  ScalarType step = this->m_solver.getStep();

  while(!(check*sign < 0))
  {
    VectorType fieldDirection = this->m_solver.getVectorField()(midVector(setPosition));
    ScalarType fieldSectionDirection = abs(fieldDirection * Grad);
    ScalarType approxTime = ((maxDistance/2 - approxDistance ) / fieldSectionDirection).mid();

    // if approxTime is too big, it can cause problems with finding an enclosure.
    // In this case we use original time step (possibly several times)
    approxTime = capd::min(approxTime, abs(step));

    // approxTime cannot be too small, otherwise it can occur that we will never cross the section
//    approxTime = capd::max(approxTime, minCrossingTimeStep);
    approxTime = capd::max(approxTime/10, minCrossingTimeStep);

    int correctionAttempts = maxCorrectionAttempts;
    while(true)
    {
      this->m_solver.setMaxStep(approxTime.left());
      original->move(this->m_solver,*tempSet);
      setPosition = VectorType(*tempSet);
      check = this->m_section(setPosition);

      if(direction == PlusMinus)
        approxDistance = - check.rightBound();
      else
        approxDistance = check.leftBound();

      if((approxDistance  > maxDistance)&& (correctionAttempts-- != 0)){  // the step was too big
        approxTime *= timeStepCorrectionFactor;  // so we shorten it
      }else{
        break;
      }
    }

    typename SolverT::SolutionCurve c = this->m_solver.getCurve();
    ScalarType domain(c.getLeftDomain(),c.getRightDomain());
    VectorType bound = c(domain);

    checkTransversability(*original, bound);
    if( (check*sign > 0)){ // we did not touch the section yet
       result = setPosition;
       this->saveJacEnclosure(*tempSet);
    }else{
      this->updateJacEnclosure(*original);

      // TODO: It was computed already in checkTranversability
      VectorType Fresult = this->m_solver.getVectorField()(bound);

      for(size_type i=0;i<this->getVectorField().dimension();i++)
      {
        if(!(Fresult[i].contains(0.)))
           result[i] = intervalHull(result[i],setPosition[i]);
        else
           result[i] = intervalHull(result[i],bound[i]); // bound is an enclosure
      }
    }
    T* swapSet = original;
    original = tempSet;
    tempSet = swapSet;

  } // end while

  theSet = setAfterTheSection;
  return result;
}

/// Function checks if we crossed section and then returned in one step.
/// In this case it throws an exception of PoincareException<T> type.
template <typename SolverT, typename FunT>
template<typename T>
void PoincareMap<SolverT,FunT>::checkTransversability(
  const T & theSet, const VectorType & bound
){
  ScalarType check = this->m_section(bound);

  if(subset(ScalarType(0.0), check)) {  // Is the section crossed?
    ScalarType innerProduct = this->m_section.gradient(bound)*this->m_solver.getVectorField()(bound);
    if (innerProduct.contains(0.0)) {  // Is the vector field orthogonal to section gradient?
      throw PoincareException<T>(
              "PoincareMap error: possible nontransversal return to the section ", theSet, theSet,
              bound, this->m_solver.getVectorField()(bound),
              check, this->m_section.gradient(bound), innerProduct
              );
    }
  }

}

}} // namespace capd::poincare

#endif // _CAPD_POINCARE_POINCARE_MAP_TEMPLATE_MEMBERS_H_

/// @}
