/*
 * AbstractFuelControl_M1.c
 *
 * Academic License - for use in teaching, academic research, and meeting
 * course requirements at degree granting institutions only.  Not for
 * government, commercial, or other organizational use.
 *
 * Code generation for model "AbstractFuelControl_M1".
 *
 * Model version              : 1.388
 * Simulink Coder version : 8.9 (R2015b) 13-Aug-2015
 * C source code generated on : Fri Feb 12 15:59:20 2016
 *
 * Target selection: grt.tlc
 * Note: GRT includes extra infrastructure and instrumentation for prototyping
 * Embedded hardware selection: 32-bit Generic
 * Code generation objectives: Unspecified
 * Validation result: Not run
 */

#include "AbstractFuelControl_M1.h"
#include "AbstractFuelControl_M1_private.h"

/* Block signals (auto storage) */
B_AbstractFuelControl_M1_T AbstractFuelControl_M1_B;

/* Continuous states */
X_AbstractFuelControl_M1_T AbstractFuelControl_M1_X;

/* Block states (auto storage) */
DW_AbstractFuelControl_M1_T AbstractFuelControl_M1_DW;

/* Previous zero-crossings (trigger) states */
PrevZCX_AbstractFuelControl_M_T AbstractFuelControl_M1_PrevZCX;

/* External inputs (root inport signals with auto storage) */
ExtU_AbstractFuelControl_M1_T AbstractFuelControl_M1_U;

/* External outputs (root outports fed by signals with auto storage) */
ExtY_AbstractFuelControl_M1_T AbstractFuelControl_M1_Y;

/* Real-time model */
RT_MODEL_AbstractFuelControl__T AbstractFuelControl_M1_M_;
RT_MODEL_AbstractFuelControl__T *const AbstractFuelControl_M1_M =
  &AbstractFuelControl_M1_M_;

/*
 * Writes out MAT-file header.  Returns success or failure.
 * Returns:
 *      0 - success
 *      1 - failure
 */
int_T rt_WriteMat4FileHeader(FILE *fp, int32_T m, int32_T n, const char *name)
{
  typedef enum { ELITTLE_ENDIAN, EBIG_ENDIAN } ByteOrder;

  int16_T one = 1;
  ByteOrder byteOrder = (*((int8_T *)&one)==1) ? ELITTLE_ENDIAN : EBIG_ENDIAN;
  int32_T type = (byteOrder == ELITTLE_ENDIAN) ? 0: 1000;
  int32_T imagf = 0;
  int32_T name_len = (int32_T)strlen(name) + 1;
  if ((fwrite(&type, sizeof(int32_T), 1, fp) == 0) ||
      (fwrite(&m, sizeof(int32_T), 1, fp) == 0) ||
      (fwrite(&n, sizeof(int32_T), 1, fp) == 0) ||
      (fwrite(&imagf, sizeof(int32_T), 1, fp) == 0) ||
      (fwrite(&name_len, sizeof(int32_T), 1, fp) == 0) ||
      (fwrite(name, sizeof(char), name_len, fp) == 0)) {
    return(1);
  } else {
    return(0);
  }
}                                      /* end rt_WriteMat4FileHeader */

/* For variable transport delay block, find the real delay time */
real_T rt_VTDelayfindtDInterpolate(
  real_T x,real_T* tBuf,real_T* uBuf,real_T* xBuf,int_T bufSz,int_T head,int_T
  tail,int_T* pLast,real_T t,real_T tStart,boolean_T discrete,boolean_T
  minorStepAndTAtLastMajorOutput,real_T initOutput,real_T* appliedDelay)
{
  int_T n, k;
  real_T f;
  int_T kp1;
  real_T tminustD, tL, tR, uD, uL, uR, fU;
  if (minorStepAndTAtLastMajorOutput) {
    /* pretend that the entry at head has not been added */
    if (*pLast == head) {
      *pLast = (*pLast == 0) ? bufSz-1 : *pLast-1;
    }

    head = (head == 0) ? bufSz-1 : head-1;
  }

  /*
   * The loop below finds k such that:
   *      x(t)-x(tminustD) =1 or
   *      x - xBuf[k+1] <= 1.0 < x - xBuf[k]
   *
   * Note that we have:
   *
   * tStart = tBuf[0] < tBuf[1] < ... < tBuf[tail] < ... tBuf[head] <= t
   *      0 = xBuf[0] < xBuf[1] < ... < xBuf[tail] < ... xBuf[head] <  x
   *
   * This is true if we assume the direction of transport is always positive
   * such as a flow goes through a pipe from one end to another. However, for
   * model such as convey belt, the transportation can change direction. For
   * this case, there will be more than one solution to x(t)-x(tminustD) = 1,
   * should found the minimum tminustD and tminustD > 0. The search will not
   * be as efficient as the following code.
   */

  /*
   * when x<=1, physically it means the flow didn't reach the output yet,
   * t-tD will be less then zero, so force output to be the initial output
   */
  if (x <= 1) {
    return initOutput;
  }

  /*
   * if the x is monoton increase, only one solution. use k=pLast for now
   */
  k= *pLast;
  n = 0;
  for (;;) {
    n++;
    if (n>bufSz)
      break;
    if (x - xBuf[k] > 1.0) {
      /* move k forward, unless k = head */
      if (k == head) {
        /* xxx this situation means tD= appliedDelay = 0
         *
         * linearly interpolate using (tBuf[head], xBuf[head])
         * and (t,x) to find (tD,xD) such that: x - xD = 1.0
         */
        int_T km1;
        f = (x - 1.0 - xBuf[k]) / (x - xBuf[k]);
        tminustD = (1.0-f)*tBuf[k] + f*t;
        km1 = k-1;
        if (km1 < 0)
          km1 = bufSz-1;
        tL = tBuf[km1];
        tR = tBuf[k];
        uL = uBuf[km1];
        uR = uBuf[k];
        break;
      }

      kp1 = k+1;
      if (kp1 == bufSz)
        kp1 = 0;
      if (x - xBuf[kp1] <= 1.0) {
        /*
         * linearly interpolate using (tBuf[k], xBuf[k])
         * and  (tBuf[k+1], xBuf[k+1]) to find (tminustD,xD)
         * such that: x - xD = 1.0
         */
        f = (x - 1.0 - xBuf[k]) / (xBuf[kp1] - xBuf[k]);
        tL = tBuf[k];
        tR = tBuf[kp1];
        uL = uBuf[k];
        uR = uBuf[kp1];
        tminustD = (1.0-f)*tL + f*tR;
        break;
      }

      k = kp1;
    } else {
      /* moved k backward, unless k = tail */
      if (k == tail) {
        /* This situation means tminustD <= Ttail*/
        f = (x - 1.0)/xBuf[k];
        if (discrete) {
          return(uBuf[tail]);
        }

        kp1 = k+1;
        if (kp1 == bufSz)
          kp1 = 0;

        /* * linearly interpolate using (tStart, 0)
         * and  (tBuf[tail], xBuf[tail]) to find (tminustD,xD)
         * such that: x - xD = 1.0
         */

        /* Here it is better to use Tstart because since x>1, tminustD
         * must > 0. Since x is monotone increase, its linearity is
         * better.
         */
        tminustD = (1-f)*tStart + f*tBuf[k];

        /* linearly interpolate using (t[tail], x[tail])
         * and  (tBuf[tail+1], xBuf[tail+1]) to find (tminustD,xD)
         * such that: x - xD = 1.0.
         * For time delay block, use t[tail] and t[tail+1], not good
         * for transport delay block since it may give tminstD < 0
         */

        /*  f = (tBuf[kp1]-tBuf[k])/(xBuf[kp1]-xBuf[k]);
         *  tminustD = tBuf[kp1]-f*(1+xBuf[kp1]-x);
         */
        tL = tBuf[k];
        tR = tBuf[kp1];
        uL = uBuf[k];
        uR = uBuf[kp1];
        break;
      }

      k = k - 1;
      if (k < 0)
        k = bufSz-1;
    }
  }

  *pLast = k;
  if (tR == tL) {
    fU = 1.0;
  } else {
    fU = (tminustD-tL)/(tR-tL);
  }

  /* for discrete signal, no interpolation, use either uL or uR
   * depend on wehre tminustD is.
   */
  if (discrete) {
    uD= (fU > (1.0-fU))? uR: uL;
  } else {
    uD = (1.0-fU)*uL + fU*uR;
  }

  /* we want return tD= t-(t-tD);*/
  *appliedDelay = t-tminustD;
  return uD;
}

real_T look2_binlxpw(real_T u0, real_T u1, const real_T bp0[], const real_T bp1[],
                     const real_T table[], const uint32_T maxIndex[], uint32_T
                     stride)
{
  real_T frac;
  uint32_T bpIndices[2];
  real_T fractions[2];
  real_T yL_1d;
  uint32_T iRght;
  uint32_T bpIdx;
  uint32_T iLeft;

  /* Lookup 2-D
     Search method: 'binary'
     Use previous index: 'off'
     Interpolation method: 'Linear'
     Extrapolation method: 'Linear'
     Use last breakpoint for index at or above upper limit: 'off'
     Remove protection against out-of-range input in generated code: 'off'
   */
  /* Prelookup - Index and Fraction
     Index Search method: 'binary'
     Extrapolation method: 'Linear'
     Use previous index: 'off'
     Use last breakpoint for index at or above upper limit: 'off'
     Remove protection against out-of-range input in generated code: 'off'
   */
  if (u0 <= bp0[0U]) {
    iLeft = 0U;
    frac = (u0 - bp0[0U]) / (bp0[1U] - bp0[0U]);
  } else if (u0 < bp0[maxIndex[0U]]) {
    /* Binary Search */
    bpIdx = maxIndex[0U] >> 1U;
    iLeft = 0U;
    iRght = maxIndex[0U];
    while (iRght - iLeft > 1U) {
      if (u0 < bp0[bpIdx]) {
        iRght = bpIdx;
      } else {
        iLeft = bpIdx;
      }

      bpIdx = (iRght + iLeft) >> 1U;
    }

    frac = (u0 - bp0[iLeft]) / (bp0[iLeft + 1U] - bp0[iLeft]);
  } else {
    iLeft = maxIndex[0U] - 1U;
    frac = (u0 - bp0[maxIndex[0U] - 1U]) / (bp0[maxIndex[0U]] - bp0[maxIndex[0U]
      - 1U]);
  }

  fractions[0U] = frac;
  bpIndices[0U] = iLeft;

  /* Prelookup - Index and Fraction
     Index Search method: 'binary'
     Extrapolation method: 'Linear'
     Use previous index: 'off'
     Use last breakpoint for index at or above upper limit: 'off'
     Remove protection against out-of-range input in generated code: 'off'
   */
  if (u1 <= bp1[0U]) {
    iLeft = 0U;
    frac = (u1 - bp1[0U]) / (bp1[1U] - bp1[0U]);
  } else if (u1 < bp1[maxIndex[1U]]) {
    /* Binary Search */
    bpIdx = maxIndex[1U] >> 1U;
    iLeft = 0U;
    iRght = maxIndex[1U];
    while (iRght - iLeft > 1U) {
      if (u1 < bp1[bpIdx]) {
        iRght = bpIdx;
      } else {
        iLeft = bpIdx;
      }

      bpIdx = (iRght + iLeft) >> 1U;
    }

    frac = (u1 - bp1[iLeft]) / (bp1[iLeft + 1U] - bp1[iLeft]);
  } else {
    iLeft = maxIndex[1U] - 1U;
    frac = (u1 - bp1[maxIndex[1U] - 1U]) / (bp1[maxIndex[1U]] - bp1[maxIndex[1U]
      - 1U]);
  }

  /* Interpolation 2-D
     Interpolation method: 'Linear'
     Use last breakpoint for index at or above upper limit: 'off'
     Overflow mode: 'portable wrapping'
   */
  bpIdx = iLeft * stride + bpIndices[0U];
  yL_1d = (table[bpIdx + 1U] - table[bpIdx]) * fractions[0U] + table[bpIdx];
  bpIdx += stride;
  return (((table[bpIdx + 1U] - table[bpIdx]) * fractions[0U] + table[bpIdx]) -
          yL_1d) * frac + yL_1d;
}

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
  int_T nXc = 7;
  rtsiSetSimTimeStep(si,MINOR_TIME_STEP);

  /* Save the state values at time t in y, we'll use x as ynew. */
  (void) memcpy(y, x,
                (uint_T)nXc*sizeof(real_T));

  /* Assumes that rtsiSetT and ModelOutputs are up-to-date */
  /* f0 = f(t,y) */
  rtsiSetdX(si, f0);
  AbstractFuelControl_M1_derivatives();

  /* f(:,2) = feval(odefile, t + hA(1), y + f*hB(:,1), args(:)(*)); */
  hB[0] = h * rt_ODE3_B[0][0];
  for (i = 0; i < nXc; i++) {
    x[i] = y[i] + (f0[i]*hB[0]);
  }

  rtsiSetT(si, t + h*rt_ODE3_A[0]);
  rtsiSetdX(si, f1);
  AbstractFuelControl_M1_output();
  AbstractFuelControl_M1_derivatives();

  /* f(:,3) = feval(odefile, t + hA(2), y + f*hB(:,2), args(:)(*)); */
  for (i = 0; i <= 1; i++) {
    hB[i] = h * rt_ODE3_B[1][i];
  }

  for (i = 0; i < nXc; i++) {
    x[i] = y[i] + (f0[i]*hB[0] + f1[i]*hB[1]);
  }

  rtsiSetT(si, t + h*rt_ODE3_A[1]);
  rtsiSetdX(si, f2);
  AbstractFuelControl_M1_output();
  AbstractFuelControl_M1_derivatives();

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

