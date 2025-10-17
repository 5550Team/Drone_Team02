#ifndef __PIEZO_H__
#define __PIEZO_H__

#include <stdint.h>
#include <stdbool.h>

#include "config.h"

void piezoInit();

bool piezoTest(void);

void piezoSetRatio(uint8_t ratio);

void piezoSetFreq(uint16_t freq);

#endif 
