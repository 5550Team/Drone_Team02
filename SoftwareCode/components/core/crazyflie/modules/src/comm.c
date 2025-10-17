#include <stdbool.h>
#define DEBUG_MODULE "COMM"
#include "comm.h"
#include "config.h"

#include "crtp.h"
#include "console.h"
#include "crtpservice.h"
#include "param.h"
#include "debug_cf.h"
#include "log.h"
#include  "wifi_esp32.h"
#include "wifilink.h"
#include "platformservice.h"
#include "crtp_localization_service.h"

static bool isInit;

void commInit(void)
{
  if (isInit) {
    return;
  }


  crtpSetLink(wifilinkGetLink());
  crtpserviceInit();
  platformserviceInit();
  logInit();
  paramInit();
  isInit = true;
}

bool commTest(void)
{
	bool pass = isInit;
	pass &= wifilinkTest();
	DEBUG_PRINTI("wifilinkTest = %d ", pass);
	pass &= crtpTest();
	DEBUG_PRINTI("crtpTest = %d ", pass);
	pass &= crtpserviceTest();
	DEBUG_PRINTI("crtpserviceTest = %d ", pass);
	pass &= platformserviceTest();
	DEBUG_PRINTI("platformserviceTest = %d ", pass);
	pass &= consoleTest();
	DEBUG_PRINTI("consoleTest = %d ", pass);
	pass &= paramTest();
	DEBUG_PRINTI("paramTest = %d ", pass);
  return pass;
}

