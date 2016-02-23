/*
 * AbstractFuelControl_M1.h
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

#ifndef RTW_HEADER_AbstractFuelControl_M1_h_
#define RTW_HEADER_AbstractFuelControl_M1_h_
#include <stddef.h>
#include <math.h>
#include <string.h>
#ifndef AbstractFuelControl_M1_COMMON_INCLUDES_
# define AbstractFuelControl_M1_COMMON_INCLUDES_
#include <stdio.h>
#include <string.h>
#include "rtwtypes.h"
#include "zero_crossing_types.h"
#include "simstruc.h"
#include "fixedpoint.h"
#include "rt_logging.h"
#endif                                 /* AbstractFuelControl_M1_COMMON_INCLUDES_ */

#include "AbstractFuelControl_M1_types.h"

/* Shared type includes */
#include "multiword_types.h"
#include "rt_zcfcn.h"
#include "rt_nonfinite.h"
#include "rtGetInf.h"
#include "rt_defines.h"

/* Macros for accessing real-time model data structure */
#ifndef rtmGetBlkStateChangeFlag
# define rtmGetBlkStateChangeFlag(rtm) ((rtm)->ModelData.blkStateChange)
#endif

#ifndef rtmSetBlkStateChangeFlag
# define rtmSetBlkStateChangeFlag(rtm, val) ((rtm)->ModelData.blkStateChange = (val))
#endif

#ifndef rtmGetBlockIO
# define rtmGetBlockIO(rtm)            ((rtm)->ModelData.blockIO)
#endif

#ifndef rtmSetBlockIO
# define rtmSetBlockIO(rtm, val)       ((rtm)->ModelData.blockIO = (val))
#endif

#ifndef rtmGetChecksums
# define rtmGetChecksums(rtm)          ((rtm)->Sizes.checksums)
#endif

#ifndef rtmSetChecksums
# define rtmSetChecksums(rtm, val)     ((rtm)->Sizes.checksums = (val))
#endif

#ifndef rtmGetConstBlockIO
# define rtmGetConstBlockIO(rtm)       ((rtm)->ModelData.constBlockIO)
#endif

#ifndef rtmSetConstBlockIO
# define rtmSetConstBlockIO(rtm, val)  ((rtm)->ModelData.constBlockIO = (val))
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

#ifndef rtmGetDataMapInfo
# define rtmGetDataMapInfo(rtm)        ()
#endif

#ifndef rtmSetDataMapInfo
# define rtmSetDataMapInfo(rtm, val)   ()
#endif

#ifndef rtmGetDefaultParam
# define rtmGetDefaultParam(rtm)       ((rtm)->ModelData.defaultParam)
#endif

#ifndef rtmSetDefaultParam
# define rtmSetDefaultParam(rtm, val)  ((rtm)->ModelData.defaultParam = (val))
#endif

#ifndef rtmGetDerivCacheNeedsReset
# define rtmGetDerivCacheNeedsReset(rtm) ((rtm)->ModelData.derivCacheNeedsReset)
#endif

#ifndef rtmSetDerivCacheNeedsReset
# define rtmSetDerivCacheNeedsReset(rtm, val) ((rtm)->ModelData.derivCacheNeedsReset = (val))
#endif

#ifndef rtmGetDirectFeedThrough
# define rtmGetDirectFeedThrough(rtm)  ((rtm)->Sizes.sysDirFeedThru)
#endif

#ifndef rtmSetDirectFeedThrough
# define rtmSetDirectFeedThrough(rtm, val) ((rtm)->Sizes.sysDirFeedThru = (val))
#endif

#ifndef rtmGetErrorStatusFlag
# define rtmGetErrorStatusFlag(rtm)    ((rtm)->errorStatus)
#endif

#ifndef rtmSetErrorStatusFlag
# define rtmSetErrorStatusFlag(rtm, val) ((rtm)->errorStatus = (val))
#endif

#ifndef rtmGetFinalTime
# define rtmGetFinalTime(rtm)          ((rtm)->Timing.tFinal)
#endif

#ifndef rtmSetFinalTime
# define rtmSetFinalTime(rtm, val)     ((rtm)->Timing.tFinal = (val))
#endif

#ifndef rtmGetFirstInitCondFlag
# define rtmGetFirstInitCondFlag(rtm)  ()
#endif

#ifndef rtmSetFirstInitCondFlag
# define rtmSetFirstInitCondFlag(rtm, val) ()
#endif

#ifndef rtmGetIntgData
# define rtmGetIntgData(rtm)           ((rtm)->ModelData.intgData)
#endif

#ifndef rtmSetIntgData
# define rtmSetIntgData(rtm, val)      ((rtm)->ModelData.intgData = (val))
#endif

#ifndef rtmGetMdlRefGlobalTID
# define rtmGetMdlRefGlobalTID(rtm)    ()
#endif

#ifndef rtmSetMdlRefGlobalTID
# define rtmSetMdlRefGlobalTID(rtm, val) ()
#endif

#ifndef rtmGetMdlRefTriggerTID
# define rtmGetMdlRefTriggerTID(rtm)   ()
#endif

#ifndef rtmSetMdlRefTriggerTID
# define rtmSetMdlRefTriggerTID(rtm, val) ()
#endif

#ifndef rtmGetModelMappingInfo
# define rtmGetModelMappingInfo(rtm)   ((rtm)->SpecialInfo.mappingInfo)
#endif

#ifndef rtmSetModelMappingInfo
# define rtmSetModelMappingInfo(rtm, val) ((rtm)->SpecialInfo.mappingInfo = (val))
#endif

#ifndef rtmGetModelName
# define rtmGetModelName(rtm)          ((rtm)->modelName)
#endif

#ifndef rtmSetModelName
# define rtmSetModelName(rtm, val)     ((rtm)->modelName = (val))
#endif

#ifndef rtmGetNonInlinedSFcns
# define rtmGetNonInlinedSFcns(rtm)    ()
#endif

#ifndef rtmSetNonInlinedSFcns
# define rtmSetNonInlinedSFcns(rtm, val) ()
#endif

#ifndef rtmGetNumBlockIO
# define rtmGetNumBlockIO(rtm)         ((rtm)->Sizes.numBlockIO)
#endif

#ifndef rtmSetNumBlockIO
# define rtmSetNumBlockIO(rtm, val)    ((rtm)->Sizes.numBlockIO = (val))
#endif

#ifndef rtmGetNumBlockParams
# define rtmGetNumBlockParams(rtm)     ((rtm)->Sizes.numBlockPrms)
#endif

#ifndef rtmSetNumBlockParams
# define rtmSetNumBlockParams(rtm, val) ((rtm)->Sizes.numBlockPrms = (val))
#endif

#ifndef rtmGetNumBlocks
# define rtmGetNumBlocks(rtm)          ((rtm)->Sizes.numBlocks)
#endif

#ifndef rtmSetNumBlocks
# define rtmSetNumBlocks(rtm, val)     ((rtm)->Sizes.numBlocks = (val))
#endif

#ifndef rtmGetNumContStates
# define rtmGetNumContStates(rtm)      ((rtm)->Sizes.numContStates)
#endif

#ifndef rtmSetNumContStates
# define rtmSetNumContStates(rtm, val) ((rtm)->Sizes.numContStates = (val))
#endif

#ifndef rtmGetNumDWork
# define rtmGetNumDWork(rtm)           ((rtm)->Sizes.numDwork)
#endif

#ifndef rtmSetNumDWork
# define rtmSetNumDWork(rtm, val)      ((rtm)->Sizes.numDwork = (val))
#endif

#ifndef rtmGetNumInputPorts
# define rtmGetNumInputPorts(rtm)      ((rtm)->Sizes.numIports)
#endif

