#ifndef __SENSORS_BMI088_BMP388_H__
#define __SENSORS_BMI088_BMP388_H__

#include "sensors.h"

void sensorsBmi088Bmp388Init(void);
bool sensorsBmi088Bmp388Test(void);
bool sensorsBmi088Bmp388AreCalibrated(void);
bool sensorsBmi088Bmp388ManufacturingTest(void);
void sensorsBmi088Bmp388Acquire(sensorData_t *sensors, const uint32_t tick);
void sensorsBmi088Bmp388WaitDataReady(void);
bool sensorsBmi088Bmp388ReadGyro(Axis3f *gyro);
bool sensorsBmi088Bmp388ReadAcc(Axis3f *acc);
bool sensorsBmi088Bmp388ReadMag(Axis3f *mag);
bool sensorsBmi088Bmp388ReadBaro(baro_t *baro);
void sensorsBmi088Bmp388SetAccMode(accModes accMode);
void sensorsBmi088Bmp388DataAvailableCallback(void);

#endif // __SENSORS_BMI088_BMP388_H__