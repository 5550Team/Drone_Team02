#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "eprintf.h"
#include "hmc5883l.h"
#include "i2cdev.h"
#define DEBUG_MODULE "HMC5883L"
#include "debug_cf.h"

static uint8_t devAddr;
static uint8_t buffer[6];
static uint8_t mode;
static I2C_Dev *I2Cx;
static bool isInit;

void hmc5883lInit(I2C_Dev *i2cPort)
{
    if (isInit) {
        return;
    }

    I2Cx = i2cPort;
    devAddr = HMC5883L_ADDRESS;

    // write CONFIG_A register
    i2cdevWriteByte(I2Cx, devAddr, HMC5883L_RA_CONFIG_A,
                    (HMC5883L_AVERAGING_8 << (HMC5883L_CRA_AVERAGE_BIT - HMC5883L_CRA_AVERAGE_LENGTH + 1)) |
                    (HMC5883L_RATE_15 << (HMC5883L_CRA_RATE_BIT - HMC5883L_CRA_RATE_LENGTH + 1)) |
                    (HMC5883L_BIAS_NORMAL << (HMC5883L_CRA_BIAS_BIT - HMC5883L_CRA_BIAS_LENGTH + 1))); //配置信息：采样率15hz，每次结果来自8次采样，不进行校准

    // write CONFIG_B register
    hmc5883lSetGain(HMC5883L_GAIN_660); //测量范围 +-2.5Ga

    isInit = true;
}


bool hmc5883lTestConnection()
{
    if (i2cdevReadReg8(I2Cx, devAddr, HMC5883L_RA_ID_A, 3, buffer)) {
        return (buffer[0] == 'H' && buffer[1] == '4' && buffer[2] == '3');
    }

    return false;
}


bool hmc5883lSelfTest()
{
    bool testStatus = true;
    int16_t mxp, myp, mzp;  // positive magnetometer measurements
    int16_t mxn, myn, mzn;  // negative magnetometer measurements
    struct {
        uint8_t configA;
        uint8_t configB;
        uint8_t mode;
    } regSave;

    // Save register values
    if (i2cdevReadReg8(I2Cx, devAddr, HMC5883L_RA_CONFIG_A, sizeof(regSave), (uint8_t *)&regSave) == false) {
        // TODO: error handling
        return false;
    }

    // Set gain (sensitivity)
    hmc5883lSetGain(HMC5883L_ST_GAIN);

    // Write CONFIG_A register and do positive test
    i2cdevWriteByte(I2Cx, devAddr, HMC5883L_RA_CONFIG_A,
                    (HMC5883L_AVERAGING_1 << (HMC5883L_CRA_AVERAGE_BIT - HMC5883L_CRA_AVERAGE_LENGTH + 1)) |
                    (HMC5883L_RATE_15 << (HMC5883L_CRA_RATE_BIT - HMC5883L_CRA_RATE_LENGTH + 1)) |
                    (HMC5883L_BIAS_POSITIVE << (HMC5883L_CRA_BIAS_BIT - HMC5883L_CRA_BIAS_LENGTH + 1)));

    /* Perform test measurement & check results */
    hmc5883lSetMode(HMC5883L_MODE_SINGLE);
    vTaskDelay(M2T(HMC5883L_ST_DELAY_MS));
    hmc5883lGetHeading(&mxp, &myp, &mzp);

    // Write CONFIG_A register and do negative test
    i2cdevWriteByte(I2Cx, devAddr, HMC5883L_RA_CONFIG_A,
                    (HMC5883L_AVERAGING_1 << (HMC5883L_CRA_AVERAGE_BIT - HMC5883L_CRA_AVERAGE_LENGTH + 1)) |
                    (HMC5883L_RATE_15 << (HMC5883L_CRA_RATE_BIT - HMC5883L_CRA_RATE_LENGTH + 1)) |
                    (HMC5883L_BIAS_NEGATIVE << (HMC5883L_CRA_BIAS_BIT - HMC5883L_CRA_BIAS_LENGTH + 1)));

    /* Perform test measurement & check results */
    hmc5883lSetMode(HMC5883L_MODE_SINGLE);
    vTaskDelay(M2T(HMC5883L_ST_DELAY_MS));
    hmc5883lGetHeading(&mxn, &myn, &mzn);

    if (hmc5883lEvaluateSelfTest(HMC5883L_ST_X_MIN, HMC5883L_ST_X_MAX, mxp, "pos X") &&
            hmc5883lEvaluateSelfTest(HMC5883L_ST_Y_MIN, HMC5883L_ST_Y_MAX, myp, "pos Y") &&
            hmc5883lEvaluateSelfTest(HMC5883L_ST_Z_MIN, HMC5883L_ST_Z_MAX, mzp, "pos Z") &&
            hmc5883lEvaluateSelfTest(-HMC5883L_ST_X_MAX, -HMC5883L_ST_X_MIN, mxn, "neg X") &&
            hmc5883lEvaluateSelfTest(-HMC5883L_ST_Y_MAX, -HMC5883L_ST_Y_MIN, myn, "neg Y") &&
            hmc5883lEvaluateSelfTest(-HMC5883L_ST_Z_MAX, -HMC5883L_ST_Z_MIN, mzn, "neg Z")) {
        DEBUG_PRINTD("hmc5883l Self test [OK].\n");
    } else {
        testStatus = false;
    }

    // Restore registers
    if (i2cdevWriteReg8(I2Cx, devAddr, HMC5883L_RA_CONFIG_A, sizeof(regSave), (uint8_t *)&regSave) == false) {
        // TODO: error handling
        return false;
    }

    return testStatus;
}

