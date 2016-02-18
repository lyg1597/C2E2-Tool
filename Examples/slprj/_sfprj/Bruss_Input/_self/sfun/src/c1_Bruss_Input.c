/* Include files */

#include <stddef.h>
#include "blas.h"
#include "Bruss_Input_sfun.h"
#include "c1_Bruss_Input.h"
#define CHARTINSTANCE_CHARTNUMBER      (chartInstance->chartNumber)
#define CHARTINSTANCE_INSTANCENUMBER   (chartInstance->instanceNumber)
#include "Bruss_Input_sfun_debug_macros.h"
#define _SF_MEX_LISTEN_FOR_CTRL_C(S)   sf_mex_listen_for_ctrl_c_with_debugger(S, sfGlobalDebugInstanceStruct);

static void chart_debug_initialization(SimStruct *S, unsigned int
  fullDebuggerInitialization);
static void chart_debug_initialize_data_addresses(SimStruct *S);
static const mxArray* sf_opaque_get_hover_data_for_msg(void *chartInstance,
  int32_T msgSSID);

/* Type Definitions */

/* Named Constants */
#define CALL_EVENT                     (-1)
#define c1_IN_NO_ACTIVE_CHILD          ((uint8_T)0U)
#define c1_IN_Brussellator             ((uint8_T)1U)

/* Variable Declarations */

/* Variable Definitions */
static real_T _sfTime_;

/* Function Declarations */
static void c1_durationReferenceTimeUpdater(SFc1_Bruss_InputInstanceStruct
  *chartInstance);
static void initialize_c1_Bruss_Input(SFc1_Bruss_InputInstanceStruct
  *chartInstance);
static void initialize_params_c1_Bruss_Input(SFc1_Bruss_InputInstanceStruct
  *chartInstance);
static void enable_c1_Bruss_Input(SFc1_Bruss_InputInstanceStruct *chartInstance);
static void disable_c1_Bruss_Input(SFc1_Bruss_InputInstanceStruct *chartInstance);
static void c1_update_debugger_state_c1_Bruss_Input
  (SFc1_Bruss_InputInstanceStruct *chartInstance);
static const mxArray *get_sim_state_c1_Bruss_Input
  (SFc1_Bruss_InputInstanceStruct *chartInstance);
static void set_sim_state_c1_Bruss_Input(SFc1_Bruss_InputInstanceStruct
  *chartInstance, const mxArray *c1_st);
static void c1_set_sim_state_side_effects_c1_Bruss_Input
  (SFc1_Bruss_InputInstanceStruct *chartInstance);
static void finalize_c1_Bruss_Input(SFc1_Bruss_InputInstanceStruct
  *chartInstance);
static void sf_gateway_c1_Bruss_Input(SFc1_Bruss_InputInstanceStruct
  *chartInstance);
static void mdl_start_c1_Bruss_Input(SFc1_Bruss_InputInstanceStruct
  *chartInstance);
static void zeroCrossings_c1_Bruss_Input(SFc1_Bruss_InputInstanceStruct
  *chartInstance);
static void derivatives_c1_Bruss_Input(SFc1_Bruss_InputInstanceStruct
  *chartInstance);
static void outputs_c1_Bruss_Input(SFc1_Bruss_InputInstanceStruct *chartInstance);
static void initSimStructsc1_Bruss_Input(SFc1_Bruss_InputInstanceStruct
  *chartInstance);
static void c1_eml_ini_fcn_to_be_inlined_37(SFc1_Bruss_InputInstanceStruct
  *chartInstance);
static void c1_eml_term_fcn_to_be_inlined_37(SFc1_Bruss_InputInstanceStruct
  *chartInstance);
static void init_script_number_translation(uint32_T c1_machineNumber, uint32_T
  c1_chartNumber, uint32_T c1_instanceNumber);
static const mxArray *c1_emlrt_marshallOut(SFc1_Bruss_InputInstanceStruct
  *chartInstance, const int32_T c1_u);
static const mxArray *c1_sf_marshallOut(void *chartInstanceVoid, void *c1_inData);
static int32_T c1_emlrt_marshallIn(SFc1_Bruss_InputInstanceStruct *chartInstance,
  const mxArray *c1_b_sfEvent, const char_T *c1_identifier);
static int32_T c1_b_emlrt_marshallIn(SFc1_Bruss_InputInstanceStruct
  *chartInstance, const mxArray *c1_u, const emlrtMsgIdentifier *c1_parentId);
static void c1_sf_marshallIn(void *chartInstanceVoid, const mxArray
  *c1_mxArrayInData, const char_T *c1_varName, void *c1_outData);
static const mxArray *c1_b_emlrt_marshallOut(SFc1_Bruss_InputInstanceStruct
  *chartInstance, const uint8_T c1_u);
static const mxArray *c1_b_sf_marshallOut(void *chartInstanceVoid, void
  *c1_inData);
static uint8_T c1_c_emlrt_marshallIn(SFc1_Bruss_InputInstanceStruct
  *chartInstance, const mxArray *c1_b_tp_Brussellator, const char_T
  *c1_identifier);
static uint8_T c1_d_emlrt_marshallIn(SFc1_Bruss_InputInstanceStruct
  *chartInstance, const mxArray *c1_u, const emlrtMsgIdentifier *c1_parentId);
static void c1_b_sf_marshallIn(void *chartInstanceVoid, const mxArray
  *c1_mxArrayInData, const char_T *c1_varName, void *c1_outData);
static const mxArray *c1_c_emlrt_marshallOut(SFc1_Bruss_InputInstanceStruct
  *chartInstance, const real_T c1_u);
static const mxArray *c1_c_sf_marshallOut(void *chartInstanceVoid, void
  *c1_inData);
static real_T c1_e_emlrt_marshallIn(SFc1_Bruss_InputInstanceStruct
  *chartInstance, const mxArray *c1_b_x, const char_T *c1_identifier);
static real_T c1_f_emlrt_marshallIn(SFc1_Bruss_InputInstanceStruct
  *chartInstance, const mxArray *c1_u, const emlrtMsgIdentifier *c1_parentId);
static void c1_c_sf_marshallIn(void *chartInstanceVoid, const mxArray
  *c1_mxArrayInData, const char_T *c1_varName, void *c1_outData);
static const mxArray *c1_d_emlrt_marshallOut(SFc1_Bruss_InputInstanceStruct
  *chartInstance);
static void c1_g_emlrt_marshallIn(SFc1_Bruss_InputInstanceStruct *chartInstance,
  const mxArray *c1_u);
static const mxArray *c1_h_emlrt_marshallIn(SFc1_Bruss_InputInstanceStruct
  *chartInstance, const mxArray *c1_b_setSimStateSideEffectsInfo, const char_T
  *c1_identifier);
static const mxArray *c1_i_emlrt_marshallIn(SFc1_Bruss_InputInstanceStruct
  *chartInstance, const mxArray *c1_u, const emlrtMsgIdentifier *c1_parentId);
static const mxArray *sf_get_hover_data_for_msg(SFc1_Bruss_InputInstanceStruct
  *chartInstance, int32_T c1_ssid);
static void c1_init_sf_message_store_memory(SFc1_Bruss_InputInstanceStruct
  *chartInstance);
static void init_dsm_address_info(SFc1_Bruss_InputInstanceStruct *chartInstance);
static void init_simulink_io_address(SFc1_Bruss_InputInstanceStruct
  *chartInstance);

/* Function Definitions */
static void c1_durationReferenceTimeUpdater(SFc1_Bruss_InputInstanceStruct
  *chartInstance)
{
  (void)chartInstance;
}

