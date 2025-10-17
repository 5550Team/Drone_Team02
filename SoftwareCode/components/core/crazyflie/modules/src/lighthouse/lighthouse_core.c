#include "stm32fxxx.h"
#include "FreeRTOS.h"
#include "task.h"

#include <math.h>
#include <stdbool.h>
#include <stdint.h>

#include "system.h"
#include "log.h"
#include "param.h"
#include "statsCnt.h"

#define DEBUG_MODULE "LH"
#include "debug.h"
#include "uart1.h"
#include "crtp_localization_service.h"

#include "pulse_processor.h"
#include "pulse_processor_v1.h"
#include "pulse_processor_v2.h"

#include "lighthouse_deck_flasher.h"
#include "lighthouse_position_est.h"
#include "lighthouse_core.h"

#include "storage.h"

#include "test_support.h"
#include "static_mem.h"

static const uint32_t MAX_WAIT_TIME_FOR_HEALTH_MS = 4000;

static pulseProcessorResult_t angles;
static lighthouseUartFrame_t frame;
static lighthouseBsIdentificationData_t bsIdentificationData;

// Stats
static bool uartSynchronized = false;

#define ONE_SECOND 1000
#define HALF_SECOND 500
static STATS_CNT_RATE_DEFINE(serialFrameRate, ONE_SECOND);
static STATS_CNT_RATE_DEFINE(frameRate, ONE_SECOND);
static STATS_CNT_RATE_DEFINE(cycleRate, ONE_SECOND);

static STATS_CNT_RATE_DEFINE(bs0Rate, HALF_SECOND);
static STATS_CNT_RATE_DEFINE(bs1Rate, HALF_SECOND);
static statsCntRateLogger_t* bsRates[PULSE_PROCESSOR_N_BASE_STATIONS] = {&bs0Rate, &bs1Rate};

static uint16_t pulseWidth[PULSE_PROCESSOR_N_SENSORS];
pulseProcessor_t lighthouseCoreState = {
 
};

#if LIGHTHOUSE_FORCE_TYPE == 1
pulseProcessorProcessPulse_t pulseProcessorProcessPulse = pulseProcessorV1ProcessPulse;
#elif LIGHTHOUSE_FORCE_TYPE == 2
pulseProcessorProcessPulse_t pulseProcessorProcessPulse = pulseProcessorV2ProcessPulse;
#else
pulseProcessorProcessPulse_t pulseProcessorProcessPulse = (void*)0;
#endif

#define UART_FRAME_LENGTH 12


// Persistent storage
#define STORAGE_VERSION_KEY "lh/ver"
#define CURRENT_STORAGE_VERSION "1"
#define STORAGE_KEY_GEO "lh/sys/0/geo/"
#define STORAGE_KEY_CALIB "lh/sys/0/cal/"
#define KEY_LEN 20

static void verifySetStorageVersion();
static baseStationGeometry_t geoBuffer;
TESTABLE_STATIC void initializeGeoDataFromStorage();
static lighthouseCalibration_t calibBuffer;
TESTABLE_STATIC void initializeCalibDataFromStorage();


void lighthouseCoreInit() {
  lighthousePositionEstInit();
}

TESTABLE_STATIC bool getUartFrameRaw(lighthouseUartFrame_t *frame) {
  static char data[UART_FRAME_LENGTH];
  int syncCounter = 0;

  for(int i = 0; i < UART_FRAME_LENGTH; i++) {
    uart1Getchar(&data[i]);
    if ((unsigned char)data[i] == 0xff) {
      syncCounter += 1;
    }
  }

  memset(frame, 0, sizeof(*frame));

  frame->isSyncFrame = (syncCounter == UART_FRAME_LENGTH);

  frame->data.sensor = data[0] & 0x03;
  frame->data.channelFound = (data[0] & 0x80) == 0;
  frame->data.channel = (data[0] >> 3) & 0x0f;
  frame->data.slowbit = (data[0] >> 2) & 0x01;
  memcpy(&frame->data.width, &data[1], 2);
  memcpy(&frame->data.offset, &data[3], 3);
  memcpy(&frame->data.beamData, &data[6], 3);
  memcpy(&frame->data.timestamp, &data[9], 3);

  // Offset is expressed in a 6 MHz clock, convert to the 24 MHz that is used for timestamps
  frame->data.offset *= 4;

  bool isPaddingZero = (((data[5] | data[8]) & 0xfe) == 0);
  bool isFrameValid = (isPaddingZero || frame->isSyncFrame);

  STATS_CNT_RATE_EVENT(&serialFrameRate);

  return isFrameValid;
}

