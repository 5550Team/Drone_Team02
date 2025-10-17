#pragma once

#include <stdbool.h>
#include <stdint.h>

#include "pulse_processor.h"

typedef struct {
  bool isSyncFrame;
  pulseProcessorFrame_t data;
} lighthouseUartFrame_t;

typedef struct {
  int sampleCount;
  int hitCount;
} lighthouseBsIdentificationData_t;

void lighthouseCoreInit();
void lighthouseCoreTask(void *param);


void lighthouseCoreSetCalibrationData(const uint8_t baseStation, const lighthouseCalibration_t* calibration);

bool lighthouseCorePersistData(const uint8_t baseStation, const bool geoData, const bool calibData);
