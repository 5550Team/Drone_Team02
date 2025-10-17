#ifndef __SOUND_H__
#define __SOUND_H__

#include <stdint.h>
#include <stdbool.h>

#define SND_OFF         0
#define FACTORY_TEST    1
#define SND_USB_DISC    2
#define SND_USB_CONN    3
#define SND_BAT_FULL    4
#define SND_BAT_LOW     5
#define SND_STARTUP     6
#define SND_CALIB       7


void soundInit(void);


bool soundTest(void);

void soundSetEffect(uint32_t effect);

void soundSetFreq(uint32_t freq);

#endif 

