#ifndef __SITAW_H__
#define __SITAW_H__

#include <stdint.h>
#include <stdbool.h>

#include "stabilizer_types.h"

void sitAwInit(void);
void sitAwUpdateSetpoint(setpoint_t *setpoint, const sensorData_t *sensorData,
                                               const state_t *state);
/* Enable the situation awareness framework. */
#define SITAW_ENABLED

#define SITAW_TU_ENABLED           /* Uncomment to enable */

/* Configuration options for the 'Free Fall' detection. */
#define SITAW_FF_THRESHOLD 0.1     /* The default tolerance for AccWZ deviations from -1, indicating Free Fall. */
#define SITAW_FF_TRIGGER_COUNT 15  /* The number of consecutive tests for Free Fall to be detected. Configured for 250Hz testing. */

/* Configuration options for the 'At Rest' detection. */
#define SITAW_AR_THRESHOLD 0.05    /* The default tolerance for AccZ deviations from 1 and AccX, AccY deviations from 0, indicating At Rest. */
#define SITAW_AR_TRIGGER_COUNT 500 /* The number of consecutive tests for At Rest to be detected. Configured for 250Hz testing. */

/* Configuration options for the 'Tumbled' detection. */
#define SITAW_TU_ACC_THRESHOLD (-0.5f)     /* The maximum acc.z value indicating a Tumbled situation. */
#define SITAW_TU_ACC_TRIGGER_COUNT 30      /* The number of consecutive tests for Tumbled to be detected. Configured for 250Hz testing. */
#define SITAW_TU_IN_FLIGHT_THRESHOLD 1000  /* Minimum summed motor PWM that means we are flying */

/* LOG configurations. Enable these to be able to log detection in the cfclient. */
#define SITAW_LOG_ENABLED            /* Uncomment to enable LOG framework. */

#define SITAW_LOG_ALL_DETECT_ENABLED /* Uncomment to enable LOG framework for all 'Detected' flags. */

#ifdef SITAW_FF_ENABLED
bool sitAwFFDetected(void);
#endif
#ifdef SITAW_AR_ENABLED
bool sitAwARDetected(void);
#endif
#ifdef SITAW_TU_ENABLED
bool sitAwTuDetected(void);
#endif

#endif
