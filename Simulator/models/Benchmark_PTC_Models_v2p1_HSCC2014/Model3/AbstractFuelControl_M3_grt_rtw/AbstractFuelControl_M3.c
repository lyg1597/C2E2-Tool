/*
 * AbstractFuelControl_M3.c
 *
 * Academic License - for use in teaching, academic research, and meeting
 * course requirements at degree granting institutions only.  Not for
 * government, commercial, or other organizational use.
 *
 * Code generation for model "AbstractFuelControl_M3".
 *
 * Model version              : 1.41
 * Simulink Coder version : 8.9 (R2015b) 13-Aug-2015
 * C source code generated on : Fri Feb 12 16:59:56 2016
 *
 * Target selection: grt.tlc
 * Note: GRT includes extra infrastructure and instrumentation for prototyping
 * Embedded hardware selection: 32-bit Generic
 * Code generation objectives: Unspecified
 * Validation result: Not run
 */

#include "AbstractFuelControl_M3.h"
#include "AbstractFuelControl_M3_private.h"

/* Block signals (auto storage) */
B_AbstractFuelControl_M3_T AbstractFuelControl_M3_B;

/* Continuous states */
X_AbstractFuelControl_M3_T AbstractFuelControl_M3_X;

/* Block states (auto storage) */
DW_AbstractFuelControl_M3_T AbstractFuelControl_M3_DW;

/* External inputs (root inport signals with auto storage) */
ExtU_AbstractFuelControl_M3_T AbstractFuelControl_M3_U;

/* External outputs (root outports fed by signals with auto storage) */
ExtY_AbstractFuelControl_M3_T AbstractFuelControl_M3_Y;

/* Real-time model */
RT_MODEL_AbstractFuelControl__T AbstractFuelControl_M3_M_;
RT_MODEL_AbstractFuelControl__T *const AbstractFuelControl_M3_M =
  &AbstractFuelControl_M3_M_;

/*
 * This function updates continuous states using the ODE3 fixed-step
 * solver algorithm
 */
static void rt_ertODEUpdateContinuousStates(RTWSolverInfo *si )
{
  /* Solver Matrices */
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
  int_T nXc = 6;
  rtsiSetSimTimeStep(si,MINOR_TIME_STEP);

  /* Save the state values at time t in y, we'll use x as ynew. */
  (void) memcpy(y, x,
                (uint_T)nXc*sizeof(real_T));

  /* Assumes that rtsiSetT and ModelOutputs are up-to-date */
  /* f0 = f(t,y) */
  rtsiSetdX(si, f0);
  AbstractFuelControl_M3_derivatives();

  /* f(:,2) = feval(odefile, t + hA(1), y + f*hB(:,1), args(:)(*)); */
  hB[0] = h * rt_ODE3_B[0][0];
  for (i = 0; i < nXc; i++) {
    x[i] = y[i] + (f0[i]*hB[0]);
  }

  rtsiSetT(si, t + h*rt_ODE3_A[0]);
  rtsiSetdX(si, f1);
  AbstractFuelControl_M3_step();
  AbstractFuelControl_M3_derivatives();

  /* f(:,3) = feval(odefile, t + hA(2), y + f*hB(:,2), args(:)(*)); */
  for (i = 0; i <= 1; i++) {
    hB[i] = h * rt_ODE3_B[1][i];
  }

  for (i = 0; i < nXc; i++) {
    x[i] = y[i] + (f0[i]*hB[0] + f1[i]*hB[1]);
  }

  rtsiSetT(si, t + h*rt_ODE3_A[1]);
  rtsiSetdX(si, f2);
  AbstractFuelControl_M3_step();
  AbstractFuelControl_M3_derivatives();

  /* tnew = t + hA(3);
     ynew = y + f*hB(:,3); */
  for (i = 0; i <= 2; i++) {
    hB[i] = h * rt_ODE3_B[2][i];
  }

  for (i = 0; i < nXc; i++) {
    x[i] = y[i] + (f0[i]*hB[0] + f1[i]*hB[1] + f2[i]*hB[2]);
  }

  rtsiSetT(si, tnew);
  rtsiSetSimTimeStep(si,MAJOR_TIME_STEP);
}

real_T rt_powd_snf(real_T u0, real_T u1)
{
  real_T y;
  real_T tmp;
  real_T tmp_0;
  if (rtIsNaN(u0) || rtIsNaN(u1)) {
    y = (rtNaN);
  } else {
    tmp = fabs(u0);
    tmp_0 = fabs(u1);
    if (rtIsInf(u1)) {
      if (tmp == 1.0) {
        y = (rtNaN);
      } else if (tmp > 1.0) {
        if (u1 > 0.0) {
          y = (rtInf);
        } else {
          y = 0.0;
        }
      } else if (u1 > 0.0) {
        y = 0.0;
      } else {
        y = (rtInf);
      }
    } else if (tmp_0 == 0.0) {
      y = 1.0;
    } else if (tmp_0 == 1.0) {
      if (u1 > 0.0) {
        y = u0;
      } else {
        y = 1.0 / u0;
      }
    } else if (u1 == 2.0) {
      y = u0 * u0;
    } else if ((u1 == 0.5) && (u0 >= 0.0)) {
      y = sqrt(u0);
    } else if ((u0 < 0.0) && (u1 > floor(u1))) {
      y = (rtNaN);
    } else {
      y = pow(u0, u1);
    }
  }

  return y;
}

