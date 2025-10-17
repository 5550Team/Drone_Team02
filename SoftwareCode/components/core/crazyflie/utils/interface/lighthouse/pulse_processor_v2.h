#pragma once

#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>

#include "pulse_processor.h"

bool pulseProcessorV2ProcessPulse(pulseProcessor_t *state, const pulseProcessorFrame_t* frameData, pulseProcessorResult_t* angles, int *baseStation, int *axis);

void pulseProcessorV2ConvertToV1Angles(const float v2Angle1, const float v2Angle2, float* v1Angles);

uint8_t pulseProcessorV2AnglesQuality();
