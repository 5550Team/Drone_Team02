
#include <string.h>

#include "tdoaStats.h"

#define STATS_INTERVAL 500


void tdoaStatsInit(tdoaStats_t* tdoaStats, uint32_t now_ms) {
  memset(tdoaStats, 0, sizeof(tdoaStats_t));
  tdoaStats->remoteAnchorId = tdoaStats->newRemoteAnchorId = 1;

  tdoaStats->nextStatisticsTime = now_ms + STATS_INTERVAL;
  tdoaStats->previousStatisticsTime = 0;

  STATS_CNT_RATE_INIT(&tdoaStats->packetsReceived, STATS_INTERVAL);
  STATS_CNT_RATE_INIT(&tdoaStats->packetsToEstimator, STATS_INTERVAL);
  STATS_CNT_RATE_INIT(&tdoaStats->clockCorrectionCount, STATS_INTERVAL);
  STATS_CNT_RATE_INIT(&tdoaStats->contextHitCount, STATS_INTERVAL);
  STATS_CNT_RATE_INIT(&tdoaStats->contextMissCount, STATS_INTERVAL);
  STATS_CNT_RATE_INIT(&tdoaStats->timeIsGood, STATS_INTERVAL);
  STATS_CNT_RATE_INIT(&tdoaStats->suitableDataFound, STATS_INTERVAL);
}

void tdoaStatsUpdate(tdoaStats_t* tdoaStats, uint32_t now_ms) {
  if (now_ms > tdoaStats->nextStatisticsTime) {
    if (tdoaStats->anchorId != tdoaStats->newAnchorId) {
      tdoaStats->anchorId = tdoaStats->newAnchorId;

      // Reset anchor stats
      tdoaStats->clockCorrection = 0.0;
      tdoaStats->tof = 0;
      tdoaStats->tdoa = 0;
    }

    if (tdoaStats->remoteAnchorId != tdoaStats->newRemoteAnchorId) {
      tdoaStats->remoteAnchorId = tdoaStats->newRemoteAnchorId;

      // Reset remote anchor stats
      tdoaStats->tof = 0;
      tdoaStats->tdoa = 0;
    }

    tdoaStats->previousStatisticsTime = now_ms;
    tdoaStats->nextStatisticsTime = now_ms + STATS_INTERVAL;
  }
}
