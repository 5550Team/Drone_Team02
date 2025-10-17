#include "lighthouse_calibration.h"

#include <math.h>

#define USE_SIMPLE_CALIBRATION

void lighthouseCalibrationInitFromFrame(lighthouseCalibration_t *calib, struct ootxDataFrame_s *frame)
{
  calib->axis[0].phase = frame->phase0;
  calib->axis[0].tilt = frame->tilt0;
  calib->axis[0].curve = frame->curve0;
  calib->axis[0].gibmag = frame->gibmag0;
  calib->axis[0].gibphase = frame->gibphase0;

  calib->axis[1].phase = frame->phase1;
  calib->axis[1].tilt = frame->tilt1;
  calib->axis[1].curve = frame->curve1;
  calib->axis[1].gibmag = frame->gibmag1;
  calib->axis[1].gibphase = frame->gibphase1;

  calib->valid = true;
}

#ifndef USE_SIMPLE_CALIBRATION
static void predict(const lighthouseCalibration_t* calib, float const* xy, float* ang) {
  float tiltX = calib->axis[0].tilt;
  float phaseX = calib->axis[0].phase;
  float curveX = calib->axis[0].curve;
  float gibmagX = calib->axis[0].gibmag;
  float gibphaseX = calib->axis[0].gibphase;

  float tiltY = calib->axis[1].tilt;
  float phaseY = calib->axis[1].phase;
  float curveY = calib->axis[1].curve;
  float gibmagY = calib->axis[1].gibmag;
  float gibphaseY = calib->axis[1].gibphase;

  ang[0] = atanf(xy[0] - (tiltX + curveX * xy[1]) * xy[1]);
  ang[1] = atanf(xy[1] - (tiltY + curveY * xy[0]) * xy[0]);
  ang[0] -= phaseX + gibmagX * sinf(ang[0] + gibphaseX);
  ang[1] -= phaseY + gibmagY * sinf(ang[1] + gibphaseY);
}

// Given the perturbed lighthouse angle, predict the ideal angle
static void correct(const lighthouseCalibration_t* calib, const float * angle, float * corrected) {
  float ideal[2], pred[2], xy[2];
  ideal[0] = angle[0];
  ideal[1] = angle[1];
  for (int i = 0; i < 10; i++) {
    xy[0] = tanf(ideal[0]);
    xy[1] = tanf(ideal[1]);
    predict(calib, xy, pred);
    ideal[0] += (angle[0] - pred[0]);
    ideal[1] += (angle[1] - pred[1]);
  }
  corrected[0] = ideal[0];
  corrected[1] = ideal[1];
}
#endif

void lighthouseCalibrationApply(lighthouseCalibration_t* calib, float rawAngles[2], float correctedAngles[2])
{
  if (calib->valid) {
    #ifdef USE_SIMPLE_CALIBRATION
    correctedAngles[0] = rawAngles[0] + calib->axis[0].phase;
    correctedAngles[1] = rawAngles[1] + calib->axis[1].phase;
    #else
    correct(calib, rawAngles, correctedAngles);
    #endif
  } else {
    correctedAngles[0] = rawAngles[0];
    correctedAngles[1] = rawAngles[1];
  }
}
