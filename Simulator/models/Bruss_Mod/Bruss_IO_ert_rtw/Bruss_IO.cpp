//
// Academic License - for use in teaching, academic research, and meeting
// course requirements at degree granting institutions only.  Not for
// government, commercial, or other organizational use.
//
// File: Bruss_IO.cpp
//
// Code generated for Simulink model 'Bruss_IO'.
//
// Model version                  : 1.47
// Simulink Coder version         : 8.9 (R2015b) 13-Aug-2015
// C/C++ source code generated on : Fri Feb 19 22:34:44 2016
//
// Target selection: ert.tlc
// Embedded hardware selection: 32-bit Generic
// Code generation objectives: Unspecified
// Validation result: Not run
//
#include "Bruss_IO.h"
#include "Bruss_IO_private.h"

//
// This function updates continuous states using the ODE3 fixed-step
// solver algorithm
//
void Bruss_IOModelClass::rt_ertODEUpdateContinuousStates(RTWSolverInfo *si )
{
  // Solver Matrices
  static const real_T rt_ODE3_A[3] = {
    1.0/2.0, 3.0/4.0, 1.0
  };

  static const real_T rt_ODE3_B[3][3] = {
    { 1.0/2.0, 0.0, 0.0 },

    { 0.0, 3.0/4.0, 0.0 },

    { 2.0/9.0, 1.0/3.0, 4.0/9.0 }
  };

  time_T t = rtsiGetT(si);
  time_T tnew = rtsiGetSolverStopTime(si);
  time_T h = rtsiGetStepSize(si);
  real_T *x = rtsiGetContStates(si);
  ODE3_IntgData *id = (ODE3_IntgData *)rtsiGetSolverData(si);
  real_T *y = id->y;
  real_T *f0 = id->f[0];
  real_T *f1 = id->f[1];
  real_T *f2 = id->f[2];
  real_T hB[3];
  int_T i;
  int_T nXc = 2;
  rtsiSetSimTimeStep(si,MINOR_TIME_STEP);

  // Save the state values at time t in y, we'll use x as ynew.
  (void) memcpy(y, x,
                (uint_T)nXc*sizeof(real_T));

  // Assumes that rtsiSetT and ModelOutputs are up-to-date
  // f0 = f(t,y)
  rtsiSetdX(si, f0);
  Bruss_IO_derivatives();

  // f(:,2) = feval(odefile, t + hA(1), y + f*hB(:,1), args(:)(*));
  hB[0] = h * rt_ODE3_B[0][0];
  for (i = 0; i < nXc; i++) {
    x[i] = y[i] + (f0[i]*hB[0]);
  }

  rtsiSetT(si, t + h*rt_ODE3_A[0]);
  rtsiSetdX(si, f1);
  this->step();
  Bruss_IO_derivatives();

  // f(:,3) = feval(odefile, t + hA(2), y + f*hB(:,2), args(:)(*));
  for (i = 0; i <= 1; i++) {
    hB[i] = h * rt_ODE3_B[1][i];
  }

  for (i = 0; i < nXc; i++) {
    x[i] = y[i] + (f0[i]*hB[0] + f1[i]*hB[1]);
  }

  rtsiSetT(si, t + h*rt_ODE3_A[1]);
  rtsiSetdX(si, f2);
  this->step();
  Bruss_IO_derivatives();

  // tnew = t + hA(3);
  // ynew = y + f*hB(:,3);
  for (i = 0; i <= 2; i++) {
    hB[i] = h * rt_ODE3_B[2][i];
  }

  for (i = 0; i < nXc; i++) {
    x[i] = y[i] + (f0[i]*hB[0] + f1[i]*hB[1] + f2[i]*hB[2]);
  }

  rtsiSetT(si, tnew);
  rtsiSetSimTimeStep(si,MAJOR_TIME_STEP);
}

