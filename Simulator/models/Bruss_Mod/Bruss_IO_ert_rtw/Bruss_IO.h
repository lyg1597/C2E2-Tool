//
// Academic License - for use in teaching, academic research, and meeting
// course requirements at degree granting institutions only.  Not for
// government, commercial, or other organizational use.
//
// File: Bruss_IO.h
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
#ifndef RTW_HEADER_Bruss_IO_h_
#define RTW_HEADER_Bruss_IO_h_
#include <string.h>
#ifndef Bruss_IO_COMMON_INCLUDES_
# define Bruss_IO_COMMON_INCLUDES_
#include "rtwtypes.h"
#include "rtw_continuous.h"
#include "rtw_solver.h"
#endif                                 // Bruss_IO_COMMON_INCLUDES_

#include "Bruss_IO_types.h"

// Macros for accessing real-time model data structure
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

// Block states (auto storage) for system '<Root>'
typedef struct {
  uint8_T is_active_c1_Bruss_IO;       // '<Root>/Chart'
} DW_Bruss_IO_T;

// Continuous states (auto storage)
typedef struct {
  real_T SFunction_CSTATE[2];          // '<Root>/Chart'
} X_Bruss_IO_T;

// State derivatives (auto storage)
typedef struct {
  real_T SFunction_CSTATE[2];          // '<Root>/Chart'
} XDot_Bruss_IO_T;

// State disabled
typedef struct {
  boolean_T SFunction_CSTATE[2];       // '<Root>/Chart'
} XDis_Bruss_IO_T;

#ifndef ODE3_INTG
#define ODE3_INTG

// ODE3 Integration Data
typedef struct {
  real_T *y;                           // output
  real_T *f[3];                        // derivatives
} ODE3_IntgData;

#endif

// External inputs (root inport signals with auto storage)
typedef struct {
  real_T X_in;                         // '<Root>/X_in'
  real_T Y_in;                         // '<Root>/Y_in'
} ExtU_Bruss_IO_T;

// External outputs (root outports fed by signals with auto storage)
typedef struct {
  real_T X_out;                        // '<Root>/X_out'
  real_T Y_out;                        // '<Root>/Y_out'
} ExtY_Bruss_IO_T;

// Real-time Model Data Structure
struct tag_RTM_Bruss_IO_T {
  const char_T *errorStatus;
  RTWSolverInfo solverInfo;

  //
  //  ModelData:
  //  The following substructure contains information regarding
  //  the data used in the model.

  struct {
    X_Bruss_IO_T *contStates;
    int_T *periodicContStateIndices;
    real_T *periodicContStateRanges;
    real_T *derivs;
    boolean_T *contStateDisabled;
    boolean_T zCCacheNeedsReset;
    boolean_T derivCacheNeedsReset;
    boolean_T blkStateChange;
    real_T odeY[2];
    real_T odeF[3][2];
    ODE3_IntgData intgData;
  } ModelData;

  //
  //  Sizes:
  //  The following substructure contains sizes information
  //  for many of the model attributes such as inputs, outputs,
  //  dwork, sample times, etc.

  struct {
    int_T numContStates;
    int_T numPeriodicContStates;
    int_T numSampTimes;
  } Sizes;

  //
  //  Timing:
  //  The following substructure contains information regarding
  //  the timing information for the model.

  struct {
    uint32_T clockTick0;
    time_T stepSize0;
    uint32_T clockTick1;
    SimTimeStep simTimeStep;
    boolean_T stopRequestedFlag;
    time_T *t;
    time_T tArray[2];
  } Timing;
};

#ifdef __cplusplus

extern "C" {

#endif

#ifdef __cplusplus

}
#endif

// Class declaration for model Bruss_IO
class Bruss_IOModelClass {
  // public data and function members
 public:
  // External inputs
  ExtU_Bruss_IO_T Bruss_IO_U;

  // External outputs
  ExtY_Bruss_IO_T Bruss_IO_Y;

  // Model entry point functions

  // model initialize function
  void initialize();

  // model step function
  void step();

  // model terminate function
  void terminate();

  // Constructor
  Bruss_IOModelClass();

  // Destructor
  ~Bruss_IOModelClass();

  // Real-Time Model get method
  RT_MODEL_Bruss_IO_T * getRTM();

  // private data and function members
 private:
  // Block states
  DW_Bruss_IO_T Bruss_IO_DW;
  X_Bruss_IO_T Bruss_IO_X;             // Block continuous states

  // Real-Time Model
  RT_MODEL_Bruss_IO_T Bruss_IO_M;

  // Continuous states update member function
  void rt_ertODEUpdateContinuousStates(RTWSolverInfo *si );

  // Derivatives member function
  void Bruss_IO_derivatives();
};

//-
//  The generated code includes comments that allow you to trace directly
//  back to the appropriate location in the model.  The basic format
//  is <system>/block_name, where system is the system number (uniquely
//  assigned by Simulink) and block_name is the name of the block.
//
//  Use the MATLAB hilite_system command to trace the generated code back
//  to the model.  For example,
//
//  hilite_system('<S3>')    - opens system 3
//  hilite_system('<S3>/Kp') - opens and selects block Kp which resides in S3
//
//  Here is the system hierarchy for this model
//
//  '<Root>' : 'Bruss_IO'
//  '<S1>'   : 'Bruss_IO/Chart'
//  '<S2>'   : 'Bruss_IO/XY Graph'

#endif                                 // RTW_HEADER_Bruss_IO_h_

//
// File trailer for generated code.
//
// [EOF]
//
