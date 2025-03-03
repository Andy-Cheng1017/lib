#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include "FG_RPM.h"

void FgInit(FgParam_t *fg_param) {
  fg_param->tmr_clk = system_core_clock;

  fg_param->h_tmr_div = (uint16_t)tmr_div_value_get(fg_param->h_tmr_x);
  fg_param->h_tmr_period = (uint16_t)tmr_period_value_get(fg_param->h_tmr_x);

  fg_param->l_tmr_div = (uint16_t)tmr_div_value_get(fg_param->l_tmr_x);
  fg_param->l_tmr_period = (uint16_t)tmr_period_value_get(fg_param->l_tmr_x);

  fg_param->h_rpm_ref_val = 60 * ((float)(fg_param->tmr_clk) / ((fg_param->h_tmr_div + 1) * fg_param->motor_phase));
  fg_param->l_rpm_ref_val = 60 * ((float)(fg_param->tmr_clk) / ((fg_param->l_tmr_div + 1) * fg_param->motor_phase));

  tmr_counter_enable(fg_param->h_tmr_x, FALSE);
  tmr_counter_enable(fg_param->l_tmr_x, FALSE);

  tmr_counter_value_set(fg_param->h_tmr_x, 0);
  tmr_counter_value_set(fg_param->l_tmr_x, 0);

  tmr_interrupt_enable(fg_param->h_tmr_x, TMR_OVF_INT, TRUE);
  tmr_interrupt_enable(fg_param->l_tmr_x, TMR_OVF_INT, TRUE);

  tmr_counter_enable(fg_param->h_tmr_x, TRUE);
  tmr_counter_enable(fg_param->l_tmr_x, TRUE);
}

void FgLowTmrIntHandler(FgParam_t *fg_param) {
  if (fg_param->h_tmr_flag == true) {
    fg_param->sample = 0;
  }

  fg_param->h_tmr_flag = true;
  fg_param->first_count_flag = false;

  tmr_counter_enable(fg_param->h_tmr_x, FALSE);
  tmr_counter_enable(fg_param->l_tmr_x, FALSE);

  tmr_counter_value_set(fg_param->h_tmr_x, 0);
  tmr_counter_value_set(fg_param->l_tmr_x, 0);

  tmr_counter_enable(fg_param->h_tmr_x, TRUE);
  tmr_counter_enable(fg_param->l_tmr_x, TRUE);
  exint_interrupt_enable(fg_param->exint_line, TRUE);
}

void FgHighTmrIntHandler(FgParam_t *fg_param) {
  if (!fg_param->first_count_flag) {
    fg_param->h_tmr_flag = false;
  }
}

void FgExintIntSampling(FgParam_t *fg_param) {
  if (!fg_param->first_count_flag) {
    fg_param->h_first_count = tmr_counter_value_get(fg_param->h_tmr_x);
    fg_param->l_first_count = tmr_counter_value_get(fg_param->l_tmr_x);

    fg_param->first_count_flag = true;
  } else {
    if (fg_param->h_tmr_flag) {
      uint16_t sec_count = tmr_counter_value_get(fg_param->h_tmr_x);

      fg_param->sample = sec_count - fg_param->h_first_count;

    } else {
      uint16_t sec_count = tmr_counter_value_get(fg_param->l_tmr_x);

      fg_param->sample = sec_count - fg_param->l_first_count;
    }
    // fg_param->first_count_flag = false;
    exint_interrupt_enable(fg_param->exint_line, FALSE);
  }
}

void FgGetRPM(FgParam_t *fg_param, uint16_t *rpm) {
  if (fg_param->sample == 0) {
    *rpm = 0;
    return;
  }
  if (fg_param->h_tmr_flag) {
    *rpm = (uint16_t)(fg_param->h_rpm_ref_val / ((float)fg_param->sample));
  } else {
    *rpm = (uint16_t)(fg_param->l_rpm_ref_val / ((float)fg_param->sample));
  }
}