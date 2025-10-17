#ifndef WIFI_ESP32_H_
#define WIFI_ESP32_H_
#include <stdbool.h>
#include <stdint.h>

// 32 bytes is enough for CRTP packets (30+1) + checksum 1
#define WIFI_RX_TX_PACKET_SIZE   (32)

/* Structure used for in/out data via USB */
typedef struct
{
  uint8_t size;
  uint8_t data[WIFI_RX_TX_PACKET_SIZE];
} UDPPacket;

void wifiInit(void);

bool wifiTest(void);

bool wifiGetDataBlocking(UDPPacket *in);

bool wifiSendData(uint32_t size, uint8_t* data);

#endif