TESTABLE_STATIC void waitForUartSynchFrame() {
  char c;
  int syncCounter = 0;
  bool synchronized = false;

  while (!synchronized) {
    uart1Getchar(&c);
    if ((unsigned char)c == 0xff) {
      syncCounter += 1;
    } else {
      syncCounter = 0;
    }
    synchronized = (syncCounter == UART_FRAME_LENGTH);
  }
}


static uint8_t estimationMethod = 1;


static void usePulseResultCrossingBeams(pulseProcessor_t *appState, pulseProcessorResult_t* angles, int basestation) {
  pulseProcessorClearOutdated(appState, angles, basestation);

  if (basestation == 1) {
    STATS_CNT_RATE_EVENT(&cycleRate);

    lighthousePositionEstimatePoseCrossingBeams(appState, angles, 1);

    pulseProcessorProcessed(angles, 0);
    pulseProcessorProcessed(angles, 1);
  }
}


static void usePulseResultSweeps(pulseProcessor_t *appState, pulseProcessorResult_t* angles, int basestation) {
  STATS_CNT_RATE_EVENT(&cycleRate);

  pulseProcessorClearOutdated(appState, angles, basestation);

  lighthousePositionEstimatePoseSweeps(appState, angles, basestation);

  pulseProcessorProcessed(angles, basestation);
}

static void convertV2AnglesToV1Angles(pulseProcessorResult_t* angles) {
  for (int sensor = 0; sensor < PULSE_PROCESSOR_N_SENSORS; sensor++) {
    for (int bs = 0; bs < PULSE_PROCESSOR_N_BASE_STATIONS; bs++) {
      pulseProcessorBaseStationMeasuremnt_t* from = &angles->sensorMeasurementsLh2[sensor].baseStatonMeasurements[bs];
      pulseProcessorBaseStationMeasuremnt_t* to = &angles->sensorMeasurementsLh1[sensor].baseStatonMeasurements[bs];

      if (2 == from->validCount) {
        pulseProcessorV2ConvertToV1Angles(from->correctedAngles[0], from->correctedAngles[1], to->correctedAngles);
        to->validCount = from->validCount;
      } else {
        to->validCount = 0;
      }
    }
  }
}

static void usePulseResult(pulseProcessor_t *appState, pulseProcessorResult_t* angles, int basestation, int sweepId) {
  if (sweepId == sweepIdSecond) {
    pulseProcessorApplyCalibration(appState, angles, basestation);
    if (lighthouseBsTypeV2 == angles->measurementType) {
      // Emulate V1 base stations for now, convert to V1 angles
      convertV2AnglesToV1Angles(angles);
    }

    // Send measurement to the ground
    locSrvSendLighthouseAngle(basestation, angles);

    switch(estimationMethod) {
      case 0:
        usePulseResultCrossingBeams(appState, angles, basestation);
        break;
      case 1:
        usePulseResultSweeps(appState, angles, basestation);
        break;
      default:
        break;
    }
  }
}

TESTABLE_STATIC lighthouseBaseStationType_t identifyBaseStationType(const lighthouseUartFrame_t* frame, lighthouseBsIdentificationData_t* state) {
    const uint32_t v1Indicator = 0x1ffff;
    const int requiredIndicatorsForV1 = 6;
    const int requiredSamplesForV2 = 20;
    state->sampleCount++;
    if (frame->data.beamData == v1Indicator) {
        state->hitCount++;
    }

    if (state->hitCount >= requiredIndicatorsForV1) {
        return lighthouseBsTypeV1;
    }

    if (state->sampleCount >= requiredSamplesForV2) {
        return lighthouseBsTypeV2;
    }

    return lighthouseBsTypeUnknown;
}

