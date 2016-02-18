/*
 * Bruss_IO.c
 *
 * Academic License - for use in teaching, academic research, and meeting
 * course requirements at degree granting institutions only.  Not for
 * government, commercial, or other organizational use.
 *
 * Code generation for model "Bruss_IO".
 *
 * Model version              : 1.40
 * Simulink Coder version : 8.9 (R2015b) 13-Aug-2015
 * C source code generated on : Wed Feb 17 14:49:36 2016
 *
 * Target selection: grt.tlc
 * Note: GRT includes extra infrastructure and instrumentation for prototyping
 * Embedded hardware selection: 32-bit Generic
 * Code generation objectives: Unspecified
 * Validation result: Not run
 */

#include "Bruss_IO.h"
#include "Bruss_IO_private.h"

/* Named constants for Chart: '<Root>/Chart' */
#define Bruss_IO_IN_Brussellator       ((uint8_T)1U)
#define Bruss_IO_IN_NO_ACTIVE_CHILD    ((uint8_T)0U)

/* Block signals (auto storage) */
B_Bruss_IO_T Bruss_IO_B;

/* Continuous states */
X_Bruss_IO_T Bruss_IO_X;

/* Block states (auto storage) */
DW_Bruss_IO_T Bruss_IO_DW;

/* Real-time model */
RT_MODEL_Bruss_IO_T Bruss_IO_M_;
RT_MODEL_Bruss_IO_T *const Bruss_IO_M = &Bruss_IO_M_;

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
  int_T nXc = 2;
  rtsiSetSimTimeStep(si,MINOR_TIME_STEP);

  /* Save the state values at time t in y, we'll use x as ynew. */
  (void) memcpy(y, x,
                (uint_T)nXc*sizeof(real_T));

  /* Assumes that rtsiSetT and ModelOutputs are up-to-date */
  /* f0 = f(t,y) */
  rtsiSetdX(si, f0);
  Bruss_IO_derivatives();

  /* f(:,2) = feval(odefile, t + hA(1), y + f*hB(:,1), args(:)(*)); */
  hB[0] = h * rt_ODE3_B[0][0];
  for (i = 0; i < nXc; i++) {
    x[i] = y[i] + (f0[i]*hB[0]);
  }

  rtsiSetT(si, t + h*rt_ODE3_A[0]);
  rtsiSetdX(si, f1);
  Bruss_IO_step();
  Bruss_IO_derivatives();

  /* f(:,3) = feval(odefile, t + hA(2), y + f*hB(:,2), args(:)(*)); */
  for (i = 0; i <= 1; i++) {
    hB[i] = h * rt_ODE3_B[1][i];
  }

  for (i = 0; i < nXc; i++) {
    x[i] = y[i] + (f0[i]*hB[0] + f1[i]*hB[1]);
  }

  rtsiSetT(si, t + h*rt_ODE3_A[1]);
  rtsiSetdX(si, f2);
  Bruss_IO_step();
  Bruss_IO_derivatives();

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

