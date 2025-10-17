/**
 * sensors_bosch.h - Sensors interface
 */
#ifndef __SENSORS_BOSCH_H__
#define __SENSORS_BOSCH_H__

#include "sensors.h"

void sensorsBoschInit(void);
bool sensorsBoschTest(void);
bool sensorsBoschAreCalibrated(void);
bool sensorsBoschManufacturingTest(void);
void sensorsBoschAcquire(sensorData_t *sensors, const uint32_t tick);
void sensorsBoschWaitDataReady(void);
bool sensorsBoschReadGyro(Axis3f *gyro);
bool sensorsBoschReadAcc(Axis3f *acc);
bool sensorsBoschReadMag(Axis3f *mag);
bool sensorsBoschReadBaro(baro_t *baro);
void sensorsBoschSetAccMode(accModes accMode);
void sensorsBoschDataAvailableCallback(void);

#endif /* __SENSORS_BOSCH_H__ */
