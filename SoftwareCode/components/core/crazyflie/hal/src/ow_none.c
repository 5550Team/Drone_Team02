#define DEBUG_MODULE "OW"

#include  <string.h>

#include "FreeRTOS.h"
#include "semphr.h"

#include "ow.h"


void owInit()
{
  owCommonInit();
}

bool owTest()
{
  return owCommonTest();
}

void owSyslinkRecieve(SyslinkPacket *slp)
{
}

bool owScan(uint8_t *nMem)
{
  return true;
}

bool owGetinfo(uint8_t selectMem, OwSerialNum *serialNum)
{
  return false;
}

bool owRead(uint8_t selectMem, uint16_t address, uint8_t length, uint8_t *data)
{
  return false;
}

bool owWrite(uint8_t selectMem, uint16_t address, uint8_t length, const uint8_t *data)
{
  return false;
}