// Model step function
void Bruss_IOModelClass::step()
{
  boolean_T stateChanged;
  if (rtmIsMajorTimeStep((&Bruss_IO_M))) {
    // set solver stop time
    rtsiSetSolverStopTime(&(&Bruss_IO_M)->solverInfo,(((&Bruss_IO_M)
      ->Timing.clockTick0+1)*(&Bruss_IO_M)->Timing.stepSize0));
  }                                    // end MajorTimeStep

  // Update absolute time of base rate at minor time step
  if (rtmIsMinorTimeStep((&Bruss_IO_M))) {
    (&Bruss_IO_M)->Timing.t[0] = rtsiGetT(&(&Bruss_IO_M)->solverInfo);
  }

  // Chart: '<Root>/Chart' incorporates:
  //   Inport: '<Root>/X_in'
  //   Inport: '<Root>/Y_in'

  if (rtmIsMajorTimeStep((&Bruss_IO_M))) {
    stateChanged = 0;

    // Gateway: Chart
    // During: Chart
    if (Bruss_IO_DW.is_active_c1_Bruss_IO == 0U) {
      // Entry: Chart
      Bruss_IO_DW.is_active_c1_Bruss_IO = 1U;

      // Entry Internal: Chart
      // Transition: '<S1>:18'
      Bruss_IO_X.SFunction_CSTATE[0] = Bruss_IO_U.X_in;
      Bruss_IO_X.SFunction_CSTATE[1] = Bruss_IO_U.Y_in;
      stateChanged = true;
    } else {
      // During 'Brussellator': '<S1>:10'
    }

    if (stateChanged) {
      rtsiSetSolverNeedsReset(&(&Bruss_IO_M)->solverInfo, true);
    }
  }

  // Outport: '<Root>/X_out' incorporates:
  //   Chart: '<Root>/Chart'

  // During 'Brussellator': '<S1>:10'
  Bruss_IO_Y.X_out = Bruss_IO_X.SFunction_CSTATE[0];

  // Outport: '<Root>/Y_out' incorporates:
  //   Chart: '<Root>/Chart'

  Bruss_IO_Y.Y_out = Bruss_IO_X.SFunction_CSTATE[1];
  if (rtmIsMajorTimeStep((&Bruss_IO_M))) {
    rt_ertODEUpdateContinuousStates(&(&Bruss_IO_M)->solverInfo);

    // Update absolute time for base rate
    // The "clockTick0" counts the number of times the code of this task has
    //  been executed. The absolute time is the multiplication of "clockTick0"
    //  and "Timing.stepSize0". Size of "clockTick0" ensures timer will not
    //  overflow during the application lifespan selected.

    ++(&Bruss_IO_M)->Timing.clockTick0;
    (&Bruss_IO_M)->Timing.t[0] = rtsiGetSolverStopTime(&(&Bruss_IO_M)
      ->solverInfo);

    {
      // Update absolute timer for sample time: [0.2s, 0.0s]
      // The "clockTick1" counts the number of times the code of this task has
      //  been executed. The resolution of this integer timer is 0.2, which is the step size
      //  of the task. Size of "clockTick1" ensures timer will not overflow during the
      //  application lifespan selected.

      (&Bruss_IO_M)->Timing.clockTick1++;
    }
  }                                    // end MajorTimeStep
}

// Derivatives for root system: '<Root>'
void Bruss_IOModelClass::Bruss_IO_derivatives()
{
  XDot_Bruss_IO_T *_rtXdot;
  _rtXdot = ((XDot_Bruss_IO_T *) (&Bruss_IO_M)->ModelData.derivs);

  // Derivatives for Chart: '<Root>/Chart'
  // During 'Brussellator': '<S1>:10'
  _rtXdot->SFunction_CSTATE[0] = (Bruss_IO_X.SFunction_CSTATE[0] *
    Bruss_IO_X.SFunction_CSTATE[0] * Bruss_IO_X.SFunction_CSTATE[1] + 1.0) - 2.5
    * Bruss_IO_X.SFunction_CSTATE[0];
  _rtXdot->SFunction_CSTATE[1] = 1.5 * Bruss_IO_X.SFunction_CSTATE[0] -
    Bruss_IO_X.SFunction_CSTATE[0] * Bruss_IO_X.SFunction_CSTATE[0] *
    Bruss_IO_X.SFunction_CSTATE[1];
}

