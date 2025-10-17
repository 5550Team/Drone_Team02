#include <stddef.h>
#include <stdlib.h>
#include <math.h>

#include "log.h"
#include "param.h"
#include "trigger.h"
#include "sitaw.h"
#include "commander.h"
#include "stabilizer.h"
#include "motors.h"

/* Trigger object used to detect Free Fall situation. */
static trigger_t sitAwFFAccWZ;

/* Trigger object used to detect At Rest situation. */
static trigger_t sitAwARAccZ;

/* Trigger object used to detect Tumbled situation. */
static trigger_t sitAwTuAcc;

#if defined(SITAW_ENABLED)

#if defined(SITAW_LOG_ENABLED) /* Enable the log group. */
LOG_GROUP_START(sitAw)
#if defined(SITAW_FF_LOG_ENABLED) /* Log trigger variables for Free Fall detection. */
LOG_ADD(LOG_UINT32, FFAccWZTestCounter, &sitAwFFAccWZ.testCounter)
LOG_ADD(LOG_UINT8, FFAccWZDetected, &sitAwFFAccWZ.released)
#endif
#if defined(SITAW_AR_LOG_ENABLED) /* Log trigger variables for At Rest detection. */
LOG_ADD(LOG_UINT32, ARTestCounter, &sitAwARAccZ.testCounter)
LOG_ADD(LOG_UINT8, ARDetected, &sitAwARAccZ.released)
#endif
#if defined(SITAW_TU_LOG_ENABLED) /* Log trigger variables for Tumbled detection. */
LOG_ADD(LOG_UINT32, TuTestCounter, &sitAwTuAcc.testCounter)
LOG_ADD(LOG_UINT8, TuDetected, &sitAwTuAcc.released)
#endif
#if defined(SITAW_LOG_ALL_DETECT_ENABLED) /* Log all the 'Detected' flags. */
LOG_ADD(LOG_UINT8, FFAccWZDetected, &sitAwFFAccWZ.released)
LOG_ADD(LOG_UINT8, ARDetected, &sitAwARAccZ.released)
LOG_ADD(LOG_UINT8, TuDetected, &sitAwTuAcc.released)
#endif
LOG_GROUP_STOP(sitAw)
#endif /* SITAW_LOG_ENABLED */

#if defined(SITAW_PARAM_ENABLED) /* Enable the param group. */
PARAM_GROUP_START(sitAw)
#if defined(SITAW_FF_PARAM_ENABLED) /* Param variables for Free Fall detection. */
PARAM_ADD(PARAM_UINT8, FFActive, &sitAwFFAccWZ.active)
PARAM_ADD(PARAM_UINT32, FFTriggerCount, &sitAwFFAccWZ.triggerCount)
PARAM_ADD(PARAM_FLOAT, FFaccWZ, &sitAwFFAccWZ.threshold)
#endif
#if defined(SITAW_AR_PARAM_ENABLED) /* Param variables for At Rest detection. */
PARAM_ADD(PARAM_UINT8, ARActive, &sitAwARAccZ.active)
PARAM_ADD(PARAM_UINT32, ARTriggerCount, &sitAwARAccZ.triggerCount)
PARAM_ADD(PARAM_FLOAT, ARaccZ, &sitAwARAccZ.threshold)
#endif
#if defined(SITAW_TU_PARAM_ENABLED) /* Param variables for Tumbled detection. */
PARAM_ADD(PARAM_UINT8, TuActive, &sitAwTuAcc.active)
PARAM_ADD(PARAM_UINT32, TuTriggerCount, &sitAwTuAcc.triggerCount)
PARAM_ADD(PARAM_FLOAT, TuAcc, &sitAwTuAcc.threshold)
#endif
PARAM_GROUP_STOP(sitAw)
#endif /* SITAW_PARAM_ENABLED */

#endif /* SITAW_ENABLED */

// forward declaration of private functions
#ifdef SITAW_FF_ENABLED
static void sitAwFFTest(float accWZ, float accMag);
#endif
#ifdef SITAW_AR_ENABLED
static void sitAwARTest(float accX, float accY, float accZ);
#endif
#ifdef SITAW_TU_ENABLED
static void sitAwTuTest(float accz);
#endif

