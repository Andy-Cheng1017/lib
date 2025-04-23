#include <stdbool.h>
#include "FreeRTOS.h"
#include "task.h"
#include "threshold_monitor.h"

bool over_threshold_with_delay(ThresholdMonitor_t *monitor, int current_value, int threshold, TickType_t delay_ms) {
  TickType_t now = xTaskGetTickCount();

  if (current_value >= threshold) {
    if (!monitor->over_limit) {
      // 第一次超標
      monitor->over_limit = true;
      monitor->start_tick = now;
      // monitor->triggered = false;
    } else if ((now - monitor->start_tick >= pdMS_TO_TICKS(delay_ms))) {
      // 超標時間已滿，觸發
      // monitor->triggered = true;
      return true;
    }
  } else {
    // 恢復正常，重設
    monitor->over_limit = false;
    // monitor->triggered = false;
  }

  return false;
}

bool under_threshold_with_delay(ThresholdMonitor_t *monitor, int current_value, int threshold, TickType_t delay_ms) {
  TickType_t now = xTaskGetTickCount();

  if (current_value <= threshold) {
    if (!monitor->over_limit) {
      monitor->over_limit = true;
      monitor->start_tick = now;
    } else if ((now - monitor->start_tick >= pdMS_TO_TICKS(delay_ms))) {
      return true;
    }
  } else {
    monitor->over_limit = false;
  }
  return false;
}

void clear_threshold_monitor(ThresholdMonitor_t *monitor) {
  monitor->over_limit = false;
}
