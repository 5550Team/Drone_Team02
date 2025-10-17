#ifndef __COLLISION_AVOIDANCE_H__
#define __COLLISION_AVOIDANCE_H__


#include "math3d.h"
#include "stabilizer_types.h"




typedef struct collision_avoidance_params_s
{

  struct vec ellipsoidRadii;

  struct vec bboxMin;
  struct vec bboxMax;

  float horizonSecs;

  float maxSpeed;

  float sidestepThreshold;

  int maxPeerLocAgeMillis;

  float voronoiProjectionTolerance;

  int voronoiProjectionMaxIters;

} collision_avoidance_params_t;



typedef struct collision_avoidance_state_s
{

  struct vec lastFeasibleSetPosition;

} collision_avoidance_state_t;


void collisionAvoidanceUpdateSetpointCore(
  collision_avoidance_params_t const *params,
  collision_avoidance_state_t *collisionState,
  int nOthers,
  float const *otherPositions,
  float *workspace,
  setpoint_t *setpoint, sensorData_t const *sensorData, state_t const *state);


#ifdef CRAZYFLIE_FW

void collisionAvoidanceInit(void);
bool collisionAvoidanceTest(void);


void collisionAvoidanceUpdateSetpoint(
  setpoint_t *setpoint, sensorData_t const *sensorData, state_t const *state, uint32_t tick);

#endif 

#endif 
