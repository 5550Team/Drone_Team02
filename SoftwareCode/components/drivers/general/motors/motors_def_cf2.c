// Connector M1,
#include "motors.h"

static const MotorPerifDef CONN_M1 = {
    .drvType = BRUSHED,
};

// Connector M2, PB11, TIM2_CH4
static const MotorPerifDef CONN_M2 = {
    .drvType = BRUSHED,
};

// Connector M3, PA15, TIM2_CH1
static const MotorPerifDef CONN_M3 = {
    .drvType = BRUSHED,
};

// Connector M4, PB9, TIM4_CH4
static const MotorPerifDef CONN_M4 = {
    .drvType = BRUSHED,
};

/**
 * Default brushed mapping to M1-M4 connectors.
 */
const MotorPerifDef *motorMapDefaultBrushed[NBR_OF_MOTORS] = {
    &CONN_M1,
    &CONN_M2,
    &CONN_M3,
    &CONN_M4
};
