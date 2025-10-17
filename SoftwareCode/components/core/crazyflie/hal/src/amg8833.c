#include "amg8833.h"

const float AMG88xx_TEMP_CONVERSION = 0.25;
const float AMG88xx_THRM_CONVERSION = 0.0625;

static uint8_t mode = 1;

bool begin(AMG8833_Dev_t *dev, I2C_Dev *I2Cx)
{
  // Set I2C parameters
  dev->I2Cx = I2Cx;
  dev->devAddr = AMG88xx_ADDRESS;
  bool i2c_complete = i2cdevInit(dev->I2Cx);
  // Enter normal mode
  bool mode_selected = write8(dev, AMG88xx_PCTL, AMG88xx_NORMAL_MODE);
  // Software reset
  bool software_resetted = write8(dev, AMG88xx_RST, AMG88xx_INITIAL_RESET);
  //disable interrupts by default
  bool interrupts_set = disableInterrupt(dev);
  //set to 10 FPS
  bool fps_set = write8(dev, AMG88xx_FPSC, (AMG88xx_FPS_10 & 0x01));
  vTaskDelay(M2T(10));
  return i2c_complete && mode_selected && software_resetted &&
    interrupts_set && fps_set;
}

void readPixels(AMG8833_Dev_t *dev, float *buf, uint8_t size)
{
  uint16_t recast;
  float converted;
  uint8_t bytesToRead = min((uint8_t) (size << 1), (uint8_t) (AMG88xx_PIXEL_ARRAY_SIZE << 1));
  uint8_t rawArray[bytesToRead];
  read(dev, AMG88xx_PIXEL_OFFSET, rawArray, bytesToRead);

  for (int i = 0; i < size; i++) {
    uint8_t pos = i << 1;
    recast = ((uint16_t) rawArray[pos + 1] << 8) | ((uint16_t) rawArray[pos]);
    converted = int12ToFloat(recast) * AMG88xx_TEMP_CONVERSION;
    buf[i] = converted;
  }
}

float readThermistor(AMG8833_Dev_t *dev)
{
  uint8_t raw[2];
  read(dev, AMG88xx_TTHL, raw, 2);
  uint16_t recast = ((uint16_t) raw[1] << 8) | ((uint16_t) raw[0]);
  return signedMag12ToFloat(recast) * AMG88xx_THRM_CONVERSION;
}
bool enableInterrupt(AMG8833_Dev_t *dev)
{
  // 0 = Difference interrupt mode
  // 1 = absolute value interrupt mode
  return write8(dev, AMG88xx_INTC, (mode << 1 | 1) & 0x03);
}

bool disableInterrupt(AMG8833_Dev_t *dev)
{
  // 0 = Difference interrupt mode
  // 1 = absolute value interrupt mode
  return write8(dev, AMG88xx_INTC, (mode << 1 | 0) & 0x03);
}

void setInterruptMode(AMG8833_Dev_t *dev, uint8_t m)
{
  mode = m;
  write8(dev, AMG88xx_INTC, (mode << 1 | 1) & 0x03);
}

void getInterrupt(AMG8833_Dev_t *dev, uint8_t *buf, uint8_t size)
{
  uint8_t bytesToRead = min(size, (uint8_t) 8);
  read(dev, AMG88xx_INT_OFFSET, buf, bytesToRead);
}

void clearInterrupt(AMG8833_Dev_t *dev)
{
  write8(dev, AMG88xx_RST, AMG88xx_FLAG_RESET);
}

void setInterruptLevels_N(AMG8833_Dev_t *dev, float high, float low)
{
  setInterruptLevels_H(dev, high, low, high * 0.95f);
}

void setInterruptLevels_H(AMG8833_Dev_t *dev, float high, float low,
  float hysteresis)
{
  int highConv = high / AMG88xx_TEMP_CONVERSION;
  highConv = constrain(highConv, -4095, 4095);
  write8(dev, AMG88xx_INTHL, (highConv & 0xFF));
  write8(dev, AMG88xx_INTHH, ((highConv & 0xF) >> 4));

  int lowConv = low / AMG88xx_TEMP_CONVERSION;
  lowConv = constrain(lowConv, -4095, 4095);
  write8(dev, AMG88xx_INTLL, (lowConv & 0xFF));
  write8(dev, AMG88xx_INTLH, (((lowConv & 0xF) >> 4) & 0xF));

  int hysConv = hysteresis / AMG88xx_TEMP_CONVERSION;
  hysConv = constrain(hysConv, -4095, 4095);
  write8(dev, AMG88xx_IHYSL, (hysConv & 0xFF));
  write8(dev, AMG88xx_IHYSH, (((hysConv & 0xF) >> 4) & 0xF));
}

void setMovingAverageMode(AMG8833_Dev_t *dev, bool mode)
{
  write8(dev, AMG88xx_AVE, (mode << 5));
}

uint8_t read8(AMG8833_Dev_t *dev, uint8_t reg)
{
  uint8_t ret;
  read(dev, reg, &ret, 1);
  return ret;
}

void read(AMG8833_Dev_t *dev, uint8_t reg, uint8_t *buf, uint8_t num)
{
  i2cdevReadReg8(dev->I2Cx, dev->devAddr, reg, num, buf);
}

bool write8(AMG8833_Dev_t *dev, uint16_t reg, uint8_t value)
{
  return (i2cdevWrite16(dev->I2Cx, dev->devAddr, reg, 1, &value));
}

void write(AMG8833_Dev_t *dev, uint8_t reg, uint8_t *buf, uint8_t num)
{
  for (int i = 0; i < num; i++) {
    write8(dev, reg, buf[i]);
  }
}

float signedMag12ToFloat(uint16_t val)
{
  // Take first 11 bits as absolute val
  uint16_t absVal = (val & 0x7FF);
  return (val & 0x800) ? 0 - (float) absVal : (float) absVal;
}

float int12ToFloat(uint16_t val)
{
  // Shift to left so that sign bit of 12 bit integer number is placed on
  // sign bit of 16 bit signed integer number
  int16_t sVal = (val << 4);
  // Shift back the signed number, return converts to float
  return sVal >> 4;
}

uint8_t min(uint8_t a, uint8_t b)
{
  return (a < b) ? a : b;
}
