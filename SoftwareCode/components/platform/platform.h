#ifndef PLATFORM_H_
#define PLATFORM_H_

#include <stdbool.h>

#include "motors.h"

#define PLATFORM_DEVICE_TYPE_STRING_MAX_LEN (32 + 1)
#define PLATFORM_DEVICE_TYPE_MAX_LEN (4 + 1)
#define SENSOR_INCLUDED_MPU6050_HMC5883L_MS5611

typedef enum {
#ifdef SENSOR_INCLUDED_BMI088_BMP388
    SensorImplementation_bmi088_bmp388,
#endif

#ifdef SENSOR_INCLUDED_BMI088_SPI_BMP388
    SensorImplementation_bmi088_spi_bmp388,
#endif

#ifdef SENSOR_INCLUDED_MPU9250_LPS25H
    SensorImplementation_mpu9250_lps25h,
#endif

#ifdef SENSOR_INCLUDED_MPU6050_HMC5883L_MS5611
    SensorImplementation_mpu6050_HMC5883L_MS5611,
#endif

#ifdef SENSOR_INCLUDED_BOSCH
    SensorImplementation_bosch,
#endif

    SensorImplementation_COUNT,
} SensorImplementation_t;

typedef struct {
    char deviceType[PLATFORM_DEVICE_TYPE_MAX_LEN];
    char deviceTypeName[20];
    SensorImplementation_t sensorImplementation;
    bool physicalLayoutAntennasAreClose;
    const MotorPerifDef **motorMap;
} platformConfig_t;

/**
 * Initilizes all platform specific things.
 */
int platformInit(void);

void platformGetDeviceTypeString(char *deviceTypeString);
int platformParseDeviceTypeString(const char *deviceTypeString, char *deviceType);
int platformInitConfiguration(const platformConfig_t *configs, const int nrOfConfigs);

// Implemented in platform specific files
const platformConfig_t *platformGetListOfConfigurations(int *nrOfConfigs);
bool platformInitHardware();


void platformSetLowInterferenceRadioMode(void);

// Functions to read configuration
const char *platformConfigGetPlatformName();
const char *platformConfigGetDeviceType();
const char *platformConfigGetDeviceTypeName();
SensorImplementation_t platformConfigGetSensorImplementation();
bool platformConfigPhysicalLayoutAntennasAreClose();
const MotorPerifDef **platformConfigGetMotorMapping();

#endif /* PLATFORM_H_ */