/* Model output function */
void AbstractFuelControl_M1_output(void)
{
  /* local block i/o variables */
  real_T rtb_fuelsystemtransportdelay;
  real_T rtb_ControllerMode;
  boolean_T rtb_DataStoreRead1_l;
  real_T rtb_c_air_chrggcyl;
  real_T rtb_radstorpm;
  real_T rtb_Gain2;
  real_T rtb_Integrator;
  real_T rtb_Kappatolerance0911;
  real_T rtb_fuelinjectortolerance095105;
  real_T rtb_fuel_puddle_evap;
  real_T rtb_pratio;
  int8_T rtAction;
  ZCEventType zcEvent;
  real32_T rtb_Switch_b;
  boolean_T LogicalOperator_a;
  boolean_T LogicalOperator;
  boolean_T RelationalOperator1;
  real32_T Sum_j;
  if (rtmIsMajorTimeStep(AbstractFuelControl_M1_M)) {
    /* set solver stop time */
    if (!(AbstractFuelControl_M1_M->Timing.clockTick0+1)) {
      rtsiSetSolverStopTime(&AbstractFuelControl_M1_M->solverInfo,
                            ((AbstractFuelControl_M1_M->Timing.clockTickH0 + 1) *
        AbstractFuelControl_M1_M->Timing.stepSize0 * 4294967296.0));
    } else {
      rtsiSetSolverStopTime(&AbstractFuelControl_M1_M->solverInfo,
                            ((AbstractFuelControl_M1_M->Timing.clockTick0 + 1) *
        AbstractFuelControl_M1_M->Timing.stepSize0 +
        AbstractFuelControl_M1_M->Timing.clockTickH0 *
        AbstractFuelControl_M1_M->Timing.stepSize0 * 4294967296.0));
    }
  }                                    /* end MajorTimeStep */

  /* Update absolute time of base rate at minor time step */
  if (rtmIsMinorTimeStep(AbstractFuelControl_M1_M)) {
    AbstractFuelControl_M1_M->Timing.t[0] = rtsiGetT
      (&AbstractFuelControl_M1_M->solverInfo);
  }

  /* Outputs for Atomic SubSystem: '<Root>/Model 1' */
  /* Gain: '<S18>/A//F_sensor' incorporates:
   *  Integrator: '<S20>/Integrator'
   */
  AbstractFuelControl_M1_B.AF_sensor = AbstractFuelControl_M1_P.AF_sensor_Gain *
    AbstractFuelControl_M1_X.Integrator_CSTATE_m;

  /* Sum: '<S1>/Sum' incorporates:
   *  Constant: '<S1>/Base opening angle'
   *  TransferFcn: '<S1>/Throttle delay'
   */
  rtb_radstorpm = AbstractFuelControl_M1_P.Throttledelay_C *
    AbstractFuelControl_M1_X.Throttledelay_CSTATE +
    AbstractFuelControl_M1_P.Baseopeningangle_Value;

  /* Saturate: '<S1>/theta [0 90]' */
  if (rtb_radstorpm > AbstractFuelControl_M1_P.theta090_UpperSat) {
    AbstractFuelControl_M1_B.theta090 =
      AbstractFuelControl_M1_P.theta090_UpperSat;
  } else if (rtb_radstorpm < AbstractFuelControl_M1_P.theta090_LowerSat) {
    AbstractFuelControl_M1_B.theta090 =
      AbstractFuelControl_M1_P.theta090_LowerSat;
  } else {
    AbstractFuelControl_M1_B.theta090 = rtb_radstorpm;
  }

  /* End of Saturate: '<S1>/theta [0 90]' */
  if (rtmIsMajorTimeStep(AbstractFuelControl_M1_M)) {
  }

  /* Saturate: '<S1>/Engine Speed [900 1100]' incorporates:
   *  Inport: '<Root>/Engine Speed (rpm)'
   */
  if (AbstractFuelControl_M1_U.EngineSpeed >
      AbstractFuelControl_M1_P.EngineSpeed9001100_UpperSat) {
    rtb_Integrator = AbstractFuelControl_M1_P.EngineSpeed9001100_UpperSat;
  } else if (AbstractFuelControl_M1_U.EngineSpeed <
             AbstractFuelControl_M1_P.EngineSpeed9001100_LowerSat) {
    rtb_Integrator = AbstractFuelControl_M1_P.EngineSpeed9001100_LowerSat;
  } else {
    rtb_Integrator = AbstractFuelControl_M1_U.EngineSpeed;
  }

  /* Gain: '<S1>/(rpm) to (rad//s)' incorporates:
   *  Saturate: '<S1>/Engine Speed [900 1100]'
   */
  rtb_radstorpm = AbstractFuelControl_M1_P.rpmtorads_Gain * rtb_Integrator;

  /* Integrator: '<S5>/p0 = 0.543 (bar)' */
  rtb_fuelsystemtransportdelay = AbstractFuelControl_M1_X.p00543bar_CSTATE;

  /* Product: '<S6>/Product1' incorporates:
   *  Constant: '<S1>/Atmospheric Pressure (bar)'
   */
  rtb_Integrator = rtb_fuelsystemtransportdelay /
    AbstractFuelControl_M1_P.AtmosphericPressurebar_Value;

  /* Product: '<S6>/Product2' incorporates:
   *  Constant: '<S1>/Atmospheric Pressure (bar)'
   */
  rtb_pratio = 1.0 / rtb_fuelsystemtransportdelay *
    AbstractFuelControl_M1_P.AtmosphericPressurebar_Value;

  /* MinMax: '<S6>/MinMax' */
  if ((rtb_Integrator <= rtb_pratio) || rtIsNaN(rtb_pratio)) {
    rtb_pratio = rtb_Integrator;
  }

  /* End of MinMax: '<S6>/MinMax' */

  /* Switch: '<S6>/Switch' incorporates:
   *  Constant: '<S6>/Sonic Flow '
   *  Fcn: '<S6>/g(pratio)'
   */
  if (rtb_pratio >= AbstractFuelControl_M1_P.Switch_Threshold_g) {
    /* Fcn: '<S6>/g(pratio)' */
    rtb_pratio -= rtb_pratio * rtb_pratio;
    if (rtb_pratio < 0.0) {
      rtb_pratio = -sqrt(-rtb_pratio);
    } else {
      rtb_pratio = sqrt(rtb_pratio);
    }

    rtb_pratio *= 2.0;
  } else {
    rtb_pratio = AbstractFuelControl_M1_P.SonicFlow_Value;
  }

  /* End of Switch: '<S6>/Switch' */

  /* Sum: '<S6>/Sum' incorporates:
   *  Constant: '<S1>/Atmospheric Pressure (bar)'
   */
  rtb_Integrator = AbstractFuelControl_M1_P.AtmosphericPressurebar_Value -
    rtb_fuelsystemtransportdelay;

  /* Signum: '<S6>/flow direction' */
  if (rtb_Integrator < 0.0) {
    rtb_Integrator = -1.0;
  } else if (rtb_Integrator > 0.0) {
    rtb_Integrator = 1.0;
  } else {
    if (rtb_Integrator == 0.0) {
      rtb_Integrator = 0.0;
    }
  }

  /* Product: '<S6>/Product' incorporates:
   *  Fcn: '<S6>/f(theta)'
   *  Signum: '<S6>/flow direction'
   */
  rtb_pratio = (((2.821 - 0.05231 * AbstractFuelControl_M1_B.theta090) + 0.10299
                 * AbstractFuelControl_M1_B.theta090 *
                 AbstractFuelControl_M1_B.theta090) - 0.00063 *
                AbstractFuelControl_M1_B.theta090 *
                AbstractFuelControl_M1_B.theta090 *
                AbstractFuelControl_M1_B.theta090) * rtb_pratio * rtb_Integrator;

  /* Outputs for Atomic SubSystem: '<S1>/AF_Controller' */
  /* Step: '<S3>/Pwon' */
  if (AbstractFuelControl_M1_M->Timing.t[0] < AbstractFuelControl_M1_P.Pwon_Time)
  {
    AbstractFuelControl_M1_B.Pwon = AbstractFuelControl_M1_P.Pwon_Y0;
  } else {
    AbstractFuelControl_M1_B.Pwon = AbstractFuelControl_M1_P.Pwon_YFinal;
  }

  /* End of Step: '<S3>/Pwon' */
  if (rtmIsMajorTimeStep(AbstractFuelControl_M1_M)) {
    /* DiscretePulseGenerator: '<S3>/PulseGenerator_10ms' */
    AbstractFuelControl_M1_B.PulseGenerator_10ms =
      (AbstractFuelControl_M1_DW.clockTickCounter_l <
       AbstractFuelControl_M1_P.PulseGenerator_10ms_Duty) &&
      (AbstractFuelControl_M1_DW.clockTickCounter_l >= 0) ?
      AbstractFuelControl_M1_P.PulseGenerator_10ms_Amp : 0.0;
    if (AbstractFuelControl_M1_DW.clockTickCounter_l >=
        AbstractFuelControl_M1_P.PulseGenerator_10ms_Period - 1.0) {
      AbstractFuelControl_M1_DW.clockTickCounter_l = 0;
    } else {
      AbstractFuelControl_M1_DW.clockTickCounter_l++;
    }

    /* End of DiscretePulseGenerator: '<S3>/PulseGenerator_10ms' */
  }

  /* Outputs for Atomic SubSystem: '<S3>/fuel_controller' */
  /* DataStoreWrite: '<S8>/DataStoreWrite' incorporates:
   *  DataTypeConversion: '<S1>/Data Type Conversion1'
   */
  AbstractFuelControl_M1_DW.engine_speed = (real32_T)rtb_radstorpm;

  /* DataStoreWrite: '<S8>/DataStoreWrite3' incorporates:
   *  DataTypeConversion: '<S1>/Data Type Conversion4'
   */
  AbstractFuelControl_M1_DW.throttle_angle = (real32_T)
    AbstractFuelControl_M1_B.theta090;

  /* DataStoreWrite: '<S8>/DataStoreWrite1' incorporates:
   *  DataTypeConversion: '<S1>/Data Type Conversion2'
   *  Gain: '<S1>/MAF sensor tolerance [0.95 1.05]'
   */
  AbstractFuelControl_M1_DW.throttle_flow = (real32_T)
    (AbstractFuelControl_M1_P.MAF_sensor_tol * rtb_pratio);

  /* End of Outputs for SubSystem: '<S3>/fuel_controller' */
  /* End of Outputs for SubSystem: '<S1>/AF_Controller' */

  /* Step: '<S1>/A//F Sensor Fault Injection' */
  if (AbstractFuelControl_M1_M->Timing.t[0] <
      AbstractFuelControl_M1_P.fault_time) {
    rtb_Integrator = AbstractFuelControl_M1_P.AFSensorFaultInjection_Y0;
  } else {
    rtb_Integrator = AbstractFuelControl_M1_P.AFSensorFaultInjection_YFinal;
  }

  /* End of Step: '<S1>/A//F Sensor Fault Injection' */

  /* Switch: '<S4>/Switch' incorporates:
   *  Constant: '<S4>/FaultySensorOutput'
   */
  if (rtb_Integrator >= AbstractFuelControl_M1_P.Switch_Threshold) {
    rtb_Integrator = AbstractFuelControl_M1_P.FaultySensorOutput_Value;
  } else {
    rtb_Integrator = AbstractFuelControl_M1_B.AF_sensor;
  }

  /* End of Switch: '<S4>/Switch' */

  /* Outputs for Atomic SubSystem: '<S1>/AF_Controller' */
  /* Outputs for Atomic SubSystem: '<S3>/fuel_controller' */
  /* DataStoreWrite: '<S8>/DataStoreWrite2' incorporates:
   *  DataTypeConversion: '<S1>/Data Type Conversion3'
   *  Gain: '<S1>/A//F sensor tolerance [0.99 1.01]'
   */
  AbstractFuelControl_M1_DW.airbyfuel_meas = (real32_T)
    (AbstractFuelControl_M1_P.AF_sensor_tol * rtb_Integrator);

  /* Outputs for Triggered SubSystem: '<S8>/fuel_controller_10ms' incorporates:
   *  TriggerPort: '<S9>/Function'
   */
  /* Outputs for Triggered SubSystem: '<S8>/fuel_controller_mode_10ms' incorporates:
   *  TriggerPort: '<S10>/Function'
   */
  /* Outputs for Triggered SubSystem: '<S8>/fuel_controller_pwon' incorporates:
   *  TriggerPort: '<S11>/Function'
   */
  if (rtmIsMajorTimeStep(AbstractFuelControl_M1_M) && rtmIsMajorTimeStep
      (AbstractFuelControl_M1_M)) {
    zcEvent = rt_ZCFcn(RISING_ZERO_CROSSING,
                       &AbstractFuelControl_M1_PrevZCX.fuel_controller_pwon_Trig_ZCE,
                       (AbstractFuelControl_M1_B.Pwon));
    if (zcEvent != NO_ZCEVENT) {
      /* DataStoreWrite: '<S11>/DataStoreWrite1' incorporates:
       *  Constant: '<S11>/Constant1'
       *  DataTypeConversion: '<S11>/Data Type Conversion'
       */
      AbstractFuelControl_M1_DW.controller_mode =
        (AbstractFuelControl_M1_P.Constant1_Value_l != 0.0F);

      /* DataStoreWrite: '<S11>/DataStoreWrite' incorporates:
       *  Constant: '<S11>/Constant2'
       */
      AbstractFuelControl_M1_DW.commanded_fuel =
        AbstractFuelControl_M1_P.Constant2_Value_k;

      /* DataStoreWrite: '<S11>/DataStoreWrite2' incorporates:
       *  Constant: '<S11>/Constant3'
       */
      AbstractFuelControl_M1_DW.airbyfuel_ref =
        AbstractFuelControl_M1_P.Constant3_Value_o;
    }

    zcEvent = rt_ZCFcn(RISING_ZERO_CROSSING,
                       &AbstractFuelControl_M1_PrevZCX.fuel_controller_mode_10ms_Trig_,
                       (AbstractFuelControl_M1_B.PulseGenerator_10ms));
    if (zcEvent != NO_ZCEVENT) {
      /* Outputs for Atomic SubSystem: '<S10>/sensor_failure_detection' */
      /* Logic: '<S17>/Logical Operator' incorporates:
       *  Constant: '<S17>/threshold'
       *  DataStoreRead: '<S10>/DataStoreRead2'
       *  RelationalOperator: '<S17>/Relational Operator'
       *  UnitDelay: '<S17>/Unit Delay'
       */
      LogicalOperator = ((AbstractFuelControl_M1_DW.airbyfuel_meas <=
                          AbstractFuelControl_M1_P.threshold_Value) ||
                         AbstractFuelControl_M1_DW.UnitDelay_DSTATE);

      /* End of Outputs for SubSystem: '<S10>/sensor_failure_detection' */

      /* Outputs for Atomic SubSystem: '<S10>/normal_mode_detection' */
      /* Sum: '<S15>/Sum' incorporates:
       *  Constant: '<S15>/sampling_sec'
       *  UnitDelay: '<S15>/Unit Delay2'
       */
      Sum_j = AbstractFuelControl_M1_DW.UnitDelay2_DSTATE +
        AbstractFuelControl_M1_P.sampling_sec_Value;

      /* Logic: '<S15>/Logical Operator' incorporates:
       *  Constant: '<S15>/normal_mode_start_sec'
       *  RelationalOperator: '<S15>/Relational Operator'
       *  UnitDelay: '<S15>/Unit Delay1'
       */
      LogicalOperator_a = ((Sum_j >=
                            AbstractFuelControl_M1_P.normal_mode_start_sec_Value)
                           || AbstractFuelControl_M1_DW.UnitDelay1_DSTATE_e);

      /* End of Outputs for SubSystem: '<S10>/normal_mode_detection' */

      /* Outputs for Atomic SubSystem: '<S10>/power_mode_detection' */
      /* Switch: '<S16>/Switch' incorporates:
       *  Constant: '<S16>/Constant'
       *  Constant: '<S16>/Constant1'
       *  Sum: '<S16>/Sum'
       *  UnitDelay: '<S16>/Unit Delay1'
       */
      if (AbstractFuelControl_M1_DW.UnitDelay1_DSTATE_a) {
        rtb_Switch_b = AbstractFuelControl_M1_P.Constant_Value_d;
      } else {
        rtb_Switch_b = AbstractFuelControl_M1_P.Constant_Value_d +
          AbstractFuelControl_M1_P.Constant1_Value_f;
      }

      /* End of Switch: '<S16>/Switch' */

      /* RelationalOperator: '<S16>/Relational Operator1' incorporates:
       *  DataStoreRead: '<S10>/DataStoreRead4'
       */
      RelationalOperator1 = (AbstractFuelControl_M1_DW.throttle_angle >=
        rtb_Switch_b);

      /* End of Outputs for SubSystem: '<S10>/power_mode_detection' */

      /* DataStoreWrite: '<S10>/DataStoreWrite' incorporates:
       *  Logic: '<S10>/Logical Operator1'
       *  Logic: '<S10>/Logical Operator2'
       */
      AbstractFuelControl_M1_DW.controller_mode = (LogicalOperator ||
        (!LogicalOperator_a) || RelationalOperator1);

      /* Switch: '<S10>/Switch' incorporates:
       *  Logic: '<S10>/Logical Operator3'
       */
      if (LogicalOperator_a && RelationalOperator1) {
        /* DataStoreWrite: '<S10>/DataStoreWrite1' incorporates:
         *  Constant: '<S10>/airbyfuel_reference_power'
         */
        AbstractFuelControl_M1_DW.airbyfuel_ref =
          AbstractFuelControl_M1_P.airbyfuel_reference_power_Value;
      } else {
        /* DataStoreWrite: '<S10>/DataStoreWrite1' incorporates:
         *  Constant: '<S10>/airbyfuel_reference'
         */
        AbstractFuelControl_M1_DW.airbyfuel_ref =
          AbstractFuelControl_M1_P.airbyfuel_reference_Value;
      }

      /* End of Switch: '<S10>/Switch' */

      /* Update for Atomic SubSystem: '<S10>/sensor_failure_detection' */
      /* Update for UnitDelay: '<S17>/Unit Delay' */
      AbstractFuelControl_M1_DW.UnitDelay_DSTATE = LogicalOperator;

      /* End of Update for SubSystem: '<S10>/sensor_failure_detection' */

      /* Update for Atomic SubSystem: '<S10>/normal_mode_detection' */
      /* Update for UnitDelay: '<S15>/Unit Delay2' */
      AbstractFuelControl_M1_DW.UnitDelay2_DSTATE = Sum_j;

      /* Update for UnitDelay: '<S15>/Unit Delay1' */
      AbstractFuelControl_M1_DW.UnitDelay1_DSTATE_e = LogicalOperator_a;

      /* End of Update for SubSystem: '<S10>/normal_mode_detection' */

      /* Update for Atomic SubSystem: '<S10>/power_mode_detection' */
      /* Update for UnitDelay: '<S16>/Unit Delay1' */
      AbstractFuelControl_M1_DW.UnitDelay1_DSTATE_a = RelationalOperator1;

      /* End of Update for SubSystem: '<S10>/power_mode_detection' */
    }

    zcEvent = rt_ZCFcn(RISING_ZERO_CROSSING,
                       &AbstractFuelControl_M1_PrevZCX.fuel_controller_10ms_Trig_ZCE,
                       (AbstractFuelControl_M1_B.PulseGenerator_10ms));
    if (zcEvent != NO_ZCEVENT) {
      /* Outputs for Atomic SubSystem: '<S9>/air_estimation' */
      /* Sum: '<S12>/Sum3' incorporates:
       *  Constant: '<S12>/Constant2'
       *  Constant: '<S12>/Constant3'
       *  Constant: '<S12>/Constant4'
       *  Constant: '<S12>/Constant5'
       *  DataStoreRead: '<S9>/DataStoreRead1'
       *  Product: '<S12>/Prod2'
       *  Product: '<S12>/Prod3'
       *  Product: '<S12>/Prod4'
       *  UnitDelay: '<S12>/UnitDelay1'
       */
      Sum_j = ((AbstractFuelControl_M1_DW.UnitDelay1_DSTATE_d *
                AbstractFuelControl_M1_DW.engine_speed *
                AbstractFuelControl_M1_P.Constant3_Value_c +
                AbstractFuelControl_M1_P.Constant2_Value_d) +
               AbstractFuelControl_M1_DW.UnitDelay1_DSTATE_d *
               AbstractFuelControl_M1_DW.UnitDelay1_DSTATE_d *
               AbstractFuelControl_M1_DW.engine_speed *
               AbstractFuelControl_M1_P.Constant4_Value) +
        AbstractFuelControl_M1_DW.engine_speed *
        AbstractFuelControl_M1_DW.engine_speed *
        AbstractFuelControl_M1_DW.UnitDelay1_DSTATE_d *
        AbstractFuelControl_M1_P.Constant5_Value;

      /* End of Outputs for SubSystem: '<S9>/air_estimation' */

      /* Logic: '<S9>/Logical Operator2' incorporates:
       *  DataStoreRead: '<S9>/DataStoreRead3'
       */
      LogicalOperator = !AbstractFuelControl_M1_DW.controller_mode;

      /* Outputs for Enabled SubSystem: '<S9>/feedback_PI_controller' incorporates:
       *  EnablePort: '<S13>/Enable'
       */
      if (LogicalOperator) {
        /* Sum: '<S13>/Sum1' incorporates:
         *  DataStoreRead: '<S9>/DataStoreRead2'
         *  DataStoreRead: '<S9>/DataStoreRead4'
         */
        rtb_Switch_b = AbstractFuelControl_M1_DW.airbyfuel_meas -
          AbstractFuelControl_M1_DW.airbyfuel_ref;

        /* Sum: '<S13>/Sum2' incorporates:
         *  Constant: '<S13>/Constant1'
         *  Gain: '<S13>/Gain1'
         *  Product: '<S13>/Prod1'
         *  UnitDelay: '<S13>/UnitDelay1'
         */
        AbstractFuelControl_M1_B.Sum2 = AbstractFuelControl_M1_P.Gain1_Gain_a *
          rtb_Switch_b * AbstractFuelControl_M1_P.Constant1_Value_h +
          AbstractFuelControl_M1_DW.UnitDelay1_DSTATE;

        /* Sum: '<S13>/Sum3' incorporates:
         *  Gain: '<S13>/Gain'
         */
        AbstractFuelControl_M1_B.Sum3 = AbstractFuelControl_M1_P.Gain_Gain_c *
          rtb_Switch_b + AbstractFuelControl_M1_B.Sum2;
      }

      /* End of Outputs for SubSystem: '<S9>/feedback_PI_controller' */

      /* Switch: '<S9>/Switch' incorporates:
       *  Constant: '<S9>/Constant3'
       *  DataStoreRead: '<S9>/DataStoreRead3'
       */
      if (AbstractFuelControl_M1_DW.controller_mode) {
        rtb_Switch_b = AbstractFuelControl_M1_P.Constant3_Value;
      } else {
        /* Sum: '<S9>/Sum1' incorporates:
         *  Constant: '<S9>/Constant2'
         */
        rtb_Switch_b = AbstractFuelControl_M1_P.Constant2_Value +
          AbstractFuelControl_M1_B.Sum3;

        /* Saturate: '<S9>/fb_fuel_saturation' */
        if (rtb_Switch_b > AbstractFuelControl_M1_P.fb_fuel_saturation_UpperSat)
        {
          rtb_Switch_b = AbstractFuelControl_M1_P.fb_fuel_saturation_UpperSat;
        } else {
          if (rtb_Switch_b <
              AbstractFuelControl_M1_P.fb_fuel_saturation_LowerSat) {
            rtb_Switch_b = AbstractFuelControl_M1_P.fb_fuel_saturation_LowerSat;
          }
        }

        /* End of Saturate: '<S9>/fb_fuel_saturation' */
      }

      /* End of Switch: '<S9>/Switch' */

      /* Outputs for Atomic SubSystem: '<S9>/feedforward_controller' */
      /* Product: '<S9>/Prod1' incorporates:
       *  DataStoreRead: '<S9>/DataStoreRead4'
       *  Product: '<S14>/Product'
       */
      rtb_Switch_b *= Sum_j / AbstractFuelControl_M1_DW.airbyfuel_ref;

      /* End of Outputs for SubSystem: '<S9>/feedforward_controller' */

      /* Saturate: '<S9>/fuel_saturation' */
      if (rtb_Switch_b > AbstractFuelControl_M1_P.fuel_saturation_UpperSat) {
        /* DataStoreWrite: '<S9>/DataStoreWrite' */
        AbstractFuelControl_M1_DW.commanded_fuel =
          AbstractFuelControl_M1_P.fuel_saturation_UpperSat;
      } else if (rtb_Switch_b <
                 AbstractFuelControl_M1_P.fuel_saturation_LowerSat) {
        /* DataStoreWrite: '<S9>/DataStoreWrite' */
        AbstractFuelControl_M1_DW.commanded_fuel =
          AbstractFuelControl_M1_P.fuel_saturation_LowerSat;
      } else {
        /* DataStoreWrite: '<S9>/DataStoreWrite' */
        AbstractFuelControl_M1_DW.commanded_fuel = rtb_Switch_b;
      }

      /* End of Saturate: '<S9>/fuel_saturation' */

      /* Update for Atomic SubSystem: '<S9>/air_estimation' */
      /* Outputs for Atomic SubSystem: '<S9>/air_estimation' */
      /* Update for UnitDelay: '<S12>/UnitDelay1' incorporates:
       *  Constant: '<S12>/Constant1'
       *  DataStoreRead: '<S9>/DataStoreRead'
       *  Gain: '<S12>/Gain'
       *  Product: '<S12>/Prod1'
       *  Sum: '<S12>/Sum1'
       *  Sum: '<S12>/Sum2'
       *  UnitDelay: '<S12>/UnitDelay1'
       */
      AbstractFuelControl_M1_DW.UnitDelay1_DSTATE_d +=
        (AbstractFuelControl_M1_DW.throttle_flow - Sum_j) *
        AbstractFuelControl_M1_P.Gain_Gain_j *
        AbstractFuelControl_M1_P.Constant1_Value;

      /* End of Outputs for SubSystem: '<S9>/air_estimation' */
      /* End of Update for SubSystem: '<S9>/air_estimation' */

      /* Update for Enabled SubSystem: '<S9>/feedback_PI_controller' incorporates:
       *  Update for EnablePort: '<S13>/Enable'
       */
      if (LogicalOperator) {
        /* Update for UnitDelay: '<S13>/UnitDelay1' */
        AbstractFuelControl_M1_DW.UnitDelay1_DSTATE =
          AbstractFuelControl_M1_B.Sum2;
      }

      /* End of Update for SubSystem: '<S9>/feedback_PI_controller' */
    }
  }

  /* End of Outputs for SubSystem: '<S8>/fuel_controller_pwon' */
  /* End of Outputs for SubSystem: '<S8>/fuel_controller_mode_10ms' */
  /* End of Outputs for SubSystem: '<S8>/fuel_controller_10ms' */
  /* End of Outputs for SubSystem: '<S3>/fuel_controller' */
  if (rtmIsMajorTimeStep(AbstractFuelControl_M1_M)) {
    /* DataStoreRead: '<S3>/DataStoreRead1' */
    rtb_DataStoreRead1_l = AbstractFuelControl_M1_DW.controller_mode;
  }

  /* DataStoreRead: '<S3>/DataStoreRead2' */
  Sum_j = AbstractFuelControl_M1_DW.airbyfuel_ref;

  /* End of Outputs for SubSystem: '<S1>/AF_Controller' */

  /* Integrator: '<S19>/Integrator' */
  rtb_Integrator = AbstractFuelControl_M1_X.Integrator_CSTATE_h;

  /* Gain: '<S20>/Gain' incorporates:
   *  Integrator: '<S19>/Integrator'
   *  Integrator: '<S20>/Integrator'
   *  Sum: '<S20>/Sum'
   */
  AbstractFuelControl_M1_B.Gain = (AbstractFuelControl_M1_X.Integrator_CSTATE_h
    - AbstractFuelControl_M1_X.Integrator_CSTATE_m) *
    AbstractFuelControl_M1_P.Gain_Gain;

  /* Fcn: '<S5>/Pumping' */
  rtb_fuelsystemtransportdelay = ((0.08979 * rtb_fuelsystemtransportdelay *
    rtb_radstorpm + -0.366) - 0.0337 * rtb_radstorpm *
    rtb_fuelsystemtransportdelay * rtb_fuelsystemtransportdelay) + 0.0001 *
    rtb_fuelsystemtransportdelay * rtb_radstorpm * rtb_radstorpm;

  /* Gain: '<S5>/Gain2' */
  rtb_Gain2 = AbstractFuelControl_M1_P.Gain2_Gain * rtb_fuelsystemtransportdelay;

  /* Gain: '<S7>/rad//s to rpm' */
  rtb_fuelsystemtransportdelay = AbstractFuelControl_M1_P.radstorpm_Gain *
    rtb_radstorpm;

  /* Gain: '<S4>/Gain' incorporates:
   *  Product: '<S4>/Product1'
   */
  rtb_c_air_chrggcyl = rtb_Gain2 / rtb_radstorpm *
    AbstractFuelControl_M1_P.Gain_Gain_l;

  /* Lookup_n-D: '<S7>/1-Kappa' */
  rtb_Kappatolerance0911 = look2_binlxpw(rtb_fuelsystemtransportdelay,
    rtb_c_air_chrggcyl, AbstractFuelControl_M1_P.uKappa_bp01Data,
    AbstractFuelControl_M1_P.uKappa_bp02Data,
    AbstractFuelControl_M1_P.uKappa_tableData,
    AbstractFuelControl_M1_P.uKappa_maxIndex, 5U);

  /* Gain: '<S7>/Kappa tolerance [0.9 1.1]' */
  rtb_Kappatolerance0911 *= AbstractFuelControl_M1_P.Kappatolerance0911_Gain;

  /* Outputs for Atomic SubSystem: '<S1>/AF_Controller' */
  /* Gain: '<S1>/fuel injector tolerance [0.95 1.05]' incorporates:
   *  DataStoreRead: '<S3>/DataStoreRead'
   *  DataTypeConversion: '<S1>/Data Type Conversion'
   */
  rtb_fuelinjectortolerance095105 = AbstractFuelControl_M1_P.fuel_inj_tol *
    AbstractFuelControl_M1_DW.commanded_fuel;

  /* End of Outputs for SubSystem: '<S1>/AF_Controller' */

  /* Lookup_n-D: '<S7>/tau_ww' */
  rtb_fuelsystemtransportdelay = look2_binlxpw(rtb_fuelsystemtransportdelay,
    rtb_c_air_chrggcyl, AbstractFuelControl_M1_P.tau_ww_bp01Data,
    AbstractFuelControl_M1_P.tau_ww_bp02Data,
    AbstractFuelControl_M1_P.tau_ww_tableData,
    AbstractFuelControl_M1_P.tau_ww_maxIndex, 5U);

  /* Product: '<S7>/Divide2' incorporates:
   *  Gain: '<S7>/tau_ww tolerance [0.9 1.1]'
   *  Integrator: '<S7>/Integrator'
   */
  rtb_fuel_puddle_evap = AbstractFuelControl_M1_X.Integrator_CSTATE_c /
    (AbstractFuelControl_M1_P.tau_wwtolerance0911_Gain *
     rtb_fuelsystemtransportdelay);

  /* Product: '<S4>/Divide' incorporates:
   *  Product: '<S7>/Divide'
   *  Sum: '<S7>/Add'
   */
  AbstractFuelControl_M1_B.airbyfuel = rtb_Gain2 / (rtb_Kappatolerance0911 *
    rtb_fuelinjectortolerance095105 + rtb_fuel_puddle_evap);

  /* VariableTransportDelay: '<S4>/fuel system transport delay' */
  {
    real_T **uBuffer = (real_T**)
      &AbstractFuelControl_M1_DW.fuelsystemtransportdelay_PWORK.TUbufferPtrs[0];
    real_T **tBuffer = (real_T**)
      &AbstractFuelControl_M1_DW.fuelsystemtransportdelay_PWORK.TUbufferPtrs[1];
    real_T **xBuffer = (real_T**)
      &AbstractFuelControl_M1_DW.fuelsystemtransportdelay_PWORK.TUbufferPtrs[2];
    real_T simTime = AbstractFuelControl_M1_M->Timing.t[0];
    real_T appliedDelay;

    /* For variable transport dealy, find the real applied dealy
     * here and then output
     */
    rtb_fuelsystemtransportdelay= rt_VTDelayfindtDInterpolate
      (AbstractFuelControl_M1_X.fuelsystemtransportdelay_CSTATE,*tBuffer,
       *uBuffer, *xBuffer,
       AbstractFuelControl_M1_DW.fuelsystemtransportdelay_IWORK.CircularBufSize,
       AbstractFuelControl_M1_DW.fuelsystemtransportdelay_IWORK.Head,
       AbstractFuelControl_M1_DW.fuelsystemtransportdelay_IWORK.Tail,
       &AbstractFuelControl_M1_DW.fuelsystemtransportdelay_IWORK.Last, simTime,
       0.0,0,
       0, AbstractFuelControl_M1_P.fuelsystemtransportdelay_InitOu,
       &appliedDelay);
  }

  /* Gain: '<S19>/Gain1' incorporates:
   *  Sum: '<S19>/Sum'
   */
  AbstractFuelControl_M1_B.Gain1 = (rtb_fuelsystemtransportdelay -
    rtb_Integrator) * AbstractFuelControl_M1_P.Gain1_Gain;

  /* Gain: '<S4>/rad//s to rpm' */
  rtb_radstorpm *= AbstractFuelControl_M1_P.radstorpm_Gain_p;

  /* Lookup_n-D: '<S4>/delay (s)' */
  AbstractFuelControl_M1_B.delays = look2_binlxpw(rtb_radstorpm,
    rtb_c_air_chrggcyl, AbstractFuelControl_M1_P.delays_bp01Data,
    AbstractFuelControl_M1_P.delays_bp02Data,
    AbstractFuelControl_M1_P.delays_tableData,
    AbstractFuelControl_M1_P.delays_maxIndex, 5U);

  /* Gain: '<S5>/RT//Vm' incorporates:
   *  Sum: '<S5>/Sum'
   */
  AbstractFuelControl_M1_B.RTVm = (rtb_pratio - rtb_Gain2) *
    AbstractFuelControl_M1_P.RTVm_Gain;

  /* Sum: '<S7>/Sum' incorporates:
   *  Constant: '<S7>/Constant'
   *  Gain: '<S7>/Gain'
   *  Product: '<S7>/Divide1'
   *  Sum: '<S7>/Add2'
   */
  AbstractFuelControl_M1_B.Sum = (AbstractFuelControl_M1_P.Gain_Gain_m *
    rtb_Kappatolerance0911 + AbstractFuelControl_M1_P.Constant_Value) *
    rtb_fuelinjectortolerance095105 - rtb_fuel_puddle_evap;

  /* End of Outputs for SubSystem: '<Root>/Model 1' */

  /* Outputs for Atomic SubSystem: '<Root>/V&V stub system' */
  /* Step: '<S2>/MeasureOn' */
  if (AbstractFuelControl_M1_M->Timing.t[0] <
      AbstractFuelControl_M1_P.measureTime) {
    rtb_Integrator = AbstractFuelControl_M1_P.MeasureOn_Y0_m;
  } else {
    rtb_Integrator = AbstractFuelControl_M1_P.MeasureOn_YFinal_n;
  }

  /* End of Step: '<S2>/MeasureOn' */

  /* RelationalOperator: '<S2>/Relational Operator' incorporates:
   *  Constant: '<S2>/Constant'
   */
  AbstractFuelControl_M1_B.RelationalOperator = (rtb_Integrator >
    AbstractFuelControl_M1_P.Constant_Value_l);
  if (rtmIsMajorTimeStep(AbstractFuelControl_M1_M)) {
    /* SignalConversion: '<S2>/HiddenBuf_InsertedFor_Calcuate Error_at_inport_3' */
    AbstractFuelControl_M1_B.HiddenBuf_InsertedFor_CalcuateE =
      AbstractFuelControl_M1_B.RelationalOperator;

    /* Outputs for Enabled SubSystem: '<S2>/Calcuate Error' incorporates:
     *  EnablePort: '<S21>/Enable'
     */
    if (rtmIsMajorTimeStep(AbstractFuelControl_M1_M)) {
      if (AbstractFuelControl_M1_B.HiddenBuf_InsertedFor_CalcuateE) {
        if (!AbstractFuelControl_M1_DW.CalcuateError_MODE) {
          AbstractFuelControl_M1_DW.CalcuateError_MODE = true;
        }
      } else {
        if (AbstractFuelControl_M1_DW.CalcuateError_MODE) {
          /* Disable for SwitchCase: '<S21>/Switch Case' */
          AbstractFuelControl_M1_DW.SwitchCase_ActiveSubsystem = -1;
          AbstractFuelControl_M1_DW.CalcuateError_MODE = false;
        }
      }
    }

    /* End of Outputs for SubSystem: '<S2>/Calcuate Error' */
  }

  /* Outputs for Enabled SubSystem: '<S2>/Calcuate Error' incorporates:
   *  EnablePort: '<S21>/Enable'
   */
  if (AbstractFuelControl_M1_DW.CalcuateError_MODE) {
    /* SwitchCase: '<S21>/Switch Case' incorporates:
     *  Constant: '<S2>/spec_num'
     */
    if (rtmIsMajorTimeStep(AbstractFuelControl_M1_M)) {
      switch (AbstractFuelControl_M1_P.spec_num) {
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

      AbstractFuelControl_M1_DW.SwitchCase_ActiveSubsystem = rtAction;
    } else {
      rtAction = AbstractFuelControl_M1_DW.SwitchCase_ActiveSubsystem;
    }

    switch (rtAction) {
     case 0:
      /* Outputs for IfAction SubSystem: '<S21>/over(under)shoot' incorporates:
       *  ActionPort: '<S23>/Action Port'
       */
      /* Sum: '<S23>/Sum1' incorporates:
       *  Constant: '<S23>/Constant'
       *  DataTypeConversion: '<S2>/Data Type Conversion'
       *  Product: '<S23>/Divide'
       */
      AbstractFuelControl_M1_B.Sum1 = AbstractFuelControl_M1_B.AF_sensor / Sum_j
        - AbstractFuelControl_M1_P.Constant_Value_i;

      /* End of Outputs for SubSystem: '<S21>/over(under)shoot' */
      break;

     case 1:
      /* Outputs for IfAction SubSystem: '<S21>/RMS error' incorporates:
       *  ActionPort: '<S22>/Action Port'
       */
      /* Sum: '<S22>/Sum1' incorporates:
       *  DataTypeConversion: '<S2>/Data Type Conversion'
       */
      rtb_radstorpm = AbstractFuelControl_M1_B.AF_sensor - Sum_j;

      /* Step: '<S22>/MeasureOn' */
      if (AbstractFuelControl_M1_M->Timing.t[0] <
          AbstractFuelControl_M1_P.MeasureOn_Time) {
        rtb_Integrator = AbstractFuelControl_M1_P.MeasureOn_Y0;
      } else {
        rtb_Integrator = AbstractFuelControl_M1_P.MeasureOn_YFinal;
      }

      /* End of Step: '<S22>/MeasureOn' */

      /* Product: '<S22>/Product' incorporates:
       *  Math: '<S22>/Math Function'
       */
      AbstractFuelControl_M1_B.Product = rtb_radstorpm * rtb_radstorpm *
        rtb_Integrator;

      /* Sqrt: '<S22>/Sqrt' incorporates:
       *  Constant: '<S22>/Constant'
       *  Integrator: '<S22>/Integrator'
       *  Product: '<S22>/Divide'
       */
      AbstractFuelControl_M1_B.Sqrt = sqrt
        (AbstractFuelControl_M1_X.Integrator_CSTATE /
         (AbstractFuelControl_M1_P.simTime - 10.0));

      /* End of Outputs for SubSystem: '<S21>/RMS error' */
      break;

     case 2:
      break;
    }

    /* End of SwitchCase: '<S21>/Switch Case' */

    /* MultiPortSwitch: '<S21>/Multiport Switch' incorporates:
     *  Constant: '<S2>/spec_num'
     */
    if (AbstractFuelControl_M1_P.spec_num == 1) {
      AbstractFuelControl_M1_B.MultiportSwitch = AbstractFuelControl_M1_B.Sum1;
    } else {
      AbstractFuelControl_M1_B.MultiportSwitch = AbstractFuelControl_M1_B.Sqrt;
    }

    /* End of MultiPortSwitch: '<S21>/Multiport Switch' */
  }

  /* End of Outputs for SubSystem: '<S2>/Calcuate Error' */
  /* End of Outputs for SubSystem: '<Root>/V&V stub system' */

  /* Outport: '<Root>/verificaton measurement' */
  AbstractFuelControl_M1_Y.verificatonmeasurement =
    AbstractFuelControl_M1_B.MultiportSwitch;
  if (rtmIsMajorTimeStep(AbstractFuelControl_M1_M)) {
    /* DataTypeConversion: '<Root>/Data Type Conversion' */
    rtb_ControllerMode = rtb_DataStoreRead1_l;

    /* Outport: '<Root>/mode' */
    AbstractFuelControl_M1_Y.mode = rtb_ControllerMode;

    /* Outport: '<Root>/pedal angle' incorporates:
     *  DiscretePulseGenerator: '<Root>/Pedal Angle (deg)'
     */
    AbstractFuelControl_M1_Y.pedalangle =
      (AbstractFuelControl_M1_DW.clockTickCounter <
       AbstractFuelControl_M1_P.PedalAngledeg_Duty) &&
      (AbstractFuelControl_M1_DW.clockTickCounter >= 0) ?
      AbstractFuelControl_M1_P.PedalAngledeg_Amp : 0.0;

    /* DiscretePulseGenerator: '<Root>/Pedal Angle (deg)' */
    if (AbstractFuelControl_M1_DW.clockTickCounter >=
        AbstractFuelControl_M1_P.PedalAngledeg_Period - 1.0) {
      AbstractFuelControl_M1_DW.clockTickCounter = 0;
    } else {
      AbstractFuelControl_M1_DW.clockTickCounter++;
    }

    /* ToFile: '<Root>/To File' */
    {
      if (!(++AbstractFuelControl_M1_DW.ToFile_IWORK.Decimation % 1) &&
          (AbstractFuelControl_M1_DW.ToFile_IWORK.Count*2)+1 < 100000000 ) {
        FILE *fp = (FILE *) AbstractFuelControl_M1_DW.ToFile_PWORK.FilePtr;
        if (fp != (NULL)) {
          real_T u[2];
          AbstractFuelControl_M1_DW.ToFile_IWORK.Decimation = 0;
          u[0] = AbstractFuelControl_M1_M->Timing.t[1];
          u[1] = AbstractFuelControl_M1_Y.pedalangle;
          if (fwrite(u, sizeof(real_T), 2, fp) != 2) {
            rtmSetErrorStatus(AbstractFuelControl_M1_M,
                              "Error writing to MAT-file pedal_angle.mat");
            return;
          }

          if (((++AbstractFuelControl_M1_DW.ToFile_IWORK.Count)*2)+1 >=
              100000000) {
            (void)fprintf(stdout,
                          "*** The ToFile block will stop logging data before\n"
                          "    the simulation has ended, because it has reached\n"
                          "    the maximum number of elements (100000000)\n"
                          "    allowed in MAT-file pedal_angle.mat.\n");
          }
        }
      }
    }
  }
}

/* Model update function */
void AbstractFuelControl_M1_update(void)
{
  /* Update for Atomic SubSystem: '<Root>/Model 1' */

  /* Update for VariableTransportDelay: '<S4>/fuel system transport delay' */
  {
    real_T **uBuffer = (real_T**)
      &AbstractFuelControl_M1_DW.fuelsystemtransportdelay_PWORK.TUbufferPtrs[0];
    real_T **tBuffer = (real_T**)
      &AbstractFuelControl_M1_DW.fuelsystemtransportdelay_PWORK.TUbufferPtrs[1];
    real_T **xBuffer = (real_T**)
      &AbstractFuelControl_M1_DW.fuelsystemtransportdelay_PWORK.TUbufferPtrs[2];
    real_T simTime = AbstractFuelControl_M1_M->Timing.t[0];
    real_T appliedDelay;
    appliedDelay = AbstractFuelControl_M1_B.delays;
    if (appliedDelay > AbstractFuelControl_M1_P.fuelsystemtransportdelay_MaxDel)
    {
      appliedDelay = AbstractFuelControl_M1_P.fuelsystemtransportdelay_MaxDel;
    }

    AbstractFuelControl_M1_DW.fuelsystemtransportdelay_IWORK.Head =
      ((AbstractFuelControl_M1_DW.fuelsystemtransportdelay_IWORK.Head <
        (AbstractFuelControl_M1_DW.fuelsystemtransportdelay_IWORK.CircularBufSize
         -1)) ? (AbstractFuelControl_M1_DW.fuelsystemtransportdelay_IWORK.Head+1)
       : 0);
    if (AbstractFuelControl_M1_DW.fuelsystemtransportdelay_IWORK.Head ==
        AbstractFuelControl_M1_DW.fuelsystemtransportdelay_IWORK.Tail) {
      AbstractFuelControl_M1_DW.fuelsystemtransportdelay_IWORK.Tail =
        ((AbstractFuelControl_M1_DW.fuelsystemtransportdelay_IWORK.Tail <
          (AbstractFuelControl_M1_DW.fuelsystemtransportdelay_IWORK.CircularBufSize
           -1)) ? (AbstractFuelControl_M1_DW.fuelsystemtransportdelay_IWORK.Tail
                   +1) : 0);
    }

    (*tBuffer)[AbstractFuelControl_M1_DW.fuelsystemtransportdelay_IWORK.Head] =
      simTime;
    (*uBuffer)[AbstractFuelControl_M1_DW.fuelsystemtransportdelay_IWORK.Head] =
      AbstractFuelControl_M1_B.airbyfuel;
    (*xBuffer)[AbstractFuelControl_M1_DW.fuelsystemtransportdelay_IWORK.Head] =
      AbstractFuelControl_M1_X.fuelsystemtransportdelay_CSTATE;

    /* when use fixed buffer, reset solver at when buffer is updated
     * to avoid output consistency fail.
     */
  }

  /* End of Update for SubSystem: '<Root>/Model 1' */
  if (rtmIsMajorTimeStep(AbstractFuelControl_M1_M)) {
    rt_ertODEUpdateContinuousStates(&AbstractFuelControl_M1_M->solverInfo);
  }

  /* Update absolute time for base rate */
  /* The "clockTick0" counts the number of times the code of this task has
   * been executed. The absolute time is the multiplication of "clockTick0"
   * and "Timing.stepSize0". Size of "clockTick0" ensures timer will not
   * overflow during the application lifespan selected.
   * Timer of this task consists of two 32 bit unsigned integers.
   * The two integers represent the low bits Timing.clockTick0 and the high bits
   * Timing.clockTickH0. When the low bit overflows to 0, the high bits increment.
   */
  if (!(++AbstractFuelControl_M1_M->Timing.clockTick0)) {
    ++AbstractFuelControl_M1_M->Timing.clockTickH0;
  }

  AbstractFuelControl_M1_M->Timing.t[0] = rtsiGetSolverStopTime
    (&AbstractFuelControl_M1_M->solverInfo);

  {
    /* Update absolute timer for sample time: [5.0E-5s, 0.0s] */
    /* The "clockTick1" counts the number of times the code of this task has
     * been executed. The absolute time is the multiplication of "clockTick1"
     * and "Timing.stepSize1". Size of "clockTick1" ensures timer will not
     * overflow during the application lifespan selected.
     * Timer of this task consists of two 32 bit unsigned integers.
     * The two integers represent the low bits Timing.clockTick1 and the high bits
     * Timing.clockTickH1. When the low bit overflows to 0, the high bits increment.
     */
    if (!(++AbstractFuelControl_M1_M->Timing.clockTick1)) {
      ++AbstractFuelControl_M1_M->Timing.clockTickH1;
    }

    AbstractFuelControl_M1_M->Timing.t[1] =
      AbstractFuelControl_M1_M->Timing.clockTick1 *
      AbstractFuelControl_M1_M->Timing.stepSize1 +
      AbstractFuelControl_M1_M->Timing.clockTickH1 *
      AbstractFuelControl_M1_M->Timing.stepSize1 * 4294967296.0;
  }
}

/* Derivatives for root system: '<Root>' */
void AbstractFuelControl_M1_derivatives(void)
{
  XDot_AbstractFuelControl_M1_T *_rtXdot;
  _rtXdot = ((XDot_AbstractFuelControl_M1_T *)
             AbstractFuelControl_M1_M->ModelData.derivs);

  /* Derivatives for Atomic SubSystem: '<Root>/Model 1' */
  /* Derivatives for Integrator: '<S20>/Integrator' */
  _rtXdot->Integrator_CSTATE_m = AbstractFuelControl_M1_B.Gain;

  /* Derivatives for TransferFcn: '<S1>/Throttle delay' */
  _rtXdot->Throttledelay_CSTATE = 0.0;
  _rtXdot->Throttledelay_CSTATE += AbstractFuelControl_M1_P.Throttledelay_A *
    AbstractFuelControl_M1_X.Throttledelay_CSTATE;
  _rtXdot->Throttledelay_CSTATE += AbstractFuelControl_M1_Y.pedalangle;

  /* Derivatives for Integrator: '<S5>/p0 = 0.543 (bar)' */
  _rtXdot->p00543bar_CSTATE = AbstractFuelControl_M1_B.RTVm;

  /* Derivatives for Integrator: '<S19>/Integrator' */
  _rtXdot->Integrator_CSTATE_h = AbstractFuelControl_M1_B.Gain1;

  /* Derivatives for Integrator: '<S7>/Integrator' */
  _rtXdot->Integrator_CSTATE_c = AbstractFuelControl_M1_B.Sum;

  /* Derivatives for VariableTransportDelay: '<S4>/fuel system transport delay' */
  {
    real_T instantDelay;
    instantDelay = AbstractFuelControl_M1_B.delays;
    if (instantDelay > AbstractFuelControl_M1_P.fuelsystemtransportdelay_MaxDel)
    {
      instantDelay = AbstractFuelControl_M1_P.fuelsystemtransportdelay_MaxDel;
    }

    if (instantDelay < 0.0) {
      ((XDot_AbstractFuelControl_M1_T *)
        AbstractFuelControl_M1_M->ModelData.derivs)
        ->fuelsystemtransportdelay_CSTATE = 0;
    } else {
      ((XDot_AbstractFuelControl_M1_T *)
        AbstractFuelControl_M1_M->ModelData.derivs)
        ->fuelsystemtransportdelay_CSTATE = 1.0/instantDelay;
    }
  }

  /* End of Derivatives for SubSystem: '<Root>/Model 1' */

  /* Derivatives for Atomic SubSystem: '<Root>/V&V stub system' */
  /* Derivatives for Enabled SubSystem: '<S2>/Calcuate Error' */
  if (AbstractFuelControl_M1_DW.CalcuateError_MODE) {
    /* Derivatives for SwitchCase: '<S21>/Switch Case' */
    ((XDot_AbstractFuelControl_M1_T *)
      AbstractFuelControl_M1_M->ModelData.derivs)->Integrator_CSTATE = 0.0;
    if (AbstractFuelControl_M1_DW.SwitchCase_ActiveSubsystem == 1) {
      /* Derivatives for IfAction SubSystem: '<S21>/RMS error' incorporates:
       *  Derivatives for ActionPort: '<S22>/Action Port'
       */
      /* Derivatives for Integrator: '<S22>/Integrator' */
      _rtXdot->Integrator_CSTATE = AbstractFuelControl_M1_B.Product;

      /* End of Derivatives for SubSystem: '<S21>/RMS error' */
    }

    /* End of Derivatives for SwitchCase: '<S21>/Switch Case' */
  } else {
    ((XDot_AbstractFuelControl_M1_T *)
      AbstractFuelControl_M1_M->ModelData.derivs)->Integrator_CSTATE = 0.0;
  }

  /* End of Derivatives for SubSystem: '<S2>/Calcuate Error' */
  /* End of Derivatives for SubSystem: '<Root>/V&V stub system' */
}

/* Model initialize function */
void AbstractFuelControl_M1_initialize(void)
{
  /* Start for Atomic SubSystem: '<S1>/AF_Controller' */
  /* Start for DiscretePulseGenerator: '<S3>/PulseGenerator_10ms' */
  AbstractFuelControl_M1_DW.clockTickCounter_l = -200;

  /* Start for Atomic SubSystem: '<S3>/fuel_controller' */
  /* Start for Triggered SubSystem: '<S8>/fuel_controller_10ms' */
  /* InitializeConditions for Enabled SubSystem: '<S9>/feedback_PI_controller' */
  /* InitializeConditions for UnitDelay: '<S13>/UnitDelay1' */
  AbstractFuelControl_M1_DW.UnitDelay1_DSTATE =
    AbstractFuelControl_M1_P.UnitDelay1_InitialCondition_l;

  /* End of InitializeConditions for SubSystem: '<S9>/feedback_PI_controller' */
  /* End of Start for SubSystem: '<S8>/fuel_controller_10ms' */

  /* Start for DataStoreMemory: '<S8>/DataStoreMemory' */
  AbstractFuelControl_M1_DW.engine_speed =
    AbstractFuelControl_M1_P.DataStoreMemory_InitialValue;

  /* Start for DataStoreMemory: '<S8>/DataStoreMemory1' */
  AbstractFuelControl_M1_DW.throttle_flow =
    AbstractFuelControl_M1_P.DataStoreMemory1_InitialValue;

  /* Start for DataStoreMemory: '<S8>/DataStoreMemory2' */
  AbstractFuelControl_M1_DW.airbyfuel_meas =
    AbstractFuelControl_M1_P.DataStoreMemory2_InitialValue;

  /* Start for DataStoreMemory: '<S8>/DataStoreMemory3' */
  AbstractFuelControl_M1_DW.throttle_angle =
    AbstractFuelControl_M1_P.DataStoreMemory3_InitialValue;

  /* End of Start for SubSystem: '<S3>/fuel_controller' */

  /* Start for DataStoreMemory: '<S3>/commanded_fuel' */
  AbstractFuelControl_M1_DW.commanded_fuel =
    AbstractFuelControl_M1_P.commanded_fuel_InitialValue;

  /* Start for DataStoreMemory: '<S3>/mode_fb' */
  AbstractFuelControl_M1_DW.controller_mode =
    AbstractFuelControl_M1_P.mode_fb_InitialValue;

  /* Start for DataStoreMemory: '<S3>/mode_fb1' */
  AbstractFuelControl_M1_DW.airbyfuel_ref =
    AbstractFuelControl_M1_P.mode_fb1_InitialValue;

  /* End of Start for SubSystem: '<S1>/AF_Controller' */
  /* Start for VariableTransportDelay: '<S4>/fuel system transport delay' */
  {
    real_T *pBuffer =
      &AbstractFuelControl_M1_DW.fuelsystemtransportdelay_RWORK.TUbufferArea[0];
    int_T j;
    AbstractFuelControl_M1_DW.fuelsystemtransportdelay_IWORK.Tail = 0;
    AbstractFuelControl_M1_DW.fuelsystemtransportdelay_IWORK.Head = 0;
    AbstractFuelControl_M1_DW.fuelsystemtransportdelay_IWORK.Last = 0;
    AbstractFuelControl_M1_DW.fuelsystemtransportdelay_IWORK.CircularBufSize =
      20480;
    for (j=0; j < 20480; j++) {
      pBuffer[j] = AbstractFuelControl_M1_P.fuelsystemtransportdelay_InitOu;
      pBuffer[20480 + j] = AbstractFuelControl_M1_M->Timing.t[0];
    }

    pBuffer[2*20480] = 0.0;
    AbstractFuelControl_M1_DW.fuelsystemtransportdelay_PWORK.TUbufferPtrs[0] =
      (void *) &pBuffer[0];
    AbstractFuelControl_M1_DW.fuelsystemtransportdelay_PWORK.TUbufferPtrs[1] =
      (void *) &pBuffer[20480];
    AbstractFuelControl_M1_DW.fuelsystemtransportdelay_PWORK.TUbufferPtrs[2] =
      (void *) &pBuffer[2*20480];
  }

  /* End of Start for SubSystem: '<Root>/Model 1' */

  /* Start for Atomic SubSystem: '<Root>/V&V stub system' */
  /* Start for Enabled SubSystem: '<S2>/Calcuate Error' */
  /* Start for SwitchCase: '<S21>/Switch Case' */
  AbstractFuelControl_M1_DW.SwitchCase_ActiveSubsystem = -1;

  /* InitializeConditions for IfAction SubSystem: '<S21>/RMS error' */
  /* InitializeConditions for Integrator: '<S22>/Integrator' */
  AbstractFuelControl_M1_X.Integrator_CSTATE =
    AbstractFuelControl_M1_P.Integrator_IC_b;

  /* End of InitializeConditions for SubSystem: '<S21>/RMS error' */
  /* End of Start for SubSystem: '<S2>/Calcuate Error' */
  /* End of Start for SubSystem: '<Root>/V&V stub system' */

  /* Start for DiscretePulseGenerator: '<Root>/Pedal Angle (deg)' */
  AbstractFuelControl_M1_DW.clockTickCounter = -60000;

  /* Start for ToFile: '<Root>/To File' */
  {
    FILE *fp = (NULL);
    char fileName[509] = "pedal_angle.mat";
    if ((fp = fopen(fileName, "wb")) == (NULL)) {
      rtmSetErrorStatus(AbstractFuelControl_M1_M,
                        "Error creating .mat file pedal_angle.mat");
      return;
    }

    if (rt_WriteMat4FileHeader(fp,2,0,"pedal_angle")) {
      rtmSetErrorStatus(AbstractFuelControl_M1_M,
                        "Error writing mat file header to file pedal_angle.mat");
      return;
    }

    AbstractFuelControl_M1_DW.ToFile_IWORK.Count = 0;
    AbstractFuelControl_M1_DW.ToFile_IWORK.Decimation = -1;
    AbstractFuelControl_M1_DW.ToFile_PWORK.FilePtr = fp;
  }

  AbstractFuelControl_M1_PrevZCX.fuel_controller_10ms_Trig_ZCE =
    UNINITIALIZED_ZCSIG;
  AbstractFuelControl_M1_PrevZCX.fuel_controller_mode_10ms_Trig_ =
    UNINITIALIZED_ZCSIG;
  AbstractFuelControl_M1_PrevZCX.fuel_controller_pwon_Trig_ZCE =
    UNINITIALIZED_ZCSIG;

  /* InitializeConditions for Atomic SubSystem: '<Root>/Model 1' */
  /* InitializeConditions for Integrator: '<S20>/Integrator' */
  AbstractFuelControl_M1_X.Integrator_CSTATE_m =
    AbstractFuelControl_M1_P.Integrator_IC;

  /* InitializeConditions for TransferFcn: '<S1>/Throttle delay' */
  AbstractFuelControl_M1_X.Throttledelay_CSTATE = 0.0;

  /* InitializeConditions for Integrator: '<S5>/p0 = 0.543 (bar)' */
  AbstractFuelControl_M1_X.p00543bar_CSTATE =
    AbstractFuelControl_M1_P.p00543bar_IC;

  /* InitializeConditions for Atomic SubSystem: '<S1>/AF_Controller' */
  /* InitializeConditions for Atomic SubSystem: '<S3>/fuel_controller' */
  /* InitializeConditions for Triggered SubSystem: '<S8>/fuel_controller_mode_10ms' */
  /* InitializeConditions for Atomic SubSystem: '<S10>/sensor_failure_detection' */
  /* InitializeConditions for UnitDelay: '<S17>/Unit Delay' */
  AbstractFuelControl_M1_DW.UnitDelay_DSTATE =
    AbstractFuelControl_M1_P.UnitDelay_InitialCondition;

  /* End of InitializeConditions for SubSystem: '<S10>/sensor_failure_detection' */

  /* InitializeConditions for Atomic SubSystem: '<S10>/normal_mode_detection' */
  /* InitializeConditions for UnitDelay: '<S15>/Unit Delay2' */
  AbstractFuelControl_M1_DW.UnitDelay2_DSTATE =
    AbstractFuelControl_M1_P.UnitDelay2_InitialCondition;

  /* InitializeConditions for UnitDelay: '<S15>/Unit Delay1' */
  AbstractFuelControl_M1_DW.UnitDelay1_DSTATE_e =
    AbstractFuelControl_M1_P.UnitDelay1_InitialCondition_c;

  /* End of InitializeConditions for SubSystem: '<S10>/normal_mode_detection' */

  /* InitializeConditions for Atomic SubSystem: '<S10>/power_mode_detection' */
  /* InitializeConditions for UnitDelay: '<S16>/Unit Delay1' */
  AbstractFuelControl_M1_DW.UnitDelay1_DSTATE_a =
    AbstractFuelControl_M1_P.UnitDelay1_InitialCondition_f;

  /* End of InitializeConditions for SubSystem: '<S10>/power_mode_detection' */
  /* End of InitializeConditions for SubSystem: '<S8>/fuel_controller_mode_10ms' */

  /* InitializeConditions for Triggered SubSystem: '<S8>/fuel_controller_10ms' */
  /* InitializeConditions for Atomic SubSystem: '<S9>/air_estimation' */
  /* InitializeConditions for UnitDelay: '<S12>/UnitDelay1' */
  AbstractFuelControl_M1_DW.UnitDelay1_DSTATE_d =
    AbstractFuelControl_M1_P.UnitDelay1_InitialCondition;

  /* End of InitializeConditions for SubSystem: '<S9>/air_estimation' */
  /* End of InitializeConditions for SubSystem: '<S8>/fuel_controller_10ms' */
  /* End of InitializeConditions for SubSystem: '<S3>/fuel_controller' */
  /* End of InitializeConditions for SubSystem: '<S1>/AF_Controller' */

  /* InitializeConditions for Integrator: '<S19>/Integrator' */
  AbstractFuelControl_M1_X.Integrator_CSTATE_h =
    AbstractFuelControl_M1_P.Integrator_IC_l;

  /* InitializeConditions for Integrator: '<S7>/Integrator' */
  AbstractFuelControl_M1_X.Integrator_CSTATE_c =
    AbstractFuelControl_M1_P.Integrator_IC_m;

  /* InitializeConditions for VariableTransportDelay: '<S4>/fuel system transport delay' */
  AbstractFuelControl_M1_X.fuelsystemtransportdelay_CSTATE = 0.0;

  /* End of InitializeConditions for SubSystem: '<Root>/Model 1' */
}

/* Model terminate function */
void AbstractFuelControl_M1_terminate(void)
{
  /* Terminate for ToFile: '<Root>/To File' */
  {
    FILE *fp = (FILE *) AbstractFuelControl_M1_DW.ToFile_PWORK.FilePtr;
    if (fp != (NULL)) {
      char fileName[509] = "pedal_angle.mat";
      if (fclose(fp) == EOF) {
        rtmSetErrorStatus(AbstractFuelControl_M1_M,
                          "Error closing MAT-file pedal_angle.mat");
        return;
      }

      if ((fp = fopen(fileName, "r+b")) == (NULL)) {
        rtmSetErrorStatus(AbstractFuelControl_M1_M,
                          "Error reopening MAT-file pedal_angle.mat");
        return;
      }

      if (rt_WriteMat4FileHeader(fp, 2,
           AbstractFuelControl_M1_DW.ToFile_IWORK.Count, "pedal_angle")) {
        rtmSetErrorStatus(AbstractFuelControl_M1_M,
                          "Error writing header for pedal_angle to MAT-file pedal_angle.mat");
      }

      if (fclose(fp) == EOF) {
        rtmSetErrorStatus(AbstractFuelControl_M1_M,
                          "Error closing MAT-file pedal_angle.mat");
        return;
      }

      AbstractFuelControl_M1_DW.ToFile_PWORK.FilePtr = (NULL);
    }
  }
}

/*========================================================================*
 * Start of Classic call interface                                        *
 *========================================================================*/

/* Solver interface called by GRT_Main */
#ifndef USE_GENERATED_SOLVER

void rt_ODECreateIntegrationData(RTWSolverInfo *si)
{
  UNUSED_PARAMETER(si);
  return;
}                                      /* do nothing */

void rt_ODEDestroyIntegrationData(RTWSolverInfo *si)
{
  UNUSED_PARAMETER(si);
  return;
}                                      /* do nothing */

void rt_ODEUpdateContinuousStates(RTWSolverInfo *si)
{
  UNUSED_PARAMETER(si);
  return;
}                                      /* do nothing */

#endif

void MdlOutputs(int_T tid)
{
  AbstractFuelControl_M1_output();
  UNUSED_PARAMETER(tid);
}

void MdlUpdate(int_T tid)
{
  AbstractFuelControl_M1_update();
  UNUSED_PARAMETER(tid);
}

void MdlInitializeSizes(void)
{
}

void MdlInitializeSampleTimes(void)
{
}

void MdlInitialize(void)
{
}

void MdlStart(void)
{
  AbstractFuelControl_M1_initialize();
}

void MdlTerminate(void)
{
  AbstractFuelControl_M1_terminate();
}

/* Registration function */
RT_MODEL_AbstractFuelControl__T *AbstractFuelControl_M1(void)
{
  /* Registration code */

  /* initialize non-finites */
  rt_InitInfAndNaN(sizeof(real_T));

  /* non-finite (run-time) assignments */
  AbstractFuelControl_M1_P.fb_fuel_saturation_UpperSat = rtInfF;

  /* initialize real-time model */
  (void) memset((void *)AbstractFuelControl_M1_M, 0,
                sizeof(RT_MODEL_AbstractFuelControl__T));

  {
    /* Setup solver object */
    rtsiSetSimTimeStepPtr(&AbstractFuelControl_M1_M->solverInfo,
                          &AbstractFuelControl_M1_M->Timing.simTimeStep);
    rtsiSetTPtr(&AbstractFuelControl_M1_M->solverInfo, &rtmGetTPtr
                (AbstractFuelControl_M1_M));
    rtsiSetStepSizePtr(&AbstractFuelControl_M1_M->solverInfo,
                       &AbstractFuelControl_M1_M->Timing.stepSize0);
    rtsiSetdXPtr(&AbstractFuelControl_M1_M->solverInfo,
                 &AbstractFuelControl_M1_M->ModelData.derivs);
    rtsiSetContStatesPtr(&AbstractFuelControl_M1_M->solverInfo, (real_T **)
                         &AbstractFuelControl_M1_M->ModelData.contStates);
    rtsiSetNumContStatesPtr(&AbstractFuelControl_M1_M->solverInfo,
      &AbstractFuelControl_M1_M->Sizes.numContStates);
    rtsiSetNumPeriodicContStatesPtr(&AbstractFuelControl_M1_M->solverInfo,
      &AbstractFuelControl_M1_M->Sizes.numPeriodicContStates);
    rtsiSetPeriodicContStateIndicesPtr(&AbstractFuelControl_M1_M->solverInfo,
      &AbstractFuelControl_M1_M->ModelData.periodicContStateIndices);
    rtsiSetPeriodicContStateRangesPtr(&AbstractFuelControl_M1_M->solverInfo,
      &AbstractFuelControl_M1_M->ModelData.periodicContStateRanges);
    rtsiSetErrorStatusPtr(&AbstractFuelControl_M1_M->solverInfo,
                          (&rtmGetErrorStatus(AbstractFuelControl_M1_M)));
    rtsiSetRTModelPtr(&AbstractFuelControl_M1_M->solverInfo,
                      AbstractFuelControl_M1_M);
  }

  rtsiSetSimTimeStep(&AbstractFuelControl_M1_M->solverInfo, MAJOR_TIME_STEP);
  AbstractFuelControl_M1_M->ModelData.intgData.y =
    AbstractFuelControl_M1_M->ModelData.odeY;
  AbstractFuelControl_M1_M->ModelData.intgData.f[0] =
    AbstractFuelControl_M1_M->ModelData.odeF[0];
  AbstractFuelControl_M1_M->ModelData.intgData.f[1] =
    AbstractFuelControl_M1_M->ModelData.odeF[1];
  AbstractFuelControl_M1_M->ModelData.intgData.f[2] =
    AbstractFuelControl_M1_M->ModelData.odeF[2];
  AbstractFuelControl_M1_M->ModelData.contStates = ((real_T *)
    &AbstractFuelControl_M1_X);
  rtsiSetSolverData(&AbstractFuelControl_M1_M->solverInfo, (void *)
                    &AbstractFuelControl_M1_M->ModelData.intgData);
  rtsiSetSolverName(&AbstractFuelControl_M1_M->solverInfo,"ode3");

  /* Initialize timing info */
  {
    int_T *mdlTsMap = AbstractFuelControl_M1_M->Timing.sampleTimeTaskIDArray;
    mdlTsMap[0] = 0;
    mdlTsMap[1] = 1;
    AbstractFuelControl_M1_M->Timing.sampleTimeTaskIDPtr = (&mdlTsMap[0]);
    AbstractFuelControl_M1_M->Timing.sampleTimes =
      (&AbstractFuelControl_M1_M->Timing.sampleTimesArray[0]);
    AbstractFuelControl_M1_M->Timing.offsetTimes =
      (&AbstractFuelControl_M1_M->Timing.offsetTimesArray[0]);

    /* task periods */
    AbstractFuelControl_M1_M->Timing.sampleTimes[0] = (0.0);
    AbstractFuelControl_M1_M->Timing.sampleTimes[1] = (5.0E-5);

    /* task offsets */
    AbstractFuelControl_M1_M->Timing.offsetTimes[0] = (0.0);
    AbstractFuelControl_M1_M->Timing.offsetTimes[1] = (0.0);
  }

  rtmSetTPtr(AbstractFuelControl_M1_M, &AbstractFuelControl_M1_M->Timing.tArray
             [0]);

  {
    int_T *mdlSampleHits = AbstractFuelControl_M1_M->Timing.sampleHitArray;
    mdlSampleHits[0] = 1;
    mdlSampleHits[1] = 1;
    AbstractFuelControl_M1_M->Timing.sampleHits = (&mdlSampleHits[0]);
  }

  rtmSetTFinal(AbstractFuelControl_M1_M, 50.0);
  AbstractFuelControl_M1_M->Timing.stepSize0 = 5.0E-5;
  AbstractFuelControl_M1_M->Timing.stepSize1 = 5.0E-5;

  /* Setup for data logging */
  {
    static RTWLogInfo rt_DataLoggingInfo;
    rt_DataLoggingInfo.loggingInterval = NULL;
    AbstractFuelControl_M1_M->rtwLogInfo = &rt_DataLoggingInfo;
  }

  /* Setup for data logging */
  {
    rtliSetLogXSignalInfo(AbstractFuelControl_M1_M->rtwLogInfo, (NULL));
    rtliSetLogXSignalPtrs(AbstractFuelControl_M1_M->rtwLogInfo, (NULL));
    rtliSetLogT(AbstractFuelControl_M1_M->rtwLogInfo, "");
    rtliSetLogX(AbstractFuelControl_M1_M->rtwLogInfo, "");
    rtliSetLogXFinal(AbstractFuelControl_M1_M->rtwLogInfo, "");
    rtliSetLogVarNameModifier(AbstractFuelControl_M1_M->rtwLogInfo, "rt_");
    rtliSetLogFormat(AbstractFuelControl_M1_M->rtwLogInfo, 0);
    rtliSetLogMaxRows(AbstractFuelControl_M1_M->rtwLogInfo, 0);
    rtliSetLogDecimation(AbstractFuelControl_M1_M->rtwLogInfo, 1);
    rtliSetLogY(AbstractFuelControl_M1_M->rtwLogInfo, "");
    rtliSetLogYSignalInfo(AbstractFuelControl_M1_M->rtwLogInfo, (NULL));
    rtliSetLogYSignalPtrs(AbstractFuelControl_M1_M->rtwLogInfo, (NULL));
  }

  AbstractFuelControl_M1_M->solverInfoPtr =
    (&AbstractFuelControl_M1_M->solverInfo);
  AbstractFuelControl_M1_M->Timing.stepSize = (5.0E-5);
  rtsiSetFixedStepSize(&AbstractFuelControl_M1_M->solverInfo, 5.0E-5);
  rtsiSetSolverMode(&AbstractFuelControl_M1_M->solverInfo,
                    SOLVER_MODE_SINGLETASKING);

  /* block I/O */
  AbstractFuelControl_M1_M->ModelData.blockIO = ((void *)
    &AbstractFuelControl_M1_B);
  (void) memset(((void *) &AbstractFuelControl_M1_B), 0,
                sizeof(B_AbstractFuelControl_M1_T));

  {
    AbstractFuelControl_M1_B.MultiportSwitch = 0.0;
    AbstractFuelControl_M1_B.Product = 0.0;
    AbstractFuelControl_M1_B.Sqrt = 0.0;
    AbstractFuelControl_M1_B.Sum1 = 0.0;
    AbstractFuelControl_M1_B.AF_sensor = 0.0;
    AbstractFuelControl_M1_B.theta090 = 0.0;
    AbstractFuelControl_M1_B.Gain = 0.0;
    AbstractFuelControl_M1_B.airbyfuel = 0.0;
    AbstractFuelControl_M1_B.Gain1 = 0.0;
    AbstractFuelControl_M1_B.delays = 0.0;
    AbstractFuelControl_M1_B.RTVm = 0.0;
    AbstractFuelControl_M1_B.Sum = 0.0;
    AbstractFuelControl_M1_B.Pwon = 0.0;
    AbstractFuelControl_M1_B.PulseGenerator_10ms = 0.0;
    AbstractFuelControl_M1_B.Sum2 = 0.0F;
    AbstractFuelControl_M1_B.Sum3 = 0.0F;
  }

  /* parameters */
  AbstractFuelControl_M1_M->ModelData.defaultParam = ((real_T *)
    &AbstractFuelControl_M1_P);

  /* states (continuous) */
  {
    real_T *x = (real_T *) &AbstractFuelControl_M1_X;
    AbstractFuelControl_M1_M->ModelData.contStates = (x);
    (void) memset((void *)&AbstractFuelControl_M1_X, 0,
                  sizeof(X_AbstractFuelControl_M1_T));
  }

  /* states (dwork) */
  AbstractFuelControl_M1_M->ModelData.dwork = ((void *)
    &AbstractFuelControl_M1_DW);
  (void) memset((void *)&AbstractFuelControl_M1_DW, 0,
                sizeof(DW_AbstractFuelControl_M1_T));
  AbstractFuelControl_M1_DW.fuelsystemtransportdelay_RWORK.modelTStart = 0.0;

  {
    int32_T i;
    for (i = 0; i < 61440; i++) {
      AbstractFuelControl_M1_DW.fuelsystemtransportdelay_RWORK.TUbufferArea[i] =
        0.0;
    }
  }

  AbstractFuelControl_M1_DW.UnitDelay2_DSTATE = 0.0F;
  AbstractFuelControl_M1_DW.UnitDelay1_DSTATE = 0.0F;
  AbstractFuelControl_M1_DW.UnitDelay1_DSTATE_d = 0.0F;
  AbstractFuelControl_M1_DW.commanded_fuel = 0.0F;
  AbstractFuelControl_M1_DW.airbyfuel_ref = 0.0F;
  AbstractFuelControl_M1_DW.engine_speed = 0.0F;
  AbstractFuelControl_M1_DW.throttle_flow = 0.0F;
  AbstractFuelControl_M1_DW.airbyfuel_meas = 0.0F;
  AbstractFuelControl_M1_DW.throttle_angle = 0.0F;

  /* external inputs */
  AbstractFuelControl_M1_M->ModelData.inputs = (((void*)
    &AbstractFuelControl_M1_U));
  AbstractFuelControl_M1_U.EngineSpeed = 0.0;

  /* external outputs */
  AbstractFuelControl_M1_M->ModelData.outputs = (&AbstractFuelControl_M1_Y);
  AbstractFuelControl_M1_Y.verificatonmeasurement = 0.0;
  AbstractFuelControl_M1_Y.mode = 0.0;
  AbstractFuelControl_M1_Y.pedalangle = 0.0;

  /* Initialize Sizes */
  AbstractFuelControl_M1_M->Sizes.numContStates = (7);/* Number of continuous states */
  AbstractFuelControl_M1_M->Sizes.numPeriodicContStates = (0);/* Number of periodic continuous states */
  AbstractFuelControl_M1_M->Sizes.numY = (3);/* Number of model outputs */
  AbstractFuelControl_M1_M->Sizes.numU = (1);/* Number of model inputs */
  AbstractFuelControl_M1_M->Sizes.sysDirFeedThru = (1);/* The model is direct feedthrough */
  AbstractFuelControl_M1_M->Sizes.numSampTimes = (2);/* Number of sample times */
  AbstractFuelControl_M1_M->Sizes.numBlocks = (196);/* Number of blocks */
  AbstractFuelControl_M1_M->Sizes.numBlockIO = (25);/* Number of block outputs */
  AbstractFuelControl_M1_M->Sizes.numBlockPrms = (190);/* Sum of parameter "widths" */
  return AbstractFuelControl_M1_M;
}

/*========================================================================*
 * End of Classic call interface                                          *
 *========================================================================*/