static pulseProcessorProcessPulse_t identifySystem(const lighthouseUartFrame_t* frame, lighthouseBsIdentificationData_t* bsIdentificationData) {
  pulseProcessorProcessPulse_t result = (void*)0;

  switch (identifyBaseStationType(frame, bsIdentificationData)) {
    case lighthouseBsTypeV1:
      DEBUG_PRINT("Locking to V1 system\n");
      result = pulseProcessorV1ProcessPulse;
      break;
    case lighthouseBsTypeV2:
      DEBUG_PRINT("Locking to V2 system\n");
      result = pulseProcessorV2ProcessPulse;
      break;
    default:
      // Nothing here
      break;
  }

  return result;
}

static void processFrame(pulseProcessor_t *appState, pulseProcessorResult_t* angles, const lighthouseUartFrame_t* frame) {
    int basestation;
    int sweepId;

    pulseWidth[frame->data.sensor] = frame->data.width;

    if (pulseProcessorProcessPulse(appState, &frame->data, angles, &basestation, &sweepId)) {
        STATS_CNT_RATE_EVENT(bsRates[basestation]);
        usePulseResult(appState, angles, basestation, sweepId);
    }
}

static void deckHealthCheck(pulseProcessor_t *appState, const lighthouseUartFrame_t* frame) {
  if (!appState->healthDetermined) {
    const uint32_t now = xTaskGetTickCount();
    if (0 == appState->healthFirstSensorTs) {
      appState->healthFirstSensorTs = now;
    }

    if (0x0f == appState->healthSensorBitField) {
      appState->healthDetermined = true;
      // DEBUG_PRINT("All sensors good\n");
    } else {
      appState->healthSensorBitField |= (0x01 << frame->data.sensor);

      if ((now - appState->healthFirstSensorTs) > MAX_WAIT_TIME_FOR_HEALTH_MS) {
        appState->healthDetermined = true;
        DEBUG_PRINT("Warning: not getting data from all sensors\n");
        for (int i = 0; i < PULSE_PROCESSOR_N_SENSORS; i++) {
          if (appState->healthSensorBitField & (0x1 << i)) {
            DEBUG_PRINT("  %d - OK\n", i);
          } else {
            DEBUG_PRINT("  %d - error\n", i);
          }
        }
      }
    }
  }
}

void lighthouseCoreTask(void *param) {
  bool isUartFrameValid = false;

  uart1Init(230400);
  systemWaitStart();

  verifySetStorageVersion();
  initializeGeoDataFromStorage();
  initializeCalibDataFromStorage();

  lighthouseDeckFlasherCheckVersionAndBoot();

  memset(&bsIdentificationData, 0, sizeof(bsIdentificationData));

  while(1) {
    memset(pulseWidth, 0, sizeof(pulseWidth[0]) * PULSE_PROCESSOR_N_SENSORS);
    waitForUartSynchFrame();
    uartSynchronized = true;

    bool previousWasSyncFrame = false;

    while((isUartFrameValid = getUartFrameRaw(&frame))) {
      // If a sync frame is getting through, we are only receiving sync frames. So nothing else. Reset state
      if(frame.isSyncFrame && previousWasSyncFrame) {
          pulseProcessorAllClear(&angles);
      }
      // Now we are receiving items
      else if(!frame.isSyncFrame) {
        STATS_CNT_RATE_EVENT(&frameRate);

        deckHealthCheck(&lighthouseCoreState, &frame);
        if (pulseProcessorProcessPulse) {
          processFrame(&lighthouseCoreState, &angles, &frame);
        } else {
          pulseProcessorProcessPulse = identifySystem(&frame, &bsIdentificationData);
        }
      }

      previousWasSyncFrame = frame.isSyncFrame;
    }

    uartSynchronized = false;
  }
}

void lighthouseCoreSetCalibrationData(const uint8_t baseStation, const lighthouseCalibration_t* calibration) {
  if (baseStation < PULSE_PROCESSOR_N_BASE_STATIONS) {
    lighthouseCoreState.bsCalibration[baseStation] = *calibration;
  }
}

static void generateStorageKey(char* buf, const char* base, const uint8_t baseStation) {
  // TOOD make an implementation that supports baseStations with 2 digits
  ASSERT(baseStation <= 9);

  const int baseLen = strlen(base);
  memcpy(buf, base, baseLen);
  buf[baseLen] = '0' + baseStation;
  buf[baseLen + 1] = '\0';
}

