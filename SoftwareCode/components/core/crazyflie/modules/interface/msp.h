#ifndef MSP_H_
#define MSP_H_
#include <stdint.h>
#include <stdbool.h>


typedef void (*MspResponseCallback)(uint8_t* pBuffer, uint32_t bufferLen);


typedef struct _MspHeader
{
  uint8_t preamble[2];    // Should always be '$M'
  uint8_t direction;
  uint8_t size;
  uint8_t command;
}__attribute__((packed)) MspHeader;


typedef struct
{
  // For storing MSP request data/state
  MspHeader requestHeader;
  uint8_t requestCrc;
  uint8_t requestState;

  // Response context
  uint8_t mspResponse[256];
  // Size of the complete response message contained
  // in the mspResponse buffer
  uint16_t mspResponseSize;

  // The client callback to be invoked
  // when a response message is ready
  MspResponseCallback responseCallback;

} MspObject;


void mspInit(MspObject* pMspObject, const MspResponseCallback callback);


void mspProcessByte(MspObject* pMspObject, const uint8_t data);

#endif /* MSP_H_ */
