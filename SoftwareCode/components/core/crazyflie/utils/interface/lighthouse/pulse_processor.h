#pragma once

#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>

#include "ootx_decoder.h"
#include "lighthouse_calibration.h"
#include "lighthouse_geometry.h"

#define PULSE_PROCESSOR_N_SWEEPS 2
#define PULSE_PROCESSOR_N_BASE_STATIONS 2
#define PULSE_PROCESSOR_N_SENSORS 4
#define PULSE_PROCRSSOR_N_CONCURRENT_BLOCKS 2
#define PULSE_PROCESSOR_N_WORKSPACE (PULSE_PROCESSOR_N_SENSORS * PULSE_PROCRSSOR_N_CONCURRENT_BLOCKS)

#define PULSE_PROCESSOR_HISTORY_LENGTH 8
#define PULSE_PROCESSOR_TIMESTAMP_BITWIDTH 24
#define PULSE_PROCESSOR_TIMESTAMP_MAX ((1 << PULSE_PROCESSOR_TIMESTAMP_BITWIDTH) - 1)
#define PULSE_PROCESSOR_TIMESTAMP_BITMASK PULSE_PROCESSOR_TIMESTAMP_MAX

inline static uint32_t TS_DIFF(const uint32_t x, const uint32_t y) {
  return (x - y) & PULSE_PROCESSOR_TIMESTAMP_BITMASK;
}

inline static uint32_t TS_ABS_DIFF_LARGER_THAN(const uint32_t a, const uint32_t b, const uint32_t limit) {
    return TS_DIFF(a + limit, b) > (limit * 2);
}

typedef struct {
  uint8_t sensor;
  uint32_t timestamp;

  // V1 base station data --------
  uint16_t width;

  // V2 base station data --------
  uint32_t beamData;
  uint32_t offset;
  // Channel is zero indexed (0-15) here, while it is one indexed in the base station config (1 - 16)
  uint8_t channel; // Valid if channelFound is true
  uint8_t slowbit; // Valid if channelFound is true
  bool channelFound;
} pulseProcessorFrame_t;

typedef enum {
    lighthouseBsTypeUnknown = 0,
    lighthouseBsTypeV1 = 1,
    lighthouseBsTypeV2 = 2,
} lighthouseBaseStationType_t;

enum pulseClass_e {unknown, sync0, sync1, sweep};

typedef struct {
  uint32_t timestamp;
  int width;
} pulseProcessorPulse_t;

typedef enum {
  sweepIdFirst = 0,  // The X sweep in LH 1
  sweepIdSecond = 1  // The Y sweep in LH 1
} SweepId_t;

typedef enum {
  sweepStorageStateWaiting = 0,
  sweepStorageStateValid,
  sweepStorageStateError,
} SweepStorageState_t;

/**
 * @brief Holds data for V1 base station decoding
 *
 */
typedef struct {
  bool synchronized;    // At true if we are currently syncthonized
  int basestationsSynchronizedCount;

  // Synchronization state
  pulseProcessorPulse_t pulseHistory[PULSE_PROCESSOR_N_SENSORS][PULSE_PROCESSOR_HISTORY_LENGTH];
  int pulseHistoryIdx[PULSE_PROCESSOR_N_SENSORS];


  // Sync pulse timestamp estimation
  uint32_t lastSync;        // Last sync seen
  uint64_t currentSyncSum;  // Sum of the timestamps of all the close-together sync
  int nSyncPulses;          // Number of sync pulses accumulated

  // Sync pulse timestamps
  uint32_t currentSync;   // Sync currently used for sweep phase measurement
  uint32_t currentSync0;  // Sync0 of the current frame
  uint32_t currentSync0Width;  // Width of sync0 in the current frame
  uint32_t currentSync1Width;  // Width of sync1 in the current frame

  uint32_t currentSync0X;
  uint32_t currentSync0Y;
  uint32_t currentSync1X;
  uint32_t currentSync1Y;

  float frameWidth[2][2];

  // Base station and axis of the current frame
  int currentBaseStation;
  SweepId_t currentAxis;

  // Sweep timestamps
  struct {
    uint32_t timestamp;
    SweepStorageState_t state;
  } sweeps[PULSE_PROCESSOR_N_SENSORS];
  bool sweepDataStored;
} pulseProcessorV1_t;



