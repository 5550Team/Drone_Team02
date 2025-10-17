#ifndef __TRIGGER_H__
#define __TRIGGER_H__

#include <stdint.h>
#include <stdbool.h>

/* Trigger function type. */
typedef enum {
  triggerFuncNone   = 0, /* No trigger function. */
  triggerFuncIsLE   = 1, /* Increases testCounter if test value is Less than or Equal to threshold value. */
  triggerFuncIsGE   = 2, /* Increases testCounter if test value is Greater than or Equal to threshold value. */
} triggerFunc_t;

/* Trigger handler function type. */
typedef void (*triggerHandler_t)(void *);

typedef struct {
    bool active;              /* Indicates if the trigger is active or not. */
    triggerFunc_t func;       /* The trigger function type. */
    float threshold;          /* The threshold the test value is compared against using the trigger function. */
    uint32_t triggerCount;    /* When testCounter reaches this value, a trigger is released. */
    triggerHandler_t handler; /* If registered, the handler function is called when the trigger is released. */
    bool handlerCalled;       /* Indicates if the handler has been called. The handler is only called once after the trigger is released. */
    void *handlerArg;         /* The argument to pass to the handler function. */
    uint32_t testCounter;     /* As long as smaller than triggerCount, this counter is incremented for each consecutive test the tested value is within the threshold. */
    bool released;            /* Indicates if the trigger has been released (true) or not (false). */
} trigger_t;

/* Trigger functionality. */
void triggerInit(trigger_t *trigger, triggerFunc_t func, float threshold, uint32_t triggerCount);
void triggerRegisterHandler(trigger_t *trigger, triggerHandler_t handler, void *handlerArg);
void triggerDeInit(trigger_t *trigger);
void triggerActivate(trigger_t *trigger, bool active);
void triggerReset(trigger_t *trigger);
bool triggerTestValue(trigger_t *trigger, float testValue);

#endif
