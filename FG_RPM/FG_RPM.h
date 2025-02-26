#ifndef FG_RPM_H
#define FG_RPM_H

#include "at32f403a_407_tmr.h"
#define FG_SAMPLE_NUM 4

typedef struct {
  tmr_type *tmr_x;
  uint32_t exint_line;
  uint32_t tmr_clk;
  uint32_t tmr_clk_div;
  uint32_t tmr_period_val;
  uint8_t motor_phase;

  uint8_t sample_index;
  bool count_flag;
  uint16_t first_count;
  bool timeout_flag;
  uint16_t sample[FG_SAMPLE_NUM];
  float rpm_ref_val;
} FgParam_t;

void FgInit(FgParam_t *fg_param);

void FgIntEnable(FgParam_t *fg_param);

void FgSampling(FgParam_t *fg_param);

void FgGetRPM(FgParam_t *fg_param, uint16_t *rpm);

#endif  // FG_RPM_H