#include <stdint.h>
#include <stdlib.h>

#include "stm32_legacy.h"
#include "param.h"
#include "buzzer.h"
#include "piezo.h"

static bool isInit;

static void buzzDeckOn(uint32_t freq)
{
    piezoSetRatio(128);
    piezoSetFreq(freq);
}

static void buzzDeckOff()
{
    piezoSetRatio(0);
}

static struct buzzerControl buzzDeckCtrl = {
    .on         = buzzDeckOn,
    .off        = buzzDeckOff
};

void buzzDeckInit()
{
    if (isInit) {
        return;
    }

    piezoInit();
    buzzerSetControl(&buzzDeckCtrl);

    isInit = true;
}


PARAM_GROUP_START(deck)
PARAM_ADD(PARAM_UINT8 | PARAM_RONLY, bcBuzzer, &isInit)
PARAM_GROUP_STOP(deck)
