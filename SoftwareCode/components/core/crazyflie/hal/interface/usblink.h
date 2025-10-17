#ifndef __USBLINK_H__
#define __USBLINK_H__

#include <stdbool.h>
#include "crtp.h"


#define CRTP_START_BYTE  0xAA
#define SYSLINK_START_BYTE1 0xBC
#define SYSLINK_START_BYTE2 0xCF


void usblinkInit();
bool usblinkTest();
struct crtpLinkOperations * usblinkGetLink();

#endif
