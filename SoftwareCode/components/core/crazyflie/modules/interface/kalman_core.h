#ifndef __KALMAN_CORE_H__
#define __KALMAN_CORE_H__

#include "cf_math.h"
#include "stabilizer_types.h"

// Indexes to access the quad's state, stored as a column vector
typedef enum
{
  KC_STATE_X, KC_STATE_Y, KC_STATE_Z, KC_STATE_PX, KC_STATE_PY, KC_STATE_PZ, KC_STATE_D0, KC_STATE_D1, KC_STATE_D2, KC_STATE_DIM
} kalmanCoreStateIdx_t;


// The data used by the kalman core implementation.
typedef struct {

  float S[KC_STATE_DIM];

  // The quad's attitude as a quaternion (w,x,y,z)
  // We store as a quaternion to allow easy normalization (in comparison to a rotation matrix),
  // while also being robust against singularities (in comparison to euler angles)
  float q[4];

  // The quad's attitude as a rotation matrix (used by the prediction, updated by the finalization)
  float R[3][3];

  // The covariance matrix
  __attribute__((aligned(4))) float P[KC_STATE_DIM][KC_STATE_DIM];
  xtensa_matrix_instance_f32 Pm;

  // Indicates that the internal state is corrupt and should be reset
  bool resetEstimation;

  float baroReferenceHeight;
} kalmanCoreData_t;


void kalmanCoreInit(kalmanCoreData_t* this);

/*  - Measurement updates based on sensors */

// Barometer
void kalmanCoreUpdateWithBaro(kalmanCoreData_t* this, float baroAsl, bool quadIsFlying);

// Absolute height measurement along the room Z
void kalmanCoreUpdateWithAbsoluteHeight(kalmanCoreData_t* this, heightMeasurement_t* height);

// Direct measurements of Crazyflie position
void kalmanCoreUpdateWithPosition(kalmanCoreData_t* this, positionMeasurement_t *xyz);

// Direct measurements of Crazyflie pose
void kalmanCoreUpdateWithPose(kalmanCoreData_t* this, poseMeasurement_t *pose);

// Distance-to-point measurements
void kalmanCoreUpdateWithDistance(kalmanCoreData_t* this, distanceMeasurement_t *d);

// Measurements of a UWB Tx/Rx
void kalmanCoreUpdateWithTDOA(kalmanCoreData_t* this, tdoaMeasurement_t *tdoa);

// Measurements of flow (dnx, dny)
void kalmanCoreUpdateWithFlow(kalmanCoreData_t* this, const flowMeasurement_t *flow, const Axis3f *gyro);

// Measurements of TOF from laser sensor
void kalmanCoreUpdateWithTof(kalmanCoreData_t* this, tofMeasurement_t *tof);

// Measurement of yaw error (outside measurement Vs current estimation)
void kalmanCoreUpdateWithYawError(kalmanCoreData_t *this, yawErrorMeasurement_t *error);

// Measurement of sweep angles from a Lighthouse base station
//void kalmanCoreUpdateWithSweepAngles(kalmanCoreData_t *this, sweepAngleMeasurement_t *angles, const uint32_t tick);

void kalmanCorePredict(kalmanCoreData_t* this, float thrust, Axis3f *acc, Axis3f *gyro, float dt, bool quadIsFlying);

void kalmanCoreAddProcessNoise(kalmanCoreData_t* this, float dt);

/*  - Finalization to incorporate attitude error into body attitude */
void kalmanCoreFinalize(kalmanCoreData_t* this, uint32_t tick);

/*  - Externalization to move the filter's internal state into the external state expected by other modules */
void kalmanCoreExternalizeState(const kalmanCoreData_t* this, state_t *state, const Axis3f *acc, uint32_t tick);

void kalmanCoreDecoupleXY(kalmanCoreData_t* this);

#endif // __KALMAN_CORE_H__
