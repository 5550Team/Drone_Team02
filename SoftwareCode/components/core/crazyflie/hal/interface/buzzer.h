
#ifndef __BUZZER_H__
#define __BUZZER_H__
 
#include <stdint.h>
#include <stdbool.h>

/** Functionpointers used to control the buzzer */
struct buzzerControl
{
  void (*off)();
  void (*on)(uint32_t freq);
};

/**
 * Initilize the buzzer sub-system.
 */
void buzzerInit();

/**
 * Test the buzzer sub-system.
 */
bool buzzerTest();

/**
 * Turn the buzzer off.
 */
void buzzerOff();

/**
 * Turn the buzzer on and set it to a specific frequency (if supported).
 */
void buzzerOn(uint32_t freq);

/**
 * Set function pointers for controlling the buzzer hardware.
 */
void buzzerSetControl(struct buzzerControl * bc);

#endif //__BUZZER_H__