typedef struct {
    pulseProcessorFrame_t slots[PULSE_PROCESSOR_N_WORKSPACE];
    int slotsUsed;
    uint32_t latestTimestamp;
} pulseProcessorV2PulseWorkspace_t;

typedef struct {
    uint32_t offset[PULSE_PROCESSOR_N_SENSORS];
    uint32_t timestamp0; // Timestamp of when the rotor has offset 0 (0 degrees)
    uint8_t channel;
} pulseProcessorV2SweepBlock_t;

typedef struct {
    pulseProcessorV2SweepBlock_t blocks[PULSE_PROCRSSOR_N_CONCURRENT_BLOCKS];
} pulseProcessorV2BlockWorkspace_t;


typedef struct {
  pulseProcessorV2PulseWorkspace_t pulseWorkspace;
  pulseProcessorV2BlockWorkspace_t blockWorkspace;

  // Latest block from each base station. Used to pair both blocks (sweeps) from one rotaion of the rotor.
  pulseProcessorV2SweepBlock_t blocks[PULSE_PROCESSOR_N_BASE_STATIONS];

  // Timestamp of the rotor zero position for the latest processed slowbit
  uint32_t ootxTimestamps[PULSE_PROCESSOR_N_BASE_STATIONS];
} pulseProcessorV2_t;

typedef struct pulseProcessor_s {
  bool receivedBsSweep[PULSE_PROCESSOR_N_BASE_STATIONS];

  union {
    struct {
      pulseProcessorV1_t v1;
    };

    struct {
      pulseProcessorV2_t v2;
    };
  };

  ootxDecoderState_t ootxDecoder[PULSE_PROCESSOR_N_BASE_STATIONS];
  lighthouseCalibration_t bsCalibration[PULSE_PROCESSOR_N_BASE_STATIONS];
  baseStationGeometry_t bsGeometry[PULSE_PROCESSOR_N_BASE_STATIONS];
  baseStationGeometryCache_t bsGeoCache[PULSE_PROCESSOR_N_BASE_STATIONS];

  // Health check data
  uint32_t healthFirstSensorTs;
  uint8_t healthSensorBitField;
  bool healthDetermined;
} pulseProcessor_t;

typedef struct {
  float angles[PULSE_PROCESSOR_N_SWEEPS];
  float correctedAngles[PULSE_PROCESSOR_N_SWEEPS];
  int validCount;
} pulseProcessorBaseStationMeasuremnt_t;

typedef struct {
  pulseProcessorBaseStationMeasuremnt_t baseStatonMeasurements[PULSE_PROCESSOR_N_BASE_STATIONS];
} pulseProcessorSensorMeasurement_t;

typedef struct {
  pulseProcessorSensorMeasurement_t sensorMeasurementsLh1[PULSE_PROCESSOR_N_SENSORS];
  pulseProcessorSensorMeasurement_t sensorMeasurementsLh2[PULSE_PROCESSOR_N_SENSORS];
  lighthouseBaseStationType_t measurementType;
} pulseProcessorResult_t;

typedef bool (*pulseProcessorProcessPulse_t)(pulseProcessor_t *state, const pulseProcessorFrame_t* frameData, pulseProcessorResult_t* angles, int *baseStation, int *axis);

void pulseProcessorApplyCalibration(pulseProcessor_t *state, pulseProcessorResult_t* angles, int baseStation);

void pulseProcessorClearOutdated(pulseProcessor_t *appState, pulseProcessorResult_t* angles, int basestation);

void pulseProcessorProcessed(pulseProcessorResult_t* angles, int baseStation);

void pulseProcessorClear(pulseProcessorResult_t* angles, int baseStation);

void pulseProcessorAllClear(pulseProcessorResult_t* angles);

uint8_t pulseProcessorAnglesQuality();
