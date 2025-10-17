#include <stdint.h>

#include "freertos/FreeRTOSConfig.h"
#include "FreeRTOS.h"
#include "task.h"

#include "config.h"
#define DEBUG_MODULE "FreeRTOSConfig"
#include "debug_cf.h"
#include "nvicconf.h"
#include "led.h"
#include "power_distribution.h"

uint32_t traceTickCount;


#ifdef UART_OUTPUT_TRACE_DATA
void debugSendTraceInfo(unsigned int taskNbr)
{
  uint32_t traceData;
  traceData = (taskNbr << 29) | (((traceTickCount << 16) + TIM1->CNT) & 0x1FFFFFF);
  uartSendDataDma(sizeof(traceData), (uint8_t*)&traceData);
}

void debugInitTrace(void)
{

}
#else
void debugSendTraceInfo(unsigned int taskNbr)
{
}
#endif
