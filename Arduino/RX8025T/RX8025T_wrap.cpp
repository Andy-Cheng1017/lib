#include "RX8025T_wrap.h"
#include "RTC_RX8025T.h"

// #define LOG_TAG "RX8025T_wrap"
// #include "elog.h"

// 在 C++ 文件中實現包裝函數
void RX8025T_init() {
    RTC_RX8025T.init();
}

uint8_t RX8025T_set(time_t t) {
    return RTC_RX8025T.set(t);
}

time_t RX8025T_get() {
    // log_i("Getting time from RTC");
    return RTC_RX8025T.get();
}
