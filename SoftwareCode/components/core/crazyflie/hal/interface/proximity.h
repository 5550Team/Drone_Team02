/*
 * proximity.h - Interface to hardware abstraction layer for proximity sensors
 */

#ifndef __PROXIMITY_H__
#define __PROXIMITY_H__

#include <stdint.h>

#define PROXIMITY_TASK_FREQ 20

#define PROXIMITY_SWIN_SIZE 9


void proximityInit(void);

uint32_t proximityGetDistance(void);

uint32_t proximityGetDistanceAvg(void);

uint32_t proximityGetDistanceMedian(void);

uint32_t proximityGetAccuracy(void);

#endif
