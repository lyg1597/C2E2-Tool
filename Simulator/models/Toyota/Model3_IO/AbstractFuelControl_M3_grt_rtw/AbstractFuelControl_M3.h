/*
 * AbstractFuelControl_M3.h
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

#ifndef RTW_HEADER_AbstractFuelControl_M3_h_
#define RTW_HEADER_AbstractFuelControl_M3_h_
#include <math.h>
#include <float.h>
#include <string.h>
#include <stddef.h>
#ifndef AbstractFuelControl_M3_COMMON_INCLUDES_
# define AbstractFuelControl_M3_COMMON_INCLUDES_
#include "rtwtypes.h"
#include "rtw_continuous.h"
#include "rtw_solver.h"
#include "rt_logging.h"
#endif                                 /* AbstractFuelControl_M3_COMMON_INCLUDES_ */

#include "AbstractFuelControl_M3_types.h"

/* Shared type includes */
#include "multiword_types.h"
#include "rt_nonfinite.h"
#include "rtGetInf.h"

/* Macros for accessing real-time model data structure */
#ifndef rtmGetBlkStateChangeFlag
# define rtmGetBlkStateChangeFlag(rtm) ((rtm)->ModelData.blkStateChange)
#endif

#ifndef rtmSetBlkStateChangeFlag
# define rtmSetBlkStateChangeFlag(rtm, val) ((rtm)->ModelData.blkStateChange = (val))
#endif

#ifndef rtmGetContStateDisabled
# define rtmGetContStateDisabled(rtm)  ((rtm)->ModelData.contStateDisabled)
#endif

#ifndef rtmSetContStateDisabled
# define rtmSetContStateDisabled(rtm, val) ((rtm)->ModelData.contStateDisabled = (val))
#endif

#ifndef rtmGetContStates
# define rtmGetContStates(rtm)         ((rtm)->ModelData.contStates)
#endif

#ifndef rtmSetContStates
# define rtmSetContStates(rtm, val)    ((rtm)->ModelData.contStates = (val))
#endif

#ifndef rtmGetDerivCacheNeedsReset
# define rtmGetDerivCacheNeedsReset(rtm) ((rtm)->ModelData.derivCacheNeedsReset)
#endif

#ifndef rtmSetDerivCacheNeedsReset
# define rtmSetDerivCacheNeedsReset(rtm, val) ((rtm)->ModelData.derivCacheNeedsReset = (val))
#endif

#ifndef rtmGetFinalTime
# define rtmGetFinalTime(rtm)          ((rtm)->Timing.tFinal)
#endif

#ifndef rtmGetIntgData
# define rtmGetIntgData(rtm)           ((rtm)->ModelData.intgData)
#endif

#ifndef rtmSetIntgData
# define rtmSetIntgData(rtm, val)      ((rtm)->ModelData.intgData = (val))
#endif

#ifndef rtmGetOdeF
# define rtmGetOdeF(rtm)               ((rtm)->ModelData.odeF)
#endif

#ifndef rtmSetOdeF
# define rtmSetOdeF(rtm, val)          ((rtm)->ModelData.odeF = (val))
#endif

#ifndef rtmGetOdeY
# define rtmGetOdeY(rtm)               ((rtm)->ModelData.odeY)
#endif

#ifndef rtmSetOdeY
# define rtmSetOdeY(rtm, val)          ((rtm)->ModelData.odeY = (val))
#endif

#ifndef rtmGetPeriodicContStateIndices
# define rtmGetPeriodicContStateIndices(rtm) ((rtm)->ModelData.periodicContStateIndices)
#endif

#ifndef rtmSetPeriodicContStateIndices
# define rtmSetPeriodicContStateIndices(rtm, val) ((rtm)->ModelData.periodicContStateIndices = (val))
#endif

#ifndef rtmGetPeriodicContStateRanges
# define rtmGetPeriodicContStateRanges(rtm) ((rtm)->ModelData.periodicContStateRanges)
#endif

#ifndef rtmSetPeriodicContStateRanges
# define rtmSetPeriodicContStateRanges(rtm, val) ((rtm)->ModelData.periodicContStateRanges = (val))
#endif

#ifndef rtmGetRTWLogInfo
# define rtmGetRTWLogInfo(rtm)         ((rtm)->rtwLogInfo)
#endif

