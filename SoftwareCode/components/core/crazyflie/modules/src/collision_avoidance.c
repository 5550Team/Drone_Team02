
#include <float.h>
#include <string.h>  // for memset

#include "collision_avoidance.h"
#include "peer_localization.h"


static struct vec vec2svec(struct vec3_s v)
{
  return mkvec(v.x, v.y, v.z);
}
static struct vec3_s svec2vec(struct vec v)
{
  struct vec3_s vv = { .x = v.x, .y = v.y, .z = v.z, };
  return vv;
}

static struct vec sidestepGoal(
  collision_avoidance_params_t const *params,
  struct vec goal,
  bool modifyIfInside,
  float const A[], float const B[], float projectionWorkspace[], int nRows)
{
  float const rayScale = rayintersectpolytope(vzero(), goal, A, B, nRows, NULL);
  if (rayScale >= 1.0f && !modifyIfInside) {
    return goal;
  }
  float const distance = vmag(goal);
  float const distFromWall = rayScale * distance;
  if (distFromWall <= params->sidestepThreshold) {
    struct vec sidestepDir = vcross(goal, mkvec(0.0f, 0.0f, 1.0f));
    float const sidestepAmount = fsqr(1.0f - distFromWall / params->sidestepThreshold);
    goal = vadd(goal, vscl(sidestepAmount, sidestepDir));
  }
  // Otherwise no sidestep, but still project
  return vprojectpolytope(
    goal,
    A, B, projectionWorkspace, nRows,
    params->voronoiProjectionTolerance,
    params->voronoiProjectionMaxIters
  );
}

void collisionAvoidanceUpdateSetpointCore(
  collision_avoidance_params_t const *params,
  collision_avoidance_state_t *collisionState,
  int nOthers,
  float const *otherPositions,
  float *workspace,
  setpoint_t *setpoint, sensorData_t const *sensorData, state_t const *state)
{
  //
  // Part 1: Construct the polytope inequalities in A, b.
  //

  int const nRows = nOthers + 6;
  float *A = workspace;
  float *B = workspace + 3 * nRows;
  float *projectionWorkspace = workspace + 4 * nRows;


  struct vec const radiiInv = veltrecip(params->ellipsoidRadii);
  struct vec const ourPos = vec2svec(state->position);

  for (int i = 0; i < nOthers; ++i) {
    struct vec peerPos = vloadf(otherPositions + 3 * i);
    struct vec const toPeerStretched = veltmul(vsub(peerPos, ourPos), radiiInv);
    float const dist = vmag(toPeerStretched);
    struct vec const a = vdiv(veltmul(toPeerStretched, radiiInv), dist);
    float const b = dist / 2.0f - 1.0f;
    float scale = 1.0f / vmag(a);
    vstoref(vscl(scale, a), A + 3 * i);
    B[i] = scale * b;
  }


  float const maxDist = params->horizonSecs * params->maxSpeed;

  memset(A + 3 * nOthers, 0, 18 * sizeof(float));

  for (int dim = 0; dim < 3; ++dim) {
    float boxMax = vindex(params->bboxMax, dim) - vindex(ourPos, dim);
    A[3 * (nOthers + dim) + dim] = 1.0f;
    B[nOthers + dim] = fminf(maxDist, boxMax);

    float boxMin = vindex(params->bboxMin, dim) - vindex(ourPos, dim);
    A[3 * (nOthers + dim + 3) + dim] = -1.0f;
    B[nOthers + dim + 3] = -fmaxf(-maxDist, boxMin);
  }


  float const inPolytopeTolerance = 10.0f * params->voronoiProjectionTolerance;

  struct vec setPos = vec2svec(setpoint->position);
  struct vec setVel = vec2svec(setpoint->velocity);

  if (setpoint->mode.x == modeVelocity) {
    // Interpret the setpoint to mean "fly with this velocity".

    if (vinpolytope(vzero(), A, B, nRows, inPolytopeTolerance)) {
      // Typical case - our current position is within our cell.
      struct vec pseudoGoal = vscl(params->horizonSecs, setVel);
      pseudoGoal = sidestepGoal(params, pseudoGoal, true, A, B, projectionWorkspace, nRows);
      if (vinpolytope(pseudoGoal, A, B, nRows, inPolytopeTolerance)) {
        setVel = vdiv(pseudoGoal, params->horizonSecs);
      }
      else {
        // Projection failed to converge. Best we can do is stay still.
        setVel = vzero();
      }
    }
    else {

      struct vec nearestInCell = vprojectpolytope(
        vzero(),
        A, B, projectionWorkspace, nRows,
        params->voronoiProjectionTolerance,
        params->voronoiProjectionMaxIters
      );
      if (vinpolytope(nearestInCell, A, B, nRows, inPolytopeTolerance)) {
        setVel = vclampnorm(nearestInCell, params->maxSpeed);
      }
      else {
        // Projection failed to converge. Best we can do is stay still.
        setVel = vzero();
      }
    }
    collisionState->lastFeasibleSetPosition = ourPos;
  }
  else if (setpoint->mode.x == modeAbs) {

    struct vec const setPosRelative = vsub(setPos, ourPos);
    struct vec const setPosRelativeNew = sidestepGoal(
      params, setPosRelative, false, A, B, projectionWorkspace, nRows);

    if (!vinpolytope(setPosRelativeNew, A, B, nRows, inPolytopeTolerance)) {

      setVel = vzero();
      if (!visnan(collisionState->lastFeasibleSetPosition)) {
        setPos = collisionState->lastFeasibleSetPosition;
      }
      else {

        setPos = ourPos;
        collisionState->lastFeasibleSetPosition = ourPos;
      }
    }
    else if (veq(setVel, vzero())) {

      setPos = vadd(ourPos, setPosRelativeNew);
    }
    else {
   
      if (vneq(setPosRelative, setPosRelativeNew)) {

        setPos = vadd(ourPos, setPosRelativeNew);
        setVel = vzero();
      }
      else {

        float const scale = rayintersectpolytope(setPosRelative, setVel, A, B, nRows, NULL);
        if (scale < 1.0f)  {
          setVel = vscl(scale, setVel);
        }
        // leave setPos unchanged.
      }
    }
    collisionState->lastFeasibleSetPosition = setPos;
  }
  else {
    // Unsupported control mode, do nothing.
  }

  setpoint->position = svec2vec(setPos);
  setpoint->velocity = svec2vec(setVel);
}


