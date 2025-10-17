

#ifndef CRTP_COMMANDER_HIGH_LEVEL_H_
#define CRTP_COMMANDER_HIGH_LEVEL_H_

#include <stdbool.h>
#include <stdint.h>

#include "math3d.h"

#include "stabilizer_types.h"

#define NUM_TRAJECTORY_DEFINITIONS 10

typedef enum {
  CRTP_CHL_TRAJECTORY_TYPE_POLY4D = 0, // struct poly4d, see pptraj.h
  CRTP_CHL_TRAJECTORY_TYPE_POLY4D_COMPRESSED = 1, // see pptraj_compressed.h
  // Future types might include versions without yaw
} crtpCommanderTrajectoryType_t;

/* Public functions */
void crtpCommanderHighLevelInit(void);

// Retrieves the current setpoint
void crtpCommanderHighLevelGetSetpoint(setpoint_t* setpoint, const state_t *state);


void crtpCommanderHighLevelTellState(const state_t *state);

// True if we have landed or emergency-stopped.
bool crtpCommanderHighLevelIsStopped();

// Public API - can be used from an app


int crtpCommanderHighLevelTakeoff(const float absoluteHeight_m, const float duration_s);


int crtpCommanderHighLevelTakeoffWithVelocity(const float height_m, const float velocity_m_s, bool relative);


int crtpCommanderHighLevelTakeoffYaw(const float absoluteHeight_m, const float duration_s, const float yaw);

int crtpCommanderHighLevelLand(const float absoluteHeight_m, const float duration_s);

int crtpCommanderHighLevelLandWithVelocity(const float height_m, const float velocity_m_s, bool relative);

int crtpCommanderHighLevelLandYaw(const float absoluteHeight_m, const float duration_s, const float yaw);

int crtpCommanderHighLevelStop();

int crtpCommanderHighLevelGoTo(const float x, const float y, const float z, const float yaw, const float duration_s, const bool relative);

bool crtpCommanderHighLevelIsTrajectoryDefined(uint8_t trajectoryId);

int crtpCommanderHighLevelStartTrajectory(const uint8_t trajectoryId, const float timeScale, const bool relative, const bool reversed);

int crtpCommanderHighLevelDefineTrajectory(const uint8_t trajectoryId, const crtpCommanderTrajectoryType_t type, const uint32_t offset, const uint8_t nPieces);

uint32_t crtpCommanderHighLevelTrajectoryMemSize();

bool crtpCommanderHighLevelWriteTrajectory(const uint32_t offset, const uint32_t length, const uint8_t* data);

bool crtpCommanderHighLevelReadTrajectory(const uint32_t offset, const uint32_t length, uint8_t* destination);

bool crtpCommanderHighLevelIsTrajectoryFinished();

#endif 