#ifndef rtmGetZCCacheNeedsReset
# define rtmGetZCCacheNeedsReset(rtm)  ((rtm)->ModelData.zCCacheNeedsReset)
#endif

#ifndef rtmSetZCCacheNeedsReset
# define rtmSetZCCacheNeedsReset(rtm, val) ((rtm)->ModelData.zCCacheNeedsReset = (val))
#endif

#ifndef rtmGetdX
# define rtmGetdX(rtm)                 ((rtm)->ModelData.derivs)
#endif

#ifndef rtmSetdX
# define rtmSetdX(rtm, val)            ((rtm)->ModelData.derivs = (val))
#endif

#ifndef rtmGetErrorStatus
# define rtmGetErrorStatus(rtm)        ((rtm)->errorStatus)
#endif

#ifndef rtmSetErrorStatus
# define rtmSetErrorStatus(rtm, val)   ((rtm)->errorStatus = (val))
#endif

#ifndef rtmGetStopRequested
# define rtmGetStopRequested(rtm)      ((rtm)->Timing.stopRequestedFlag)
#endif

#ifndef rtmSetStopRequested
# define rtmSetStopRequested(rtm, val) ((rtm)->Timing.stopRequestedFlag = (val))
#endif

#ifndef rtmGetStopRequestedPtr
# define rtmGetStopRequestedPtr(rtm)   (&((rtm)->Timing.stopRequestedFlag))
#endif

#ifndef rtmGetT
# define rtmGetT(rtm)                  (rtmGetTPtr((rtm))[0])
#endif

#ifndef rtmGetTFinal
# define rtmGetTFinal(rtm)             ((rtm)->Timing.tFinal)
#endif

/* Block signals (auto storage) */
typedef struct {
  real_T ControllerMode;               /* '<Root>/Data Type Conversion' */
  real_T MultiportSwitch;              /* '<S5>/Multiport Switch' */
  real_T Product;                      /* '<S6>/Product' */
  real_T Sqrt;                         /* '<S6>/Sqrt' */
  real_T Sum1;                         /* '<S7>/Sum1' */
  real_T Delay;                        /* '<S1>/Delay' */
  real_T PowerModeGuard;               /* '<S1>/Power Mode Guard' */
  real_T ODE1;                         /* '<S1>/ODE1' */
  real_T ODE2;                         /* '<S1>/ODE2' */
  real_T ODE3;                         /* '<S1>/ODE3' */
  real_T Switch1;                      /* '<S1>/Switch1' */
  real_T Switch2;                      /* '<S1>/Switch2' */
  boolean_T RelationalOperator;        /* '<S2>/Relational Operator' */
  boolean_T HiddenBuf_InsertedFor_CalcuateE;/* '<S2>/Relational Operator' */
  boolean_T IntegerDelay1;             /* '<S4>/Integer Delay1' */
  boolean_T LogicalOperator;           /* '<S4>/Logical Operator' */
  boolean_T IntegerDelay1_j;           /* '<S3>/Integer Delay1' */
  boolean_T LogicalOperator_b;         /* '<S3>/Logical Operator' */
} B_AbstractFuelControl_M3_T;

/* Block states (auto storage) for system '<Root>' */
typedef struct {
  real_T Delay_DSTATE;                 /* '<S1>/Delay' */
  struct {
    void *LoggedData[4];
  } Monitor_PWORK;                     /* '<Root>/Monitor ' */

  int32_T clockTickCounter;            /* '<Root>/Pedal Angle (deg)' */
  boolean_T IntegerDelay1_DSTATE;      /* '<S4>/Integer Delay1' */
  boolean_T IntegerDelay1_DSTATE_a;    /* '<S3>/Integer Delay1' */
  int8_T SwitchCase_ActiveSubsystem;   /* '<S5>/Switch Case' */
  boolean_T PowerModeGuard_Mode;       /* '<S1>/Power Mode Guard' */
  boolean_T CalcuateError_MODE;        /* '<S2>/Calcuate Error' */
} DW_AbstractFuelControl_M3_T;