static void initialize_c1_Bruss_Input(SFc1_Bruss_InputInstanceStruct
  *chartInstance)
{
  if (sf_is_first_init_cond(chartInstance->S)) {
    chart_debug_initialize_data_addresses(chartInstance->S);
  }

  chartInstance->c1_sfEvent = CALL_EVENT;
  _sfTime_ = sf_get_time(chartInstance->S);
  chartInstance->c1_doSetSimStateSideEffects = 0U;
  chartInstance->c1_setSimStateSideEffectsInfo = NULL;
  chartInstance->c1_tp_Brussellator = 0U;
  chartInstance->c1_is_active_c1_Bruss_Input = 0U;
  chartInstance->c1_is_c1_Bruss_Input = c1_IN_NO_ACTIVE_CHILD;
  *chartInstance->c1_x = 0.0;
  _SFD_DATA_RANGE_CHECK(*chartInstance->c1_x, 0U, 1U, 0U,
                        chartInstance->c1_sfEvent, false);
  *chartInstance->c1_y = 0.0;
  _SFD_DATA_RANGE_CHECK(*chartInstance->c1_y, 1U, 1U, 0U,
                        chartInstance->c1_sfEvent, false);
  if (!(sf_get_output_port_reusable(chartInstance->S, 1) != 0)) {
    *chartInstance->c1_x_out = 0.0;
    _SFD_DATA_RANGE_CHECK(*chartInstance->c1_x_out, 4U, 1U, 0U,
                          chartInstance->c1_sfEvent, false);
  }

  if (!(sf_get_output_port_reusable(chartInstance->S, 2) != 0)) {
    *chartInstance->c1_y_out = 0.0;
    _SFD_DATA_RANGE_CHECK(*chartInstance->c1_y_out, 5U, 1U, 0U,
                          chartInstance->c1_sfEvent, false);
  }
}

static void initialize_params_c1_Bruss_Input(SFc1_Bruss_InputInstanceStruct
  *chartInstance)
{
  (void)chartInstance;
}

static void enable_c1_Bruss_Input(SFc1_Bruss_InputInstanceStruct *chartInstance)
{
  _sfTime_ = sf_get_time(chartInstance->S);
}

static void disable_c1_Bruss_Input(SFc1_Bruss_InputInstanceStruct *chartInstance)
{
  _sfTime_ = sf_get_time(chartInstance->S);
}

static void c1_update_debugger_state_c1_Bruss_Input
  (SFc1_Bruss_InputInstanceStruct *chartInstance)
{
  uint32_T c1_prevAniVal;
  c1_prevAniVal = _SFD_GET_ANIMATION();
  _SFD_SET_ANIMATION(0U);
  _SFD_SET_HONOR_BREAKPOINTS(0U);
  if (chartInstance->c1_is_active_c1_Bruss_Input == 1U) {
    _SFD_CC_CALL(CHART_ACTIVE_TAG, 0U, chartInstance->c1_sfEvent);
  }

  if (chartInstance->c1_is_c1_Bruss_Input == c1_IN_Brussellator) {
    _SFD_CS_CALL(STATE_ACTIVE_TAG, 0U, chartInstance->c1_sfEvent);
  } else {
    _SFD_CS_CALL(STATE_INACTIVE_TAG, 0U, chartInstance->c1_sfEvent);
  }

  _SFD_SET_ANIMATION(c1_prevAniVal);
  _SFD_SET_HONOR_BREAKPOINTS(1U);
  _SFD_ANIMATE();
}

static const mxArray *get_sim_state_c1_Bruss_Input
  (SFc1_Bruss_InputInstanceStruct *chartInstance)
{
  const mxArray *c1_st = NULL;
  c1_st = NULL;
  sf_mex_assign(&c1_st, c1_d_emlrt_marshallOut(chartInstance), false);
  return c1_st;
}

static void set_sim_state_c1_Bruss_Input(SFc1_Bruss_InputInstanceStruct
  *chartInstance, const mxArray *c1_st)
{
  c1_g_emlrt_marshallIn(chartInstance, sf_mex_dup(c1_st));
  chartInstance->c1_doSetSimStateSideEffects = 1U;
  c1_update_debugger_state_c1_Bruss_Input(chartInstance);
  sf_mex_destroy(&c1_st);
}

static void c1_set_sim_state_side_effects_c1_Bruss_Input
  (SFc1_Bruss_InputInstanceStruct *chartInstance)
{
  if (chartInstance->c1_doSetSimStateSideEffects != 0) {
    chartInstance->c1_tp_Brussellator = (uint8_T)
      (chartInstance->c1_is_c1_Bruss_Input == c1_IN_Brussellator);
    chartInstance->c1_doSetSimStateSideEffects = 0U;
  }
}

static void finalize_c1_Bruss_Input(SFc1_Bruss_InputInstanceStruct
  *chartInstance)
{
  sf_mex_destroy(&chartInstance->c1_setSimStateSideEffectsInfo);
}

static void sf_gateway_c1_Bruss_Input(SFc1_Bruss_InputInstanceStruct
  *chartInstance)
{
  real_T c1_d0;
  real_T c1_d1;
  c1_set_sim_state_side_effects_c1_Bruss_Input(chartInstance);
  _SFD_SYMBOL_SCOPE_PUSH(0U, 0U);
  _sfTime_ = sf_get_time(chartInstance->S);
  if (ssIsMajorTimeStep(chartInstance->S) != 0) {
    chartInstance->c1_lastMajorTime = _sfTime_;
    chartInstance->c1_stateChanged = (boolean_T)0;
    _SFD_CC_CALL(CHART_ENTER_SFUNCTION_TAG, 0U, chartInstance->c1_sfEvent);
    _SFD_DATA_RANGE_CHECK(*chartInstance->c1_y_in, 3U, 1U, 0U,
                          chartInstance->c1_sfEvent, false);
    _SFD_DATA_RANGE_CHECK(*chartInstance->c1_x_in, 2U, 1U, 0U,
                          chartInstance->c1_sfEvent, false);
    chartInstance->c1_sfEvent = CALL_EVENT;
    _SFD_CC_CALL(CHART_ENTER_DURING_FUNCTION_TAG, 0U, chartInstance->c1_sfEvent);
    if (chartInstance->c1_is_active_c1_Bruss_Input == 0U) {
      _SFD_CC_CALL(CHART_ENTER_ENTRY_FUNCTION_TAG, 0U, chartInstance->c1_sfEvent);
      chartInstance->c1_stateChanged = true;
      chartInstance->c1_is_active_c1_Bruss_Input = 1U;
      _SFD_CC_CALL(EXIT_OUT_OF_FUNCTION_TAG, 0U, chartInstance->c1_sfEvent);
      _SFD_CT_CALL(TRANSITION_ACTIVE_TAG, 0U, chartInstance->c1_sfEvent);
      *chartInstance->c1_x = *chartInstance->c1_x_in;
      _SFD_DATA_RANGE_CHECK(*chartInstance->c1_x, 0U, 5U, 0U,
                            chartInstance->c1_sfEvent, false);
      c1_d0 = *chartInstance->c1_x;
      sf_mex_printf("%s =\\n", "x");
      sf_mex_call_debug(sfGlobalDebugInstanceStruct, "disp", 0U, 1U, 6, c1_d0);
      *chartInstance->c1_y = *chartInstance->c1_y_in;
      _SFD_DATA_RANGE_CHECK(*chartInstance->c1_y, 1U, 5U, 0U,
                            chartInstance->c1_sfEvent, false);
      c1_d1 = *chartInstance->c1_y;
      sf_mex_printf("%s =\\n", "y");
      sf_mex_call_debug(sfGlobalDebugInstanceStruct, "disp", 0U, 1U, 6, c1_d1);
      chartInstance->c1_stateChanged = true;
      chartInstance->c1_is_c1_Bruss_Input = c1_IN_Brussellator;
      _SFD_CS_CALL(STATE_ACTIVE_TAG, 0U, chartInstance->c1_sfEvent);
      chartInstance->c1_tp_Brussellator = 1U;
    } else {
      _SFD_CS_CALL(STATE_ENTER_DURING_FUNCTION_TAG, 0U,
                   chartInstance->c1_sfEvent);
      _SFD_CS_CALL(EXIT_OUT_OF_FUNCTION_TAG, 0U, chartInstance->c1_sfEvent);
    }

    _SFD_CC_CALL(EXIT_OUT_OF_FUNCTION_TAG, 0U, chartInstance->c1_sfEvent);
    if (chartInstance->c1_stateChanged) {
      ssSetSolverNeedsReset(chartInstance->S);
    }
  }

  _sfTime_ = sf_get_time(chartInstance->S);
  _SFD_CS_CALL(STATE_ENTER_DURING_FUNCTION_TAG, 0U, chartInstance->c1_sfEvent);
  *chartInstance->c1_x_out = *chartInstance->c1_x;
  _SFD_DATA_RANGE_CHECK(*chartInstance->c1_x_out, 4U, 4U, 0U,
                        chartInstance->c1_sfEvent, false);
  *chartInstance->c1_y_out = *chartInstance->c1_y;
  _SFD_DATA_RANGE_CHECK(*chartInstance->c1_y_out, 5U, 4U, 0U,
                        chartInstance->c1_sfEvent, false);
  _SFD_CS_CALL(EXIT_OUT_OF_FUNCTION_TAG, 0U, chartInstance->c1_sfEvent);
  _SFD_SYMBOL_SCOPE_POP();
  _SFD_CHECK_FOR_STATE_INCONSISTENCY(_Bruss_InputMachineNumber_,
    chartInstance->chartNumber, chartInstance->instanceNumber);
}

