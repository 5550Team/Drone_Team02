#include "pulse_processor.h"


void pulseProcessorApplyCalibration(pulseProcessor_t *state, pulseProcessorResult_t* angles, int baseStation)
{
  for (int sensor = 0; sensor < PULSE_PROCESSOR_N_SENSORS; sensor++) {
    pulseProcessorBaseStationMeasuremnt_t* bsMeasurement = &angles->sensorMeasurements[sensor].baseStatonMeasurements[baseStation];
    lighthouseCalibrationApply(&state->bsCalibration[baseStation], bsMeasurement->angles, bsMeasurement->correctedAngles);
  }
}

/**
 * @brief Clear result struct
 *
 * @param angles
 */
void pulseProcessorClear(pulseProcessorResult_t* angles, int baseStation)
{
  for (size_t sensor = 0; sensor < PULSE_PROCESSOR_N_SENSORS; sensor++) {
    angles->sensorMeasurements[sensor].baseStatonMeasurements[baseStation].validCount = 0;
  }
}
