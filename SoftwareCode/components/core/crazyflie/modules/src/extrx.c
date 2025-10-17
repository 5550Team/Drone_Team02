#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"


#include "stm32_legacy.h"
#include "config.h"
#include "system.h"
#include "nvicconf.h"
#include "commander.h"
#include "uart1.h"
#include "cppm.h"

#define DEBUG_MODULE  "EXTRX"
#include "debug_cf.h"
#include "log.h"
#include "static_mem.h"

//#define ENABLE_CPPM
#define ENABLE_EXTRX_LOG


#define EXTRX_NR_CHANNELS  8

#define EXTRX_CH_TRUST     2
#define EXTRX_CH_ROLL      0
#define EXTRX_CH_PITCH     1
#define EXTRX_CH_YAW       3

#define EXTRX_SIGN_ROLL    (-1)
#define EXTRX_SIGN_PITCH   (-1)
#define EXTRX_SIGN_YAW     (-1)

#define EXTRX_SCALE_ROLL   (40.0f)
#define EXTRX_SCALE_PITCH  (40.0f)
#define EXTRX_SCALE_YAW    (400.0f)

static setpoint_t extrxSetpoint;
static uint16_t ch[EXTRX_NR_CHANNELS];

static void extRxTask(void *param);
static void extRxDecodeCppm(void);
static void extRxDecodeChannels(void);

STATIC_MEM_TASK_ALLOC(extRxTask, EXTRX_TASK_STACKSIZE);

void extRxInit(void)
{

  extrxSetpoint.mode.roll = modeAbs;
  extrxSetpoint.mode.pitch = modeAbs;
  extrxSetpoint.mode.yaw = modeVelocity;

#ifdef ENABLE_CPPM
  cppmInit();
#endif

#ifdef ENABLE_SPEKTRUM
  uart1Init();
#endif

  STATIC_MEM_TASK_CREATE(extRxTask, extRxTask, EXTRX_TASK_NAME, NULL, EXTRX_TASK_PRI);
}

static void extRxTask(void *param)
{

  //Wait for the system to be fully started
  systemWaitStart();

  while (true)
  {
    extRxDecodeCppm();
  }
}

static void extRxDecodeChannels(void)
{
  extrxSetpoint.thrust = cppmConvert2uint16(ch[EXTRX_CH_TRUST]);
  extrxSetpoint.attitude.roll = EXTRX_SIGN_ROLL * cppmConvert2Float(ch[EXTRX_CH_ROLL], -EXTRX_SCALE_ROLL, EXTRX_SCALE_ROLL);
  extrxSetpoint.attitude.pitch = EXTRX_SIGN_PITCH * cppmConvert2Float(ch[EXTRX_CH_PITCH], -EXTRX_SCALE_PITCH, EXTRX_SCALE_PITCH);
  extrxSetpoint.attitude.yaw = EXTRX_SIGN_YAW * cppmConvert2Float(ch[EXTRX_CH_YAW], -EXTRX_SCALE_YAW, EXTRX_SCALE_YAW);
  commanderSetSetpoint(&extrxSetpoint, COMMANDER_PRIORITY_EXTRX);
}

static void extRxDecodeCppm(void)
{
  uint16_t ppm;
  static uint8_t currChannel = 0;

  if (cppmGetTimestamp(&ppm) == pdTRUE)
  {
    if (cppmIsAvailible() && ppm < 2100)
    {
      if (currChannel < EXTRX_NR_CHANNELS)
      {
        ch[currChannel] = ppm;
      }
      currChannel++;
    }
    else
    {
      extRxDecodeChannels();
      currChannel = 0;
    }
  }
}

#if 0

#endif

/* Loggable variables */
#ifdef ENABLE_EXTRX_LOG
LOG_GROUP_START(extrx)
LOG_ADD(LOG_UINT16, ch0, &ch[0])
LOG_ADD(LOG_UINT16, ch1, &ch[1])
LOG_ADD(LOG_UINT16, ch2, &ch[2])
LOG_ADD(LOG_UINT16, ch3, &ch[3])
LOG_ADD(LOG_UINT16, thrust, &extrxSetpoint.thrust)
LOG_ADD(LOG_FLOAT, roll, &extrxSetpoint.attitude.roll)
LOG_ADD(LOG_FLOAT, pitch, &extrxSetpoint.attitude.pitch)
LOG_ADD(LOG_FLOAT, yaw, &extrxSetpoint.attitude.yaw)
LOG_GROUP_STOP(extrx)
#endif