static void mdl_start_c1_Bruss_Input(SFc1_Bruss_InputInstanceStruct
  *chartInstance)
{
  (void)chartInstance;
}

static void zeroCrossings_c1_Bruss_Input(SFc1_Bruss_InputInstanceStruct
  *chartInstance)
{
  real_T *c1_zcVar;
  c1_zcVar = (real_T *)(ssGetNonsampledZCs_wrapper(chartInstance->S) + 0);
  _sfTime_ = sf_get_time(chartInstance->S);
  if (chartInstance->c1_lastMajorTime == _sfTime_) {
    *c1_zcVar = -1.0;
  } else {
    chartInstance->c1_stateChanged = (boolean_T)0;
    if (chartInstance->c1_is_active_c1_Bruss_Input == 0U) {
      chartInstance->c1_stateChanged = true;
    }

    if (chartInstance->c1_stateChanged) {
      *c1_zcVar = 1.0;
    } else {
      *c1_zcVar = -1.0;
    }
  }
}

static void derivatives_c1_Bruss_Input(SFc1_Bruss_InputInstanceStruct
  *chartInstance)
{
  real_T *c1_x_dot;
  real_T *c1_y_dot;
  c1_y_dot = (real_T *)(ssGetdX_wrapper(chartInstance->S) + 1);
  c1_x_dot = (real_T *)(ssGetdX_wrapper(chartInstance->S) + 0);
  *c1_x_dot = 0.0;
  _SFD_DATA_RANGE_CHECK(*c1_x_dot, 0U, 1U, 0U, chartInstance->c1_sfEvent, false);
  *c1_y_dot = 0.0;
  _SFD_DATA_RANGE_CHECK(*c1_y_dot, 1U, 1U, 0U, chartInstance->c1_sfEvent, false);
  _sfTime_ = sf_get_time(chartInstance->S);
  _SFD_CS_CALL(STATE_ENTER_DURING_FUNCTION_TAG, 0U, chartInstance->c1_sfEvent);
  *c1_x_dot = (1.0 + *chartInstance->c1_x * *chartInstance->c1_x *
               *chartInstance->c1_y) - 2.5 * *chartInstance->c1_x;
  _SFD_DATA_RANGE_CHECK(*c1_x_dot, 0U, 4U, 0U, chartInstance->c1_sfEvent, false);
  *c1_y_dot = 1.5 * *chartInstance->c1_x - *chartInstance->c1_x *
    *chartInstance->c1_x * *chartInstance->c1_y;
  _SFD_DATA_RANGE_CHECK(*c1_y_dot, 1U, 4U, 0U, chartInstance->c1_sfEvent, false);
  _SFD_DATA_RANGE_CHECK(*chartInstance->c1_x_out, 4U, 4U, 0U,
                        chartInstance->c1_sfEvent, false);
  _SFD_DATA_RANGE_CHECK(*chartInstance->c1_y_out, 5U, 4U, 0U,
                        chartInstance->c1_sfEvent, false);
  _SFD_CS_CALL(EXIT_OUT_OF_FUNCTION_TAG, 0U, chartInstance->c1_sfEvent);
}

static void outputs_c1_Bruss_Input(SFc1_Bruss_InputInstanceStruct *chartInstance)
{
  _sfTime_ = sf_get_time(chartInstance->S);
  _SFD_CS_CALL(STATE_ENTER_DURING_FUNCTION_TAG, 0U, chartInstance->c1_sfEvent);
  *chartInstance->c1_x_out = *chartInstance->c1_x;
  _SFD_DATA_RANGE_CHECK(*chartInstance->c1_x_out, 4U, 4U, 0U,
                        chartInstance->c1_sfEvent, false);
  *chartInstance->c1_y_out = *chartInstance->c1_y;
  _SFD_DATA_RANGE_CHECK(*chartInstance->c1_y_out, 5U, 4U, 0U,
                        chartInstance->c1_sfEvent, false);
  _SFD_CS_CALL(EXIT_OUT_OF_FUNCTION_TAG, 0U, chartInstance->c1_sfEvent);
}

static void initSimStructsc1_Bruss_Input(SFc1_Bruss_InputInstanceStruct
  *chartInstance)
{
  (void)chartInstance;
}

static void c1_eml_ini_fcn_to_be_inlined_37(SFc1_Bruss_InputInstanceStruct
  *chartInstance)
{
  (void)chartInstance;
}

static void c1_eml_term_fcn_to_be_inlined_37(SFc1_Bruss_InputInstanceStruct
  *chartInstance)
{
  (void)chartInstance;
}

static void init_script_number_translation(uint32_T c1_machineNumber, uint32_T
  c1_chartNumber, uint32_T c1_instanceNumber)
{
  (void)c1_machineNumber;
  (void)c1_chartNumber;
  (void)c1_instanceNumber;
}

const mxArray *sf_c1_Bruss_Input_get_eml_resolved_functions_info(void)
{
  const mxArray *c1_nameCaptureInfo = NULL;
  c1_nameCaptureInfo = NULL;
  sf_mex_assign(&c1_nameCaptureInfo, sf_mex_create("nameCaptureInfo", NULL, 0,
    0U, 1U, 0U, 2, 0, 1), false);
  return c1_nameCaptureInfo;
}

static const mxArray *c1_emlrt_marshallOut(SFc1_Bruss_InputInstanceStruct
  *chartInstance, const int32_T c1_u)
{
  const mxArray *c1_b_y = NULL;
  (void)chartInstance;
  c1_b_y = NULL;
  sf_mex_assign(&c1_b_y, sf_mex_create("y", &c1_u, 6, 0U, 0U, 0U, 0), false);
  return c1_b_y;
}

static const mxArray *c1_sf_marshallOut(void *chartInstanceVoid, void *c1_inData)
{
  const mxArray *c1_mxArrayOutData = NULL;
  SFc1_Bruss_InputInstanceStruct *chartInstance;
  chartInstance = (SFc1_Bruss_InputInstanceStruct *)chartInstanceVoid;
  c1_mxArrayOutData = NULL;
  sf_mex_assign(&c1_mxArrayOutData, c1_emlrt_marshallOut(chartInstance,
    *(int32_T *)c1_inData), false);
  return c1_mxArrayOutData;
}