/* Model step function */
void AbstractFuelControl_M3_step(void)
{
  real_T rtb_rpmtorads;
  real_T rtb_theta090;
  boolean_T rtb_DataTypeConversion1;
  real_T rtb_InputPoly;
  boolean_T rtb_LogicalOperator2;
  int8_T rtAction;
  if (rtmIsMajorTimeStep(AbstractFuelControl_M3_M)) {
    /* set solver stop time */
    if (!(AbstractFuelControl_M3_M->Timing.clockTick0+1)) {
      rtsiSetSolverStopTime(&AbstractFuelControl_M3_M->solverInfo,
                            ((AbstractFuelControl_M3_M->Timing.clockTickH0 + 1) *
        AbstractFuelControl_M3_M->Timing.stepSize0 * 4294967296.0));
    } else {
      rtsiSetSolverStopTime(&AbstractFuelControl_M3_M->solverInfo,
                            ((AbstractFuelControl_M3_M->Timing.clockTick0 + 1) *
        AbstractFuelControl_M3_M->Timing.stepSize0 +
        AbstractFuelControl_M3_M->Timing.clockTickH0 *
        AbstractFuelControl_M3_M->Timing.stepSize0 * 4294967296.0));
    }
  }                                    /* end MajorTimeStep */

  /* Update absolute time of base rate at minor time step */
  if (rtmIsMinorTimeStep(AbstractFuelControl_M3_M)) {
    AbstractFuelControl_M3_M->Timing.t[0] = rtsiGetT
      (&AbstractFuelControl_M3_M->solverInfo);
  }

  /* Outputs for Atomic SubSystem: '<Root>/Model 3' */
  /* Gain: '<S1>/(rpm) to (rad//s)' incorporates:
   *  Inport: '<Root>/Engine Speed (rpm)'
   */
  rtb_rpmtorads = AbstractFuelControl_M3_P.rpmtorads_Gain *
    AbstractFuelControl_M3_U.EngineSpeed;
  if (rtmIsMajorTimeStep(AbstractFuelControl_M3_M)) {
    /* Delay: '<S1>/Delay' */
    AbstractFuelControl_M3_B.Delay = AbstractFuelControl_M3_DW.Delay_DSTATE;
  }

  /* Sum: '<S1>/Sum' incorporates:
   *  Constant: '<S1>/Base opening angle'
   *  TransferFcn: '<S1>/Throttle  delay1'
   */
  rtb_theta090 = AbstractFuelControl_M3_P.Throttledelay1_C *
    AbstractFuelControl_M3_X.Throttledelay1_CSTATE +
    AbstractFuelControl_M3_P.Baseopeningangle_Value;

  /* Saturate: '<S1>/theta [0 90]' */
  if (rtb_theta090 > AbstractFuelControl_M3_P.theta090_UpperSat) {
    rtb_theta090 = AbstractFuelControl_M3_P.theta090_UpperSat;
  } else {
    if (rtb_theta090 < AbstractFuelControl_M3_P.theta090_LowerSat) {
      rtb_theta090 = AbstractFuelControl_M3_P.theta090_LowerSat;
    }
  }

  /* End of Saturate: '<S1>/theta [0 90]' */

  /* Fcn: '<S1>/InputPoly' */
  rtb_InputPoly = ((-0.05231 * rtb_theta090 + 2.821) + 0.10299 * rt_powd_snf
                   (rtb_theta090, 2.0)) + -0.00063 * rt_powd_snf(rtb_theta090,
    3.0);

  /* Relay: '<S1>/Power Mode Guard' */
  if (rtmIsMajorTimeStep(AbstractFuelControl_M3_M)) {
    if (rtb_theta090 >= AbstractFuelControl_M3_P.PowerModeGuard_OnVal) {
      AbstractFuelControl_M3_DW.PowerModeGuard_Mode = true;
    } else {
      if (rtb_theta090 <= AbstractFuelControl_M3_P.PowerModeGuard_OffVal) {
        AbstractFuelControl_M3_DW.PowerModeGuard_Mode = false;
      }
    }
  }

  if (AbstractFuelControl_M3_DW.PowerModeGuard_Mode) {
    AbstractFuelControl_M3_B.PowerModeGuard =
      AbstractFuelControl_M3_P.PowerModeGuard_YOn;
  } else {
    AbstractFuelControl_M3_B.PowerModeGuard =
      AbstractFuelControl_M3_P.PowerModeGuard_YOff;
  }

  /* End of Relay: '<S1>/Power Mode Guard' */

  /* Outputs for Atomic SubSystem: '<S1>/Startup Mode Latch' */
  if (rtmIsMajorTimeStep(AbstractFuelControl_M3_M)) {
    /* Delay: '<S4>/Integer Delay1' */
    AbstractFuelControl_M3_B.IntegerDelay1 =
      AbstractFuelControl_M3_DW.IntegerDelay1_DSTATE;
  }

  /* End of Outputs for SubSystem: '<S1>/Startup Mode Latch' */

  /* Step: '<S1>/Starup Mode' */
  if (AbstractFuelControl_M3_M->Timing.t[0] <
      AbstractFuelControl_M3_P.StarupMode_Time) {
    rtb_theta090 = AbstractFuelControl_M3_P.StarupMode_Y0;
  } else {
    rtb_theta090 = AbstractFuelControl_M3_P.StarupMode_YFinal;
  }

  /* End of Step: '<S1>/Starup Mode' */

  /* Outputs for Atomic SubSystem: '<S1>/Startup Mode Latch' */
  /* Logic: '<S4>/Logical Operator' incorporates:
   *  DataTypeConversion: '<S1>/Data Type Conversion1'
   *  Logic: '<S4>/Logical Operator1'
   */
  AbstractFuelControl_M3_B.LogicalOperator = ((!(rtb_theta090 != 0.0)) ||
    AbstractFuelControl_M3_B.IntegerDelay1);

  /* Logic: '<S4>/Logical Operator2' */
  rtb_LogicalOperator2 = !AbstractFuelControl_M3_B.LogicalOperator;

  /* End of Outputs for SubSystem: '<S1>/Startup Mode Latch' */

  /* Outputs for Atomic SubSystem: '<S1>/Sensor Failure Detection Latch' */
  if (rtmIsMajorTimeStep(AbstractFuelControl_M3_M)) {
    /* Delay: '<S3>/Integer Delay1' */
    AbstractFuelControl_M3_B.IntegerDelay1_j =
      AbstractFuelControl_M3_DW.IntegerDelay1_DSTATE_a;
  }

  /* End of Outputs for SubSystem: '<S1>/Sensor Failure Detection Latch' */

  /* Step: '<S1>/FaultInjection' */
  if (AbstractFuelControl_M3_M->Timing.t[0] <
      AbstractFuelControl_M3_P.fault_time) {
    rtb_theta090 = AbstractFuelControl_M3_P.FaultInjection_Y0;
  } else {
    rtb_theta090 = AbstractFuelControl_M3_P.FaultInjection_YFinal;
  }

  /* End of Step: '<S1>/FaultInjection' */

  /* Outputs for Atomic SubSystem: '<S1>/Sensor Failure Detection Latch' */
  /* Logic: '<S3>/Logical Operator' incorporates:
   *  DataTypeConversion: '<S1>/Data Type Conversion'
   */
  AbstractFuelControl_M3_B.LogicalOperator_b = ((rtb_theta090 != 0.0) ||
    AbstractFuelControl_M3_B.IntegerDelay1_j);

  /* End of Outputs for SubSystem: '<S1>/Sensor Failure Detection Latch' */

  /* Logic: '<S1>/Logical Operator1' */
  rtb_DataTypeConversion1 = ((AbstractFuelControl_M3_B.PowerModeGuard != 0.0) ||
    rtb_LogicalOperator2 || AbstractFuelControl_M3_B.LogicalOperator_b);

  /* Logic: '<S1>/Logical Operator2' incorporates:
   *  Logic: '<S1>/Logical Operator3'
   */
  rtb_LogicalOperator2 = ((!rtb_LogicalOperator2) &&
    (AbstractFuelControl_M3_B.PowerModeGuard != 0.0));

  /* Fcn: '<S1>/ODE1' incorporates:
   *  Integrator: '<S1>/p'
   */
  rtb_theta090 = rt_powd_snf(AbstractFuelControl_M3_X.p_CSTATE, 2.0);
  AbstractFuelControl_M3_B.ODE1 = (((-2.3421 * rtb_theta090 + 2.7799 *
    AbstractFuelControl_M3_X.p_CSTATE) + -0.3273) * (2.0 * rtb_InputPoly) -
    (((0.08979 * rtb_rpmtorads * AbstractFuelControl_M3_X.p_CSTATE + -0.366) +
      -0.0337 * rtb_rpmtorads * rt_powd_snf(AbstractFuelControl_M3_X.p_CSTATE,
    2.0)) + 0.0001 * AbstractFuelControl_M3_X.p_CSTATE * rt_powd_snf
     (rtb_rpmtorads, 2.0)) * 0.9) * 0.41328;

  /* Fcn: '<S1>/ODE2' incorporates:
   *  Integrator: '<S1>/lambda'
   *  Integrator: '<S1>/p'
   */
  AbstractFuelControl_M3_B.ODE2 = ((((((0.08979 * rtb_rpmtorads *
    AbstractFuelControl_M3_X.p_CSTATE + -0.366) + -0.0337 * rtb_rpmtorads *
    rt_powd_snf(AbstractFuelControl_M3_X.p_CSTATE, 2.0)) + 0.0001 *
    AbstractFuelControl_M3_X.p_CSTATE * rt_powd_snf(rtb_rpmtorads, 2.0)) * 0.9 *
    2.6287 + ((-35.2518 * AbstractFuelControl_M3_B.Delay + 13.893) + 20.7364 *
              rt_powd_snf(AbstractFuelControl_M3_B.Delay, 2.0))) + (((0.08979 *
    rtb_rpmtorads * AbstractFuelControl_M3_X.p_CSTATE + -0.366) + -0.0337 *
    rtb_rpmtorads * rt_powd_snf(AbstractFuelControl_M3_X.p_CSTATE, 2.0)) +
    0.0001 * AbstractFuelControl_M3_X.p_CSTATE * rt_powd_snf(rtb_rpmtorads, 2.0))
    * 0.9 * -1.592 * AbstractFuelControl_M3_B.Delay) -
    AbstractFuelControl_M3_X.lambda_CSTATE) * 4.0;

  /* Fcn: '<S1>/ODE3' incorporates:
   *  Integrator: '<S1>/p'
   *  Integrator: '<S1>/pe'
   */
  AbstractFuelControl_M3_B.ODE3 = (((-2.3421 * rt_powd_snf
    (AbstractFuelControl_M3_X.p_CSTATE, 2.0) + 2.7799 *
    AbstractFuelControl_M3_X.p_CSTATE) + -0.3273) * (2.0 * rtb_InputPoly) -
    (((0.08979 * rtb_rpmtorads * AbstractFuelControl_M3_X.pe_CSTATE + -0.366) +
      -0.0337 * rtb_rpmtorads * rt_powd_snf(AbstractFuelControl_M3_X.pe_CSTATE,
    2.0)) + 0.0001 * AbstractFuelControl_M3_X.pe_CSTATE * rt_powd_snf
     (rtb_rpmtorads, 2.0))) * 0.41328;

  /* Switch: '<S1>/Switch1' incorporates:
   *  Fcn: '<S1>/ODE4 Closed'
   *  Fcn: '<S1>/ODE4 Open'
   *  Integrator: '<S1>/lambda'
   */
  if (rtb_DataTypeConversion1) {
    AbstractFuelControl_M3_B.Switch1 = 0.0;
  } else {
    AbstractFuelControl_M3_B.Switch1 = (AbstractFuelControl_M3_X.lambda_CSTATE -
      14.7) * 0.14;
  }

  /* End of Switch: '<S1>/Switch1' */

  /* Switch: '<S1>/Switch2' incorporates:
   *  Constant: '<S1>/Constant3'
   *  Constant: '<S1>/Constant4'
   *  Fcn: '<S1>/Fuel Cmd Open Pwr'
   *  Integrator: '<S1>/pe'
   *  Switch: '<S1>/Switch'
   *  Switch: '<S1>/Switch3'
   */
  if (rtb_LogicalOperator2) {
    AbstractFuelControl_M3_B.Switch2 = (((0.08979 * rtb_rpmtorads *
      AbstractFuelControl_M3_X.pe_CSTATE + -0.366) + -0.0337 * rtb_rpmtorads *
      rt_powd_snf(AbstractFuelControl_M3_X.pe_CSTATE, 2.0)) + 0.0001 *
      AbstractFuelControl_M3_X.pe_CSTATE * rt_powd_snf(rtb_rpmtorads, 2.0)) *
      0.08;
    rtb_rpmtorads = AbstractFuelControl_M3_P.Constant3_Value;
  } else {
    if (rtb_DataTypeConversion1) {
      /* Switch: '<S1>/Switch' incorporates:
       *  Fcn: '<S1>/Fuel Cmd Open'
       *  Integrator: '<S1>/pe'
       */
      AbstractFuelControl_M3_B.Switch2 = (((0.08979 * rtb_rpmtorads *
        AbstractFuelControl_M3_X.pe_CSTATE + -0.366) + -0.0337 * rtb_rpmtorads *
        rt_powd_snf(AbstractFuelControl_M3_X.pe_CSTATE, 2.0)) + 0.0001 *
        AbstractFuelControl_M3_X.pe_CSTATE * rt_powd_snf(rtb_rpmtorads, 2.0)) *
        0.068027210884353748;
    } else {
      /* Switch: '<S1>/Switch' incorporates:
       *  Fcn: '<S1>/Fuel Cmd Closed'
       *  Integrator: '<S1>/i'
       *  Integrator: '<S1>/lambda'
       *  Integrator: '<S1>/pe'
       */
      AbstractFuelControl_M3_B.Switch2 = (((0.08979 * rtb_rpmtorads *
        AbstractFuelControl_M3_X.pe_CSTATE + -0.366) + -0.0337 * rtb_rpmtorads *
        rt_powd_snf(AbstractFuelControl_M3_X.pe_CSTATE, 2.0)) + 0.0001 *
        AbstractFuelControl_M3_X.pe_CSTATE * rt_powd_snf(rtb_rpmtorads, 2.0)) *
        (((AbstractFuelControl_M3_X.lambda_CSTATE - 14.7) * 0.04 + (1.0 +
           AbstractFuelControl_M3_X.i_CSTATE)) * 0.068027210884353748);
    }

    rtb_rpmtorads = AbstractFuelControl_M3_P.Constant4_Value;
  }

  /* End of Switch: '<S1>/Switch2' */
  /* End of Outputs for SubSystem: '<Root>/Model 3' */

  /* Outputs for Atomic SubSystem: '<Root>/V&V stub system' */
  /* Step: '<S2>/MeasureOn' */
  if (AbstractFuelControl_M3_M->Timing.t[0] <
      AbstractFuelControl_M3_P.measureTime) {
    rtb_theta090 = AbstractFuelControl_M3_P.MeasureOn_Y0_b;
  } else {
    rtb_theta090 = AbstractFuelControl_M3_P.MeasureOn_YFinal_k;
  }

  /* End of Step: '<S2>/MeasureOn' */

  /* RelationalOperator: '<S2>/Relational Operator' incorporates:
   *  Constant: '<S2>/Constant'
   */
  AbstractFuelControl_M3_B.RelationalOperator = (rtb_theta090 >
    AbstractFuelControl_M3_P.Constant_Value_n);
  if (rtmIsMajorTimeStep(AbstractFuelControl_M3_M)) {
    /* SignalConversion: '<S2>/HiddenBuf_InsertedFor_Calcuate Error_at_inport_3' */
    AbstractFuelControl_M3_B.HiddenBuf_InsertedFor_CalcuateE =
      AbstractFuelControl_M3_B.RelationalOperator;

    /* Outputs for Enabled SubSystem: '<S2>/Calcuate Error' incorporates:
     *  EnablePort: '<S5>/Enable'
     */
    if (rtmIsMajorTimeStep(AbstractFuelControl_M3_M)) {
      if (AbstractFuelControl_M3_B.HiddenBuf_InsertedFor_CalcuateE) {
        if (!AbstractFuelControl_M3_DW.CalcuateError_MODE) {
          AbstractFuelControl_M3_DW.CalcuateError_MODE = true;
        }
      } else {
        if (AbstractFuelControl_M3_DW.CalcuateError_MODE) {
          /* Disable for SwitchCase: '<S5>/Switch Case' */
          AbstractFuelControl_M3_DW.SwitchCase_ActiveSubsystem = -1;
          AbstractFuelControl_M3_DW.CalcuateError_MODE = false;
        }
      }
    }

    /* End of Outputs for SubSystem: '<S2>/Calcuate Error' */
  }

  /* Outputs for Enabled SubSystem: '<S2>/Calcuate Error' incorporates:
   *  EnablePort: '<S5>/Enable'
   */
  if (AbstractFuelControl_M3_DW.CalcuateError_MODE) {
    /* SwitchCase: '<S5>/Switch Case' incorporates:
     *  Constant: '<S2>/spec_num'
     */
    if (rtmIsMajorTimeStep(AbstractFuelControl_M3_M)) {
      switch (AbstractFuelControl_M3_P.spec_num) {
       case 1:
        rtAction = 0;
        break;

       case 2:
        rtAction = 1;
        break;

       default:
        rtAction = 2;
        break;
      }

      AbstractFuelControl_M3_DW.SwitchCase_ActiveSubsystem = rtAction;
    } else {
      rtAction = AbstractFuelControl_M3_DW.SwitchCase_ActiveSubsystem;
    }

    switch (rtAction) {
     case 0:
      /* Outputs for IfAction SubSystem: '<S5>/over(under)shoot' incorporates:
       *  ActionPort: '<S7>/Action Port'
       */
      /* Outputs for Atomic SubSystem: '<Root>/Model 3' */
      /* Sum: '<S7>/Sum1' incorporates:
       *  Constant: '<S7>/Constant'
       *  Integrator: '<S1>/lambda'
       *  Product: '<S7>/Divide'
       */
      AbstractFuelControl_M3_B.Sum1 = AbstractFuelControl_M3_X.lambda_CSTATE /
        rtb_rpmtorads - AbstractFuelControl_M3_P.Constant_Value;

      /* End of Outputs for SubSystem: '<Root>/Model 3' */
      /* End of Outputs for SubSystem: '<S5>/over(under)shoot' */
      break;

     case 1:
      /* Outputs for IfAction SubSystem: '<S5>/RMS error' incorporates:
       *  ActionPort: '<S6>/Action Port'
       */
      /* Outputs for Atomic SubSystem: '<Root>/Model 3' */
      /* Sum: '<S6>/Sum1' incorporates:
       *  Integrator: '<S1>/lambda'
       */
      rtb_rpmtorads = AbstractFuelControl_M3_X.lambda_CSTATE - rtb_rpmtorads;

      /* End of Outputs for SubSystem: '<Root>/Model 3' */

      /* Step: '<S6>/MeasureOn' */
      if (AbstractFuelControl_M3_M->Timing.t[0] <
          AbstractFuelControl_M3_P.MeasureOn_Time) {
        rtb_theta090 = AbstractFuelControl_M3_P.MeasureOn_Y0;
      } else {
        rtb_theta090 = AbstractFuelControl_M3_P.MeasureOn_YFinal;
      }

      /* End of Step: '<S6>/MeasureOn' */

      /* Product: '<S6>/Product' incorporates:
       *  Math: '<S6>/Math Function'
       */
      AbstractFuelControl_M3_B.Product = rtb_rpmtorads * rtb_rpmtorads *
        rtb_theta090;

      /* Sqrt: '<S6>/Sqrt' incorporates:
       *  Constant: '<S6>/Constant'
       *  Integrator: '<S6>/Integrator'
       *  Product: '<S6>/Divide'
       */
      AbstractFuelControl_M3_B.Sqrt = sqrt
        (AbstractFuelControl_M3_X.Integrator_CSTATE /
         (AbstractFuelControl_M3_P.simTime - 10.0));

      /* End of Outputs for SubSystem: '<S5>/RMS error' */
      break;

     case 2:
      break;
    }

    /* End of SwitchCase: '<S5>/Switch Case' */

    /* MultiPortSwitch: '<S5>/Multiport Switch' incorporates:
     *  Constant: '<S2>/spec_num'
     */
    if (AbstractFuelControl_M3_P.spec_num == 1) {
      AbstractFuelControl_M3_B.MultiportSwitch = AbstractFuelControl_M3_B.Sum1;
    } else {
      AbstractFuelControl_M3_B.MultiportSwitch = AbstractFuelControl_M3_B.Sqrt;
    }

    /* End of MultiPortSwitch: '<S5>/Multiport Switch' */
  }

  /* End of Outputs for SubSystem: '<S2>/Calcuate Error' */
  /* End of Outputs for SubSystem: '<Root>/V&V stub system' */

  /* Outport: '<Root>/verificaton measurement' */
  AbstractFuelControl_M3_Y.verificatonmeasurement =
    AbstractFuelControl_M3_B.MultiportSwitch;

  /* DataTypeConversion: '<Root>/Data Type Conversion' */
  AbstractFuelControl_M3_B.ControllerMode = rtb_DataTypeConversion1;

  /* Outport: '<Root>/mode' */
  AbstractFuelControl_M3_Y.mode = AbstractFuelControl_M3_B.ControllerMode;
  if (rtmIsMajorTimeStep(AbstractFuelControl_M3_M)) {
    /* Outport: '<Root>/pedal angle' incorporates:
     *  DiscretePulseGenerator: '<Root>/Pedal Angle (deg)'
     */
    AbstractFuelControl_M3_Y.pedalangle =
      (AbstractFuelControl_M3_DW.clockTickCounter <
       AbstractFuelControl_M3_P.PedalAngledeg_Duty) &&
      (AbstractFuelControl_M3_DW.clockTickCounter >= 0) ?
      AbstractFuelControl_M3_P.PedalAngledeg_Amp : 0.0;

    /* DiscretePulseGenerator: '<Root>/Pedal Angle (deg)' */
    if (AbstractFuelControl_M3_DW.clockTickCounter >=
        AbstractFuelControl_M3_P.PedalAngledeg_Period - 1.0) {
      AbstractFuelControl_M3_DW.clockTickCounter = 0;
    } else {
      AbstractFuelControl_M3_DW.clockTickCounter++;
    }
  }

  if (rtmIsMajorTimeStep(AbstractFuelControl_M3_M)) {
    /* Matfile logging */
    rt_UpdateTXYLogVars(AbstractFuelControl_M3_M->rtwLogInfo,
                        (AbstractFuelControl_M3_M->Timing.t));
  }                                    /* end MajorTimeStep */

  if (rtmIsMajorTimeStep(AbstractFuelControl_M3_M)) {
    /* Update for Atomic SubSystem: '<Root>/Model 3' */
    if (rtmIsMajorTimeStep(AbstractFuelControl_M3_M)) {
      /* Update for Delay: '<S1>/Delay' */
      AbstractFuelControl_M3_DW.Delay_DSTATE = AbstractFuelControl_M3_B.Switch2;

      /* Update for Atomic SubSystem: '<S1>/Startup Mode Latch' */
      /* Update for Delay: '<S4>/Integer Delay1' */
      AbstractFuelControl_M3_DW.IntegerDelay1_DSTATE =
        AbstractFuelControl_M3_B.LogicalOperator;

      /* End of Update for SubSystem: '<S1>/Startup Mode Latch' */

      /* Update for Atomic SubSystem: '<S1>/Sensor Failure Detection Latch' */
      /* Update for Delay: '<S3>/Integer Delay1' */
      AbstractFuelControl_M3_DW.IntegerDelay1_DSTATE_a =
        AbstractFuelControl_M3_B.LogicalOperator_b;

      /* End of Update for SubSystem: '<S1>/Sensor Failure Detection Latch' */
    }

    /* End of Update for SubSystem: '<Root>/Model 3' */
  }                                    /* end MajorTimeStep */

  if (rtmIsMajorTimeStep(AbstractFuelControl_M3_M)) {
    /* signal main to stop simulation */
    {                                  /* Sample time: [0.0s, 0.0s] */
      if ((rtmGetTFinal(AbstractFuelControl_M3_M)!=-1) &&
          !((rtmGetTFinal(AbstractFuelControl_M3_M)-
             (((AbstractFuelControl_M3_M->Timing.clockTick1+
                AbstractFuelControl_M3_M->Timing.clockTickH1* 4294967296.0)) *
              5.0E-5)) > (((AbstractFuelControl_M3_M->Timing.clockTick1+
                            AbstractFuelControl_M3_M->Timing.clockTickH1*
                            4294967296.0)) * 5.0E-5) * (DBL_EPSILON))) {
        rtmSetErrorStatus(AbstractFuelControl_M3_M, "Simulation finished");
      }
    }

    rt_ertODEUpdateContinuousStates(&AbstractFuelControl_M3_M->solverInfo);

    /* Update absolute time for base rate */
    /* The "clockTick0" counts the number of times the code of this task has
     * been executed. The absolute time is the multiplication of "clockTick0"
     * and "Timing.stepSize0". Size of "clockTick0" ensures timer will not
     * overflow during the application lifespan selected.
     * Timer of this task consists of two 32 bit unsigned integers.
     * The two integers represent the low bits Timing.clockTick0 and the high bits
     * Timing.clockTickH0. When the low bit overflows to 0, the high bits increment.
     */
    if (!(++AbstractFuelControl_M3_M->Timing.clockTick0)) {
      ++AbstractFuelControl_M3_M->Timing.clockTickH0;
    }

    AbstractFuelControl_M3_M->Timing.t[0] = rtsiGetSolverStopTime
      (&AbstractFuelControl_M3_M->solverInfo);

    {
      /* Update absolute timer for sample time: [5.0E-5s, 0.0s] */
      /* The "clockTick1" counts the number of times the code of this task has
       * been executed. The resolution of this integer timer is 5.0E-5, which is the step size
       * of the task. Size of "clockTick1" ensures timer will not overflow during the
       * application lifespan selected.
       * Timer of this task consists of two 32 bit unsigned integers.
       * The two integers represent the low bits Timing.clockTick1 and the high bits
       * Timing.clockTickH1. When the low bit overflows to 0, the high bits increment.
       */
      AbstractFuelControl_M3_M->Timing.clockTick1++;
      if (!AbstractFuelControl_M3_M->Timing.clockTick1) {
        AbstractFuelControl_M3_M->Timing.clockTickH1++;
      }
    }
  }                                    /* end MajorTimeStep */
}