/* Continuous states (auto storage) */
typedef struct {
  real_T Integrator_CSTATE;            /* '<S6>/Integrator' */
  real_T p_CSTATE;                     /* '<S1>/p' */
  real_T lambda_CSTATE;                /* '<S1>/lambda' */
  real_T pe_CSTATE;                    /* '<S1>/pe' */
  real_T i_CSTATE;                     /* '<S1>/i' */
  real_T Throttledelay1_CSTATE;        /* '<S1>/Throttle  delay1' */
} X_AbstractFuelControl_M3_T;

/* State derivatives (auto storage) */
typedef struct {
  real_T Integrator_CSTATE;            /* '<S6>/Integrator' */
  real_T p_CSTATE;                     /* '<S1>/p' */
  real_T lambda_CSTATE;                /* '<S1>/lambda' */
  real_T pe_CSTATE;                    /* '<S1>/pe' */
  real_T i_CSTATE;                     /* '<S1>/i' */
  real_T Throttledelay1_CSTATE;        /* '<S1>/Throttle  delay1' */
} XDot_AbstractFuelControl_M3_T;

/* State disabled  */
typedef struct {
  boolean_T Integrator_CSTATE;         /* '<S6>/Integrator' */
  boolean_T p_CSTATE;                  /* '<S1>/p' */
  boolean_T lambda_CSTATE;             /* '<S1>/lambda' */
  boolean_T pe_CSTATE;                 /* '<S1>/pe' */
  boolean_T i_CSTATE;                  /* '<S1>/i' */
  boolean_T Throttledelay1_CSTATE;     /* '<S1>/Throttle  delay1' */
} XDis_AbstractFuelControl_M3_T;

#ifndef ODE3_INTG
#define ODE3_INTG

/* ODE3 Integration Data */
typedef struct {
  real_T *y;                           /* output */
  real_T *f[3];                        /* derivatives */
} ODE3_IntgData;

#endif

/* External inputs (root inport signals with auto storage) */
typedef struct {
  real_T EngineSpeed;                  /* '<Root>/Engine Speed (rpm)' */
} ExtU_AbstractFuelControl_M3_T;

/* External outputs (root outports fed by signals with auto storage) */
typedef struct {
  real_T verificatonmeasurement;       /* '<Root>/verificaton measurement' */
  real_T mode;                         /* '<Root>/mode' */
  real_T pedalangle;                   /* '<Root>/pedal angle' */
} ExtY_AbstractFuelControl_M3_T;

