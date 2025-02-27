#ifndef Duty_PWM_H
#define Duty_PWM_H

#if defined(AT_START_F407) || defined(AT_START_F403A)
#include "at32f403a_407.h"
#elif defined AT_START_F403
#include "at32f403.h"
#elif defined AT_START_F413
#include "at32f413.h"
#elif defined AT_START_F415
#include "at32f415.h"
#elif defined AT_START_F421
#include "at32f421.h"
#elif defined AT_START_F425
#include "at32f425.h"
#elif defined AT_START_F435_437
#include "at32f435_437.h"
#endif
typedef struct {
  tmr_type *tmr_x;
  tmr_channel_select_type channel;
  uint32_t tmr_period_val;
} PwmParam_t;


void PwmInit(PwmParam_t *pwm);

void PwmSetDuty(PwmParam_t *pwm, uint16_t duty);

#endif