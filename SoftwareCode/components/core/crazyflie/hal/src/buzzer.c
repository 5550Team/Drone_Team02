#define DEBUG_MODULE "BUZZER"

#include "buzzer.h"
//#include "buzzdeck.h"
#include "sdkconfig.h"

static struct buzzerControl * ctrl;

extern void buzzDeckInit();

void buzzerInit()
{
#ifdef CONFIG_BUZZER_ON
    buzzDeckInit();
#endif
}

bool buzzerTest()
{
  return true;
}

void buzzerOff()
{
  if (ctrl)
    ctrl->off();
}

void buzzerOn(uint32_t freq)
{
  if (ctrl)
    ctrl->on(freq);
}

void buzzerSetControl(struct buzzerControl * bc)
{
  ctrl = bc;
}