bool hmc5883lEvaluateSelfTest(int16_t min, int16_t max, int16_t value, char *string)
{
    if (value < min || value > max) {
        DEBUG_PRINTD("Self test %s [FAIL]. low: %d, high: %d, measured: %d\n",
                     string, min, max, value);
        return false;
    }

    return true;
}

uint8_t hmc5883lGetSampleAveraging()
{
    i2cdevReadBits(I2Cx, devAddr, HMC5883L_RA_CONFIG_A, HMC5883L_CRA_AVERAGE_BIT, HMC5883L_CRA_AVERAGE_LENGTH, buffer);
    return buffer[0];
}

void hmc5883lSetSampleAveraging(uint8_t averaging)
{
    i2cdevWriteBits(I2Cx, devAddr, HMC5883L_RA_CONFIG_A, HMC5883L_CRA_AVERAGE_BIT, HMC5883L_CRA_AVERAGE_LENGTH, averaging);
}

uint8_t hmc5883lGetDataRate()
{
    i2cdevReadBits(I2Cx, devAddr, HMC5883L_RA_CONFIG_A, HMC5883L_CRA_RATE_BIT, HMC5883L_CRA_RATE_LENGTH, buffer);
    return buffer[0];
}

void hmc5883lSetDataRate(uint8_t rate)
{
    i2cdevWriteBits(I2Cx, devAddr, HMC5883L_RA_CONFIG_A, HMC5883L_CRA_RATE_BIT, HMC5883L_CRA_RATE_LENGTH, rate);
}

uint8_t hmc5883lGetMeasurementBias()
{
    i2cdevReadBits(I2Cx, devAddr, HMC5883L_RA_CONFIG_A, HMC5883L_CRA_BIAS_BIT, HMC5883L_CRA_BIAS_LENGTH, buffer);
    return buffer[0];
}

void hmc5883lSetMeasurementBias(uint8_t bias)
{
    i2cdevWriteBits(I2Cx, devAddr, HMC5883L_RA_CONFIG_A, HMC5883L_CRA_BIAS_BIT, HMC5883L_CRA_BIAS_LENGTH, bias);
}

// CONFIG_B register

uint8_t hmc5883lGetGain()
{
    i2cdevReadBits(I2Cx, devAddr, HMC5883L_RA_CONFIG_B, HMC5883L_CRB_GAIN_BIT, HMC5883L_CRB_GAIN_LENGTH, buffer);
    return buffer[0];
}

void hmc5883lSetGain(uint8_t gain)
{

    i2cdevWriteByte(I2Cx, devAddr, HMC5883L_RA_CONFIG_B, gain << (HMC5883L_CRB_GAIN_BIT - HMC5883L_CRB_GAIN_LENGTH + 1));
}

uint8_t hmc5883lGetMode()
{
    i2cdevReadBits(I2Cx, devAddr, HMC5883L_RA_MODE, HMC5883L_MODEREG_BIT, HMC5883L_MODEREG_LENGTH, buffer);
    return buffer[0];
}