static int32_T c1_emlrt_marshallIn(SFc1_Bruss_InputInstanceStruct *chartInstance,
  const mxArray *c1_b_sfEvent, const char_T *c1_identifier)
{
  int32_T c1_b_y;
  emlrtMsgIdentifier c1_thisId;
  c1_thisId.fIdentifier = c1_identifier;
  c1_thisId.fParent = NULL;
  c1_thisId.bParentIsCell = false;
  c1_b_y = c1_b_emlrt_marshallIn(chartInstance, sf_mex_dup(c1_b_sfEvent),
    &c1_thisId);
  sf_mex_destroy(&c1_b_sfEvent);
  return c1_b_y;
}

static int32_T c1_b_emlrt_marshallIn(SFc1_Bruss_InputInstanceStruct
  *chartInstance, const mxArray *c1_u, const emlrtMsgIdentifier *c1_parentId)
{
  int32_T c1_b_y;
  int32_T c1_i0;
  (void)chartInstance;
  sf_mex_import(c1_parentId, sf_mex_dup(c1_u), &c1_i0, 1, 6, 0U, 0, 0U, 0);
  c1_b_y = c1_i0;
  sf_mex_destroy(&c1_u);
  return c1_b_y;
}

static void c1_sf_marshallIn(void *chartInstanceVoid, const mxArray
  *c1_mxArrayInData, const char_T *c1_varName, void *c1_outData)
{
  SFc1_Bruss_InputInstanceStruct *chartInstance;
  chartInstance = (SFc1_Bruss_InputInstanceStruct *)chartInstanceVoid;
  *(int32_T *)c1_outData = c1_emlrt_marshallIn(chartInstance, sf_mex_dup
    (c1_mxArrayInData), c1_varName);
  sf_mex_destroy(&c1_mxArrayInData);
}

static const mxArray *c1_b_emlrt_marshallOut(SFc1_Bruss_InputInstanceStruct
  *chartInstance, const uint8_T c1_u)
{
  const mxArray *c1_b_y = NULL;
  (void)chartInstance;
  c1_b_y = NULL;
  sf_mex_assign(&c1_b_y, sf_mex_create("y", &c1_u, 3, 0U, 0U, 0U, 0), false);
  return c1_b_y;
}

static const mxArray *c1_b_sf_marshallOut(void *chartInstanceVoid, void
  *c1_inData)
{
  const mxArray *c1_mxArrayOutData = NULL;
  SFc1_Bruss_InputInstanceStruct *chartInstance;
  chartInstance = (SFc1_Bruss_InputInstanceStruct *)chartInstanceVoid;
  c1_mxArrayOutData = NULL;
  sf_mex_assign(&c1_mxArrayOutData, c1_b_emlrt_marshallOut(chartInstance,
    *(uint8_T *)c1_inData), false);
  return c1_mxArrayOutData;
}

static uint8_T c1_c_emlrt_marshallIn(SFc1_Bruss_InputInstanceStruct
  *chartInstance, const mxArray *c1_b_tp_Brussellator, const char_T
  *c1_identifier)
{
  uint8_T c1_b_y;
  emlrtMsgIdentifier c1_thisId;
  c1_thisId.fIdentifier = c1_identifier;
  c1_thisId.fParent = NULL;
  c1_thisId.bParentIsCell = false;
  c1_b_y = c1_d_emlrt_marshallIn(chartInstance, sf_mex_dup(c1_b_tp_Brussellator),
    &c1_thisId);
  sf_mex_destroy(&c1_b_tp_Brussellator);
  return c1_b_y;
}

static uint8_T c1_d_emlrt_marshallIn(SFc1_Bruss_InputInstanceStruct
  *chartInstance, const mxArray *c1_u, const emlrtMsgIdentifier *c1_parentId)
{
  uint8_T c1_b_y;
  uint8_T c1_u0;
  (void)chartInstance;
  sf_mex_import(c1_parentId, sf_mex_dup(c1_u), &c1_u0, 1, 3, 0U, 0, 0U, 0);
  c1_b_y = c1_u0;
  sf_mex_destroy(&c1_u);
  return c1_b_y;
}

static void c1_b_sf_marshallIn(void *chartInstanceVoid, const mxArray
  *c1_mxArrayInData, const char_T *c1_varName, void *c1_outData)
{
  SFc1_Bruss_InputInstanceStruct *chartInstance;
  chartInstance = (SFc1_Bruss_InputInstanceStruct *)chartInstanceVoid;
  *(uint8_T *)c1_outData = c1_c_emlrt_marshallIn(chartInstance, sf_mex_dup
    (c1_mxArrayInData), c1_varName);
  sf_mex_destroy(&c1_mxArrayInData);
}

static const mxArray *c1_c_emlrt_marshallOut(SFc1_Bruss_InputInstanceStruct
  *chartInstance, const real_T c1_u)
{
  const mxArray *c1_b_y = NULL;
  (void)chartInstance;
  c1_b_y = NULL;
  sf_mex_assign(&c1_b_y, sf_mex_create("y", &c1_u, 0, 0U, 0U, 0U, 0), false);
  return c1_b_y;
}

static const mxArray *c1_c_sf_marshallOut(void *chartInstanceVoid, void
  *c1_inData)
{
  const mxArray *c1_mxArrayOutData = NULL;
  SFc1_Bruss_InputInstanceStruct *chartInstance;
  chartInstance = (SFc1_Bruss_InputInstanceStruct *)chartInstanceVoid;
  c1_mxArrayOutData = NULL;
  sf_mex_assign(&c1_mxArrayOutData, c1_c_emlrt_marshallOut(chartInstance,
    *(real_T *)c1_inData), false);
  return c1_mxArrayOutData;
}

static real_T c1_e_emlrt_marshallIn(SFc1_Bruss_InputInstanceStruct
  *chartInstance, const mxArray *c1_b_x, const char_T *c1_identifier)
{
  real_T c1_b_y;
  emlrtMsgIdentifier c1_thisId;
  c1_thisId.fIdentifier = c1_identifier;
  c1_thisId.fParent = NULL;
  c1_thisId.bParentIsCell = false;
  c1_b_y = c1_f_emlrt_marshallIn(chartInstance, sf_mex_dup(c1_b_x), &c1_thisId);
  sf_mex_destroy(&c1_b_x);
  return c1_b_y;
}

static real_T c1_f_emlrt_marshallIn(SFc1_Bruss_InputInstanceStruct
  *chartInstance, const mxArray *c1_u, const emlrtMsgIdentifier *c1_parentId)
{
  real_T c1_b_y;
  real_T c1_d2;
  (void)chartInstance;
  sf_mex_import(c1_parentId, sf_mex_dup(c1_u), &c1_d2, 1, 0, 0U, 0, 0U, 0);
  c1_b_y = c1_d2;
  sf_mex_destroy(&c1_u);
  return c1_b_y;
}

static void c1_c_sf_marshallIn(void *chartInstanceVoid, const mxArray
  *c1_mxArrayInData, const char_T *c1_varName, void *c1_outData)
{
  SFc1_Bruss_InputInstanceStruct *chartInstance;
  chartInstance = (SFc1_Bruss_InputInstanceStruct *)chartInstanceVoid;
  *(real_T *)c1_outData = c1_e_emlrt_marshallIn(chartInstance, sf_mex_dup
    (c1_mxArrayInData), c1_varName);
  sf_mex_destroy(&c1_mxArrayInData);
}