#ifndef rtmSetNumInputPorts
# define rtmSetNumInputPorts(rtm, val) ((rtm)->Sizes.numIports = (val))
#endif

#ifndef rtmGetNumNonSampledZCs
# define rtmGetNumNonSampledZCs(rtm)   ((rtm)->Sizes.numNonSampZCs)
#endif

#ifndef rtmSetNumNonSampledZCs
# define rtmSetNumNonSampledZCs(rtm, val) ((rtm)->Sizes.numNonSampZCs = (val))
#endif

#ifndef rtmGetNumOutputPorts
# define rtmGetNumOutputPorts(rtm)     ((rtm)->Sizes.numOports)
#endif

#ifndef rtmSetNumOutputPorts
# define rtmSetNumOutputPorts(rtm, val) ((rtm)->Sizes.numOports = (val))
#endif

#ifndef rtmGetNumPeriodicContStates
# define rtmGetNumPeriodicContStates(rtm) ((rtm)->Sizes.numPeriodicContStates)
#endif

#ifndef rtmSetNumPeriodicContStates
# define rtmSetNumPeriodicContStates(rtm, val) ((rtm)->Sizes.numPeriodicContStates = (val))
#endif

#ifndef rtmGetNumSFcnParams
# define rtmGetNumSFcnParams(rtm)      ((rtm)->Sizes.numSFcnPrms)
#endif

#ifndef rtmSetNumSFcnParams
# define rtmSetNumSFcnParams(rtm, val) ((rtm)->Sizes.numSFcnPrms = (val))
#endif

#ifndef rtmGetNumSFunctions
# define rtmGetNumSFunctions(rtm)      ((rtm)->Sizes.numSFcns)
#endif

#ifndef rtmSetNumSFunctions
# define rtmSetNumSFunctions(rtm, val) ((rtm)->Sizes.numSFcns = (val))
#endif

#ifndef rtmGetNumSampleTimes
# define rtmGetNumSampleTimes(rtm)     ((rtm)->Sizes.numSampTimes)
#endif

#ifndef rtmSetNumSampleTimes
# define rtmSetNumSampleTimes(rtm, val) ((rtm)->Sizes.numSampTimes = (val))
#endif

#ifndef rtmGetNumU
# define rtmGetNumU(rtm)               ((rtm)->Sizes.numU)
#endif

#ifndef rtmSetNumU
# define rtmSetNumU(rtm, val)          ((rtm)->Sizes.numU = (val))
#endif

#ifndef rtmGetNumY
# define rtmGetNumY(rtm)               ((rtm)->Sizes.numY)
#endif

#ifndef rtmSetNumY
# define rtmSetNumY(rtm, val)          ((rtm)->Sizes.numY = (val))
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

#ifndef rtmGetOffsetTimeArray
# define rtmGetOffsetTimeArray(rtm)    ((rtm)->Timing.offsetTimesArray)
#endif

#ifndef rtmSetOffsetTimeArray
# define rtmSetOffsetTimeArray(rtm, val) ((rtm)->Timing.offsetTimesArray = (val))
#endif

#ifndef rtmGetOffsetTimePtr
# define rtmGetOffsetTimePtr(rtm)      ((rtm)->Timing.offsetTimes)
#endif

#ifndef rtmSetOffsetTimePtr
# define rtmSetOffsetTimePtr(rtm, val) ((rtm)->Timing.offsetTimes = (val))
#endif

#ifndef rtmGetOptions
# define rtmGetOptions(rtm)            ((rtm)->Sizes.options)
#endif

#ifndef rtmSetOptions
# define rtmSetOptions(rtm, val)       ((rtm)->Sizes.options = (val))
#endif

#ifndef rtmGetParamIsMalloced
# define rtmGetParamIsMalloced(rtm)    ()
#endif

#ifndef rtmSetParamIsMalloced
# define rtmSetParamIsMalloced(rtm, val) ()
#endif

#ifndef rtmGetPath
# define rtmGetPath(rtm)               ((rtm)->path)
#endif

#ifndef rtmSetPath
# define rtmSetPath(rtm, val)          ((rtm)->path = (val))
#endif

#ifndef rtmGetPerTaskSampleHits
# define rtmGetPerTaskSampleHits(rtm)  ()
#endif

#ifndef rtmSetPerTaskSampleHits
# define rtmSetPerTaskSampleHits(rtm, val) ()
#endif

#ifndef rtmGetPerTaskSampleHitsArray
# define rtmGetPerTaskSampleHitsArray(rtm) ((rtm)->Timing.perTaskSampleHitsArray)
#endif

#ifndef rtmSetPerTaskSampleHitsArray
# define rtmSetPerTaskSampleHitsArray(rtm, val) ((rtm)->Timing.perTaskSampleHitsArray = (val))
#endif

#ifndef rtmGetPerTaskSampleHitsPtr
# define rtmGetPerTaskSampleHitsPtr(rtm) ((rtm)->Timing.perTaskSampleHits)
#endif

#ifndef rtmSetPerTaskSampleHitsPtr
# define rtmSetPerTaskSampleHitsPtr(rtm, val) ((rtm)->Timing.perTaskSampleHits = (val))
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

#ifndef rtmGetPrevZCSigState
# define rtmGetPrevZCSigState(rtm)     ((rtm)->ModelData.prevZCSigState)
#endif

#ifndef rtmSetPrevZCSigState
# define rtmSetPrevZCSigState(rtm, val) ((rtm)->ModelData.prevZCSigState = (val))
#endif

#ifndef rtmGetRTWExtModeInfo
# define rtmGetRTWExtModeInfo(rtm)     ((rtm)->extModeInfo)
#endif

#ifndef rtmSetRTWExtModeInfo
# define rtmSetRTWExtModeInfo(rtm, val) ((rtm)->extModeInfo = (val))
#endif

#ifndef rtmGetRTWGeneratedSFcn
# define rtmGetRTWGeneratedSFcn(rtm)   ((rtm)->Sizes.rtwGenSfcn)
#endif

#ifndef rtmSetRTWGeneratedSFcn
# define rtmSetRTWGeneratedSFcn(rtm, val) ((rtm)->Sizes.rtwGenSfcn = (val))
#endif

#ifndef rtmGetRTWLogInfo
# define rtmGetRTWLogInfo(rtm)         ((rtm)->rtwLogInfo)
#endif

#ifndef rtmSetRTWLogInfo
# define rtmSetRTWLogInfo(rtm, val)    ((rtm)->rtwLogInfo = (val))
#endif

#ifndef rtmGetRTWRTModelMethodsInfo
# define rtmGetRTWRTModelMethodsInfo(rtm) ()
#endif

#ifndef rtmSetRTWRTModelMethodsInfo
# define rtmSetRTWRTModelMethodsInfo(rtm, val) ()
#endif

#ifndef rtmGetRTWSfcnInfo
# define rtmGetRTWSfcnInfo(rtm)        ((rtm)->sfcnInfo)
#endif

#ifndef rtmSetRTWSfcnInfo
# define rtmSetRTWSfcnInfo(rtm, val)   ((rtm)->sfcnInfo = (val))
#endif

#ifndef rtmGetRTWSolverInfo
# define rtmGetRTWSolverInfo(rtm)      ((rtm)->solverInfo)
#endif

#ifndef rtmSetRTWSolverInfo
# define rtmSetRTWSolverInfo(rtm, val) ((rtm)->solverInfo = (val))
#endif

#ifndef rtmGetRTWSolverInfoPtr
# define rtmGetRTWSolverInfoPtr(rtm)   ((rtm)->solverInfoPtr)
#endif

#ifndef rtmSetRTWSolverInfoPtr
# define rtmSetRTWSolverInfoPtr(rtm, val) ((rtm)->solverInfoPtr = (val))
#endif

