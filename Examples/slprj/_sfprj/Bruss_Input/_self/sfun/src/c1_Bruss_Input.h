#ifndef __c1_Bruss_Input_h__
#define __c1_Bruss_Input_h__

/* Include files */
#include "sf_runtime/sfc_sf.h"
#include "sf_runtime/sfc_mex.h"
#include "rtwtypes.h"
#include "multiword_types.h"

/* Type Definitions */
#ifndef typedef_SFc1_Bruss_InputInstanceStruct
#define typedef_SFc1_Bruss_InputInstanceStruct

typedef struct {
  SimStruct *S;
  ChartInfoStruct chartInfo;
  uint32_T chartNumber;
  uint32_T instanceNumber;
  int32_T c1_sfEvent;
  uint8_T c1_tp_Brussellator;
  boolean_T c1_isStable;
  boolean_T c1_stateChanged;
  real_T c1_lastMajorTime;
  uint8_T c1_is_active_c1_Bruss_Input;
  uint8_T c1_is_c1_Bruss_Input;
  uint8_T c1_doSetSimStateSideEffects;
  const mxArray *c1_setSimStateSideEffectsInfo;
  real_T *c1_x;
  real_T *c1_y;
  real_T *c1_x_out;
  real_T *c1_y_out;
  real_T *c1_x_in;
  real_T *c1_y_in;
} SFc1_Bruss_InputInstanceStruct;

#endif                                 /*typedef_SFc1_Bruss_InputInstanceStruct*/

/* Named Constants */

/* Variable Declarations */
extern struct SfDebugInstanceStruct *sfGlobalDebugInstanceStruct;

/* Variable Definitions */

/* Function Declarations */
extern const mxArray *sf_c1_Bruss_Input_get_eml_resolved_functions_info(void);

/* Function Definitions */
extern void sf_c1_Bruss_Input_get_check_sum(mxArray *plhs[]);
extern void c1_Bruss_Input_method_dispatcher(SimStruct *S, int_T method, void
  *data);

#endif