/* Parameters (auto storage) */
struct P_AbstractFuelControl_M3_T_ {
  real_T fault_time;                   /* Variable: fault_time
                                        * Referenced by: '<S1>/FaultInjection'
                                        */
  real_T measureTime;                  /* Variable: measureTime
                                        * Referenced by: '<S2>/MeasureOn'
                                        */
  real_T simTime;                      /* Variable: simTime
                                        * Referenced by: '<S6>/Constant'
                                        */
  uint8_T spec_num;                    /* Variable: spec_num
                                        * Referenced by: '<S2>/spec_num'
                                        */
  real_T rpmtorads_Gain;               /* Expression: pi/30
                                        * Referenced by: '<S1>/(rpm) to (rad//s)'
                                        */
  real_T Baseopeningangle_Value;       /* Expression: 8.8
                                        * Referenced by: '<S1>/Base opening angle'
                                        */
  real_T Constant3_Value;              /* Expression: 12.5
                                        * Referenced by: '<S1>/Constant3'
                                        */
  real_T Constant4_Value;              /* Expression: 14.7
                                        * Referenced by: '<S1>/Constant4'
                                        */
  real_T FaultInjection_Y0;            /* Expression: 0
                                        * Referenced by: '<S1>/FaultInjection'
                                        */
  real_T FaultInjection_YFinal;        /* Expression: 1
                                        * Referenced by: '<S1>/FaultInjection'
                                        */
  real_T StarupMode_Time;              /* Expression: 10
                                        * Referenced by: '<S1>/Starup Mode'
                                        */
  real_T StarupMode_Y0;                /* Expression: 1
                                        * Referenced by: '<S1>/Starup Mode'
                                        */
  real_T StarupMode_YFinal;            /* Expression: 0
                                        * Referenced by: '<S1>/Starup Mode'
                                        */
  real_T Delay_InitialCondition;       /* Expression: 0
                                        * Referenced by: '<S1>/Delay'
                                        */
  real_T p_IC;                         /* Expression: 0.6353
                                        * Referenced by: '<S1>/p'
                                        */
  real_T lambda_IC;                    /* Expression: 14.7
                                        * Referenced by: '<S1>/lambda'
                                        */
  real_T pe_IC;                        /* Expression: 0.5573
                                        * Referenced by: '<S1>/pe'
                                        */
  real_T i_IC;                         /* Expression: 0.017
                                        * Referenced by: '<S1>/i'
                                        */
  real_T Throttledelay1_A;             /* Computed Parameter: Throttledelay1_A
                                        * Referenced by: '<S1>/Throttle  delay1'
                                        */
  real_T Throttledelay1_C;             /* Computed Parameter: Throttledelay1_C
                                        * Referenced by: '<S1>/Throttle  delay1'
                                        */
  real_T theta090_UpperSat;            /* Expression: 90
                                        * Referenced by: '<S1>/theta [0 90]'
                                        */
  real_T theta090_LowerSat;            /* Expression: 0
                                        * Referenced by: '<S1>/theta [0 90]'
                                        */
  real_T PowerModeGuard_OnVal;         /* Expression: 70
                                        * Referenced by: '<S1>/Power Mode Guard'
                                        */
  real_T PowerModeGuard_OffVal;        /* Expression: 50
                                        * Referenced by: '<S1>/Power Mode Guard'
                                        */
  real_T PowerModeGuard_YOn;           /* Expression: 1
                                        * Referenced by: '<S1>/Power Mode Guard'
                                        */
  real_T PowerModeGuard_YOff;          /* Expression: 0
                                        * Referenced by: '<S1>/Power Mode Guard'
                                        */
  real_T Constant_Value;               /* Expression: 1
                                        * Referenced by: '<S7>/Constant'
                                        */
  real_T Integrator_IC;                /* Expression: 0
                                        * Referenced by: '<S6>/Integrator'
                                        */
  real_T MeasureOn_Time;               /* Expression: 10
                                        * Referenced by: '<S6>/MeasureOn'
                                        */
  real_T MeasureOn_Y0;                 /* Expression: 0
                                        * Referenced by: '<S6>/MeasureOn'
                                        */
  real_T MeasureOn_YFinal;             /* Expression: 1
                                        * Referenced by: '<S6>/MeasureOn'
                                        */
  real_T MeasureOn_Y0_b;               /* Expression: 0
                                        * Referenced by: '<S2>/MeasureOn'
                                        */
  real_T MeasureOn_YFinal_k;           /* Expression: 1
                                        * Referenced by: '<S2>/MeasureOn'
                                        */
  real_T Constant_Value_n;             /* Expression: 0.5
                                        * Referenced by: '<S2>/Constant'
                                        */
  real_T PedalAngledeg_Amp;            /* Expression: 53.468
                                        * Referenced by: '<Root>/Pedal Angle (deg)'
                                        */
  real_T PedalAngledeg_Period;         /* Computed Parameter: PedalAngledeg_Period
                                        * Referenced by: '<Root>/Pedal Angle (deg)'
                                        */
  real_T PedalAngledeg_Duty;           /* Computed Parameter: PedalAngledeg_Duty
                                        * Referenced by: '<Root>/Pedal Angle (deg)'
                                        */
  real_T PedalAngledeg_PhaseDelay;     /* Expression: 3
                                        * Referenced by: '<Root>/Pedal Angle (deg)'
                                        */
  uint32_T IntegerDelay1_DelayLength;  /* Computed Parameter: IntegerDelay1_DelayLength
                                        * Referenced by: '<S3>/Integer Delay1'
                                        */
  uint32_T IntegerDelay1_DelayLength_f;/* Computed Parameter: IntegerDelay1_DelayLength_f
                                        * Referenced by: '<S4>/Integer Delay1'
                                        */
  uint32_T Delay_DelayLength;          /* Computed Parameter: Delay_DelayLength
                                        * Referenced by: '<S1>/Delay'
                                        */
  boolean_T IntegerDelay1_InitialCondition;/* Computed Parameter: IntegerDelay1_InitialCondition
                                            * Referenced by: '<S3>/Integer Delay1'
                                            */
  boolean_T IntegerDelay1_InitialConditio_e;/* Computed Parameter: IntegerDelay1_InitialConditio_e
                                             * Referenced by: '<S4>/Integer Delay1'
                                             */
};