// Model initialize function
void Bruss_IOModelClass::initialize()
{
  // Registration code

  // initialize real-time model
  (void) memset((void *)(&Bruss_IO_M), 0,
                sizeof(RT_MODEL_Bruss_IO_T));

  {
    // Setup solver object
    rtsiSetSimTimeStepPtr(&(&Bruss_IO_M)->solverInfo, &(&Bruss_IO_M)
                          ->Timing.simTimeStep);
    rtsiSetTPtr(&(&Bruss_IO_M)->solverInfo, &rtmGetTPtr((&Bruss_IO_M)));
    rtsiSetStepSizePtr(&(&Bruss_IO_M)->solverInfo, &(&Bruss_IO_M)
                       ->Timing.stepSize0);
    rtsiSetdXPtr(&(&Bruss_IO_M)->solverInfo, &(&Bruss_IO_M)->ModelData.derivs);
    rtsiSetContStatesPtr(&(&Bruss_IO_M)->solverInfo, (real_T **) &(&Bruss_IO_M
                         )->ModelData.contStates);
    rtsiSetNumContStatesPtr(&(&Bruss_IO_M)->solverInfo, &(&Bruss_IO_M)
      ->Sizes.numContStates);
    rtsiSetNumPeriodicContStatesPtr(&(&Bruss_IO_M)->solverInfo, &(&Bruss_IO_M)
      ->Sizes.numPeriodicContStates);
    rtsiSetPeriodicContStateIndicesPtr(&(&Bruss_IO_M)->solverInfo, &(&Bruss_IO_M)
      ->ModelData.periodicContStateIndices);
    rtsiSetPeriodicContStateRangesPtr(&(&Bruss_IO_M)->solverInfo, &(&Bruss_IO_M
      )->ModelData.periodicContStateRanges);
    rtsiSetErrorStatusPtr(&(&Bruss_IO_M)->solverInfo, (&rtmGetErrorStatus
      ((&Bruss_IO_M))));
    rtsiSetRTModelPtr(&(&Bruss_IO_M)->solverInfo, (&Bruss_IO_M));
  }

  rtsiSetSimTimeStep(&(&Bruss_IO_M)->solverInfo, MAJOR_TIME_STEP);
  (&Bruss_IO_M)->ModelData.intgData.y = (&Bruss_IO_M)->ModelData.odeY;
  (&Bruss_IO_M)->ModelData.intgData.f[0] = (&Bruss_IO_M)->ModelData.odeF[0];
  (&Bruss_IO_M)->ModelData.intgData.f[1] = (&Bruss_IO_M)->ModelData.odeF[1];
  (&Bruss_IO_M)->ModelData.intgData.f[2] = (&Bruss_IO_M)->ModelData.odeF[2];
  (&Bruss_IO_M)->ModelData.contStates = ((X_Bruss_IO_T *) &Bruss_IO_X);
  rtsiSetSolverData(&(&Bruss_IO_M)->solverInfo, (void *)&(&Bruss_IO_M)
                    ->ModelData.intgData);
  rtsiSetSolverName(&(&Bruss_IO_M)->solverInfo,"ode3");
  rtmSetTPtr((&Bruss_IO_M), &(&Bruss_IO_M)->Timing.tArray[0]);
  (&Bruss_IO_M)->Timing.stepSize0 = 0.2;

  // states (continuous)
  {
    (void) memset((void *)&Bruss_IO_X, 0,
                  sizeof(X_Bruss_IO_T));
  }

  // states (dwork)
  (void) memset((void *)&Bruss_IO_DW, 0,
                sizeof(DW_Bruss_IO_T));

  // external inputs
  (void) memset((void *)&Bruss_IO_U, 0,
                sizeof(ExtU_Bruss_IO_T));

  // external outputs
  (void) memset((void *)&Bruss_IO_Y, 0,
                sizeof(ExtY_Bruss_IO_T));

  // InitializeConditions for Chart: '<Root>/Chart'
  Bruss_IO_DW.is_active_c1_Bruss_IO = 0U;
  Bruss_IO_X.SFunction_CSTATE[0] = 0.0;
  Bruss_IO_X.SFunction_CSTATE[1] = 0.0;
}

// Model terminate function
void Bruss_IOModelClass::terminate()
{
  // (no terminate code required)
}

// Constructor
Bruss_IOModelClass::Bruss_IOModelClass()
{
}

// Destructor
Bruss_IOModelClass::~Bruss_IOModelClass()
{
  // Currently there is no destructor body generated.
}

// Real-Time Model get method
RT_MODEL_Bruss_IO_T * Bruss_IOModelClass::getRTM()
{
  return (&Bruss_IO_M);
}

//
// File trailer for generated code.
//
// [EOF]
//