#ifndef rtmGetReservedForXPC
# define rtmGetReservedForXPC(rtm)     ((rtm)->SpecialInfo.xpcData)
#endif

#ifndef rtmSetReservedForXPC
# define rtmSetReservedForXPC(rtm, val) ((rtm)->SpecialInfo.xpcData = (val))
#endif

#ifndef rtmGetRootDWork
# define rtmGetRootDWork(rtm)          ((rtm)->ModelData.dwork)
#endif

#ifndef rtmSetRootDWork
# define rtmSetRootDWork(rtm, val)     ((rtm)->ModelData.dwork = (val))
#endif

#ifndef rtmGetSFunctions
# define rtmGetSFunctions(rtm)         ((rtm)->childSfunctions)
#endif

#ifndef rtmSetSFunctions
# define rtmSetSFunctions(rtm, val)    ((rtm)->childSfunctions = (val))
#endif

#ifndef rtmGetSampleHitArray
# define rtmGetSampleHitArray(rtm)     ((rtm)->Timing.sampleHitArray)
#endif

#ifndef rtmSetSampleHitArray
# define rtmSetSampleHitArray(rtm, val) ((rtm)->Timing.sampleHitArray = (val))
#endif

#ifndef rtmGetSampleHitPtr
# define rtmGetSampleHitPtr(rtm)       ((rtm)->Timing.sampleHits)
#endif

#ifndef rtmSetSampleHitPtr
# define rtmSetSampleHitPtr(rtm, val)  ((rtm)->Timing.sampleHits = (val))
#endif

#ifndef rtmGetSampleTimeArray
# define rtmGetSampleTimeArray(rtm)    ((rtm)->Timing.sampleTimesArray)
#endif

#ifndef rtmSetSampleTimeArray
# define rtmSetSampleTimeArray(rtm, val) ((rtm)->Timing.sampleTimesArray = (val))
#endif

#ifndef rtmGetSampleTimePtr
# define rtmGetSampleTimePtr(rtm)      ((rtm)->Timing.sampleTimes)
#endif

#ifndef rtmSetSampleTimePtr
# define rtmSetSampleTimePtr(rtm, val) ((rtm)->Timing.sampleTimes = (val))
#endif

#ifndef rtmGetSampleTimeTaskIDArray
# define rtmGetSampleTimeTaskIDArray(rtm) ((rtm)->Timing.sampleTimeTaskIDArray)
#endif

#ifndef rtmSetSampleTimeTaskIDArray
# define rtmSetSampleTimeTaskIDArray(rtm, val) ((rtm)->Timing.sampleTimeTaskIDArray = (val))
#endif

#ifndef rtmGetSampleTimeTaskIDPtr
# define rtmGetSampleTimeTaskIDPtr(rtm) ((rtm)->Timing.sampleTimeTaskIDPtr)
#endif

#ifndef rtmSetSampleTimeTaskIDPtr
# define rtmSetSampleTimeTaskIDPtr(rtm, val) ((rtm)->Timing.sampleTimeTaskIDPtr = (val))
#endif

#ifndef rtmGetSimMode
# define rtmGetSimMode(rtm)            ((rtm)->simMode)
#endif

#ifndef rtmSetSimMode
# define rtmSetSimMode(rtm, val)       ((rtm)->simMode = (val))
#endif

#ifndef rtmGetSimTimeStep
# define rtmGetSimTimeStep(rtm)        ((rtm)->Timing.simTimeStep)
#endif

#ifndef rtmSetSimTimeStep
# define rtmSetSimTimeStep(rtm, val)   ((rtm)->Timing.simTimeStep = (val))
#endif

#ifndef rtmGetStartTime
# define rtmGetStartTime(rtm)          ((rtm)->Timing.tStart)
#endif

#ifndef rtmSetStartTime
# define rtmSetStartTime(rtm, val)     ((rtm)->Timing.tStart = (val))
#endif

#ifndef rtmGetStepSize
# define rtmGetStepSize(rtm)           ((rtm)->Timing.stepSize)
#endif

#ifndef rtmSetStepSize
# define rtmSetStepSize(rtm, val)      ((rtm)->Timing.stepSize = (val))
#endif

#ifndef rtmGetStopRequestedFlag
# define rtmGetStopRequestedFlag(rtm)  ((rtm)->Timing.stopRequestedFlag)
#endif

#ifndef rtmSetStopRequestedFlag
# define rtmSetStopRequestedFlag(rtm, val) ((rtm)->Timing.stopRequestedFlag = (val))
#endif

#ifndef rtmGetTaskCounters
# define rtmGetTaskCounters(rtm)       ()
#endif

#ifndef rtmSetTaskCounters
# define rtmSetTaskCounters(rtm, val)  ()
#endif

#ifndef rtmGetTaskTimeArray
# define rtmGetTaskTimeArray(rtm)      ((rtm)->Timing.tArray)
#endif

#ifndef rtmSetTaskTimeArray
# define rtmSetTaskTimeArray(rtm, val) ((rtm)->Timing.tArray = (val))
#endif

#ifndef rtmGetTimePtr
# define rtmGetTimePtr(rtm)            ((rtm)->Timing.t)
#endif

#ifndef rtmSetTimePtr
# define rtmSetTimePtr(rtm, val)       ((rtm)->Timing.t = (val))
#endif

#ifndef rtmGetTimingData
# define rtmGetTimingData(rtm)         ((rtm)->Timing.timingData)
#endif

#ifndef rtmSetTimingData
# define rtmSetTimingData(rtm, val)    ((rtm)->Timing.timingData = (val))
#endif

#ifndef rtmGetU
# define rtmGetU(rtm)                  ((rtm)->ModelData.inputs)
#endif

#ifndef rtmSetU
# define rtmSetU(rtm, val)             ((rtm)->ModelData.inputs = (val))
#endif

#ifndef rtmGetVarNextHitTimesListPtr
# define rtmGetVarNextHitTimesListPtr(rtm) ((rtm)->Timing.varNextHitTimesList)
#endif

#ifndef rtmSetVarNextHitTimesListPtr
# define rtmSetVarNextHitTimesListPtr(rtm, val) ((rtm)->Timing.varNextHitTimesList = (val))
#endif

#ifndef rtmGetY
# define rtmGetY(rtm)                  ((rtm)->ModelData.outputs)
#endif

#ifndef rtmSetY
# define rtmSetY(rtm, val)             ((rtm)->ModelData.outputs = (val))
#endif

#ifndef rtmGetZCCacheNeedsReset
# define rtmGetZCCacheNeedsReset(rtm)  ((rtm)->ModelData.zCCacheNeedsReset)
#endif

#ifndef rtmSetZCCacheNeedsReset
# define rtmSetZCCacheNeedsReset(rtm, val) ((rtm)->ModelData.zCCacheNeedsReset = (val))
#endif

#ifndef rtmGetZCSignalValues
# define rtmGetZCSignalValues(rtm)     ((rtm)->ModelData.zcSignalValues)
#endif

#ifndef rtmSetZCSignalValues
# define rtmSetZCSignalValues(rtm, val) ((rtm)->ModelData.zcSignalValues = (val))
#endif

#ifndef rtmGet_TimeOfLastOutput
# define rtmGet_TimeOfLastOutput(rtm)  ((rtm)->Timing.timeOfLastOutput)
#endif

#ifndef rtmSet_TimeOfLastOutput
# define rtmSet_TimeOfLastOutput(rtm, val) ((rtm)->Timing.timeOfLastOutput = (val))
#endif

#ifndef rtmGetdX
# define rtmGetdX(rtm)                 ((rtm)->ModelData.derivs)
#endif

#ifndef rtmSetdX
# define rtmSetdX(rtm, val)            ((rtm)->ModelData.derivs = (val))
#endif

