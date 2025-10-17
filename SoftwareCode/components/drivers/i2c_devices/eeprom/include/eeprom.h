#ifndef EERROM_H
#define EERROM_H

#include <stdbool.h>
#include "i2cdev.h"

#define EEPROM_IN_FLASH_ADDR  0x3d2000 //TODO: CHANGE 20200106 NEED TO TEST
#define EEPROM_I2C_ADDR     0x50 //dont have a eeprom just for latency
#define EEPROM_SIZE         0x1FFF   //EEPROM_IN_FLASH_ADDR


bool eepromInit(I2C_Dev *i2cPort);

bool eepromTest(void);

bool eepromTestConnection(void);

bool eepromReadBuffer(uint8_t *buffer, size_t readAddr, size_t len);

bool eepromWriteBuffer(uint8_t *buffer, size_t writeAddr, size_t len);

// TODO
bool eepromWritePage(uint8_t *buffer, uint16_t writeAddr);

#endif // EERROM_H
