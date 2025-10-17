#ifndef PID_H_
#define PID_H_

#include <stdbool.h>
#include "filter.h"

#ifdef CONFIG_TARGET_ESP32_S2_DRONE_V1_2
  #define PID_ROLL_RATE_KP  190.0
  #define PID_ROLL_RATE_KI  440.0
  #define PID_ROLL_RATE_KD  2.6
  #define PID_ROLL_RATE_INTEGRATION_LIMIT    33.3

  #define PID_PITCH_RATE_KP  190.0
  #define PID_PITCH_RATE_KI  440.0
  #define PID_PITCH_RATE_KD  2.6
  #define PID_PITCH_RATE_INTEGRATION_LIMIT   33.3

  #define PID_YAW_RATE_KP  120.0
  #define PID_YAW_RATE_KI  16.7
  #define PID_YAW_RATE_KD  0.0
  #define PID_YAW_RATE_INTEGRATION_LIMIT     166.7

  #define PID_ROLL_KP  5.3
  #define PID_ROLL_KI  2.5
  #define PID_ROLL_KD  0.0
  #define PID_ROLL_INTEGRATION_LIMIT    20.0

  #define PID_PITCH_KP  5.3
  #define PID_PITCH_KI  2.5
  #define PID_PITCH_KD  0.0
  #define PID_PITCH_INTEGRATION_LIMIT   20.0

  #define PID_YAW_KP  6.0
  #define PID_YAW_KI  1.0
  #define PID_YAW_KD  0.35
  #define PID_YAW_INTEGRATION_LIMIT     360.0


  #define DEFAULT_PID_INTEGRATION_LIMIT 5000.0
  #define DEFAULT_PID_OUTPUT_LIMIT      0.0
#else
  #define PID_ROLL_RATE_KP  250.0
  #define PID_ROLL_RATE_KI  500.0
  #define PID_ROLL_RATE_KD  2.5
  #define PID_ROLL_RATE_INTEGRATION_LIMIT    33.3

  #define PID_PITCH_RATE_KP  250.0
  #define PID_PITCH_RATE_KI  500.0
  #define PID_PITCH_RATE_KD  2.5
  #define PID_PITCH_RATE_INTEGRATION_LIMIT   33.3

  #define PID_YAW_RATE_KP  120.0
  #define PID_YAW_RATE_KI  16.7
  #define PID_YAW_RATE_KD  0.0
  #define PID_YAW_RATE_INTEGRATION_LIMIT     166.7

  #define PID_ROLL_KP  5.9
  #define PID_ROLL_KI  2.9
  #define PID_ROLL_KD  0.0
  #define PID_ROLL_INTEGRATION_LIMIT    20.0

  #define PID_PITCH_KP  5.9
  #define PID_PITCH_KI  2.9
  #define PID_PITCH_KD  0.0
  #define PID_PITCH_INTEGRATION_LIMIT   20.0

  #define PID_YAW_KP  6.0
  #define PID_YAW_KI  1.0
  #define PID_YAW_KD  0.35
  #define PID_YAW_INTEGRATION_LIMIT     360.0


  #define DEFAULT_PID_INTEGRATION_LIMIT 5000.0
  #define DEFAULT_PID_OUTPUT_LIMIT      0.0
#endif 

typedef struct
{
  float desired;      //< set point
  float error;        //< error
  float prevError;    //< previous error
  float integ;        //< integral
  float deriv;        //< derivative
  float kp;           //< proportional gain
  float ki;           //< integral gain
  float kd;           //< derivative gain
  float outP;         //< proportional output (debugging)
  float outI;         //< integral output (debugging)
  float outD;         //< derivative output (debugging)
  float iLimit;       //< integral limit, absolute value. '0' means no limit.
  float outputLimit;  //< total PID output limit, absolute value. '0' means no limit.
  float dt;           //< delta-time dt
  lpf2pData dFilter;  //< filter for D term
  bool enableDFilter; //< filter for D term enable flag
} PidObject;


 void pidInit(PidObject* pid, const float desired, const float kp,
              const float ki, const float kd, const float dt,
              const float samplingRate, const float cutoffFreq,
              bool enableDFilter);


void pidSetIntegralLimit(PidObject* pid, const float limit);

void pidReset(PidObject* pid);

float pidUpdate(PidObject* pid, const float measured, const bool updateError);


void pidSetDesired(PidObject* pid, const float desired);


float pidGetDesired(PidObject* pid);


bool pidIsActive(PidObject* pid);


void pidSetError(PidObject* pid, const float error);


void pidSetKp(PidObject* pid, const float kp);


void pidSetKi(PidObject* pid, const float ki);


void pidSetKd(PidObject* pid, const float kd);


void pidSetDt(PidObject* pid, const float dt);
#endif 
