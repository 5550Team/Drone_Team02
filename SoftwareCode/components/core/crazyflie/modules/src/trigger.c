
#include <stddef.h>
#include <stdlib.h>
#include "stm32_legacy.h"
#include "trigger.h"

void triggerInit(trigger_t *trigger, triggerFunc_t func, float threshold, uint32_t triggerCount)
{
  assert_param(trigger != NULL);
  assert_param(func != triggerFuncNone);

  trigger->active = false;
  trigger->func = func;
  trigger->threshold = threshold;
  trigger->triggerCount = triggerCount;

  triggerReset(trigger);
}

void triggerRegisterHandler(trigger_t *trigger, triggerHandler_t handler, void *handlerArg)
{
  assert_param(trigger != NULL);
  assert_param(handler != NULL);

  trigger->handler = handler;
  trigger->handlerArg = handlerArg;
  trigger->handlerCalled = false;
}

void triggerDeInit(trigger_t *trigger)
{
  assert_param(trigger != NULL);

  triggerInit(trigger, triggerFuncNone, 0, 0);
  triggerRegisterHandler(trigger, NULL, NULL);
  triggerReset(trigger);
}

void triggerActivate(trigger_t *trigger, bool active)
{
  assert_param(trigger != NULL);

  triggerReset(trigger);
  trigger->active = active;
}

void triggerReset(trigger_t *trigger)
{
  assert_param(trigger != NULL);

  trigger->testCounter = 0;
  trigger->released = false;
  trigger->handlerCalled = false;
}

static void triggerIncTestCounter(trigger_t *trigger)
{
  assert_param(trigger != NULL);

   if(trigger->testCounter < trigger->triggerCount) {
     trigger->testCounter++;
   }
}

bool triggerTestValue(trigger_t *trigger, float testValue)
{
  assert_param(trigger != NULL);

  /* Do not do anything if the trigger has been deactivated. */
  if(!trigger->active) {
    return false;
  }

  switch(trigger->func) {
    case triggerFuncIsLE: {
      if(testValue <= trigger->threshold) {
        triggerIncTestCounter(trigger);
        break;
      }
      else {
        /* Reset the trigger if the test failed. */
        triggerReset(trigger);
        return false;
      }
    }
    case triggerFuncIsGE: {
      if(testValue >= trigger->threshold) {
        triggerIncTestCounter(trigger);
        break;
      }
      else {
        /* Reset the trigger if the test failed. */
        triggerReset(trigger);
        return false;
      }
    }
    case triggerFuncNone: {
      /* No action, included to avoid compiler warnings. */
      break;
    }
  }

  /* Check if the triggerCount has been reached. */
  trigger->released = (trigger->testCounter >= trigger->triggerCount);

  /* If the trigger has not been release, exit immediately. */
  if(!trigger->released) {
    return false;
  }

  /* The trigger object may be reset by the handler, thus make an internal copy of the released flag. */
  bool iReleased = trigger->released;
  if(trigger->released && (trigger->handler != NULL) && (!trigger->handlerCalled)) {
    /* Set the handlerCalled = true before calling, since the handler may choose to reset the object. */
    trigger->handlerCalled = true;
    trigger->handler(trigger->handlerArg);
  }

  /* Return the release flag. This may return the value true more than once. */
  return iReleased;
}