static const mxArray *c1_d_emlrt_marshallOut(SFc1_Bruss_InputInstanceStruct
  *chartInstance)
{
  const mxArray *c1_b_y;
  c1_b_y = NULL;
  c1_b_y = NULL;
  sf_mex_assign(&c1_b_y, sf_mex_createcellmatrix(6, 1), false);
  sf_mex_setcell(c1_b_y, 0, c1_c_emlrt_marshallOut(chartInstance,
    *chartInstance->c1_x_out));
  sf_mex_setcell(c1_b_y, 1, c1_c_emlrt_marshallOut(chartInstance,
    *chartInstance->c1_y_out));
  sf_mex_setcell(c1_b_y, 2, c1_c_emlrt_marshallOut(chartInstance,
    *chartInstance->c1_x));
  sf_mex_setcell(c1_b_y, 3, c1_c_emlrt_marshallOut(chartInstance,
    *chartInstance->c1_y));
  sf_mex_setcell(c1_b_y, 4, c1_b_emlrt_marshallOut(chartInstance,
    chartInstance->c1_is_active_c1_Bruss_Input));
  sf_mex_setcell(c1_b_y, 5, c1_b_emlrt_marshallOut(chartInstance,
    chartInstance->c1_is_c1_Bruss_Input));
  return c1_b_y;
}

static void c1_g_emlrt_marshallIn(SFc1_Bruss_InputInstanceStruct *chartInstance,
  const mxArray *c1_u)
{
  *chartInstance->c1_x_out = c1_e_emlrt_marshallIn(chartInstance, sf_mex_dup
    (sf_mex_getcell("x_out", c1_u, 0)), "x_out");
  *chartInstance->c1_y_out = c1_e_emlrt_marshallIn(chartInstance, sf_mex_dup
    (sf_mex_getcell("y_out", c1_u, 1)), "y_out");
  *chartInstance->c1_x = c1_e_emlrt_marshallIn(chartInstance, sf_mex_dup
    (sf_mex_getcell("x", c1_u, 2)), "x");
  *chartInstance->c1_y = c1_e_emlrt_marshallIn(chartInstance, sf_mex_dup
    (sf_mex_getcell("y", c1_u, 3)), "y");
  chartInstance->c1_is_active_c1_Bruss_Input = c1_c_emlrt_marshallIn
    (chartInstance, sf_mex_dup(sf_mex_getcell("is_active_c1_Bruss_Input", c1_u,
       4)), "is_active_c1_Bruss_Input");
  chartInstance->c1_is_c1_Bruss_Input = c1_c_emlrt_marshallIn(chartInstance,
    sf_mex_dup(sf_mex_getcell("is_c1_Bruss_Input", c1_u, 5)),
    "is_c1_Bruss_Input");
  sf_mex_assign(&chartInstance->c1_setSimStateSideEffectsInfo,
                c1_h_emlrt_marshallIn(chartInstance, sf_mex_dup(sf_mex_getcell(
    "setSimStateSideEffectsInfo", c1_u, 6)), "setSimStateSideEffectsInfo"), true);
  sf_mex_destroy(&c1_u);
}

static const mxArray *c1_h_emlrt_marshallIn(SFc1_Bruss_InputInstanceStruct
  *chartInstance, const mxArray *c1_b_setSimStateSideEffectsInfo, const char_T
  *c1_identifier)
{
  const mxArray *c1_b_y = NULL;
  emlrtMsgIdentifier c1_thisId;
  c1_b_y = NULL;
  c1_thisId.fIdentifier = c1_identifier;
  c1_thisId.fParent = NULL;
  c1_thisId.bParentIsCell = false;
  sf_mex_assign(&c1_b_y, c1_i_emlrt_marshallIn(chartInstance, sf_mex_dup
    (c1_b_setSimStateSideEffectsInfo), &c1_thisId), false);
  sf_mex_destroy(&c1_b_setSimStateSideEffectsInfo);
  return c1_b_y;
}

static const mxArray *c1_i_emlrt_marshallIn(SFc1_Bruss_InputInstanceStruct
  *chartInstance, const mxArray *c1_u, const emlrtMsgIdentifier *c1_parentId)
{
  const mxArray *c1_b_y = NULL;
  (void)chartInstance;
  (void)c1_parentId;
  c1_b_y = NULL;
  sf_mex_assign(&c1_b_y, sf_mex_duplicatearraysafe(&c1_u), false);
  sf_mex_destroy(&c1_u);
  return c1_b_y;
}

static const mxArray *sf_get_hover_data_for_msg(SFc1_Bruss_InputInstanceStruct
  *chartInstance, int32_T c1_ssid)
{
  (void)chartInstance;
  (void)c1_ssid;
  return NULL;
}

static void c1_init_sf_message_store_memory(SFc1_Bruss_InputInstanceStruct
  *chartInstance)
{
  (void)chartInstance;
}

static void init_dsm_address_info(SFc1_Bruss_InputInstanceStruct *chartInstance)
{
  (void)chartInstance;
}

static void init_simulink_io_address(SFc1_Bruss_InputInstanceStruct
  *chartInstance)
{
  chartInstance->c1_x = (real_T *)(ssGetContStates_wrapper(chartInstance->S) + 0);
  chartInstance->c1_y = (real_T *)(ssGetContStates_wrapper(chartInstance->S) + 1);
  chartInstance->c1_x_out = (real_T *)ssGetOutputPortSignal_wrapper
    (chartInstance->S, 1);
  chartInstance->c1_y_out = (real_T *)ssGetOutputPortSignal_wrapper
    (chartInstance->S, 2);
  chartInstance->c1_x_in = (real_T *)ssGetInputPortSignal_wrapper
    (chartInstance->S, 0);
  chartInstance->c1_y_in = (real_T *)ssGetInputPortSignal_wrapper
    (chartInstance->S, 1);
}

/* SFunction Glue Code */
#ifdef utFree
#undef utFree
#endif

#ifdef utMalloc
#undef utMalloc
#endif

#ifdef __cplusplus

extern "C" void *utMalloc(size_t size);
extern "C" void utFree(void*);

#else

extern void *utMalloc(size_t size);
extern void utFree(void*);

#endif

void sf_c1_Bruss_Input_get_check_sum(mxArray *plhs[])
{
  ((real_T *)mxGetPr((plhs[0])))[0] = (real_T)(529279431U);
  ((real_T *)mxGetPr((plhs[0])))[1] = (real_T)(3040767890U);
  ((real_T *)mxGetPr((plhs[0])))[2] = (real_T)(2395880863U);
  ((real_T *)mxGetPr((plhs[0])))[3] = (real_T)(1536461713U);
}