bool lighthouseCorePersistData(const uint8_t baseStation, const bool geoData, const bool calibData) {
  bool result = true;
  char key[KEY_LEN];

  if (baseStation < PULSE_PROCESSOR_N_BASE_STATIONS) {
    if (geoData) {
      generateStorageKey(key, STORAGE_KEY_GEO, baseStation);
      result = result && storageStore(key, &lighthouseCoreState.bsGeometry[baseStation], sizeof(lighthouseCoreState.bsGeometry[baseStation]));
    }
    if (calibData) {
      generateStorageKey(key, STORAGE_KEY_CALIB, baseStation);
      result = result && storageStore(key, &lighthouseCoreState.bsCalibration[baseStation], sizeof(lighthouseCoreState.bsCalibration[baseStation]));
    }
  }

  return result;
}

static void verifySetStorageVersion() {
  const int bufLen = 5;
  char buffer[bufLen];

  const size_t fetched = storageFetch(STORAGE_VERSION_KEY, buffer, bufLen);
  if (fetched == 0) {
    storageStore(STORAGE_VERSION_KEY, CURRENT_STORAGE_VERSION, strlen(CURRENT_STORAGE_VERSION) + 1);
  } else {
    if (strcmp(buffer, CURRENT_STORAGE_VERSION) != 0) {
      // The storage format version is wrong! What to do?
      // No need to handle until we bump the storage version, assert for now.
      ASSERT_FAILED();
    }
  }
}

TESTABLE_STATIC void initializeGeoDataFromStorage() {
  char key[KEY_LEN];

  for (int baseStation = 0; baseStation < PULSE_PROCESSOR_N_BASE_STATIONS; baseStation++) {
    if (!lighthouseCoreState.bsGeometry[baseStation].valid) {
      generateStorageKey(key, STORAGE_KEY_GEO, baseStation);
      const size_t geoSize = sizeof(geoBuffer);
      const size_t fetched = storageFetch(key, (void*)&geoBuffer, geoSize);
      if (fetched == geoSize) {
        lighthousePositionSetGeometryData(baseStation, &geoBuffer);
      }
    }
  }
}

TESTABLE_STATIC void initializeCalibDataFromStorage() {
  char key[KEY_LEN];

  for (int baseStation = 0; baseStation < PULSE_PROCESSOR_N_BASE_STATIONS; baseStation++) {
    if (!lighthouseCoreState.bsCalibration[baseStation].valid) {
      generateStorageKey(key, STORAGE_KEY_CALIB, baseStation);
      const size_t calibSize = sizeof(calibBuffer);
      const size_t fetched = storageFetch(key, (void*)&calibBuffer, calibSize);
      if (fetched == calibSize) {
        lighthouseCoreSetCalibrationData(baseStation, &calibBuffer);
      }
    }
  }
}

LOG_GROUP_START(lighthouse)
LOG_ADD_BY_FUNCTION(LOG_UINT8, validAngles, &pulseProcessorAnglesQuality)

LOG_ADD(LOG_FLOAT, rawAngle0x, &angles.sensorMeasurementsLh1[0].baseStatonMeasurements[0].angles[0])
LOG_ADD(LOG_FLOAT, rawAngle0y, &angles.sensorMeasurementsLh1[0].baseStatonMeasurements[0].angles[1])
LOG_ADD(LOG_FLOAT, rawAngle1x, &angles.sensorMeasurementsLh1[0].baseStatonMeasurements[1].angles[0])
LOG_ADD(LOG_FLOAT, rawAngle1y, &angles.sensorMeasurementsLh1[0].baseStatonMeasurements[1].angles[1])
LOG_ADD(LOG_FLOAT, angle0x, &angles.sensorMeasurementsLh1[0].baseStatonMeasurements[0].correctedAngles[0])
LOG_ADD(LOG_FLOAT, angle0y, &angles.sensorMeasurementsLh1[0].baseStatonMeasurements[0].correctedAngles[1])
LOG_ADD(LOG_FLOAT, angle1x, &angles.sensorMeasurementsLh1[0].baseStatonMeasurements[1].correctedAngles[0])
LOG_ADD(LOG_FLOAT, angle1y, &angles.sensorMeasurementsLh1[0].baseStatonMeasurements[1].correctedAngles[1])

