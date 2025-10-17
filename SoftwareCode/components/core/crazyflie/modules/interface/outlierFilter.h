#ifndef __OUTLIER_FILTER_H__
#define __OUTLIER_FILTER_H__

#include "stabilizer_types.h"

bool outlierFilterValidateTdoaSimple(const tdoaMeasurement_t* tdoa);
bool outlierFilterValidateTdoaSteps(const tdoaMeasurement_t* tdoa, const float error, const vector_t* jacobian, const point_t* estPos);

typedef struct {
    uint32_t openingTime;
    int32_t openingWindow;
} OutlierFilterLhState_t;
bool outlierFilterValidateLighthouseSweep(OutlierFilterLhState_t* this, const float distanceToBs, const float angleError, const uint32_t now);
void outlierFilterReset(OutlierFilterLhState_t* this, const uint32_t now);


#endif // __OUTLIER_FILTER_H__
