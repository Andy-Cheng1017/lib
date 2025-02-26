#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include "FG_RPM.h"
#include "at32f403a_407_exint.h"

#define SAMPLE_MASK (FG_SAMPLE_NUM - 1)
#define SMP_NUM_PWR (__builtin_ctz(FG_SAMPLE_NUM))

_Static_assert((FG_SAMPLE_NUM & (FG_SAMPLE_NUM - 1)) == 0, "FG_SAMPLE_NUM must be a power of 2");

void FgInit(FgParam_t *fg_param) {
  memset(fg_param->sample, 0, sizeof(fg_param->sample));

  fg_param->rpm_ref_val = 60 * ((float)(fg_param->tmr_clk) / ((fg_param->tmr_clk_div + 1) * fg_param->motor_phase));
}

void FgIntEnable(FgParam_t *fg_param) {
  exint_interrupt_enable(fg_param->exint_line, TRUE);

  if (fg_param->timeout_flag) {
    fg_param->sample[fg_param->sample_index] = 0;
  }

  fg_param->sample_index = (fg_param->sample_index + 1) & SAMPLE_MASK;

  fg_param->timeout_flag = true;
}

void FgSampling(FgParam_t *fg_param) {
  fg_param->timeout_flag = false;

  if (!fg_param->count_flag) {
    fg_param->first_count = tmr_counter_value_get(fg_param->tmr_x);
    fg_param->count_flag = true;
  } else {
    uint16_t sec_count = tmr_counter_value_get(fg_param->tmr_x);

    if (sec_count > fg_param->first_count) {
      fg_param->sample[fg_param->sample_index] = sec_count - fg_param->first_count;
    } else {
      fg_param->sample[fg_param->sample_index] = fg_param->tmr_period_val - fg_param->first_count + sec_count;
    }

    fg_param->count_flag = false;

    exint_interrupt_enable(fg_param->exint_line, FALSE);
  }
}

void FgGetRPM(FgParam_t *fg_param, uint16_t *rpm) {
  uint8_t check_sample_0 = 0;
  uint32_t sum = 0;

  for (uint8_t i = 0; i < FG_SAMPLE_NUM; i++) {
    if (fg_param->sample[i] == 0) {
      check_sample_0++;
    }
    sum += fg_param->sample[i];
  }

  if (sum != 0) {
    *rpm = (uint16_t)(fg_param->rpm_ref_val / ((float)(sum) / (FG_SAMPLE_NUM - check_sample_0)));
  } else {
    *rpm = 0;
  }
}