/* Model step function */
void Bruss_IO_step(void)
{
  /* local block i/o variables */
  real_T rtb_TmpSignalConversionAtToFile[2];
  boolean_T stateChanged;
  if (rtmIsMajorTimeStep(Bruss_IO_M)) {
    /* set solver stop time */
    if (!(Bruss_IO_M->Timing.clockTick0+1)) {
      rtsiSetSolverStopTime(&Bruss_IO_M->solverInfo,
                            ((Bruss_IO_M->Timing.clockTickH0 + 1) *
        Bruss_IO_M->Timing.stepSize0 * 4294967296.0));
    } else {
      rtsiSetSolverStopTime(&Bruss_IO_M->solverInfo,
                            ((Bruss_IO_M->Timing.clockTick0 + 1) *
        Bruss_IO_M->Timing.stepSize0 + Bruss_IO_M->Timing.clockTickH0 *
        Bruss_IO_M->Timing.stepSize0 * 4294967296.0));
    }
  }                                    /* end MajorTimeStep */

  /* Update absolute time of base rate at minor time step */
  if (rtmIsMinorTimeStep(Bruss_IO_M)) {
    Bruss_IO_M->Timing.t[0] = rtsiGetT(&Bruss_IO_M->solverInfo);
  }

  /* FromFile: '<Root>/From File' */
  {
    const real_T *pT = (const real_T *) Bruss_IO_DW.FromFile_PWORK.PrevTimePtr;
    Bruss_IO_B.FromFile[0] = pT[1];
    Bruss_IO_B.FromFile[1] = pT[2];
  }

  /* Chart: '<Root>/Chart' */
  if (rtmIsMajorTimeStep(Bruss_IO_M)) {
    Bruss_IO_DW.lastMajorTime = Bruss_IO_M->Timing.t[0];
    stateChanged = 0;

    /* Gateway: Chart */
    /* During: Chart */
    if (Bruss_IO_DW.is_active_c1_Bruss_IO == 0U) {
      /* Entry: Chart */
      Bruss_IO_DW.is_active_c1_Bruss_IO = 1U;

      /* Entry Internal: Chart */
      /* Transition: '<S1>:18' */
      Bruss_IO_X.SFunction_CSTATE[0] = Bruss_IO_B.FromFile[0];
      Bruss_IO_X.SFunction_CSTATE[1] = Bruss_IO_B.FromFile[1];
      stateChanged = true;
      Bruss_IO_DW.is_c1_Bruss_IO = Bruss_IO_IN_Brussellator;
    } else {
      /* During 'Brussellator': '<S1>:10' */
    }

    if (stateChanged) {
      rtsiSetSolverNeedsReset(&Bruss_IO_M->solverInfo, true);
    }
  }

  /* During 'Brussellator': '<S1>:10' */
  Bruss_IO_B.x_out = Bruss_IO_X.SFunction_CSTATE[0];
  Bruss_IO_B.y_out = Bruss_IO_X.SFunction_CSTATE[1];

  /* End of Chart: '<Root>/Chart' */
  if (rtmIsMajorTimeStep(Bruss_IO_M)) {
    /* SignalConversion: '<Root>/TmpSignal ConversionAtTo FileInport1' */
    rtb_TmpSignalConversionAtToFile[0] = Bruss_IO_B.x_out;
    rtb_TmpSignalConversionAtToFile[1] = Bruss_IO_B.y_out;

    /* ToFile: '<Root>/To File' */
    {
      if (!(++Bruss_IO_DW.ToFile_IWORK.Decimation % 1) &&
          (Bruss_IO_DW.ToFile_IWORK.Count*3)+1 < 100000000 ) {
        FILE *fp = (FILE *) Bruss_IO_DW.ToFile_PWORK.FilePtr;
        if (fp != (NULL)) {
          real_T u[3];
          Bruss_IO_DW.ToFile_IWORK.Decimation = 0;
          u[0] = (((Bruss_IO_M->Timing.clockTick1+Bruss_IO_M->Timing.clockTickH1*
                    4294967296.0)) * 0.2);
          u[1] = rtb_TmpSignalConversionAtToFile[0];
          u[2] = rtb_TmpSignalConversionAtToFile[1];
          if (fwrite(u, sizeof(real_T), 3, fp) != 3) {
            rtmSetErrorStatus(Bruss_IO_M, "Error writing to MAT-file output.mat");
            return;
          }

          if (((++Bruss_IO_DW.ToFile_IWORK.Count)*3)+1 >= 100000000) {
            (void)fprintf(stdout,
                          "*** The ToFile block will stop logging data before\n"
                          "    the simulation has ended, because it has reached\n"
                          "    the maximum number of elements (100000000)\n"
                          "    allowed in MAT-file output.mat.\n");
          }
        }
      }
    }
  }

  if (rtmIsMajorTimeStep(Bruss_IO_M)) {
    /* Matfile logging */
    rt_UpdateTXYLogVars(Bruss_IO_M->rtwLogInfo, (Bruss_IO_M->Timing.t));
  }                                    /* end MajorTimeStep */

  if (rtmIsMajorTimeStep(Bruss_IO_M)) {
    /* signal main to stop simulation */
    {                                  /* Sample time: [0.0s, 0.0s] */
      if ((rtmGetTFinal(Bruss_IO_M)!=-1) &&
          !((rtmGetTFinal(Bruss_IO_M)-(((Bruss_IO_M->Timing.clockTick1+
               Bruss_IO_M->Timing.clockTickH1* 4294967296.0)) * 0.2)) >
            (((Bruss_IO_M->Timing.clockTick1+Bruss_IO_M->Timing.clockTickH1*
               4294967296.0)) * 0.2) * (DBL_EPSILON))) {
        rtmSetErrorStatus(Bruss_IO_M, "Simulation finished");
      }
    }

    rt_ertODEUpdateContinuousStates(&Bruss_IO_M->solverInfo);

    /* Update absolute time for base rate */
    /* The "clockTick0" counts the number of times the code of this task has
     * been executed. The absolute time is the multiplication of "clockTick0"
     * and "Timing.stepSize0". Size of "clockTick0" ensures timer will not
     * overflow during the application lifespan selected.
     * Timer of this task consists of two 32 bit unsigned integers.
     * The two integers represent the low bits Timing.clockTick0 and the high bits
     * Timing.clockTickH0. When the low bit overflows to 0, the high bits increment.
     */
    if (!(++Bruss_IO_M->Timing.clockTick0)) {
      ++Bruss_IO_M->Timing.clockTickH0;
    }

    Bruss_IO_M->Timing.t[0] = rtsiGetSolverStopTime(&Bruss_IO_M->solverInfo);

    {
      /* Update absolute timer for sample time: [0.2s, 0.0s] */
      /* The "clockTick1" counts the number of times the code of this task has
       * been executed. The resolution of this integer timer is 0.2, which is the step size
       * of the task. Size of "clockTick1" ensures timer will not overflow during the
       * application lifespan selected.
       * Timer of this task consists of two 32 bit unsigned integers.
       * The two integers represent the low bits Timing.clockTick1 and the high bits
       * Timing.clockTickH1. When the low bit overflows to 0, the high bits increment.
       */
      Bruss_IO_M->Timing.clockTick1++;
      if (!Bruss_IO_M->Timing.clockTick1) {
        Bruss_IO_M->Timing.clockTickH1++;
      }
    }
  }                                    /* end MajorTimeStep */
}

