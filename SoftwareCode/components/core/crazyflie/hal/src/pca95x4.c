#define DEBUG_MODULE "PCA95X4"

#include "i2cdev.h"

#include "pca95x4.h"

#include "debug_cf.h"

static uint8_t devAddr;
static I2C_Dev *I2Cx;

void pca95x4Init()
{
  i2cdevInit(I2C1_DEV);
  I2Cx = I2C1_DEV;
  devAddr = PCA95X4_DEFAULT_ADDRESS;
}

bool pca95x4Test()
{
  uint8_t tb;
  bool pass;

  // Test reading the config register
  pass = i2cdevReadByte(I2Cx, devAddr, PCA95X4_CONFIG_REG, &tb);

  return pass;
}

bool pca95x4ConfigOutput(uint32_t val) {
  bool pass;

  pass = i2cdevWriteByte(I2Cx, devAddr, PCA95X4_CONFIG_REG, val);
  return pass;
}

bool pca95x4SetOutput(uint32_t mask) {
  uint8_t val;
  bool pass;

  pass = i2cdevReadByte(I2Cx, devAddr, PCA95X4_OUTPUT_REG, &val);
  val |= mask;
  pass = i2cdevWriteByte(I2Cx, devAddr, PCA95X4_OUTPUT_REG, val);

  return pass;
}

bool pca95x4ClearOutput(uint32_t mask) {
  uint8_t val;
  bool pass;

  pass = i2cdevReadByte(I2Cx, devAddr, PCA95X4_OUTPUT_REG, &val);
  val &= ~mask;
  pass = i2cdevWriteByte(I2Cx, devAddr, PCA95X4_OUTPUT_REG, val);

  return pass;
}