void hmc5883lSetMode(uint8_t newMode)
{

    i2cdevWriteByte(I2Cx, devAddr, HMC5883L_RA_MODE, mode << (HMC5883L_MODEREG_BIT - HMC5883L_MODEREG_LENGTH + 1));
    mode = newMode;// track to tell if we have to clear bit 7 after a read
}

// DATA* registers

void hmc5883lGetHeading(int16_t *x, int16_t *y, int16_t *z)
{
    i2cdevReadReg8(I2Cx, devAddr, HMC5883L_RA_DATAX_H, 6, buffer);

    if (mode == HMC5883L_MODE_SINGLE) {
        i2cdevWriteByte(I2Cx, devAddr, HMC5883L_RA_MODE, HMC5883L_MODE_SINGLE << (HMC5883L_MODEREG_BIT - HMC5883L_MODEREG_LENGTH + 1));
    }

    *x = (((int16_t)buffer[0]) << 8) | buffer[1];
    *y = (((int16_t)buffer[4]) << 8) | buffer[5];
    *z = (((int16_t)buffer[2]) << 8) | buffer[3];
}

int16_t hmc5883lGetHeadingX()
{
    // each axis read requires that ALL axis registers be read, even if only
    // one is used; this was not done ineffiently in the code by accident
    i2cdevReadReg8(I2Cx, devAddr, HMC5883L_RA_DATAX_H, 6, buffer);

    if (mode == HMC5883L_MODE_SINGLE) {
        i2cdevWriteByte(I2Cx, devAddr, HMC5883L_RA_MODE, HMC5883L_MODE_SINGLE << (HMC5883L_MODEREG_BIT - HMC5883L_MODEREG_LENGTH + 1));
    }

    return (((int16_t)buffer[0]) << 8) | buffer[1];
}

int16_t hmc5883lGetHeadingY()
{
    // each axis read requires that ALL axis registers be read, even if only
    // one is used; this was not done ineffiently in the code by accident
    i2cdevReadReg8(I2Cx, devAddr, HMC5883L_RA_DATAX_H, 6, buffer);

    if (mode == HMC5883L_MODE_SINGLE) {
        i2cdevWriteByte(I2Cx, devAddr, HMC5883L_RA_MODE, HMC5883L_MODE_SINGLE << (HMC5883L_MODEREG_BIT - HMC5883L_MODEREG_LENGTH + 1));
    }

    return (((int16_t)buffer[4]) << 8) | buffer[5];
}

int16_t hmc5883lGetHeadingZ()
{
    // each axis read requires that ALL axis registers be read, even if only
    // one is used; this was not done ineffiently in the code by accident
    i2cdevReadReg8(I2Cx, devAddr, HMC5883L_RA_DATAX_H, 6, buffer);

    if (mode == HMC5883L_MODE_SINGLE) {
        i2cdevWriteByte(I2Cx, devAddr, HMC5883L_RA_MODE, HMC5883L_MODE_SINGLE << (HMC5883L_MODEREG_BIT - HMC5883L_MODEREG_LENGTH + 1));
    }

    return (((int16_t)buffer[2]) << 8) | buffer[3];
}

// STATUS register

bool hmc5883lGetLockStatus()
{
    i2cdevReadBit(I2Cx, devAddr, HMC5883L_RA_STATUS, HMC5883L_STATUS_LOCK_BIT, buffer);
    return buffer[0];
}

bool hmc5883lGetReadyStatus()
{
    i2cdevReadBit(I2Cx, devAddr, HMC5883L_RA_STATUS, HMC5883L_STATUS_READY_BIT, buffer);
    return buffer[0];
}

// ID_* registers

uint8_t hmc5883lGetIDA()
{
    i2cdevReadByte(I2Cx, devAddr, HMC5883L_RA_ID_A, buffer);
    return buffer[0];
}

uint8_t hmc5883lGetIDB()
{
    i2cdevReadByte(I2Cx, devAddr, HMC5883L_RA_ID_B, buffer);
    return buffer[0];
}

uint8_t hmc5883lGetIDC()
{
    i2cdevReadByte(I2Cx, devAddr, HMC5883L_RA_ID_C, buffer);
    return buffer[0];
}