#ifndef rtmGetChecksumVal
# define rtmGetChecksumVal(rtm, idx)   ((rtm)->Sizes.checksums[idx])
#endif

#ifndef rtmSetChecksumVal
# define rtmSetChecksumVal(rtm, idx, val) ((rtm)->Sizes.checksums[idx] = (val))
#endif

#ifndef rtmGetDWork
# define rtmGetDWork(rtm, idx)         ((rtm)->ModelData.dwork[idx])
#endif

#ifndef rtmSetDWork
# define rtmSetDWork(rtm, idx, val)    ((rtm)->ModelData.dwork[idx] = (val))
#endif

#ifndef rtmGetOffsetTime
# define rtmGetOffsetTime(rtm, idx)    ((rtm)->Timing.offsetTimes[idx])
#endif

#ifndef rtmSetOffsetTime
# define rtmSetOffsetTime(rtm, idx, val) ((rtm)->Timing.offsetTimes[idx] = (val))
#endif

#ifndef rtmGetSFunction
# define rtmGetSFunction(rtm, idx)     ((rtm)->childSfunctions[idx])
#endif

#ifndef rtmSetSFunction
# define rtmSetSFunction(rtm, idx, val) ((rtm)->childSfunctions[idx] = (val))
#endif

#ifndef rtmGetSampleTime
# define rtmGetSampleTime(rtm, idx)    ((rtm)->Timing.sampleTimes[idx])
#endif

#ifndef rtmSetSampleTime
# define rtmSetSampleTime(rtm, idx, val) ((rtm)->Timing.sampleTimes[idx] = (val))
#endif

#ifndef rtmGetSampleTimeTaskID
# define rtmGetSampleTimeTaskID(rtm, idx) ((rtm)->Timing.sampleTimeTaskIDPtr[idx])
#endif

#ifndef rtmSetSampleTimeTaskID
# define rtmSetSampleTimeTaskID(rtm, idx, val) ((rtm)->Timing.sampleTimeTaskIDPtr[idx] = (val))
#endif

#ifndef rtmGetVarNextHitTimeList
# define rtmGetVarNextHitTimeList(rtm, idx) ((rtm)->Timing.varNextHitTimesList[idx])
#endif

#ifndef rtmSetVarNextHitTimeList
# define rtmSetVarNextHitTimeList(rtm, idx, val) ((rtm)->Timing.varNextHitTimesList[idx] = (val))
#endif

#ifndef rtmIsContinuousTask
# define rtmIsContinuousTask(rtm, tid) ((tid) == 0)
#endif

#ifndef rtmGetErrorStatus
# define rtmGetErrorStatus(rtm)        ((rtm)->errorStatus)
#endif

#ifndef rtmSetErrorStatus
# define rtmSetErrorStatus(rtm, val)   ((rtm)->errorStatus = (val))
#endif

#ifndef rtmIsMajorTimeStep
# define rtmIsMajorTimeStep(rtm)       (((rtm)->Timing.simTimeStep) == MAJOR_TIME_STEP)
#endif

#ifndef rtmIsMinorTimeStep
# define rtmIsMinorTimeStep(rtm)       (((rtm)->Timing.simTimeStep) == MINOR_TIME_STEP)
#endif

#ifndef rtmIsSampleHit
# define rtmIsSampleHit(rtm, sti, tid) ((rtmIsMajorTimeStep((rtm)) && (rtm)->Timing.sampleHits[(rtm)->Timing.sampleTimeTaskIDPtr[sti]]))
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

#ifndef rtmSetT
# define rtmSetT(rtm, val)                                       /* Do Nothing */
#endif

#ifndef rtmGetTFinal
# define rtmGetTFinal(rtm)             ((rtm)->Timing.tFinal)
#endif

#ifndef rtmSetTFinal
# define rtmSetTFinal(rtm, val)        ((rtm)->Timing.tFinal = (val))
#endif

#ifndef rtmGetTPtr
# define rtmGetTPtr(rtm)               ((rtm)->Timing.t)
#endif

#ifndef rtmSetTPtr
# define rtmSetTPtr(rtm, val)          ((rtm)->Timing.t = (val))
#endif

#ifndef rtmGetTStart
# define rtmGetTStart(rtm)             ((rtm)->Timing.tStart)
#endif

#ifndef rtmSetTStart
# define rtmSetTStart(rtm, val)        ((rtm)->Timing.tStart = (val))
#endif

#ifndef rtmGetTaskTime
# define rtmGetTaskTime(rtm, sti)      (rtmGetTPtr((rtm))[(rtm)->Timing.sampleTimeTaskIDPtr[sti]])
#endif

#ifndef rtmSetTaskTime
# define rtmSetTaskTime(rtm, sti, val) (rtmGetTPtr((rtm))[sti] = (val))
#endif

#ifndef rtmGetTimeOfLastOutput
# define rtmGetTimeOfLastOutput(rtm)   ((rtm)->Timing.timeOfLastOutput)
#endif

#ifdef rtmGetRTWSolverInfo
#undef rtmGetRTWSolverInfo
#endif

#define rtmGetRTWSolverInfo(rtm)       &((rtm)->solverInfo)

/* Definition for use in the target main file */
#define AbstractFuelControl_M1_rtModel RT_MODEL_AbstractFuelControl__T

/* Block signals (auto storage) */
typedef struct {
  real_T MultiportSwitch;              /* '<S21>/Multiport Switch' */
  real_T Product;                      /* '<S22>/Product' */
  real_T Sqrt;                         /* '<S22>/Sqrt' */
  real_T Sum1;                         /* '<S23>/Sum1' */
  real_T AF_sensor;                    /* '<S18>/A//F_sensor' */
  real_T theta090;                     /* '<S1>/theta [0 90]' */
  real_T Gain;                         /* '<S20>/Gain' */
  real_T airbyfuel;                    /* '<S4>/Divide' */
  real_T Gain1;                        /* '<S19>/Gain1' */
  real_T delays;                       /* '<S4>/delay (s)' */
  real_T RTVm;                         /* '<S5>/RT//Vm' */
  real_T Sum;                          /* '<S7>/Sum' */
  real_T Pwon;                         /* '<S3>/Pwon' */
  real_T PulseGenerator_10ms;          /* '<S3>/PulseGenerator_10ms' */
  real32_T Sum2;                       /* '<S13>/Sum2' */
  real32_T Sum3;                       /* '<S13>/Sum3' */
  boolean_T RelationalOperator;        /* '<S2>/Relational Operator' */
  boolean_T HiddenBuf_InsertedFor_CalcuateE;/* '<S2>/Relational Operator' */
} B_AbstractFuelControl_M1_T;