mxArray* sf_c1_Bruss_Input_get_post_codegen_info(void);
mxArray *sf_c1_Bruss_Input_get_autoinheritance_info(void)
{
  const char *autoinheritanceFields[] = { "checksum", "inputs", "parameters",
    "outputs", "locals", "postCodegenInfo" };

  mxArray *mxAutoinheritanceInfo = mxCreateStructMatrix(1, 1, sizeof
    (autoinheritanceFields)/sizeof(autoinheritanceFields[0]),
    autoinheritanceFields);

  {
    mxArray *mxChecksum = mxCreateString("RAjpj0v02GhCtNoUE7VI0D");
    mxSetField(mxAutoinheritanceInfo,0,"checksum",mxChecksum);
  }

  {
    const char *dataFields[] = { "size", "type", "complexity" };

    mxArray *mxData = mxCreateStructMatrix(1,2,3,dataFields);

    {
      mxArray *mxSize = mxCreateDoubleMatrix(1,0,mxREAL);
      double *pr = mxGetPr(mxSize);
      mxSetField(mxData,0,"size",mxSize);
    }

    {
      const char *typeFields[] = { "base", "fixpt", "isFixedPointType" };

      mxArray *mxType = mxCreateStructMatrix(1,1,sizeof(typeFields)/sizeof
        (typeFields[0]),typeFields);
      mxSetField(mxType,0,"base",mxCreateDoubleScalar(10));
      mxSetField(mxType,0,"fixpt",mxCreateDoubleMatrix(0,0,mxREAL));
      mxSetField(mxType,0,"isFixedPointType",mxCreateDoubleScalar(0));
      mxSetField(mxData,0,"type",mxType);
    }

    mxSetField(mxData,0,"complexity",mxCreateDoubleScalar(0));

    {
      mxArray *mxSize = mxCreateDoubleMatrix(1,0,mxREAL);
      double *pr = mxGetPr(mxSize);
      mxSetField(mxData,1,"size",mxSize);
    }

    {
      const char *typeFields[] = { "base", "fixpt", "isFixedPointType" };

      mxArray *mxType = mxCreateStructMatrix(1,1,sizeof(typeFields)/sizeof
        (typeFields[0]),typeFields);
      mxSetField(mxType,0,"base",mxCreateDoubleScalar(10));
      mxSetField(mxType,0,"fixpt",mxCreateDoubleMatrix(0,0,mxREAL));
      mxSetField(mxType,0,"isFixedPointType",mxCreateDoubleScalar(0));
      mxSetField(mxData,1,"type",mxType);
    }

    mxSetField(mxData,1,"complexity",mxCreateDoubleScalar(0));
    mxSetField(mxAutoinheritanceInfo,0,"inputs",mxData);
  }

  {
    mxSetField(mxAutoinheritanceInfo,0,"parameters",mxCreateDoubleMatrix(0,0,
                mxREAL));
  }

  {
    const char *dataFields[] = { "size", "type", "complexity" };

    mxArray *mxData = mxCreateStructMatrix(1,2,3,dataFields);

    {
      mxArray *mxSize = mxCreateDoubleMatrix(1,0,mxREAL);
      double *pr = mxGetPr(mxSize);
      mxSetField(mxData,0,"size",mxSize);
    }

    {
      const char *typeFields[] = { "base", "fixpt", "isFixedPointType" };

      mxArray *mxType = mxCreateStructMatrix(1,1,sizeof(typeFields)/sizeof
        (typeFields[0]),typeFields);
      mxSetField(mxType,0,"base",mxCreateDoubleScalar(10));
      mxSetField(mxType,0,"fixpt",mxCreateDoubleMatrix(0,0,mxREAL));
      mxSetField(mxType,0,"isFixedPointType",mxCreateDoubleScalar(0));
      mxSetField(mxData,0,"type",mxType);
    }

    mxSetField(mxData,0,"complexity",mxCreateDoubleScalar(0));

    {
      mxArray *mxSize = mxCreateDoubleMatrix(1,0,mxREAL);
      double *pr = mxGetPr(mxSize);
      mxSetField(mxData,1,"size",mxSize);
    }

    {
      const char *typeFields[] = { "base", "fixpt", "isFixedPointType" };

      mxArray *mxType = mxCreateStructMatrix(1,1,sizeof(typeFields)/sizeof
        (typeFields[0]),typeFields);
      mxSetField(mxType,0,"base",mxCreateDoubleScalar(10));
      mxSetField(mxType,0,"fixpt",mxCreateDoubleMatrix(0,0,mxREAL));
      mxSetField(mxType,0,"isFixedPointType",mxCreateDoubleScalar(0));
      mxSetField(mxData,1,"type",mxType);
    }

    mxSetField(mxData,1,"complexity",mxCreateDoubleScalar(0));
    mxSetField(mxAutoinheritanceInfo,0,"outputs",mxData);
  }

  {
    mxSetField(mxAutoinheritanceInfo,0,"locals",mxCreateDoubleMatrix(0,0,mxREAL));
  }

  {
    mxArray* mxPostCodegenInfo = sf_c1_Bruss_Input_get_post_codegen_info();
    mxSetField(mxAutoinheritanceInfo,0,"postCodegenInfo",mxPostCodegenInfo);
  }

  return(mxAutoinheritanceInfo);
}

mxArray *sf_c1_Bruss_Input_third_party_uses_info(void)
{
  mxArray * mxcell3p = mxCreateCellMatrix(1,0);
  return(mxcell3p);
}

mxArray *sf_c1_Bruss_Input_jit_fallback_info(void)
{
  const char *infoFields[] = { "fallbackType", "fallbackReason",
    "hiddenFallbackType", "hiddenFallbackReason", "incompatibleSymbol" };

  mxArray *mxInfo = mxCreateStructMatrix(1, 1, 5, infoFields);
  mxArray *fallbackType = mxCreateString("early");
  mxArray *fallbackReason = mxCreateString("plant_model_chart");
  mxArray *hiddenFallbackType = mxCreateString("");
  mxArray *hiddenFallbackReason = mxCreateString("");
  mxArray *incompatibleSymbol = mxCreateString("");
  mxSetField(mxInfo, 0, infoFields[0], fallbackType);
  mxSetField(mxInfo, 0, infoFields[1], fallbackReason);
  mxSetField(mxInfo, 0, infoFields[2], hiddenFallbackType);
  mxSetField(mxInfo, 0, infoFields[3], hiddenFallbackReason);
  mxSetField(mxInfo, 0, infoFields[4], incompatibleSymbol);
  return mxInfo;
}

mxArray *sf_c1_Bruss_Input_updateBuildInfo_args_info(void)
{
  mxArray *mxBIArgs = mxCreateCellMatrix(1,0);
  return mxBIArgs;
}

mxArray* sf_c1_Bruss_Input_get_post_codegen_info(void)
{
  const char* fieldNames[] = { "exportedFunctionsUsedByThisChart",
    "exportedFunctionsChecksum" };

  mwSize dims[2] = { 1, 1 };

  mxArray* mxPostCodegenInfo = mxCreateStructArray(2, dims, sizeof(fieldNames)/
    sizeof(fieldNames[0]), fieldNames);

  {
    mxArray* mxExportedFunctionsChecksum = mxCreateString("");
    mwSize exp_dims[2] = { 0, 1 };

    mxArray* mxExportedFunctionsUsedByThisChart = mxCreateCellArray(2, exp_dims);
    mxSetField(mxPostCodegenInfo, 0, "exportedFunctionsUsedByThisChart",
               mxExportedFunctionsUsedByThisChart);
    mxSetField(mxPostCodegenInfo, 0, "exportedFunctionsChecksum",
               mxExportedFunctionsChecksum);
  }

  return mxPostCodegenInfo;
}

static const mxArray *sf_get_sim_state_info_c1_Bruss_Input(void)
{
  const char *infoFields[] = { "chartChecksum", "varInfo" };

  mxArray *mxInfo = mxCreateStructMatrix(1, 1, 2, infoFields);
  const char *infoEncStr[] = {
    "100 S1x6'type','srcId','name','auxInfo'{{M[1],M[24],T\"x_out\",},{M[1],M[25],T\"y_out\",},{M[5],M[21],T\"x\",},{M[5],M[22],T\"y\",},{M[8],M[0],T\"is_active_c1_Bruss_Input\",},{M[9],M[0],T\"is_c1_Bruss_Input\",}}"
  };

  mxArray *mxVarInfo = sf_mex_decode_encoded_mx_struct_array(infoEncStr, 6, 10);
  mxArray *mxChecksum = mxCreateDoubleMatrix(1, 4, mxREAL);
  sf_c1_Bruss_Input_get_check_sum(&mxChecksum);
  mxSetField(mxInfo, 0, infoFields[0], mxChecksum);
  mxSetField(mxInfo, 0, infoFields[1], mxVarInfo);
  return mxInfo;
}

static const mxArray* sf_opaque_get_hover_data_for_msg(void* chartInstance,
  int32_T msgSSID)
{
  return sf_get_hover_data_for_msg( (SFc1_Bruss_InputInstanceStruct *)
    chartInstance, msgSSID);
}