/* Derivatives for root system: '<Root>' */
void Bruss_IO_derivatives(void)
{
  XDot_Bruss_IO_T *_rtXdot;
  _rtXdot = ((XDot_Bruss_IO_T *) Bruss_IO_M->ModelData.derivs);

  /* Derivatives for Chart: '<Root>/Chart' */
  /* During 'Brussellator': '<S1>:10' */
  _rtXdot->SFunction_CSTATE[0] = (Bruss_IO_X.SFunction_CSTATE[0] *
    Bruss_IO_X.SFunction_CSTATE[0] * Bruss_IO_X.SFunction_CSTATE[1] + 1.0) - 2.5
    * Bruss_IO_X.SFunction_CSTATE[0];
  _rtXdot->SFunction_CSTATE[1] = 1.5 * Bruss_IO_X.SFunction_CSTATE[0] -
    Bruss_IO_X.SFunction_CSTATE[0] * Bruss_IO_X.SFunction_CSTATE[0] *
    Bruss_IO_X.SFunction_CSTATE[1];
}

/* Model initialize function */
void Bruss_IO_initialize(void)
{
  /* Registration code */

  /* initialize non-finites */
  rt_InitInfAndNaN(sizeof(real_T));

  /* initialize real-time model */
  (void) memset((void *)Bruss_IO_M, 0,
                sizeof(RT_MODEL_Bruss_IO_T));

  {
    /* Setup solver object */
    rtsiSetSimTimeStepPtr(&Bruss_IO_M->solverInfo,
                          &Bruss_IO_M->Timing.simTimeStep);
    rtsiSetTPtr(&Bruss_IO_M->solverInfo, &rtmGetTPtr(Bruss_IO_M));
    rtsiSetStepSizePtr(&Bruss_IO_M->solverInfo, &Bruss_IO_M->Timing.stepSize0);
    rtsiSetdXPtr(&Bruss_IO_M->solverInfo, &Bruss_IO_M->ModelData.derivs);
    rtsiSetContStatesPtr(&Bruss_IO_M->solverInfo, (real_T **)
                         &Bruss_IO_M->ModelData.contStates);
    rtsiSetNumContStatesPtr(&Bruss_IO_M->solverInfo,
      &Bruss_IO_M->Sizes.numContStates);
    rtsiSetNumPeriodicContStatesPtr(&Bruss_IO_M->solverInfo,
      &Bruss_IO_M->Sizes.numPeriodicContStates);
    rtsiSetPeriodicContStateIndicesPtr(&Bruss_IO_M->solverInfo,
      &Bruss_IO_M->ModelData.periodicContStateIndices);
    rtsiSetPeriodicContStateRangesPtr(&Bruss_IO_M->solverInfo,
      &Bruss_IO_M->ModelData.periodicContStateRanges);
    rtsiSetErrorStatusPtr(&Bruss_IO_M->solverInfo, (&rtmGetErrorStatus
      (Bruss_IO_M)));
    rtsiSetRTModelPtr(&Bruss_IO_M->solverInfo, Bruss_IO_M);
  }

  rtsiSetSimTimeStep(&Bruss_IO_M->solverInfo, MAJOR_TIME_STEP);
  Bruss_IO_M->ModelData.intgData.y = Bruss_IO_M->ModelData.odeY;
  Bruss_IO_M->ModelData.intgData.f[0] = Bruss_IO_M->ModelData.odeF[0];
  Bruss_IO_M->ModelData.intgData.f[1] = Bruss_IO_M->ModelData.odeF[1];
  Bruss_IO_M->ModelData.intgData.f[2] = Bruss_IO_M->ModelData.odeF[2];
  Bruss_IO_M->ModelData.contStates = ((X_Bruss_IO_T *) &Bruss_IO_X);
  rtsiSetSolverData(&Bruss_IO_M->solverInfo, (void *)
                    &Bruss_IO_M->ModelData.intgData);
  rtsiSetSolverName(&Bruss_IO_M->solverInfo,"ode3");
  rtmSetTPtr(Bruss_IO_M, &Bruss_IO_M->Timing.tArray[0]);
  rtmSetTFinal(Bruss_IO_M, 10.0);
  Bruss_IO_M->Timing.stepSize0 = 0.2;

  /* Setup for data logging */
  {
    static RTWLogInfo rt_DataLoggingInfo;
    rt_DataLoggingInfo.loggingInterval = NULL;
    Bruss_IO_M->rtwLogInfo = &rt_DataLoggingInfo;
  }

  /* Setup for data logging */
  {
    rtliSetLogXSignalInfo(Bruss_IO_M->rtwLogInfo, (NULL));
    rtliSetLogXSignalPtrs(Bruss_IO_M->rtwLogInfo, (NULL));
    rtliSetLogT(Bruss_IO_M->rtwLogInfo, "tout");
    rtliSetLogX(Bruss_IO_M->rtwLogInfo, "");
    rtliSetLogXFinal(Bruss_IO_M->rtwLogInfo, "");
    rtliSetLogVarNameModifier(Bruss_IO_M->rtwLogInfo, "rt_");
    rtliSetLogFormat(Bruss_IO_M->rtwLogInfo, 0);
    rtliSetLogMaxRows(Bruss_IO_M->rtwLogInfo, 1000);
    rtliSetLogDecimation(Bruss_IO_M->rtwLogInfo, 1);
    rtliSetLogY(Bruss_IO_M->rtwLogInfo, "");
    rtliSetLogYSignalInfo(Bruss_IO_M->rtwLogInfo, (NULL));
    rtliSetLogYSignalPtrs(Bruss_IO_M->rtwLogInfo, (NULL));
  }

  /* block I/O */
  (void) memset(((void *) &Bruss_IO_B), 0,
                sizeof(B_Bruss_IO_T));

  /* states (continuous) */
  {
    (void) memset((void *)&Bruss_IO_X, 0,
                  sizeof(X_Bruss_IO_T));
  }

  /* states (dwork) */
  (void) memset((void *)&Bruss_IO_DW, 0,
                sizeof(DW_Bruss_IO_T));

  /* Matfile logging */
  rt_StartDataLoggingWithStartTime(Bruss_IO_M->rtwLogInfo, 0.0, rtmGetTFinal
    (Bruss_IO_M), Bruss_IO_M->Timing.stepSize0, (&rtmGetErrorStatus(Bruss_IO_M)));

  /* Start for FromFile: '<Root>/From File' */
  {
    static const real_T tuData[3] = { 0.0, 0.95, 0.05 } ;

    Bruss_IO_DW.FromFile_PWORK.PrevTimePtr = (void *) &tuData[0];
  }

  /* Start for ToFile: '<Root>/To File' */
  {
    FILE *fp = (NULL);
    char fileName[509] = "output.mat";
    if ((fp = fopen(fileName, "wb")) == (NULL)) {
      rtmSetErrorStatus(Bruss_IO_M, "Error creating .mat file output.mat");
      return;
    }

    if (rt_WriteMat4FileHeader(fp,3,0,"ans")) {
      rtmSetErrorStatus(Bruss_IO_M,
                        "Error writing mat file header to file output.mat");
      return;
    }

    Bruss_IO_DW.ToFile_IWORK.Count = 0;
    Bruss_IO_DW.ToFile_IWORK.Decimation = -1;
    Bruss_IO_DW.ToFile_PWORK.FilePtr = fp;
  }

  /* InitializeConditions for Chart: '<Root>/Chart' */
  Bruss_IO_DW.is_active_c1_Bruss_IO = 0U;
  Bruss_IO_DW.is_c1_Bruss_IO = Bruss_IO_IN_NO_ACTIVE_CHILD;
  Bruss_IO_X.SFunction_CSTATE[0] = 0.0;
  Bruss_IO_X.SFunction_CSTATE[1] = 0.0;
  Bruss_IO_B.x_out = 0.0;
  Bruss_IO_B.y_out = 0.0;
}

/* Model terminate function */
void Bruss_IO_terminate(void)
{
  /* Terminate for ToFile: '<Root>/To File' */
  {
    FILE *fp = (FILE *) Bruss_IO_DW.ToFile_PWORK.FilePtr;
    if (fp != (NULL)) {
      char fileName[509] = "output.mat";
      if (fclose(fp) == EOF) {
        rtmSetErrorStatus(Bruss_IO_M, "Error closing MAT-file output.mat");
        return;
      }

      if ((fp = fopen(fileName, "r+b")) == (NULL)) {
        rtmSetErrorStatus(Bruss_IO_M, "Error reopening MAT-file output.mat");
        return;
      }

      if (rt_WriteMat4FileHeader(fp, 3, Bruss_IO_DW.ToFile_IWORK.Count, "ans"))
      {
        rtmSetErrorStatus(Bruss_IO_M,
                          "Error writing header for ans to MAT-file output.mat");
      }

      if (fclose(fp) == EOF) {
        rtmSetErrorStatus(Bruss_IO_M, "Error closing MAT-file output.mat");
        return;
      }

      Bruss_IO_DW.ToFile_PWORK.FilePtr = (NULL);
    }
  }
}
