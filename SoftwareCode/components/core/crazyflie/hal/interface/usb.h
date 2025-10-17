/**
 * usb.h - USB CRTP link and raw access functions
 */
#ifndef USB_H_
#define USB_H_

#include <stdbool.h>
#include <stdint.h>

#include "usbd_conf.h"


#define USB_RX_TX_PACKET_SIZE   (64)

/* Structure used for in/out data via USB */
typedef struct
{
  uint8_t size;
  uint8_t data[USB_RX_TX_PACKET_SIZE];
} USBPacket;

void usbInit(void);

bool usbTest(void);

struct crtpLinkOperations * usbGetLink();

bool usbGetDataBlocking(USBPacket *in);

bool usbSendData(uint32_t size, uint8_t* data);


#endif /* UART_H_ */
