

#ifndef _NTC_H
#define _NTC_H
/*
  Author:     Nima Askari
  WebSite:    http://www.github.com/NimaLTD
  Instagram:  http://instagram.com/github.NimaLTD
  Youtube:    https://www.youtube.com/channel/UCUhY7qY1klJm1d2kulr9ckw
  
  Version:    1.0.0
  
  
  Reversion History:
  
  (1.0.0)
  First release.
*/

#include <stdint.h>
#include <math.h>

#define _NTC_R_SERIES         10000.0f
#define _NTC_R_NOMINAL        10000.0f
#define _NTC_TEMP_NOMINAL     25.0f
#define _NTC_ADC_MAX          4095 //  12bit
#define _NTC_BETA             3435

typedef enum{
  NTC_OK,
  NTC_LOWER,
  NTC_HIGHER,
}ErrNtc_t;

ErrNtc_t Ntc_ConvertToC(uint32_t adcValue, int32_t *ret_temp);
ErrNtc_t Ntc_ConvertToF(uint32_t adcValue, int32_t *ret_temp);

ErrNtc_t Ntc_ConvertToC_Volt(float_t volt, int32_t *ret_temp);
  
#endif
