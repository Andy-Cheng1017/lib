#ifndef _Time_h
#ifdef __cplusplus
#define _Time_h

#ifndef ARDUINO
#define ARDUINO 100
#endif

#include <inttypes.h>
#include <time.h>

extern "C++" {

typedef enum {
  tmSecond,
  tmMinute,
  tmHour,
  tmWday,
  tmDay,
  tmMonth,
  tmYear,
  tmNbrFields
} tmByteFields;

typedef struct {
  uint8_t Second;
  uint8_t Minute;
  uint8_t Hour;
  uint8_t Wday;  // day of week, sunday is day 1
  uint8_t Day;
  uint8_t Month;
  uint8_t Year;  // offset from 1970;
} tmElements_t;

/*==============================================================================*/
/* Useful Constants */
#define SECS_PER_MIN  ((time_t)(60UL))
#define SECS_PER_HOUR ((time_t)(3600UL))
#define SECS_PER_DAY  ((time_t)(SECS_PER_HOUR * 24UL))
#define DAYS_PER_WEEK ((time_t)(7UL))
#define SECS_PER_WEEK ((time_t)(SECS_PER_DAY * DAYS_PER_WEEK))
#define SECS_PER_YEAR ((time_t)(SECS_PER_DAY * 365UL)) // TODO: ought to handle leap years
#define SECS_YR_2000  ((time_t)(946684800UL)) // the time at the start of y2k

//convenience macros to convert to and from tm years 
#define  tmYearToCalendar(Y) ((Y) + 1970)  // full four digit year 
#define  CalendarYrToTm(Y)   ((Y) - 1970)
#define  tmYearToY2k(Y)      ((Y) - 30)    // offset is from 2000
#define  y2kYearToTm(Y)      ((Y) + 30)  

/* low level functions to convert to and from system time                     */
void breakTime(time_t time, tmElements_t &tm);  // break time_t into elements
time_t makeTime(const tmElements_t &tm);  // convert time elements into time_t

}  // extern "C++"
#endif  // __cplusplus
#endif  /* _Time_h */
