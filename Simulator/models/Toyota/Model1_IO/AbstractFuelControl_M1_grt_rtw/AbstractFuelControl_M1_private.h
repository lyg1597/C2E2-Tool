/*
 * AbstractFuelControl_M1_private.h
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

#ifndef RTW_HEADER_AbstractFuelControl_M1_private_h_
#define RTW_HEADER_AbstractFuelControl_M1_private_h_
#include "rtwtypes.h"
#include "builtin_typeid_types.h"
#include "multiword_types.h"
#include "zero_crossing_types.h"

int_T rt_WriteMat4FileHeader(FILE *fp,
  int32_T m,
  int32_T n,
  const char_T *name);
real_T rt_VTDelayfindtDInterpolate(
  real_T x,real_T* tBuf,real_T* uBuf,real_T* xBuf,int_T bufSz,int_T head,int_T
  tail,int_T* pLast,real_T t,real_T tStart,boolean_T discrete,boolean_T
  minorStepAndTAtLastMajorOutput,real_T initOutput,real_T* appliedDelay);
extern real_T look2_binlxpw(real_T u0, real_T u1, const real_T bp0[], const
  real_T bp1[], const real_T table[], const uint32_T maxIndex[], uint32_T stride);

/* private model entry point functions */
extern void AbstractFuelControl_M1_derivatives(void);

#endif                                 /* RTW_HEADER_AbstractFuelControl_M1_private_h_ */