/* Real-time Model Data Structure */
struct tag_RTM_AbstractFuelControl_M_T {
  const char_T *errorStatus;
  RTWLogInfo *rtwLogInfo;
  RTWSolverInfo solverInfo;

  /*
   * ModelData:
   * The following substructure contains information regarding
   * the data used in the model.
   */
  struct {
    X_AbstractFuelControl_M3_T *contStates;
    int_T *periodicContStateIndices;
    real_T *periodicContStateRanges;
    real_T *derivs;
    boolean_T *contStateDisabled;
    boolean_T zCCacheNeedsReset;
    boolean_T derivCacheNeedsReset;
    boolean_T blkStateChange;
    real_T odeY[6];
    real_T odeF[3][6];
    ODE3_IntgData intgData;
  } ModelData;

  /*
   * Sizes:
   * The following substructure contains sizes information
   * for many of the model attributes such as inputs, outputs,
   * dwork, sample times, etc.
   */
  struct {
    int_T numContStates;
    int_T numPeriodicContStates;
    int_T numSampTimes;
  } Sizes;

  /*
   * Timing:
   * The following substructure contains information regarding
   * the timing information for the model.
   */
  struct {
    uint32_T clockTick0;
    uint32_T clockTickH0;
    time_T stepSize0;
    uint32_T clockTick1;
    uint32_T clockTickH1;
    time_T tFinal;
    SimTimeStep simTimeStep;
    boolean_T stopRequestedFlag;
    time_T *t;
    time_T tArray[2];
  } Timing;
};

/* Block parameters (auto storage) */
extern P_AbstractFuelControl_M3_T AbstractFuelControl_M3_P;

/* Block signals (auto storage) */
extern B_AbstractFuelControl_M3_T AbstractFuelControl_M3_B;

/* Continuous states (auto storage) */
extern X_AbstractFuelControl_M3_T AbstractFuelControl_M3_X;

/* Block states (auto storage) */
extern DW_AbstractFuelControl_M3_T AbstractFuelControl_M3_DW;

/* External inputs (root inport signals with auto storage) */
extern ExtU_AbstractFuelControl_M3_T AbstractFuelControl_M3_U;

/* External outputs (root outports fed by signals with auto storage) */
extern ExtY_AbstractFuelControl_M3_T AbstractFuelControl_M3_Y;

/* Model entry point functions */
extern void AbstractFuelControl_M3_initialize(void);
extern void AbstractFuelControl_M3_step(void);
extern void AbstractFuelControl_M3_terminate(void);

/* Real-time Model object */
extern RT_MODEL_AbstractFuelControl__T *const AbstractFuelControl_M3_M;

/*-
 * The generated code includes comments that allow you to trace directly
 * back to the appropriate location in the model.  The basic format
 * is <system>/block_name, where system is the system number (uniquely
 * assigned by Simulink) and block_name is the name of the block.
 *
 * Use the MATLAB hilite_system command to trace the generated code back
 * to the model.  For example,
 *
 * hilite_system('<S3>')    - opens system 3
 * hilite_system('<S3>/Kp') - opens and selects block Kp which resides in S3
 *
 * Here is the system hierarchy for this model
 *
 * '<Root>' : 'AbstractFuelControl_M3'
 * '<S1>'   : 'AbstractFuelControl_M3/Model 3'
 * '<S2>'   : 'AbstractFuelControl_M3/V&V stub system'
 * '<S3>'   : 'AbstractFuelControl_M3/Model 3/Sensor Failure Detection Latch'
 * '<S4>'   : 'AbstractFuelControl_M3/Model 3/Startup Mode Latch'
 * '<S5>'   : 'AbstractFuelControl_M3/V&V stub system/Calcuate Error'
 * '<S6>'   : 'AbstractFuelControl_M3/V&V stub system/Calcuate Error/RMS error'
 * '<S7>'   : 'AbstractFuelControl_M3/V&V stub system/Calcuate Error/over(under)shoot'
 */
#endif                                 /* RTW_HEADER_AbstractFuelControl_M3_h_ */