static void chart_debug_initialization(SimStruct *S, unsigned int
  fullDebuggerInitialization)
{
  if (!sim_mode_is_rtw_gen(S)) {
    SFc1_Bruss_InputInstanceStruct *chartInstance =
      (SFc1_Bruss_InputInstanceStruct *)sf_get_chart_instance_ptr(S);
    if (ssIsFirstInitCond(S) && fullDebuggerInitialization==1) {
      /* do this only if simulation is starting */
      {
        unsigned int chartAlreadyPresent;
        chartAlreadyPresent = sf_debug_initialize_chart
          (sfGlobalDebugInstanceStruct,
           _Bruss_InputMachineNumber_,
           1,
           1,
           1,
           0,
           6,
           0,
           0,
           0,
           0,
           0,
           &(chartInstance->chartNumber),
           &(chartInstance->instanceNumber),
           (void *)S);

        /* Each instance must initialize its own list of scripts */
        init_script_number_translation(_Bruss_InputMachineNumber_,
          chartInstance->chartNumber,chartInstance->instanceNumber);
        if (chartAlreadyPresent==0) {
          /* this is the first instance */
          sf_debug_set_chart_disable_implicit_casting
            (sfGlobalDebugInstanceStruct,_Bruss_InputMachineNumber_,
             chartInstance->chartNumber,1);
          sf_debug_set_chart_event_thresholds(sfGlobalDebugInstanceStruct,
            _Bruss_InputMachineNumber_,
            chartInstance->chartNumber,
            0,
            0,
            0);
          _SFD_SET_DATA_PROPS(0,0,0,0,"x");
          _SFD_SET_DATA_PROPS(1,0,0,0,"y");
          _SFD_SET_DATA_PROPS(2,1,1,0,"x_in");
          _SFD_SET_DATA_PROPS(3,1,1,0,"y_in");
          _SFD_SET_DATA_PROPS(4,2,0,1,"x_out");
          _SFD_SET_DATA_PROPS(5,2,0,1,"y_out");
          _SFD_STATE_INFO(0,0,0);
          _SFD_CH_SUBSTATE_COUNT(1);
          _SFD_CH_SUBSTATE_DECOMP(0);
          _SFD_CH_SUBSTATE_INDEX(0,0);
          _SFD_ST_SUBSTATE_COUNT(0,0);
        }

        _SFD_CV_INIT_CHART(1,0,0,0);

        {
          _SFD_CV_INIT_STATE(0,0,0,0,0,0,NULL,NULL);
        }

        _SFD_CV_INIT_TRANS(0,0,NULL,NULL,0,NULL);
        _SFD_SET_DATA_COMPILED_PROPS(0,SF_DOUBLE,0,NULL,0,0,0,0.0,1.0,0,0,
          (MexFcnForType)c1_c_sf_marshallOut,(MexInFcnForType)c1_c_sf_marshallIn);
        _SFD_SET_DATA_COMPILED_PROPS(1,SF_DOUBLE,0,NULL,0,0,0,0.0,1.0,0,0,
          (MexFcnForType)c1_c_sf_marshallOut,(MexInFcnForType)c1_c_sf_marshallIn);
        _SFD_SET_DATA_COMPILED_PROPS(2,SF_DOUBLE,0,NULL,0,0,0,0.0,1.0,0,0,
          (MexFcnForType)c1_c_sf_marshallOut,(MexInFcnForType)NULL);
        _SFD_SET_DATA_COMPILED_PROPS(3,SF_DOUBLE,0,NULL,0,0,0,0.0,1.0,0,0,
          (MexFcnForType)c1_c_sf_marshallOut,(MexInFcnForType)NULL);
        _SFD_SET_DATA_COMPILED_PROPS(4,SF_DOUBLE,0,NULL,0,0,0,0.0,1.0,0,0,
          (MexFcnForType)c1_c_sf_marshallOut,(MexInFcnForType)c1_c_sf_marshallIn);
        _SFD_SET_DATA_COMPILED_PROPS(5,SF_DOUBLE,0,NULL,0,0,0,0.0,1.0,0,0,
          (MexFcnForType)c1_c_sf_marshallOut,(MexInFcnForType)c1_c_sf_marshallIn);
      }
    } else {
      sf_debug_reset_current_state_configuration(sfGlobalDebugInstanceStruct,
        _Bruss_InputMachineNumber_,chartInstance->chartNumber,
        chartInstance->instanceNumber);
    }
  }
}

static void chart_debug_initialize_data_addresses(SimStruct *S)
{
  if (!sim_mode_is_rtw_gen(S)) {
    SFc1_Bruss_InputInstanceStruct *chartInstance =
      (SFc1_Bruss_InputInstanceStruct *)sf_get_chart_instance_ptr(S);
    if (ssIsFirstInitCond(S)) {
      /* do this only if simulation is starting and after we know the addresses of all data */
      {
        _SFD_SET_DATA_VALUE_PTR(0U, chartInstance->c1_x);
        _SFD_SET_DATA_VALUE_PTR(1U, chartInstance->c1_y);
        _SFD_SET_DATA_VALUE_PTR(4U, chartInstance->c1_x_out);
        _SFD_SET_DATA_VALUE_PTR(5U, chartInstance->c1_y_out);
        _SFD_SET_DATA_VALUE_PTR(2U, chartInstance->c1_x_in);
        _SFD_SET_DATA_VALUE_PTR(3U, chartInstance->c1_y_in);
      }
    }
  }
}

static const char* sf_get_instance_specialization(void)
{
  return "sVT7W4RaSzRwXICFUJurAlE";
}

static void sf_opaque_initialize_c1_Bruss_Input(void *chartInstanceVar)
{
  chart_debug_initialization(((SFc1_Bruss_InputInstanceStruct*) chartInstanceVar)
    ->S,0);
  initialize_params_c1_Bruss_Input((SFc1_Bruss_InputInstanceStruct*)
    chartInstanceVar);
  initialize_c1_Bruss_Input((SFc1_Bruss_InputInstanceStruct*) chartInstanceVar);
}

static void sf_opaque_enable_c1_Bruss_Input(void *chartInstanceVar)
{
  enable_c1_Bruss_Input((SFc1_Bruss_InputInstanceStruct*) chartInstanceVar);
}

static void sf_opaque_disable_c1_Bruss_Input(void *chartInstanceVar)
{
  disable_c1_Bruss_Input((SFc1_Bruss_InputInstanceStruct*) chartInstanceVar);
}

static void sf_opaque_zeroCrossings_c1_Bruss_Input(void *chartInstanceVar)
{
  zeroCrossings_c1_Bruss_Input((SFc1_Bruss_InputInstanceStruct*)
    chartInstanceVar);
}

static void sf_opaque_derivatives_c1_Bruss_Input(void *chartInstanceVar)
{
  derivatives_c1_Bruss_Input((SFc1_Bruss_InputInstanceStruct*) chartInstanceVar);
}

static void sf_opaque_outputs_c1_Bruss_Input(void *chartInstanceVar)
{
  outputs_c1_Bruss_Input((SFc1_Bruss_InputInstanceStruct*) chartInstanceVar);
}

static void sf_opaque_gateway_c1_Bruss_Input(void *chartInstanceVar)
{
  sf_gateway_c1_Bruss_Input((SFc1_Bruss_InputInstanceStruct*) chartInstanceVar);
}

static const mxArray* sf_opaque_get_sim_state_c1_Bruss_Input(SimStruct* S)
{
  return get_sim_state_c1_Bruss_Input((SFc1_Bruss_InputInstanceStruct *)
    sf_get_chart_instance_ptr(S));     /* raw sim ctx */
}

static void sf_opaque_set_sim_state_c1_Bruss_Input(SimStruct* S, const mxArray
  *st)
{
  set_sim_state_c1_Bruss_Input((SFc1_Bruss_InputInstanceStruct*)
    sf_get_chart_instance_ptr(S), st);
}

static void sf_opaque_terminate_c1_Bruss_Input(void *chartInstanceVar)
{
  if (chartInstanceVar!=NULL) {
    SimStruct *S = ((SFc1_Bruss_InputInstanceStruct*) chartInstanceVar)->S;
    if (sim_mode_is_rtw_gen(S) || sim_mode_is_external(S)) {
      sf_clear_rtw_identifier(S);
      unload_Bruss_Input_optimization_info();
    }

    finalize_c1_Bruss_Input((SFc1_Bruss_InputInstanceStruct*) chartInstanceVar);
    utFree(chartInstanceVar);
    if (ssGetUserData(S)!= NULL) {
      sf_free_ChartRunTimeInfo(S);
    }

    ssSetUserData(S,NULL);
  }
}

