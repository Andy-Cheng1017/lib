#ifndef THRESHOLD_MONITOR_H
#define THRESHOLD_MONITOR_H

#include <stdbool.h>
#include "FreeRTOS.h"
#include "task.h"

typedef struct {
  bool over_limit;
  // bool triggered;
  TickType_t start_tick;
} ThresholdMonitor_t;

bool over_threshold_with_delay(ThresholdMonitor_t *monitor, int current_value, int threshold, TickType_t delay_ms);
bool under_threshold_with_delay(ThresholdMonitor_t *monitor, int current_value, int threshold, TickType_t delay_ms);
void clear_threshold_monitor(ThresholdMonitor_t *monitor);
#endif
