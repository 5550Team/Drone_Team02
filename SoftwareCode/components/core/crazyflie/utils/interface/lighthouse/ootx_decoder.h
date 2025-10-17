

#pragma once

#include <stdbool.h>
#include <stdint.h>


#define OOTX_MAX_FRAME_LENGTH 43

struct ootxDataFrame_s {
  uint16_t protocolVersion:6;
  uint16_t firmwareVersion:10;
  uint32_t id;
  __fp16 phase0;
  __fp16 phase1;
  __fp16 tilt0;
  __fp16 tilt1;
  uint8_t unlockCount;
  uint8_t hwVersion;
  __fp16 curve0;
  __fp16 curve1;
  int8_t accelX;
  int8_t accelY;
  int8_t accelZ;
  __fp16 gibphase0;
  __fp16 gibphase1;
  __fp16 gibmag0;
  __fp16 gibmag1;
  uint8_t mode;
  uint8_t faults;

  // Only used in LH 2 that uses a longer data block
  __fp16 ogeephase0;
  __fp16 ogeephase1;
  __fp16 ogeemag0;
  __fp16 ogeemag1;
} __attribute__((packed));

typedef struct ootxDecoderState_s {
  int frameLength;
  int bytesReceived;

  uint16_t currentWord;

  uint32_t crc32;

  int bitInWord;
  int wordReceived;
  bool synchronized;
  int nZeros;
  enum {rxLength, rxData, rxCrc0, rxCrc1, rxDone} rxState;

  union {
    uint16_t data[(OOTX_MAX_FRAME_LENGTH+1) / 2];
    struct ootxDataFrame_s frame;
  };
} ootxDecoderState_t;

bool ootxDecoderProcessBit(ootxDecoderState_t * state, int data);
