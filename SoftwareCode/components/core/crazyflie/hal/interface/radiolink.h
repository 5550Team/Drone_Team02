/*
 * radiolink.c - Radio link layer
 */

#ifndef __RADIO_H__
#define __RADIO_H__

#include <stdint.h>
#include <stdbool.h>
#include "syslink.h"

#define P2P_MAX_DATA_SIZE 60

typedef struct _P2PPacket
{
  uint8_t size;                         //< Size of data
  uint8_t rssi;                         //< Received Signal Strength Intensity
  union {
    struct {
      uint8_t port;                 //< Header selecting channel and port
      uint8_t data[P2P_MAX_DATA_SIZE]; //< Data
    };
    uint8_t raw[P2P_MAX_DATA_SIZE+1];  //< The full packet "raw"
  };
} __attribute__((packed)) P2PPacket;

typedef void (*P2PCallback)(P2PPacket *);

void radiolinkInit(void);
bool radiolinkTest(void);
void radiolinkSetChannel(uint8_t channel);
void radiolinkSetDatarate(uint8_t datarate);
void radiolinkSetAddress(uint64_t address);
void radiolinkSetPowerDbm(int8_t powerDbm);
void radiolinkSyslinkDispatch(SyslinkPacket *slp);
struct crtpLinkOperations * radiolinkGetLink();
bool radiolinkSendP2PPacketBroadcast(P2PPacket *p2pp);
void p2pRegisterCB(P2PCallback cb);


#endif //__RADIO_H__