/* Derivatives for root system: '<Root>' */
void AbstractFuelControl_M3_derivatives(void)
{
  XDot_AbstractFuelControl_M3_T *_rtXdot;
  _rtXdot = ((XDot_AbstractFuelControl_M3_T *)
             AbstractFuelControl_M3_M->ModelData.derivs);

  /* Derivatives for Atomic SubSystem: '<Root>/Model 3' */
  /* Derivatives for Integrator: '<S1>/p' */
  _rtXdot->p_CSTATE = AbstractFuelControl_M3_B.ODE1;

  /* Derivatives for Integrator: '<S1>/lambda' */
  _rtXdot->lambda_CSTATE = AbstractFuelControl_M3_B.ODE2;

  /* Derivatives for Integrator: '<S1>/pe' */
  _rtXdot->pe_CSTATE = AbstractFuelControl_M3_B.ODE3;

  /* Derivatives for Integrator: '<S1>/i' */
  _rtXdot->i_CSTATE = AbstractFuelControl_M3_B.Switch1;

  /* Derivatives for TransferFcn: '<S1>/Throttle  delay1' */
  _rtXdot->Throttledelay1_CSTATE = 0.0;
  _rtXdot->Throttledelay1_CSTATE += AbstractFuelControl_M3_P.Throttledelay1_A *
    AbstractFuelControl_M3_X.Throttledelay1_CSTATE;
  _rtXdot->Throttledelay1_CSTATE += AbstractFuelControl_M3_Y.pedalangle;

  /* End of Derivatives for SubSystem: '<Root>/Model 3' */

  /* Derivatives for Atomic SubSystem: '<Root>/V&V stub system' */
  /* Derivatives for Enabled SubSystem: '<S2>/Calcuate Error' */
  if (AbstractFuelControl_M3_DW.CalcuateError_MODE) {
    /* Derivatives for SwitchCase: '<S5>/Switch Case' */
    ((XDot_AbstractFuelControl_M3_T *)
      AbstractFuelControl_M3_M->ModelData.derivs)->Integrator_CSTATE = 0.0;
    if (AbstractFuelControl_M3_DW.SwitchCase_ActiveSubsystem == 1) {
      /* Derivatives for IfAction SubSystem: '<S5>/RMS error' incorporates:
       *  Derivatives for ActionPort: '<S6>/Action Port'
       */
      /* Derivatives for Integrator: '<S6>/Integrator' */
      _rtXdot->Integrator_CSTATE = AbstractFuelControl_M3_B.Product;

      /* End of Derivatives for SubSystem: '<S5>/RMS error' */
    }

    /* End of Derivatives for SwitchCase: '<S5>/Switch Case' */
  } else {
    ((XDot_AbstractFuelControl_M3_T *)
      AbstractFuelControl_M3_M->ModelData.derivs)->Integrator_CSTATE = 0.0;
  }

  /* End of Derivatives for SubSystem: '<S2>/Calcuate Error' */
  /* End of Derivatives for SubSystem: '<Root>/V&V stub system' */
}