static void sf_opaque_init_subchart_simstructs(void *chartInstanceVar)
{
  initSimStructsc1_Bruss_Input((SFc1_Bruss_InputInstanceStruct*)
    chartInstanceVar);
}

extern unsigned int sf_machine_global_initializer_called(void);
static void mdlProcessParameters_c1_Bruss_Input(SimStruct *S)
{
  int i;
  for (i=0;i<ssGetNumRunTimeParams(S);i++) {
    if (ssGetSFcnParamTunable(S,i)) {
      ssUpdateDlgParamAsRunTimeParam(S,i);
    }
  }

  if (sf_machine_global_initializer_called()) {
    initialize_params_c1_Bruss_Input((SFc1_Bruss_InputInstanceStruct*)
      sf_get_chart_instance_ptr(S));
  }
}

static void mdlSetWorkWidths_c1_Bruss_Input(SimStruct *S)
{
  ssMdlUpdateIsEmpty(S, 1);
  if (sim_mode_is_rtw_gen(S) || sim_mode_is_external(S)) {
    mxArray *infoStruct = load_Bruss_Input_optimization_info();
    int_T chartIsInlinable =
      (int_T)sf_is_chart_inlinable(sf_get_instance_specialization(),infoStruct,1);
    ssSetStateflowIsInlinable(S,chartIsInlinable);
    ssSetRTWCG(S,1);
    ssSetEnableFcnIsTrivial(S,1);
    ssSetDisableFcnIsTrivial(S,1);
    ssSetNotMultipleInlinable(S,sf_rtw_info_uint_prop
      (sf_get_instance_specialization(),infoStruct,1,
       "gatewayCannotBeInlinedMultipleTimes"));
    sf_set_chart_accesses_machine_info(S, sf_get_instance_specialization(),
      infoStruct, 1);
    sf_update_buildInfo(sf_get_instance_specialization(),infoStruct,1);
    if (chartIsInlinable) {
      ssSetInputPortOptimOpts(S, 0, SS_REUSABLE_AND_LOCAL);
      ssSetInputPortOptimOpts(S, 1, SS_REUSABLE_AND_LOCAL);
      sf_mark_chart_expressionable_inputs(S,sf_get_instance_specialization(),
        infoStruct,1,2);
      sf_mark_chart_reusable_outputs(S,sf_get_instance_specialization(),
        infoStruct,1,2);
    }

    {
      unsigned int outPortIdx;
      for (outPortIdx=1; outPortIdx<=2; ++outPortIdx) {
        ssSetOutputPortOptimizeInIR(S, outPortIdx, 1U);
      }
    }

    {
      unsigned int inPortIdx;
      for (inPortIdx=0; inPortIdx < 2; ++inPortIdx) {
        ssSetInputPortOptimizeInIR(S, inPortIdx, 1U);
      }
    }

    sf_set_rtw_dwork_info(S,sf_get_instance_specialization(),infoStruct,1);
    ssSetHasSubFunctions(S,!(chartIsInlinable));
  } else {
  }

  ssSetOptions(S,ssGetOptions(S)|SS_OPTION_WORKS_WITH_CODE_REUSE);
  ssSetChecksum0(S,(672048359U));
  ssSetChecksum1(S,(742238986U));
  ssSetChecksum2(S,(3275108912U));
  ssSetChecksum3(S,(2236168117U));
  ssSetNumContStates(S,2);
  ssSetExplicitFCSSCtrl(S,1);
  ssSupportsMultipleExecInstances(S,1);
}

static void mdlRTW_c1_Bruss_Input(SimStruct *S)
{
  if (sim_mode_is_rtw_gen(S)) {
    ssWriteRTWStrParam(S, "StateflowChartType", "Stateflow");
  }
}

static void mdlStart_c1_Bruss_Input(SimStruct *S)
{
  SFc1_Bruss_InputInstanceStruct *chartInstance;
  chartInstance = (SFc1_Bruss_InputInstanceStruct *)utMalloc(sizeof
    (SFc1_Bruss_InputInstanceStruct));
  if (chartInstance==NULL) {
    sf_mex_error_message("Could not allocate memory for chart instance.");
  }

  memset(chartInstance, 0, sizeof(SFc1_Bruss_InputInstanceStruct));
  chartInstance->chartInfo.chartInstance = chartInstance;
  chartInstance->chartInfo.isEMLChart = 0;
  chartInstance->chartInfo.chartInitialized = 0;
  chartInstance->chartInfo.sFunctionGateway = sf_opaque_gateway_c1_Bruss_Input;
  chartInstance->chartInfo.initializeChart = sf_opaque_initialize_c1_Bruss_Input;
  chartInstance->chartInfo.terminateChart = sf_opaque_terminate_c1_Bruss_Input;
  chartInstance->chartInfo.enableChart = sf_opaque_enable_c1_Bruss_Input;
  chartInstance->chartInfo.disableChart = sf_opaque_disable_c1_Bruss_Input;
  chartInstance->chartInfo.getSimState = sf_opaque_get_sim_state_c1_Bruss_Input;
  chartInstance->chartInfo.setSimState = sf_opaque_set_sim_state_c1_Bruss_Input;
  chartInstance->chartInfo.getSimStateInfo =
    sf_get_sim_state_info_c1_Bruss_Input;
  chartInstance->chartInfo.zeroCrossings =
    sf_opaque_zeroCrossings_c1_Bruss_Input;
  chartInstance->chartInfo.outputs = sf_opaque_outputs_c1_Bruss_Input;
  chartInstance->chartInfo.derivatives = sf_opaque_derivatives_c1_Bruss_Input;
  chartInstance->chartInfo.mdlRTW = mdlRTW_c1_Bruss_Input;
  chartInstance->chartInfo.mdlStart = mdlStart_c1_Bruss_Input;
  chartInstance->chartInfo.mdlSetWorkWidths = mdlSetWorkWidths_c1_Bruss_Input;
  chartInstance->chartInfo.callGetHoverDataForMsg =
    sf_opaque_get_hover_data_for_msg;
  chartInstance->chartInfo.extModeExec = NULL;
  chartInstance->chartInfo.restoreLastMajorStepConfiguration = NULL;
  chartInstance->chartInfo.restoreBeforeLastMajorStepConfiguration = NULL;
  chartInstance->chartInfo.storeCurrentConfiguration = NULL;
  chartInstance->chartInfo.callAtomicSubchartUserFcn = NULL;
  chartInstance->chartInfo.callAtomicSubchartAutoFcn = NULL;
  chartInstance->chartInfo.debugInstance = sfGlobalDebugInstanceStruct;
  chartInstance->S = S;
  sf_init_ChartRunTimeInfo(S, &(chartInstance->chartInfo), false, 0);
  init_dsm_address_info(chartInstance);
  init_simulink_io_address(chartInstance);
  if (!sim_mode_is_rtw_gen(S)) {
  }

  chart_debug_initialization(S,1);
  mdl_start_c1_Bruss_Input(chartInstance);
}

void c1_Bruss_Input_method_dispatcher(SimStruct *S, int_T method, void *data)
{
  switch (method) {
   case SS_CALL_MDL_START:
    mdlStart_c1_Bruss_Input(S);
    break;

   case SS_CALL_MDL_SET_WORK_WIDTHS:
    mdlSetWorkWidths_c1_Bruss_Input(S);
    break;

   case SS_CALL_MDL_PROCESS_PARAMETERS:
    mdlProcessParameters_c1_Bruss_Input(S);
    break;

   default:
    /* Unhandled method */
    sf_mex_error_message("Stateflow Internal Error:\n"
                         "Error calling c1_Bruss_Input_method_dispatcher.\n"
                         "Can't handle method %d.\n", method);
    break;
  }
}
