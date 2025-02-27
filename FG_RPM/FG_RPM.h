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

typedef struct {
  tmr_type *h_tmr_x;
  tmr_type *l_tmr_x;
  uint32_t exint_line;
  uint8_t motor_phase;

  uint32_t tmr_clk;
  uint16_t h_tmr_div;
  uint16_t l_tmr_div;
  uint16_t h_tmr_period;
  uint16_t l_tmr_period;
  float h_rpm_ref_val;
  float l_rpm_ref_val;

  bool h_tmr_flag;
  bool first_count_flag;
  uint16_t h_first_count;
  uint16_t l_first_count;
  uint16_t sample;
} FgParam_t;

void FgInit(FgParam_t *fg_param);

void FgLowTmrIntHandler(FgParam_t *fg_param);

void FgHighTmrIntHandler(FgParam_t *fg_param);

void FgExintIntSampling(FgParam_t *fg_param);

void FgGetRPM(FgParam_t *fg_param, uint16_t *rpm);

#endif  // FG_RPM_H