/* Block states (auto storage) for system '<Root>' */
typedef struct {
  struct {
    real_T modelTStart;
    real_T TUbufferArea[61440];
  } fuelsystemtransportdelay_RWORK;    /* '<S4>/fuel system transport delay' */

  struct {
    void *LoggedData[4];
  } Monitor_PWORK;                     /* '<Root>/Monitor ' */

  struct {
    void *FilePtr;
  } ToFile_PWORK;                      /* '<Root>/To File' */

  struct {
    void *LoggedData[3];
  } Monitor_PWORK_h;                   /* '<S1>/Monitor' */

  struct {
    void *TUbufferPtrs[3];
  } fuelsystemtransportdelay_PWORK;    /* '<S4>/fuel system transport delay' */

  real32_T UnitDelay2_DSTATE;          /* '<S15>/Unit Delay2' */
  real32_T UnitDelay1_DSTATE;          /* '<S13>/UnitDelay1' */
  real32_T UnitDelay1_DSTATE_d;        /* '<S12>/UnitDelay1' */
  real32_T commanded_fuel;             /* '<S3>/commanded_fuel' */
  real32_T airbyfuel_ref;              /* '<S3>/mode_fb1' */
  real32_T engine_speed;               /* '<S8>/DataStoreMemory' */
  real32_T throttle_flow;              /* '<S8>/DataStoreMemory1' */
  real32_T airbyfuel_meas;             /* '<S8>/DataStoreMemory2' */
  real32_T throttle_angle;             /* '<S8>/DataStoreMemory3' */
  int32_T clockTickCounter;            /* '<Root>/Pedal Angle (deg)' */
  int32_T clockTickCounter_l;          /* '<S3>/PulseGenerator_10ms' */
  struct {
    int_T Count;
    int_T Decimation;
  } ToFile_IWORK;                      /* '<Root>/To File' */

  struct {
    int_T Tail;
    int_T Head;
    int_T Last;
    int_T CircularBufSize;
  } fuelsystemtransportdelay_IWORK;    /* '<S4>/fuel system transport delay' */

  boolean_T UnitDelay_DSTATE;          /* '<S17>/Unit Delay' */
  boolean_T UnitDelay1_DSTATE_a;       /* '<S16>/Unit Delay1' */
  boolean_T UnitDelay1_DSTATE_e;       /* '<S15>/Unit Delay1' */
  int8_T SwitchCase_ActiveSubsystem;   /* '<S21>/Switch Case' */
  boolean_T controller_mode;           /* '<S3>/mode_fb' */
  boolean_T CalcuateError_MODE;        /* '<S2>/Calcuate Error' */
} DW_AbstractFuelControl_M1_T;

/* Continuous states (auto storage) */
typedef struct {
  real_T Integrator_CSTATE;            /* '<S22>/Integrator' */
  real_T Integrator_CSTATE_m;          /* '<S20>/Integrator' */
  real_T Throttledelay_CSTATE;         /* '<S1>/Throttle delay' */
  real_T p00543bar_CSTATE;             /* '<S5>/p0 = 0.543 (bar)' */
  real_T Integrator_CSTATE_h;          /* '<S19>/Integrator' */
  real_T Integrator_CSTATE_c;          /* '<S7>/Integrator' */
  real_T fuelsystemtransportdelay_CSTATE;/* '<S4>/fuel system transport delay' */
} X_AbstractFuelControl_M1_T;

/* State derivatives (auto storage) */
typedef struct {
  real_T Integrator_CSTATE;            /* '<S22>/Integrator' */
  real_T Integrator_CSTATE_m;          /* '<S20>/Integrator' */
  real_T Throttledelay_CSTATE;         /* '<S1>/Throttle delay' */
  real_T p00543bar_CSTATE;             /* '<S5>/p0 = 0.543 (bar)' */
  real_T Integrator_CSTATE_h;          /* '<S19>/Integrator' */
  real_T Integrator_CSTATE_c;          /* '<S7>/Integrator' */
  real_T fuelsystemtransportdelay_CSTATE;/* '<S4>/fuel system transport delay' */
} XDot_AbstractFuelControl_M1_T;

/* State disabled  */
typedef struct {
  boolean_T Integrator_CSTATE;         /* '<S22>/Integrator' */
  boolean_T Integrator_CSTATE_m;       /* '<S20>/Integrator' */
  boolean_T Throttledelay_CSTATE;      /* '<S1>/Throttle delay' */
  boolean_T p00543bar_CSTATE;          /* '<S5>/p0 = 0.543 (bar)' */
  boolean_T Integrator_CSTATE_h;       /* '<S19>/Integrator' */
  boolean_T Integrator_CSTATE_c;       /* '<S7>/Integrator' */
  boolean_T fuelsystemtransportdelay_CSTATE;/* '<S4>/fuel system transport delay' */
} XDis_AbstractFuelControl_M1_T;

/* Zero-crossing (trigger) state */
typedef struct {
  ZCSigState fuel_controller_pwon_Trig_ZCE;/* '<S8>/fuel_controller_pwon' */
  ZCSigState fuel_controller_mode_10ms_Trig_;/* '<S8>/fuel_controller_mode_10ms' */
  ZCSigState fuel_controller_10ms_Trig_ZCE;/* '<S8>/fuel_controller_10ms' */
} PrevZCX_AbstractFuelControl_M_T;

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
} ExtU_AbstractFuelControl_M1_T;

/* External outputs (root outports fed by signals with auto storage) */
typedef struct {
  real_T verificatonmeasurement;       /* '<Root>/verificaton measurement' */
  real_T mode;                         /* '<Root>/mode' */
  real_T pedalangle;                   /* '<Root>/pedal angle' */
} ExtY_AbstractFuelControl_M1_T;

/* Backward compatible GRT Identifiers */
#define rtB                            AbstractFuelControl_M1_B
#define BlockIO                        B_AbstractFuelControl_M1_T
#define rtU                            AbstractFuelControl_M1_U
#define ExternalInputs                 ExtU_AbstractFuelControl_M1_T
#define rtX                            AbstractFuelControl_M1_X
#define ContinuousStates               X_AbstractFuelControl_M1_T
#define rtXdot                         AbstractFuelControl_M1_XDot
#define StateDerivatives               XDot_AbstractFuelControl_M1_T
#define tXdis                          AbstractFuelControl_M1_XDis
#define StateDisabled                  XDis_AbstractFuelControl_M1_T
#define rtY                            AbstractFuelControl_M1_Y
#define ExternalOutputs                ExtY_AbstractFuelControl_M1_T
#define rtP                            AbstractFuelControl_M1_P
#define Parameters                     P_AbstractFuelControl_M1_T
#define rtDWork                        AbstractFuelControl_M1_DW
#define D_Work                         DW_AbstractFuelControl_M1_T
#define rtPrevZCSigState               AbstractFuelControl_M1_PrevZCX
#define PrevZCSigStates                PrevZCX_AbstractFuelControl_M_T

