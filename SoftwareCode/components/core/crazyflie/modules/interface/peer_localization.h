#ifndef __PEER_LOCALIZATION_H__
#define __PEER_LOCALIZATION_H__

#include <stdbool.h>
#include "math3d.h"
#include "stabilizer_types.h"

#define PEER_LOCALIZATION_MAX_NEIGHBORS 10

// Initialize and test the module.
void peerLocalizationInit();
bool peerLocalizationTest();

typedef struct peerLocalizationOtherPosition_s {
  uint8_t id;  // CF id
  point_t pos; // position and timestamp (millisecs)
} peerLocalizationOtherPosition_t;

// Tell the peer localization system the position of another Crazyflie.
// Should be called when the position is already known with high accuracy,
// e.g. when a motion capture measurement packet is received.
bool peerLocalizationTellPosition(int id, positionMeasurement_t const *pos);

// Returns true if we have a position value for the given radio ID.
bool peerLocalizationIsIDActive(uint8_t id);

// Returns the position value for the given radio ID, or NULL if none exists.
// Performs a linear search.
peerLocalizationOtherPosition_t *peerLocalizationGetPositionByID(uint8_t id);

// Returns the position value based on index, uncorrelated with radio ID. More
// efficient if iterating over all peers is needed.
peerLocalizationOtherPosition_t *peerLocalizationGetPositionByIdx(uint8_t idx);

#endif // __PEER_LOCALIZATION_H__
