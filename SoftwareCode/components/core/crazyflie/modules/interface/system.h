

#ifndef __SYSTEM_H__
#define __SYSTEM_H__

#include <stdbool.h>
#include <stdint.h>

void systemInit(void);
bool systemTest(void);

void systemLaunch(void);


void systemStart();
void systemWaitStart(void);
void systemSetCanFly(bool val);
bool systemCanFly(void);
void systemSetArmed(bool val);
bool systemIsArmed();

#endif //__SYSTEM_H__
