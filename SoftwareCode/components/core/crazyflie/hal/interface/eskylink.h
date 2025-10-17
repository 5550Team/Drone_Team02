/*
 * eskylink.h: esky 2.4GHz-compatible link driver
 */

#ifndef __ESKYLINK_H__
#define __ESKYLINK_H__

#include "crtp.h"

void eskylinkInit();
bool eskylinkTest();
struct crtpLinkOperations * eskylinkGetLink();
void eskylinkReInit(void);

#endif