#ifdef CRAZYFLIE_FW

#include "FreeRTOS.h"
#include "task.h"

#include "param.h"
#include "log.h"


static uint8_t collisionAvoidanceEnable = 0;

static collision_avoidance_params_t params = {
  .ellipsoidRadii = { .x = 0.3, .y = 0.3, .z = 0.9 },
  .bboxMin = { .x = -FLT_MAX, .y = -FLT_MAX, .z = -FLT_MAX },
  .bboxMax = { .x = FLT_MAX, .y = FLT_MAX, .z = FLT_MAX },
  .horizonSecs = 1.0f,
  .maxSpeed = 0.5f,  // Fairly conservative.
  .sidestepThreshold = 0.25f,
  .maxPeerLocAgeMillis = 5000,  // Probably longer than desired in most applications.
  .voronoiProjectionTolerance = 1e-5,
  .voronoiProjectionMaxIters = 100,
};

static collision_avoidance_state_t collisionState = {
  .lastFeasibleSetPosition = { .x = NAN, .y = NAN, .z = NAN },
};

void collisionAvoidanceInit()
{
}

bool collisionAvoidanceTest()
{
  return true;
}


#define MAX_CELL_ROWS (PEER_LOCALIZATION_MAX_NEIGHBORS + 6)
static float workspace[7 * MAX_CELL_ROWS];

// Latency counter for logging.
static uint32_t latency = 0;

void collisionAvoidanceUpdateSetpoint(
  setpoint_t *setpoint, sensorData_t const *sensorData, state_t const *state, uint32_t tick)
{
  if (!collisionAvoidanceEnable) {
    return;
  }

  TickType_t const time = xTaskGetTickCount();
  bool doAgeFilter = params.maxPeerLocAgeMillis >= 0;

  // Counts the actual number of neighbors after we filter stale measurements.
  int nOthers = 0;

  for (int i = 0; i < PEER_LOCALIZATION_MAX_NEIGHBORS; ++i) {

    peerLocalizationOtherPosition_t const *otherPos = peerLocalizationGetPositionByIdx(i);

    if (otherPos == NULL || otherPos->id == 0) {
      continue;
    }

    if (doAgeFilter && (time - otherPos->pos.timestamp > params.maxPeerLocAgeMillis)) {
      continue;
    }

    workspace[3 * nOthers + 0] = otherPos->pos.x;
    workspace[3 * nOthers + 1] = otherPos->pos.y;
    workspace[3 * nOthers + 2] = otherPos->pos.z;
    ++nOthers;
  }

  collisionAvoidanceUpdateSetpointCore(&params, &collisionState, nOthers, workspace, workspace, setpoint, sensorData, state);

  latency = xTaskGetTickCount() - time;
}

LOG_GROUP_START(colAv)
  LOG_ADD(LOG_UINT32, latency, &latency)
LOG_GROUP_STOP(colAv)


PARAM_GROUP_START(colAv)
  PARAM_ADD(PARAM_UINT8, enable, &collisionAvoidanceEnable)

  PARAM_ADD(PARAM_FLOAT, ellipsoidX, &params.ellipsoidRadii.x)
  PARAM_ADD(PARAM_FLOAT, ellipsoidY, &params.ellipsoidRadii.y)
  PARAM_ADD(PARAM_FLOAT, ellipsoidZ, &params.ellipsoidRadii.z)

  PARAM_ADD(PARAM_FLOAT, bboxMinX, &params.bboxMin.x)
  PARAM_ADD(PARAM_FLOAT, bboxMinY, &params.bboxMin.y)
  PARAM_ADD(PARAM_FLOAT, bboxMinZ, &params.bboxMin.z)

  PARAM_ADD(PARAM_FLOAT, bboxMaxX, &params.bboxMax.x)
  PARAM_ADD(PARAM_FLOAT, bboxMaxY, &params.bboxMax.y)
  PARAM_ADD(PARAM_FLOAT, bboxMaxZ, &params.bboxMax.z)

  PARAM_ADD(PARAM_FLOAT, horizon, &params.horizonSecs)
  PARAM_ADD(PARAM_FLOAT, maxSpeed, &params.maxSpeed)
  PARAM_ADD(PARAM_FLOAT, sidestepThrsh, &params.sidestepThreshold)
  PARAM_ADD(PARAM_INT32, maxPeerLocAge, &params.maxPeerLocAgeMillis)
  PARAM_ADD(PARAM_FLOAT, vorTol, &params.voronoiProjectionTolerance)
  PARAM_ADD(PARAM_INT32, vorIters, &params.voronoiProjectionMaxIters)
PARAM_GROUP_STOP(colAv)

#endif  