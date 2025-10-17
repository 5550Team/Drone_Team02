#include "kalman_supervisor.h"

#include "param.h"

// The bounds on states, these shouldn't be hit...
float maxPosition = 100; //meters
float maxVelocity = 10; //meters per second

bool kalmanSupervisorIsStateWithinBounds(const kalmanCoreData_t* this) {
  for (int i = 0; i < 3; i++) {
    if (maxPosition > 0.0f) {
      if (this->S[KC_STATE_X + i] > maxPosition) {
        return false;
      } else if (this->S[KC_STATE_X + i] < -maxPosition) {
        return false;
      }
    }

    if (maxVelocity > 0.0f) {
      if (this->S[KC_STATE_PX + i] > maxVelocity) {
        return false;
      } else if (this->S[KC_STATE_PX + i] < -maxVelocity) {
        return false;
      }
    }
  }

  return true;
}

PARAM_GROUP_START(kalman)
  PARAM_ADD(PARAM_FLOAT, maxPos, &maxPosition)
  PARAM_ADD(PARAM_FLOAT, maxVel, &maxVelocity)
PARAM_GROUP_STOP(kalman)
