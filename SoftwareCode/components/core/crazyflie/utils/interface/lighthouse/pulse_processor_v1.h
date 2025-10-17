#pragma once

#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>

#include "pulse_processor.h"

bool pulseProcessorV1ProcessPulse(pulseProcessor_t *state, const pulseProcessorFrame_t* frameData, pulseProcessorResult_t* angles, int *baseStation, int *axis);

void pulseProcessorV1ProcessValidAngles(pulseProcessorResult_t* angles, int basestation);

uint8_t pulseProcessorV1AnglesQuality();
