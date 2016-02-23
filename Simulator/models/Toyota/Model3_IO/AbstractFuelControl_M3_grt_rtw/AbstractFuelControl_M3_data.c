/*
 * AbstractFuelControl_M3_data.c
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

/* Block parameters (auto storage) */
P_AbstractFuelControl_M3_T AbstractFuelControl_M3_P = {
  60.0,                                /* Variable: fault_time
                                        * Referenced by: '<S1>/FaultInjection'
                                        */
  1.0,                                 /* Variable: measureTime
                                        * Referenced by: '<S2>/MeasureOn'
                                        */
  50.0,                                /* Variable: simTime
                                        * Referenced by: '<S6>/Constant'
                                        */
  1U,                                  /* Variable: spec_num
                                        * Referenced by: '<S2>/spec_num'
                                        */
  0.10471975511965977,                 /* Expression: pi/30
                                        * Referenced by: '<S1>/(rpm) to (rad//s)'
                                        */
  8.8,                                 /* Expression: 8.8
                                        * Referenced by: '<S1>/Base opening angle'
                                        */
  12.5,                                /* Expression: 12.5
                                        * Referenced by: '<S1>/Constant3'
                                        */
  14.7,                                /* Expression: 14.7
                                        * Referenced by: '<S1>/Constant4'
                                        */
  0.0,                                 /* Expression: 0
                                        * Referenced by: '<S1>/FaultInjection'
                                        */
  1.0,                                 /* Expression: 1
                                        * Referenced by: '<S1>/FaultInjection'
                                        */
  10.0,                                /* Expression: 10
                                        * Referenced by: '<S1>/Starup Mode'
                                        */
  1.0,                                 /* Expression: 1
                                        * Referenced by: '<S1>/Starup Mode'
                                        */
  0.0,                                 /* Expression: 0
                                        * Referenced by: '<S1>/Starup Mode'
                                        */
  0.0,                                 /* Expression: 0
                                        * Referenced by: '<S1>/Delay'
                                        */
  0.6353,                              /* Expression: 0.6353
                                        * Referenced by: '<S1>/p'
                                        */
  14.7,                                /* Expression: 14.7
                                        * Referenced by: '<S1>/lambda'
                                        */
  0.5573,                              /* Expression: 0.5573
                                        * Referenced by: '<S1>/pe'
                                        */
  0.017,                               /* Expression: 0.017
                                        * Referenced by: '<S1>/i'
                                        */
  -10.0,                               /* Computed Parameter: Throttledelay1_A
                                        * Referenced by: '<S1>/Throttle  delay1'
                                        */
  10.0,                                /* Computed Parameter: Throttledelay1_C
                                        * Referenced by: '<S1>/Throttle  delay1'
                                        */
  90.0,                                /* Expression: 90
                                        * Referenced by: '<S1>/theta [0 90]'
                                        */
  0.0,                                 /* Expression: 0
                                        * Referenced by: '<S1>/theta [0 90]'
                                        */
  70.0,                                /* Expression: 70
                                        * Referenced by: '<S1>/Power Mode Guard'
                                        */
  50.0,                                /* Expression: 50
                                        * Referenced by: '<S1>/Power Mode Guard'
                                        */
  1.0,                                 /* Expression: 1
                                        * Referenced by: '<S1>/Power Mode Guard'
                                        */
  0.0,                                 /* Expression: 0
                                        * Referenced by: '<S1>/Power Mode Guard'
                                        */
  1.0,                                 /* Expression: 1
                                        * Referenced by: '<S7>/Constant'
                                        */
  0.0,                                 /* Expression: 0
                                        * Referenced by: '<S6>/Integrator'
                                        */
  10.0,                                /* Expression: 10
                                        * Referenced by: '<S6>/MeasureOn'
                                        */
  0.0,                                 /* Expression: 0
                                        * Referenced by: '<S6>/MeasureOn'
                                        */
  1.0,                                 /* Expression: 1
                                        * Referenced by: '<S6>/MeasureOn'
                                        */
  0.0,                                 /* Expression: 0
                                        * Referenced by: '<S2>/MeasureOn'
                                        */
  1.0,                                 /* Expression: 1
                                        * Referenced by: '<S2>/MeasureOn'
                                        */
  0.5,                                 /* Expression: 0.5
                                        * Referenced by: '<S2>/Constant'
                                        */
  53.468,                              /* Expression: 53.468
                                        * Referenced by: '<Root>/Pedal Angle (deg)'
                                        */
  410446.0,                            /* Computed Parameter: PedalAngledeg_Period
                                        * Referenced by: '<Root>/Pedal Angle (deg)'
                                        */
  205223.0,                            /* Computed Parameter: PedalAngledeg_Duty
                                        * Referenced by: '<Root>/Pedal Angle (deg)'
                                        */
  3.0,                                 /* Expression: 3
                                        * Referenced by: '<Root>/Pedal Angle (deg)'
                                        */
  1U,                                  /* Computed Parameter: IntegerDelay1_DelayLength
                                        * Referenced by: '<S3>/Integer Delay1'
                                        */
  1U,                                  /* Computed Parameter: IntegerDelay1_DelayLength_f
                                        * Referenced by: '<S4>/Integer Delay1'
                                        */
  1U,                                  /* Computed Parameter: Delay_DelayLength
                                        * Referenced by: '<S1>/Delay'
                                        */
  0,                                   /* Computed Parameter: IntegerDelay1_InitialCondition
                                        * Referenced by: '<S3>/Integer Delay1'
                                        */
  0                                    /* Computed Parameter: IntegerDelay1_InitialConditio_e
                                        * Referenced by: '<S4>/Integer Delay1'
                                        */
};