/* Model initialize function */
void AbstractFuelControl_M3_initialize(void)
{
  /* Registration code */

  /* initialize non-finites */
  rt_InitInfAndNaN(sizeof(real_T));

  /* initialize real-time model */
  (void) memset((void *)AbstractFuelControl_M3_M, 0,
                sizeof(RT_MODEL_AbstractFuelControl__T));

  {
    /* Setup solver object */
    rtsiSetSimTimeStepPtr(&AbstractFuelControl_M3_M->solverInfo,
                          &AbstractFuelControl_M3_M->Timing.simTimeStep);
    rtsiSetTPtr(&AbstractFuelControl_M3_M->solverInfo, &rtmGetTPtr
                (AbstractFuelControl_M3_M));
    rtsiSetStepSizePtr(&AbstractFuelControl_M3_M->solverInfo,
                       &AbstractFuelControl_M3_M->Timing.stepSize0);
    rtsiSetdXPtr(&AbstractFuelControl_M3_M->solverInfo,
                 &AbstractFuelControl_M3_M->ModelData.derivs);
    rtsiSetContStatesPtr(&AbstractFuelControl_M3_M->solverInfo, (real_T **)
                         &AbstractFuelControl_M3_M->ModelData.contStates);
    rtsiSetNumContStatesPtr(&AbstractFuelControl_M3_M->solverInfo,
      &AbstractFuelControl_M3_M->Sizes.numContStates);
    rtsiSetNumPeriodicContStatesPtr(&AbstractFuelControl_M3_M->solverInfo,
      &AbstractFuelControl_M3_M->Sizes.numPeriodicContStates);
    rtsiSetPeriodicContStateIndicesPtr(&AbstractFuelControl_M3_M->solverInfo,
      &AbstractFuelControl_M3_M->ModelData.periodicContStateIndices);
    rtsiSetPeriodicContStateRangesPtr(&AbstractFuelControl_M3_M->solverInfo,
      &AbstractFuelControl_M3_M->ModelData.periodicContStateRanges);
    rtsiSetErrorStatusPtr(&AbstractFuelControl_M3_M->solverInfo,
                          (&rtmGetErrorStatus(AbstractFuelControl_M3_M)));
    rtsiSetRTModelPtr(&AbstractFuelControl_M3_M->solverInfo,
                      AbstractFuelControl_M3_M);
  }

  rtsiSetSimTimeStep(&AbstractFuelControl_M3_M->solverInfo, MAJOR_TIME_STEP);
  AbstractFuelControl_M3_M->ModelData.intgData.y =
    AbstractFuelControl_M3_M->ModelData.odeY;
  AbstractFuelControl_M3_M->ModelData.intgData.f[0] =
    AbstractFuelControl_M3_M->ModelData.odeF[0];
  AbstractFuelControl_M3_M->ModelData.intgData.f[1] =
    AbstractFuelControl_M3_M->ModelData.odeF[1];
  AbstractFuelControl_M3_M->ModelData.intgData.f[2] =
    AbstractFuelControl_M3_M->ModelData.odeF[2];
  AbstractFuelControl_M3_M->ModelData.contStates = ((X_AbstractFuelControl_M3_T *)
    &AbstractFuelControl_M3_X);
  rtsiSetSolverData(&AbstractFuelControl_M3_M->solverInfo, (void *)
                    &AbstractFuelControl_M3_M->ModelData.intgData);
  rtsiSetSolverName(&AbstractFuelControl_M3_M->solverInfo,"ode3");
  rtmSetTPtr(AbstractFuelControl_M3_M, &AbstractFuelControl_M3_M->Timing.tArray
             [0]);
  rtmSetTFinal(AbstractFuelControl_M3_M, 50.0);
  AbstractFuelControl_M3_M->Timing.stepSize0 = 5.0E-5;

  /* Setup for data logging */
  {
    static RTWLogInfo rt_DataLoggingInfo;
    rt_DataLoggingInfo.loggingInterval = NULL;
    AbstractFuelControl_M3_M->rtwLogInfo = &rt_DataLoggingInfo;
  }

  /* Setup for data logging */
  {
    rtliSetLogXSignalInfo(AbstractFuelControl_M3_M->rtwLogInfo, (NULL));
    rtliSetLogXSignalPtrs(AbstractFuelControl_M3_M->rtwLogInfo, (NULL));
    rtliSetLogT(AbstractFuelControl_M3_M->rtwLogInfo, "");
    rtliSetLogX(AbstractFuelControl_M3_M->rtwLogInfo, "");
    rtliSetLogXFinal(AbstractFuelControl_M3_M->rtwLogInfo, "");
    rtliSetLogVarNameModifier(AbstractFuelControl_M3_M->rtwLogInfo, "rt_");
    rtliSetLogFormat(AbstractFuelControl_M3_M->rtwLogInfo, 2);
    rtliSetLogMaxRows(AbstractFuelControl_M3_M->rtwLogInfo, 0);
    rtliSetLogDecimation(AbstractFuelControl_M3_M->rtwLogInfo, 1);
    rtliSetLogY(AbstractFuelControl_M3_M->rtwLogInfo, "");
    rtliSetLogYSignalInfo(AbstractFuelControl_M3_M->rtwLogInfo, (NULL));
    rtliSetLogYSignalPtrs(AbstractFuelControl_M3_M->rtwLogInfo, (NULL));
  }

  /* block I/O */
  (void) memset(((void *) &AbstractFuelControl_M3_B), 0,
                sizeof(B_AbstractFuelControl_M3_T));

  /* states (continuous) */
  {
    (void) memset((void *)&AbstractFuelControl_M3_X, 0,
                  sizeof(X_AbstractFuelControl_M3_T));
  }

  /* states (dwork) */
  (void) memset((void *)&AbstractFuelControl_M3_DW, 0,
                sizeof(DW_AbstractFuelControl_M3_T));

  /* external inputs */
  AbstractFuelControl_M3_U.EngineSpeed = 0.0;

  /* external outputs */
  (void) memset((void *)&AbstractFuelControl_M3_Y, 0,
                sizeof(ExtY_AbstractFuelControl_M3_T));

  /* Matfile logging */
  rt_StartDataLoggingWithStartTime(AbstractFuelControl_M3_M->rtwLogInfo, 0.0,
    rtmGetTFinal(AbstractFuelControl_M3_M),
    AbstractFuelControl_M3_M->Timing.stepSize0, (&rtmGetErrorStatus
    (AbstractFuelControl_M3_M)));

  /* Start for Atomic SubSystem: '<Root>/V&V stub system' */
  /* Start for Enabled SubSystem: '<S2>/Calcuate Error' */
  /* Start for SwitchCase: '<S5>/Switch Case' */
  AbstractFuelControl_M3_DW.SwitchCase_ActiveSubsystem = -1;

  /* InitializeConditions for IfAction SubSystem: '<S5>/RMS error' */
  /* InitializeConditions for Integrator: '<S6>/Integrator' */
  AbstractFuelControl_M3_X.Integrator_CSTATE =
    AbstractFuelControl_M3_P.Integrator_IC;

  /* End of InitializeConditions for SubSystem: '<S5>/RMS error' */
  /* End of Start for SubSystem: '<S2>/Calcuate Error' */
  /* End of Start for SubSystem: '<Root>/V&V stub system' */

  /* Start for DiscretePulseGenerator: '<Root>/Pedal Angle (deg)' */
  AbstractFuelControl_M3_DW.clockTickCounter = -60000;

  /* InitializeConditions for Atomic SubSystem: '<Root>/Model 3' */
  /* InitializeConditions for Delay: '<S1>/Delay' */
  AbstractFuelControl_M3_DW.Delay_DSTATE =
    AbstractFuelControl_M3_P.Delay_InitialCondition;

  /* InitializeConditions for Integrator: '<S1>/p' */
  AbstractFuelControl_M3_X.p_CSTATE = AbstractFuelControl_M3_P.p_IC;

  /* InitializeConditions for Integrator: '<S1>/lambda' */
  AbstractFuelControl_M3_X.lambda_CSTATE = AbstractFuelControl_M3_P.lambda_IC;

  /* InitializeConditions for Integrator: '<S1>/pe' */
  AbstractFuelControl_M3_X.pe_CSTATE = AbstractFuelControl_M3_P.pe_IC;

  /* InitializeConditions for Integrator: '<S1>/i' */
  AbstractFuelControl_M3_X.i_CSTATE = AbstractFuelControl_M3_P.i_IC;

  /* InitializeConditions for TransferFcn: '<S1>/Throttle  delay1' */
  AbstractFuelControl_M3_X.Throttledelay1_CSTATE = 0.0;

  /* InitializeConditions for Atomic SubSystem: '<S1>/Startup Mode Latch' */
  /* InitializeConditions for Delay: '<S4>/Integer Delay1' */
  AbstractFuelControl_M3_DW.IntegerDelay1_DSTATE =
    AbstractFuelControl_M3_P.IntegerDelay1_InitialConditio_e;

  /* End of InitializeConditions for SubSystem: '<S1>/Startup Mode Latch' */

  /* InitializeConditions for Atomic SubSystem: '<S1>/Sensor Failure Detection Latch' */
  /* InitializeConditions for Delay: '<S3>/Integer Delay1' */
  AbstractFuelControl_M3_DW.IntegerDelay1_DSTATE_a =
    AbstractFuelControl_M3_P.IntegerDelay1_InitialCondition;

  /* End of InitializeConditions for SubSystem: '<S1>/Sensor Failure Detection Latch' */
  /* End of InitializeConditions for SubSystem: '<Root>/Model 3' */
}

/* Model terminate function */
void AbstractFuelControl_M3_terminate(void)
{
  /* (no terminate code required) */
}
