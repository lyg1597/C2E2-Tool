#ifndef __c1_Bruss_h__
#define __c1_Bruss_h__

/* Include files */
#include "sf_runtime/sfc_sf.h"
#include "sf_runtime/sfc_mex.h"
#include "rtwtypes.h"
#include "multiword_types.h"

/* Type Definitions */
#ifndef typedef_SFc1_BrussInstanceStruct
#define typedef_SFc1_BrussInstanceStruct

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
  uint8_T c1_is_active_c1_Bruss;
  uint8_T c1_is_c1_Bruss;
  uint8_T c1_doSetSimStateSideEffects;
  const mxArray *c1_setSimStateSideEffectsInfo;
  real_T *c1_x;
  real_T *c1_y;
  real_T *c1_x_out;
  real_T *c1_y_out;
} SFc1_BrussInstanceStruct;

#endif                                 /*typedef_SFc1_BrussInstanceStruct*/

/* Named Constants */

/* Variable Declarations */
extern struct SfDebugInstanceStruct *sfGlobalDebugInstanceStruct;

/* Variable Definitions */

/* Function Declarations */
extern const mxArray *sf_c1_Bruss_get_eml_resolved_functions_info(void);

/* Function Definitions */
extern void sf_c1_Bruss_get_check_sum(mxArray *plhs[]);
extern void c1_Bruss_method_dispatcher(SimStruct *S, int_T method, void *data);

#endif