LOG_ADD(LOG_FLOAT, angle0x_1, &angles.sensorMeasurementsLh1[1].baseStatonMeasurements[0].correctedAngles[0])
LOG_ADD(LOG_FLOAT, angle0y_1, &angles.sensorMeasurementsLh1[1].baseStatonMeasurements[0].correctedAngles[1])
LOG_ADD(LOG_FLOAT, angle1x_1, &angles.sensorMeasurementsLh1[1].baseStatonMeasurements[1].correctedAngles[0])
LOG_ADD(LOG_FLOAT, angle1y_1, &angles.sensorMeasurementsLh1[1].baseStatonMeasurements[1].correctedAngles[1])

LOG_ADD(LOG_FLOAT, angle0x_2, &angles.sensorMeasurementsLh1[2].baseStatonMeasurements[0].correctedAngles[0])
LOG_ADD(LOG_FLOAT, angle0y_2, &angles.sensorMeasurementsLh1[2].baseStatonMeasurements[0].correctedAngles[1])
LOG_ADD(LOG_FLOAT, angle1x_2, &angles.sensorMeasurementsLh1[2].baseStatonMeasurements[1].correctedAngles[0])
LOG_ADD(LOG_FLOAT, angle1y_2, &angles.sensorMeasurementsLh1[2].baseStatonMeasurements[1].correctedAngles[1])

LOG_ADD(LOG_FLOAT, angle0x_3, &angles.sensorMeasurementsLh1[3].baseStatonMeasurements[0].correctedAngles[0])
LOG_ADD(LOG_FLOAT, angle0y_3, &angles.sensorMeasurementsLh1[3].baseStatonMeasurements[0].correctedAngles[1])
LOG_ADD(LOG_FLOAT, angle1x_3, &angles.sensorMeasurementsLh1[3].baseStatonMeasurements[1].correctedAngles[0])
LOG_ADD(LOG_FLOAT, angle1y_3, &angles.sensorMeasurementsLh1[3].baseStatonMeasurements[1].correctedAngles[1])

LOG_ADD(LOG_FLOAT, rawAngle0xlh2, &angles.sensorMeasurementsLh2[0].baseStatonMeasurements[0].angles[0])
LOG_ADD(LOG_FLOAT, rawAngle0ylh2, &angles.sensorMeasurementsLh2[0].baseStatonMeasurements[0].angles[1])
LOG_ADD(LOG_FLOAT, rawAngle1xlh2, &angles.sensorMeasurementsLh2[0].baseStatonMeasurements[1].angles[0])
LOG_ADD(LOG_FLOAT, rawAngle1ylh2, &angles.sensorMeasurementsLh2[0].baseStatonMeasurements[1].angles[1])

LOG_ADD(LOG_FLOAT, angle0x_0lh2, &angles.sensorMeasurementsLh2[0].baseStatonMeasurements[0].correctedAngles[0])
LOG_ADD(LOG_FLOAT, angle0y_0lh2, &angles.sensorMeasurementsLh2[0].baseStatonMeasurements[0].correctedAngles[1])
LOG_ADD(LOG_FLOAT, angle1x_0lh2, &angles.sensorMeasurementsLh2[0].baseStatonMeasurements[1].correctedAngles[0])
LOG_ADD(LOG_FLOAT, angle1y_0lh2, &angles.sensorMeasurementsLh2[0].baseStatonMeasurements[1].correctedAngles[1])

STATS_CNT_RATE_LOG_ADD(serRt, &serialFrameRate)
STATS_CNT_RATE_LOG_ADD(frmRt, &frameRate)
STATS_CNT_RATE_LOG_ADD(cycleRt, &cycleRate)

STATS_CNT_RATE_LOG_ADD(bs0Rt, &bs0Rate)
STATS_CNT_RATE_LOG_ADD(bs1Rt, &bs1Rate)

LOG_ADD(LOG_UINT16, width0, &pulseWidth[0])
LOG_ADD(LOG_UINT16, width1, &pulseWidth[1])
LOG_ADD(LOG_UINT16, width2, &pulseWidth[2])
LOG_ADD(LOG_UINT16, width3, &pulseWidth[3])

LOG_ADD(LOG_UINT8, comSync, &uartSynchronized)
LOG_GROUP_STOP(lighthouse)

PARAM_GROUP_START(lighthouse)
PARAM_ADD(PARAM_UINT8, method, &estimationMethod)
PARAM_GROUP_STOP(lighthouse)