static void sitAwPostStateUpdateCallOut(const sensorData_t *sensorData,
                                        const state_t *state)
{
  /* Code that shall run AFTER each attitude update, should be placed here. */

#if defined(SITAW_ENABLED)
#ifdef SITAW_FF_ENABLED
  float accMAG = (sensorData->acc.x*sensorData->acc.x) +
                 (sensorData->acc.y*sensorData->acc.y) +
                 (sensorData->acc.z*sensorData->acc.z);

  /* Test values for Free Fall detection. */
  sitAwFFTest(state->acc.z, accMAG);
#endif
#ifdef SITAW_TU_ENABLED
  /* check if we actually fly */
  int sumRatio = 0;
  for (int i = 0; i < NBR_OF_MOTORS; ++i) {
    sumRatio += motorsGetRatio(i);
  }
  bool isFlying = sumRatio > SITAW_TU_IN_FLIGHT_THRESHOLD;
  if (isFlying) {
    /* Test values for Tumbled detection. */
    sitAwTuTest(sensorData->acc.z);
  }
#endif
#ifdef SITAW_AR_ENABLED
/* Test values for At Rest detection. */
  sitAwARTest(sensorData->acc.x, sensorData->acc.y, sensorData->acc.z);
#endif
#endif
}

static void sitAwPreThrustUpdateCallOut(setpoint_t *setpoint)
{
  /* Code that shall run BEFORE each thrust distribution update, should be placed here. */

#if defined(SITAW_ENABLED)
#ifdef SITAW_TU_ENABLED
      if(sitAwTuDetected()) {
        /* Kill the thrust to the motors if a Tumbled situation is detected. */
        stabilizerSetEmergencyStop();
      }
#endif

#ifdef SITAW_FF_ENABLED
      /* Force altHold mode if free fall is detected.
         FIXME: Needs a flying/landing state (as soon as althold is enabled,
                                              we are not freefalling anymore)
       */
      if(sitAwFFDetected() && !sitAwTuDetected()) {
        setpoint->mode.z = modeVelocity;
        setpoint->velocity.z = 0;
      }
#endif
#endif
}

void sitAwUpdateSetpoint(setpoint_t *setpoint, const sensorData_t *sensorData,
                                               const state_t *state)
{
  sitAwPostStateUpdateCallOut(sensorData, state);
  sitAwPreThrustUpdateCallOut(setpoint);
}

#ifdef SITAW_FF_ENABLED

void sitAwFFInit(void)
{
  triggerInit(&sitAwFFAccWZ, triggerFuncIsLE, SITAW_FF_THRESHOLD, SITAW_FF_TRIGGER_COUNT);
  triggerActivate(&sitAwFFAccWZ, true);
}


void sitAwFFTest(float accWZ, float accMAG)
{
  /* Check that the total acceleration is close to zero. */
  if(fabs(accMAG) > SITAW_FF_THRESHOLD) {
    /* If the total acceleration deviates from 0, this is not a free fall situation. */
    triggerReset(&sitAwFFAccWZ);
  } else {

   
    triggerTestValue(&sitAwFFAccWZ, fabs(accWZ + 1));
  }
}


bool sitAwFFDetected(void)
{
  return sitAwFFAccWZ.released;
}
#endif

#ifdef SITAW_AR_ENABLED

void sitAwARInit(void)
{
  triggerInit(&sitAwARAccZ, triggerFuncIsLE, SITAW_AR_THRESHOLD, SITAW_AR_TRIGGER_COUNT);
  triggerActivate(&sitAwARAccZ, true);
}


void sitAwARTest(float accX, float accY, float accZ)
{
  /* Check that there are no horizontal accelerations. At rest, these are 0. */
  if((fabs(accX) > SITAW_AR_THRESHOLD) || (fabs(accY) > SITAW_AR_THRESHOLD)) {
    /* If the X or Y accelerations are different than 0, the crazyflie is not at rest. */
    triggerReset(&sitAwARAccZ);
  }


  triggerTestValue(&sitAwARAccZ, fabs(accZ - 1));
}


bool sitAwARDetected(void)
{
  return sitAwARAccZ.released;
}
#endif

#ifdef SITAW_TU_ENABLED

void sitAwTuInit(void)
{
  triggerInit(&sitAwTuAcc, triggerFuncIsLE, SITAW_TU_ACC_THRESHOLD, SITAW_TU_ACC_TRIGGER_COUNT);
  triggerActivate(&sitAwTuAcc, true);
}


void sitAwTuTest(float accz)
{
  triggerTestValue(&sitAwTuAcc, accz);
}

bool sitAwTuDetected(void)
{
  return sitAwTuAcc.released;
}
#endif


void sitAwInit(void)
{
#ifdef SITAW_FF_ENABLED
  sitAwFFInit();
#endif
#ifdef SITAW_AR_ENABLED
  sitAwARInit();
#endif
#ifdef SITAW_TU_ENABLED
  sitAwTuInit();
#endif
}
