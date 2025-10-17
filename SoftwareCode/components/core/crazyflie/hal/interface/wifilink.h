#ifndef __WIFILINK_H__
#define __WIFILINK_H__

#include <stdbool.h>
#include "crtp.h"

#define SYSLINK_MTU 64

#define CRTP_START_BYTE  0xAA
#define SYSLINK_START_BYTE1 0xBC
#define SYSLINK_START_BYTE2 0xCF

void wifilinkInit();
bool wifilinkTest();
struct crtpLinkOperations *wifilinkGetLink();

#endif
