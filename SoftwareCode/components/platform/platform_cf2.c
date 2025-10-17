#include <string.h>

#include "platform.h"
#define DEBUG_MODULE "PLATFORM"
#include "debug_cf.h"

/*to support different hardware platform */
static platformConfig_t configs[] = {

    {
        .deviceType = "EP20",
        .deviceTypeName = "ESPlane 2.0 ",
        .sensorImplementation = SensorImplementation_mpu6050_HMC5883L_MS5611,
        .physicalLayoutAntennasAreClose = false,
        .motorMap = motorMapDefaultBrushed,
    },
    {
        .deviceType = "ED12",
        .deviceTypeName = "ESP_Drone_v1_2",
        .sensorImplementation = SensorImplementation_mpu6050_HMC5883L_MS5611,
        .physicalLayoutAntennasAreClose = false,
        .motorMap = motorMapDefaultBrushed,
    },

};

const platformConfig_t *platformGetListOfConfigurations(int *nrOfConfigs)
{
    *nrOfConfigs = sizeof(configs) / sizeof(platformConfig_t);
    return configs;
}

bool platformInitHardware()
{
    //TODO:
    return true;
}

// Config functions ------------------------

const char *platformConfigGetPlatformName()
{
    return "ED12";
}
