#ifndef Duty_PWM_H
#define Duty_PWM_H

#include "at32f403a_407_tmr.h"

typedef struct {
  tmr_type *tmr_x;
  tmr_channel_select_type channel;
  uint32_t tmr_period_val;
} PwmParam_t;


void PwmInit(PwmParam_t *pwm);

void PwmSetDuty(PwmParam_t *pwm, uint16_t duty);

#endif