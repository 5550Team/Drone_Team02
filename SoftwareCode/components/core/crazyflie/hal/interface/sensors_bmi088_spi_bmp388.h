
#ifndef __SENSORS_BMI088SPI_BMP388_H__
#define __SENSORS_BMI088SPI_BMP388_H__

#include "sensors.h"

void sensorsBmi088SpiBmp388Init(void);
bool sensorsBmi088SpiBmp388Test(void);
bool sensorsBmi088SpiBmp388AreCalibrated(void);
bool sensorsBmi088SpiBmp388ManufacturingTest(void);
void sensorsBmi088SpiBmp388Acquire(sensorData_t *sensors, const uint32_t tick);
void sensorsBmi088SpiBmp388WaitDataReady(void);
bool sensorsBmi088SpiBmp388ReadGyro(Axis3f *gyro);
bool sensorsBmi088SpiBmp388ReadAcc(Axis3f *acc);
bool sensorsBmi088SpiBmp388ReadMag(Axis3f *mag);
bool sensorsBmi088SpiBmp388ReadBaro(baro_t *baro);
void sensorsBmi088SpiBmp388SetAccMode(accModes accMode);
void sensorsBmi088SpiBmp388DataAvailableCallback(void);

#endif // __SENSORS_BMI088SPI_BMP388_H__