/* Parameters (auto storage) */
struct P_AbstractFuelControl_M1_T_ {
  real_T AF_sensor_tol;                /* Variable: AF_sensor_tol
                                        * Referenced by: '<S1>/A//F sensor tolerance [0.99 1.01]'
                                        */
  real_T MAF_sensor_tol;               /* Variable: MAF_sensor_tol
                                        * Referenced by: '<S1>/MAF sensor tolerance [0.95 1.05]'
                                        */
  real_T fault_time;                   /* Variable: fault_time
                                        * Referenced by: '<S1>/A//F Sensor Fault Injection'
                                        */
  real_T fuel_inj_tol;                 /* Variable: fuel_inj_tol
                                        * Referenced by: '<S1>/fuel injector tolerance [0.95 1.05]'
                                        */
  real_T measureTime;                  /* Variable: measureTime
                                        * Referenced by: '<S2>/MeasureOn'
                                        */
  real_T simTime;                      /* Variable: simTime
                                        * Referenced by: '<S22>/Constant'
                                        */
  uint8_T spec_num;                    /* Variable: spec_num
                                        * Referenced by: '<S2>/spec_num'
                                        */
  real_T Pwon_Time;                    /* Expression: 0.001
                                        * Referenced by: '<S3>/Pwon'
                                        */
  real_T Pwon_Y0;                      /* Expression: 0
                                        * Referenced by: '<S3>/Pwon'
                                        */
  real_T Pwon_YFinal;                  /* Expression: 1
                                        * Referenced by: '<S3>/Pwon'
                                        */
  real_T PulseGenerator_10ms_Amp;      /* Expression: 1
                                        * Referenced by: '<S3>/PulseGenerator_10ms'
                                        */
  real_T PulseGenerator_10ms_Period;   /* Computed Parameter: PulseGenerator_10ms_Period
                                        * Referenced by: '<S3>/PulseGenerator_10ms'
                                        */
  real_T PulseGenerator_10ms_Duty;     /* Computed Parameter: PulseGenerator_10ms_Duty
                                        * Referenced by: '<S3>/PulseGenerator_10ms'
                                        */
  real_T PulseGenerator_10ms_PhaseDelay;/* Expression: 0.01
                                         * Referenced by: '<S3>/PulseGenerator_10ms'
                                         */
  real_T Integrator_IC;                /* Expression: 14.7
                                        * Referenced by: '<S20>/Integrator'
                                        */
  real_T AF_sensor_Gain;               /* Expression: 1
                                        * Referenced by: '<S18>/A//F_sensor'
                                        */
  real_T Throttledelay_A;              /* Computed Parameter: Throttledelay_A
                                        * Referenced by: '<S1>/Throttle delay'
                                        */
  real_T Throttledelay_C;              /* Computed Parameter: Throttledelay_C
                                        * Referenced by: '<S1>/Throttle delay'
                                        */
  real_T Baseopeningangle_Value;       /* Expression: 8.8
                                        * Referenced by: '<S1>/Base opening angle'
                                        */
  real_T theta090_UpperSat;            /* Expression: 90
                                        * Referenced by: '<S1>/theta [0 90]'
                                        */
  real_T theta090_LowerSat;            /* Expression: 0
                                        * Referenced by: '<S1>/theta [0 90]'
                                        */
  real_T EngineSpeed9001100_UpperSat;  /* Expression: 1100
                                        * Referenced by: '<S1>/Engine Speed [900 1100]'
                                        */
  real_T EngineSpeed9001100_LowerSat;  /* Expression: 900
                                        * Referenced by: '<S1>/Engine Speed [900 1100]'
                                        */
  real_T rpmtorads_Gain;               /* Expression: pi/30
                                        * Referenced by: '<S1>/(rpm) to (rad//s)'
                                        */
  real_T AFSensorFaultInjection_Y0;    /* Expression: 0
                                        * Referenced by: '<S1>/A//F Sensor Fault Injection'
                                        */
  real_T AFSensorFaultInjection_YFinal;/* Expression: 1
                                        * Referenced by: '<S1>/A//F Sensor Fault Injection'
                                        */
  real_T FaultySensorOutput_Value;     /* Expression: -1
                                        * Referenced by: '<S4>/FaultySensorOutput'
                                        */
  real_T Switch_Threshold;             /* Expression: 0.5
                                        * Referenced by: '<S4>/Switch'
                                        */
  real_T p00543bar_IC;                 /* Expression: 0.982
                                        * Referenced by: '<S5>/p0 = 0.543 (bar)'
                                        */
  real_T AtmosphericPressurebar_Value; /* Expression: 1
                                        * Referenced by: '<S1>/Atmospheric Pressure (bar)'
                                        */
  real_T SonicFlow_Value;              /* Expression: 1.0
                                        * Referenced by: '<S6>/Sonic Flow '
                                        */
  real_T Switch_Threshold_g;           /* Expression: 0.5
                                        * Referenced by: '<S6>/Switch'
                                        */
  real_T Integrator_IC_l;              /* Expression: 14.7
                                        * Referenced by: '<S19>/Integrator'
                                        */
  real_T Gain_Gain;                    /* Expression: 50
                                        * Referenced by: '<S20>/Gain'
                                        */
  real_T radstorpm_Gain;               /* Expression: 60/(2*pi)
                                        * Referenced by: '<S7>/rad//s to rpm'
                                        */
  real_T Gain_Gain_l;                  /* Expression: 4*pi/4
                                        * Referenced by: '<S4>/Gain'
                                        */
  real_T uKappa_tableData[20];         /* Expression: reshape([0.8,.7,.7,.8,.9,.7,.66,.65,.73,.85,.66,.66,.63,.66,.8,.6,.6,.6,.6,.7],5,4)
                                        * Referenced by: '<S7>/1-Kappa'
                                        */
  real_T uKappa_bp01Data[5];           /* Expression: [1000,1500,2000,2500,3000]
                                        * Referenced by: '<S7>/1-Kappa'
                                        */
  real_T uKappa_bp02Data[4];           /* Expression: [.1,.2,.3,.4]
                                        * Referenced by: '<S7>/1-Kappa'
                                        */
  real_T Kappatolerance0911_Gain;      /* Expression: 1
                                        * Referenced by: '<S7>/Kappa tolerance [0.9 1.1]'
                                        */
  real_T Integrator_IC_m;              /* Expression: .0112
                                        * Referenced by: '<S7>/Integrator'
                                        */
  real_T tau_ww_tableData[20];         /* Expression: reshape([.4,.3,.35,.3,.2,.22,.22,.4,.35,.5,.20,.22,.5,.4,.35,.35,.3,.45,.5,.4],5,4)
                                        * Referenced by: '<S7>/tau_ww'
                                        */
  real_T tau_ww_bp01Data[5];           /* Expression: [1000,1500,2000,2500,3000]
                                        * Referenced by: '<S7>/tau_ww'
                                        */
  real_T tau_ww_bp02Data[4];           /* Expression: [0.1,0.2,0.3,0.4]
                                        * Referenced by: '<S7>/tau_ww'
                                        */
  real_T tau_wwtolerance0911_Gain;     /* Expression: 1
                                        * Referenced by: '<S7>/tau_ww tolerance [0.9 1.1]'
                                        */
  real_T fuelsystemtransportdelay_MaxDel;/* Expression: 10
                                          * Referenced by: '<S4>/fuel system transport delay'
                                          */
  real_T fuelsystemtransportdelay_InitOu;/* Expression: 14.7
                                          * Referenced by: '<S4>/fuel system transport delay'
                                          */
  real_T Gain1_Gain;                   /* Expression: 10
                                        * Referenced by: '<S19>/Gain1'
                                        */
  real_T radstorpm_Gain_p;             /* Expression: 60/(2*pi)
                                        * Referenced by: '<S4>/rad//s to rpm'
                                        */
  real_T delays_tableData[20];         /* Expression: reshape([0.8,0.6,0.4,0.3,0.2,0.4,0.3,0.2,0.2,0.2,0.3,0.25,0.2,0.2,0.2,0.25,0.2,0.2,0.2,0.2],5,4)
                                        * Referenced by: '<S4>/delay (s)'
                                        */
  real_T delays_bp01Data[5];           /* Expression: [800,1000,1500,2000,3000]
                                        * Referenced by: '<S4>/delay (s)'
                                        */
  real_T delays_bp02Data[4];           /* Expression: [0.05,0.15,0.2,0.25]
                                        * Referenced by: '<S4>/delay (s)'
                                        */
  real_T RTVm_Gain;                    /* Expression: 0.41328
                                        * Referenced by: '<S5>/RT//Vm'
                                        */
  real_T Gain_Gain_m;                  /* Expression: -1
                                        * Referenced by: '<S7>/Gain'
                                        */
  real_T Constant_Value;               /* Expression: 1
                                        * Referenced by: '<S7>/Constant'
                                        */
  real_T Constant_Value_i;             /* Expression: 1
                                        * Referenced by: '<S23>/Constant'
                                        */
  real_T Integrator_IC_b;              /* Expression: 0
                                        * Referenced by: '<S22>/Integrator'
                                        */
  real_T MeasureOn_Time;               /* Expression: 10
                                        * Referenced by: '<S22>/MeasureOn'
                                        */
  real_T MeasureOn_Y0;                 /* Expression: 0
                                        * Referenced by: '<S22>/MeasureOn'
                                        */
  real_T MeasureOn_YFinal;             /* Expression: 1
                                        * Referenced by: '<S22>/MeasureOn'
                                        */
  real_T MeasureOn_Y0_m;               /* Expression: 0
                                        * Referenced by: '<S2>/MeasureOn'
                                        */
  real_T MeasureOn_YFinal_n;           /* Expression: 1
                                        * Referenced by: '<S2>/MeasureOn'
                                        */
  real_T Constant_Value_l;             /* Expression: 0.5
                                        * Referenced by: '<S2>/Constant'
                                        */
  real_T PedalAngledeg_Amp;            /* Expression: 30.9433
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
  real32_T Constant2_Value;            /* Computed Parameter: Constant2_Value
                                        * Referenced by: '<S9>/Constant2'
                                        */
  real32_T fb_fuel_saturation_UpperSat;/* Computed Parameter: fb_fuel_saturation_UpperSat
                                        * Referenced by: '<S9>/fb_fuel_saturation'
                                        */
  real32_T fb_fuel_saturation_LowerSat;/* Computed Parameter: fb_fuel_saturation_LowerSat
                                        * Referenced by: '<S9>/fb_fuel_saturation'
                                        */
  real32_T Constant3_Value;            /* Computed Parameter: Constant3_Value
                                        * Referenced by: '<S9>/Constant3'
                                        */
  real32_T Constant1_Value;            /* Computed Parameter: Constant1_Value
                                        * Referenced by: '<S12>/Constant1'
                                        */
  real32_T Constant2_Value_d;          /* Computed Parameter: Constant2_Value_d
                                        * Referenced by: '<S12>/Constant2'
                                        */
  real32_T Constant3_Value_c;          /* Computed Parameter: Constant3_Value_c
                                        * Referenced by: '<S12>/Constant3'
                                        */
  real32_T Constant4_Value;            /* Computed Parameter: Constant4_Value
                                        * Referenced by: '<S12>/Constant4'
                                        */
  real32_T Constant5_Value;            /* Computed Parameter: Constant5_Value
                                        * Referenced by: '<S12>/Constant5'
                                        */
  real32_T UnitDelay1_InitialCondition;/* Computed Parameter: UnitDelay1_InitialCondition
                                        * Referenced by: '<S12>/UnitDelay1'
                                        */
  real32_T Gain_Gain_j;                /* Computed Parameter: Gain_Gain_j
                                        * Referenced by: '<S12>/Gain'
                                        */
  real32_T Constant1_Value_h;          /* Computed Parameter: Constant1_Value_h
                                        * Referenced by: '<S13>/Constant1'
                                        */
  real32_T Gain_Gain_c;                /* Computed Parameter: Gain_Gain_c
                                        * Referenced by: '<S13>/Gain'
                                        */
  real32_T Gain1_Gain_a;               /* Computed Parameter: Gain1_Gain_a
                                        * Referenced by: '<S13>/Gain1'
                                        */
  real32_T UnitDelay1_InitialCondition_l;/* Computed Parameter: UnitDelay1_InitialCondition_l
                                          * Referenced by: '<S13>/UnitDelay1'
                                          */
  real32_T fuel_saturation_UpperSat;   /* Computed Parameter: fuel_saturation_UpperSat
                                        * Referenced by: '<S9>/fuel_saturation'
                                        */
  real32_T fuel_saturation_LowerSat;   /* Computed Parameter: fuel_saturation_LowerSat
                                        * Referenced by: '<S9>/fuel_saturation'
                                        */
  real32_T airbyfuel_reference_Value;  /* Computed Parameter: airbyfuel_reference_Value
                                        * Referenced by: '<S10>/airbyfuel_reference'
                                        */
  real32_T airbyfuel_reference_power_Value;/* Computed Parameter: airbyfuel_reference_power_Value
                                            * Referenced by: '<S10>/airbyfuel_reference_power'
                                            */
  real32_T UnitDelay2_InitialCondition;/* Computed Parameter: UnitDelay2_InitialCondition
                                        * Referenced by: '<S15>/Unit Delay2'
                                        */
  real32_T sampling_sec_Value;         /* Computed Parameter: sampling_sec_Value
                                        * Referenced by: '<S15>/sampling_sec'
                                        */
  real32_T normal_mode_start_sec_Value;/* Computed Parameter: normal_mode_start_sec_Value
                                        * Referenced by: '<S15>/normal_mode_start_sec'
                                        */
  real32_T Constant1_Value_f;          /* Computed Parameter: Constant1_Value_f
                                        * Referenced by: '<S16>/Constant1'
                                        */
  real32_T Constant_Value_d;           /* Computed Parameter: Constant_Value_d
                                        * Referenced by: '<S16>/Constant'
                                        */
  real32_T threshold_Value;            /* Computed Parameter: threshold_Value
                                        * Referenced by: '<S17>/threshold'
                                        */
  real32_T Constant1_Value_l;          /* Computed Parameter: Constant1_Value_l
                                        * Referenced by: '<S11>/Constant1'
                                        */
  real32_T Constant2_Value_k;          /* Computed Parameter: Constant2_Value_k
                                        * Referenced by: '<S11>/Constant2'
                                        */
  real32_T Constant3_Value_o;          /* Computed Parameter: Constant3_Value_o
                                        * Referenced by: '<S11>/Constant3'
                                        */
  real32_T DataStoreMemory_InitialValue;/* Computed Parameter: DataStoreMemory_InitialValue
                                         * Referenced by: '<S8>/DataStoreMemory'
                                         */
  real32_T DataStoreMemory1_InitialValue;/* Computed Parameter: DataStoreMemory1_InitialValue
                                          * Referenced by: '<S8>/DataStoreMemory1'
                                          */
  real32_T DataStoreMemory2_InitialValue;/* Computed Parameter: DataStoreMemory2_InitialValue
                                          * Referenced by: '<S8>/DataStoreMemory2'
                                          */
  real32_T DataStoreMemory3_InitialValue;/* Computed Parameter: DataStoreMemory3_InitialValue
                                          * Referenced by: '<S8>/DataStoreMemory3'
                                          */
  real32_T commanded_fuel_InitialValue;/* Computed Parameter: commanded_fuel_InitialValue
                                        * Referenced by: '<S3>/commanded_fuel'
                                        */
  real32_T mode_fb1_InitialValue;      /* Computed Parameter: mode_fb1_InitialValue
                                        * Referenced by: '<S3>/mode_fb1'
                                        */
  real32_T Gain2_Gain;                 /* Computed Parameter: Gain2_Gain
                                        * Referenced by: '<S5>/Gain2'
                                        */
  uint32_T uKappa_maxIndex[2];         /* Computed Parameter: uKappa_maxIndex
                                        * Referenced by: '<S7>/1-Kappa'
                                        */
  uint32_T tau_ww_maxIndex[2];         /* Computed Parameter: tau_ww_maxIndex
                                        * Referenced by: '<S7>/tau_ww'
                                        */
  uint32_T delays_maxIndex[2];         /* Computed Parameter: delays_maxIndex
                                        * Referenced by: '<S4>/delay (s)'
                                        */
  boolean_T UnitDelay1_InitialCondition_c;/* Computed Parameter: UnitDelay1_InitialCondition_c
                                           * Referenced by: '<S15>/Unit Delay1'
                                           */
  boolean_T UnitDelay1_InitialCondition_f;/* Computed Parameter: UnitDelay1_InitialCondition_f
                                           * Referenced by: '<S16>/Unit Delay1'
                                           */
  boolean_T UnitDelay_InitialCondition;/* Computed Parameter: UnitDelay_InitialCondition
                                        * Referenced by: '<S17>/Unit Delay'
                                        */
  boolean_T mode_fb_InitialValue;      /* Computed Parameter: mode_fb_InitialValue
                                        * Referenced by: '<S3>/mode_fb'
                                        */
};

