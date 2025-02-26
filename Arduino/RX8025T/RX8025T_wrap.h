#ifndef RX8025T_WRAP_H
#define RX8025T_WRAP_H

#include <time.h>

#ifdef __cplusplus
extern "C" {
#endif

void RX8025T_init();
uint8_t RX8025T_set(time_t t);
time_t RX8025T_get();

#ifdef __cplusplus
}
#endif

#endif  // RX8025T_WRAP_H
