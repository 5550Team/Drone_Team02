#ifndef _ZRANGER_H_
#define _ZRANGER_H_

#include "stabilizer_types.h"
//#include "deck_core.h"

void zRangerInit(void);//DeckInfo* info);

bool zRangerTest(void);
void zRangerTask(void* arg);

bool zRangerReadRange(zDistance_t* zrange, const uint32_t tick);

int16_t zRangerMeasurementAndRestart();

#endif /* _ZRANGER_H_ */
