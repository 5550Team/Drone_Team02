#pragma once

#include "ootx_decoder.h"

typedef struct {
  float phase;
  float tilt;
  float curve;
  float gibmag;
  float gibphase;
  // Lh2 extra params
  float ogeemag;
  float ogeephase;
} __attribute__((packed)) lighthouseCalibrationSweep_t;

typedef struct {
  lighthouseCalibrationSweep_t sweep[2];
  bool valid;
} __attribute__((packed)) lighthouseCalibration_t;

void lighthouseCalibrationInitFromFrame(lighthouseCalibration_t *calib, struct ootxDataFrame_s *frame);

void lighthouseCalibrationApplyV1(const lighthouseCalibration_t* calib, const float* rawAngles, float* correctedAngles);

void lighthouseCalibrationApplyV2(const lighthouseCalibration_t* calib, const float* rawAngles, float* correctedAngles);

void lighthouseCalibrationApplyNothing(const float rawAngles[2], float correctedAngles[2]);

typedef float (*lighthouseCalibrationMeasurementModel_t)(const float x, const float y, const float z, const float t, const lighthouseCalibrationSweep_t* calib);

float lighthouseCalibrationMeasurementModelLh1(const float x, const float y, const float z, const float t, const lighthouseCalibrationSweep_t* calib);

float lighthouseCalibrationMeasurementModelLh2(const float x, const float y, const float z, const float t, const lighthouseCalibrationSweep_t* calib);
