/**
 * ow.h - One-wire functions
 */
#ifndef __OW_H__
#define __OW_H__

#include <stdint.h>
#include "syslink.h"

#define OW_MAX_SIZE        112
#define OW_READ_SIZE       29
#define OW_MAX_WRITE_SIZE  26 // Use even numbers because of 16bits segments

typedef struct owCommand_s {
  uint8_t nmem;
  union {
    struct {
      uint8_t memId[8];
    } __attribute__((packed)) info;
    struct  {
      uint16_t address;
      uint8_t data[29];
    } __attribute__((packed)) read;
    struct write {
      uint16_t address;
      uint16_t length;
      char data[27];
    } __attribute__((packed)) write;
  };
} __attribute__((packed)) OwCommand;

typedef struct owSerialNum_s
{
  union {
    struct {
      uint8_t type;
      uint8_t id[6];
      uint8_t crc;
    };
    uint8_t data[8];
  };
} OwSerialNum;

void owInit();
bool owTest();
void owCommonInit();
bool owCommonTest();
void owSyslinkRecieve(SyslinkPacket *slp);
bool owScan(uint8_t *nMem);
bool owGetinfo(uint8_t selectMem, OwSerialNum *serialNum);
bool owRead(uint8_t selectMem, uint16_t address, uint8_t length, uint8_t *data);
bool owWrite(uint8_t selectMem, uint16_t address, uint8_t length, const uint8_t *data);

#endif //__OW_H__