/* Real-time Model Data Structure */
struct tag_RTM_AbstractFuelControl_M_T {
  const char_T *path;
  const char_T *modelName;
  struct SimStruct_tag * *childSfunctions;
  const char_T *errorStatus;
  SS_SimMode simMode;
  RTWLogInfo *rtwLogInfo;
  RTWExtModeInfo *extModeInfo;
  RTWSolverInfo solverInfo;
  RTWSolverInfo *solverInfoPtr;
  void *sfcnInfo;

  /*
   * ModelData:
   * The following substructure contains information regarding
   * the data used in the model.
   */
  struct {
    void *blockIO;
    const void *constBlockIO;
    void *defaultParam;
    ZCSigState *prevZCSigState;
    real_T *contStates;
    int_T *periodicContStateIndices;
    real_T *periodicContStateRanges;
    real_T *derivs;
    void *zcSignalValues;
    void *inputs;
    void *outputs;
    boolean_T *contStateDisabled;
    boolean_T zCCacheNeedsReset;
    boolean_T derivCacheNeedsReset;
    boolean_T blkStateChange;
    real_T odeY[7];
    real_T odeF[3][7];
    ODE3_IntgData intgData;
    void *dwork;
  } ModelData;

  /*
   * Sizes:
   * The following substructure contains sizes information
   * for many of the model attributes such as inputs, outputs,
   * dwork, sample times, etc.
   */
  struct {
    uint32_T checksums[4];
    uint32_T options;
    int_T numContStates;
    int_T numPeriodicContStates;
    int_T numU;
    int_T numY;
    int_T numSampTimes;
    int_T numBlocks;
    int_T numBlockIO;
    int_T numBlockPrms;
    int_T numDwork;
    int_T numSFcnPrms;
    int_T numSFcns;
    int_T numIports;
    int_T numOports;
    int_T numNonSampZCs;
    int_T sysDirFeedThru;
    int_T rtwGenSfcn;
  } Sizes;

