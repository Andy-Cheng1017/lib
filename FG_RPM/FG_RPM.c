#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include "FG_RPM.h"

void FgInit(FgParam_t *fg_param) {
  fg_param->tmr_clk = system_core_clock;

  for (int i = 0; i < fg_param->timer_count; i++) {
    fg_param->tmr_div[i] = (uint16_t)tmr_div_value_get(fg_param->tmr_list[i]);
    fg_param->tmr_period[i] = (uint16_t)tmr_period_value_get(fg_param->tmr_list[i]);
    fg_param->rpm_ref_val[i] = 60 * ((fg_param->tmr_clk) / ((fg_param->tmr_div[i] + 1) * fg_param->motor_phase));
    tmr_counter_enable(fg_param->tmr_list[i], FALSE);
    tmr_counter_value_set(fg_param->tmr_list[i], 0);
    tmr_counter_enable(fg_param->tmr_list[i], TRUE);
    tmr_interrupt_enable(fg_param->tmr_list[i], TMR_OVF_INT, TRUE);
  }
}

void FgTimerIntHandler(FgParam_t *fg_param, tmr_type *tmr_x) {
  for (int i = 0; i < fg_param->timer_count; i++) {
    if (fg_param->tmr_list[i] == tmr_x) {
      fg_param->interrupt_count[i]++;
    }
  }
}

void FgExintIntSampling(FgParam_t *fg_param) {
  if (!fg_param->first_count_flag) {
    for (int i = 0; i < fg_param->timer_count; i++) {
      fg_param->first_count[i] = tmr_counter_value_get(fg_param->tmr_list[i]);
    }

    fg_param->first_count_flag = true;
  } else {
    exint_interrupt_enable(fg_param->exint_line, FALSE);

    for (int i = 0; i < fg_param->timer_count; i++) {
      uint16_t secon_count = tmr_counter_value_get(fg_param->tmr_list[i]);

      if (fg_param->interrupt_count[i] == 0) {
        fg_param->sample = secon_count - fg_param->first_count[i];
        fg_param->rpm_ref_val_index = i;
        break;
      } else if (fg_param->interrupt_count[i] == 1) {
        if (secon_count < fg_param->first_count[i]) {
          fg_param->sample = secon_count + (fg_param->tmr_period[i] - fg_param->first_count[i]);
          fg_param->rpm_ref_val_index = i;
          break;
        }
      }
    }
  }
}

void FgGetRPM(FgParam_t *fg_param, uint16_t *rpm) {
  if (fg_param->first_count_flag == false) {
    *rpm = 0;
  } else {
    *rpm = (uint16_t)(fg_param->rpm_ref_val[fg_param->rpm_ref_val_index] / fg_param->sample);
  }

  exint_interrupt_enable(fg_param->exint_line, TRUE);
  fg_param->first_count_flag = false;
  for (int i = 0; i < fg_param->timer_count; i++) {
    fg_param->interrupt_count[i] = 0;
  }
}