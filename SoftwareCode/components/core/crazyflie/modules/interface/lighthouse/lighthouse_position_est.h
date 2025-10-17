
#pragma once

#include "pulse_processor.h"
#include "lighthouse_geometry.h"


void lighthousePositionEstInit();


void lighthousePositionSetGeometryData(const uint8_t baseStation, const baseStationGeometry_t* geometry);

void lighthousePositionEstimatePoseCrossingBeams(const pulseProcessor_t *state, pulseProcessorResult_t* angles, int baseStation);
void lighthousePositionEstimatePoseSweeps(const pulseProcessor_t *state, pulseProcessorResult_t* angles, int baseStation);