  /*
   * SpecialInfo:
   * The following substructure contains special information
   * related to other components that are dependent on RTW.
   */
  struct {
    const void *mappingInfo;
    void *xpcData;
  } SpecialInfo;

  /*
   * Timing:
   * The following substructure contains information regarding
   * the timing information for the model.
   */
  struct {
    time_T stepSize;
    uint32_T clockTick0;
    uint32_T clockTickH0;
    time_T stepSize0;
    uint32_T clockTick1;
    uint32_T clockTickH1;
    time_T stepSize1;
    time_T tStart;
    time_T tFinal;
    time_T timeOfLastOutput;
    void *timingData;
    real_T *varNextHitTimesList;
    SimTimeStep simTimeStep;
    boolean_T stopRequestedFlag;
    time_T *sampleTimes;
    time_T *offsetTimes;
    int_T *sampleTimeTaskIDPtr;
    int_T *sampleHits;
    int_T *perTaskSampleHits;
    time_T *t;
    time_T sampleTimesArray[2];
    time_T offsetTimesArray[2];
    int_T sampleTimeTaskIDArray[2];
    int_T sampleHitArray[2];
    int_T perTaskSampleHitsArray[4];
    time_T tArray[2];
  } Timing;
};

/* Block parameters (auto storage) */
extern P_AbstractFuelControl_M1_T AbstractFuelControl_M1_P;

/* Block signals (auto storage) */
extern B_AbstractFuelControl_M1_T AbstractFuelControl_M1_B;

/* Continuous states (auto storage) */
extern X_AbstractFuelControl_M1_T AbstractFuelControl_M1_X;

/* Block states (auto storage) */
extern DW_AbstractFuelControl_M1_T AbstractFuelControl_M1_DW;

/* External inputs (root inport signals with auto storage) */
extern ExtU_AbstractFuelControl_M1_T AbstractFuelControl_M1_U;

/* External outputs (root outports fed by signals with auto storage) */
extern ExtY_AbstractFuelControl_M1_T AbstractFuelControl_M1_Y;

/* Model entry point functions */
extern void AbstractFuelControl_M1_initialize(void);
extern void AbstractFuelControl_M1_output(void);
extern void AbstractFuelControl_M1_update(void);
extern void AbstractFuelControl_M1_terminate(void);

/*====================*
 * External functions *
 *====================*/
extern AbstractFuelControl_M1_rtModel *AbstractFuelControl_M1(void);
extern void MdlInitializeSizes(void);
extern void MdlInitializeSampleTimes(void);
extern void MdlInitialize(void);
extern void MdlStart(void);
extern void MdlOutputs(int_T tid);
extern void MdlUpdate(int_T tid);
extern void MdlTerminate(void);

/* Real-time Model object */
extern RT_MODEL_AbstractFuelControl__T *const AbstractFuelControl_M1_M;

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
 * '<Root>' : 'AbstractFuelControl_M1'
 * '<S1>'   : 'AbstractFuelControl_M1/Model 1'
 * '<S2>'   : 'AbstractFuelControl_M1/V&V stub system'
 * '<S3>'   : 'AbstractFuelControl_M1/Model 1/AF_Controller'
 * '<S4>'   : 'AbstractFuelControl_M1/Model 1/Cylinder and Exhaust'
 * '<S5>'   : 'AbstractFuelControl_M1/Model 1/Intake Manifold'
 * '<S6>'   : 'AbstractFuelControl_M1/Model 1/Throttle'
 * '<S7>'   : 'AbstractFuelControl_M1/Model 1/Wall wetting'
 * '<S8>'   : 'AbstractFuelControl_M1/Model 1/AF_Controller/fuel_controller'
 * '<S9>'   : 'AbstractFuelControl_M1/Model 1/AF_Controller/fuel_controller/fuel_controller_10ms'
 * '<S10>'  : 'AbstractFuelControl_M1/Model 1/AF_Controller/fuel_controller/fuel_controller_mode_10ms'
 * '<S11>'  : 'AbstractFuelControl_M1/Model 1/AF_Controller/fuel_controller/fuel_controller_pwon'
 * '<S12>'  : 'AbstractFuelControl_M1/Model 1/AF_Controller/fuel_controller/fuel_controller_10ms/air_estimation'
 * '<S13>'  : 'AbstractFuelControl_M1/Model 1/AF_Controller/fuel_controller/fuel_controller_10ms/feedback_PI_controller'
 * '<S14>'  : 'AbstractFuelControl_M1/Model 1/AF_Controller/fuel_controller/fuel_controller_10ms/feedforward_controller'
 * '<S15>'  : 'AbstractFuelControl_M1/Model 1/AF_Controller/fuel_controller/fuel_controller_mode_10ms/normal_mode_detection'
 * '<S16>'  : 'AbstractFuelControl_M1/Model 1/AF_Controller/fuel_controller/fuel_controller_mode_10ms/power_mode_detection'
 * '<S17>'  : 'AbstractFuelControl_M1/Model 1/AF_Controller/fuel_controller/fuel_controller_mode_10ms/sensor_failure_detection'
 * '<S18>'  : 'AbstractFuelControl_M1/Model 1/Cylinder and Exhaust/A//F_sensor'
 * '<S19>'  : 'AbstractFuelControl_M1/Model 1/Cylinder and Exhaust/Filter'
 * '<S20>'  : 'AbstractFuelControl_M1/Model 1/Cylinder and Exhaust/A//F_sensor/Filter'
 * '<S21>'  : 'AbstractFuelControl_M1/V&V stub system/Calcuate Error'
 * '<S22>'  : 'AbstractFuelControl_M1/V&V stub system/Calcuate Error/RMS error'
 * '<S23>'  : 'AbstractFuelControl_M1/V&V stub system/Calcuate Error/over(under)shoot'
 */
#endif                                 /* RTW_HEADER_AbstractFuelControl_M1_h_ */
