
#ifndef CRTP_COMMANDER_H_
#define CRTP_COMMANDER_H_

#include <stdint.h>
#include "stabilizer_types.h"
#include "crtp.h"

typedef enum {
    STABILIZE_MODE        = 0x00,
    ALTHOLD_MODE          = 0x01,
    POSHOLD_MODE          = 0x02,
    POSSET_MODE           = 0x03,  
}FlightMode;

void crtpCommanderInit(void);
void crtpCommanderRpytDecodeSetpoint(setpoint_t *setpoint, CRTPPacket *pk);
void crtpCommanderGenericDecodeSetpoint(setpoint_t *setpoint, CRTPPacket *pk);
void setCommandermode(FlightMode mode);

#endif /* CRTP_COMMANDER_H_ */
