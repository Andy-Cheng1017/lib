#ifndef FG_RPM_H
#define FG_RPM_H

#include <stdbool.h>
#include <stdint.h>

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

#define MAX_TIMERS 5

typedef struct {
  tmr_type *tmr_list[MAX_TIMERS];
  uint8_t timer_count;

  uint32_t exint_line;
  uint8_t motor_phase;

  uint32_t tmr_clk;
  uint32_t tmr_div[MAX_TIMERS];
  uint32_t tmr_period[MAX_TIMERS];
  uint32_t rpm_ref_val[MAX_TIMERS];
  uint8_t rpm_ref_val_index;

  bool first_count_flag;
  uint16_t first_count[MAX_TIMERS];
  uint16_t sample;
  uint8_t interrupt_count[MAX_TIMERS];
} FgParam_t;

void FgInit(FgParam_t *fg_param);

void FgTimerIntHandler(FgParam_t *fg_param, tmr_type *tmr_x);

void FgExintIntSampling(FgParam_t *fg_param);

void FgGetRPM(FgParam_t *fg_param, uint16_t *rpm);

#endif  