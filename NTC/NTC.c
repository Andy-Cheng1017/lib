
#include "NTC.h"

ErrNtc_t Ntc_ConvertToC(uint32_t adcValue, int32_t *ret_temp) {
  if (adcValue <= 20) {
    *ret_temp = 0X0;
    return NTC_LOWER;
  } else if (adcValue >= 4020) {
    *ret_temp = 0xFFFFFFFF;
    return NTC_HIGHER;
  }
  float rntc = (float)_NTC_R_SERIES * (((float)_NTC_ADC_MAX / (float)adcValue) - 1.0f);
  float temp;
  temp = rntc / (float)_NTC_R_NOMINAL;
  temp = logf(temp);
  temp /= (float)_NTC_BETA;
  temp += 1.0f / ((float)_NTC_TEMP_NOMINAL + 273.15f);
  temp = 1.0f / temp;
  temp -= 273.15f;
  *ret_temp = (int32_t)(temp * 1000);
  return NTC_OK;
}

ErrNtc_t Ntc_ConvertToC_Volt(float volt, int32_t *ret_temp) {
  // if (adcValue <= 20) {
  //   return NTC_LOWER;
  // } else if (adcValue >= 4020) {
  //   return NTC_HIGHER;
  // }
  float rntc = (float)_NTC_R_SERIES * (3.3 / volt - 1.0f);
  float temp;
  temp = rntc / (float)_NTC_R_NOMINAL;
  temp = logf(temp);
  temp /= (float)_NTC_BETA;
  temp += 1.0f / ((float)_NTC_TEMP_NOMINAL + 273.15f);
  temp = 1.0f / temp;
  temp -= 273.15f;
  *ret_temp = (int32_t)(temp * 1000);
  return NTC_OK;
}

ErrNtc_t Ntc_ConvertToF(uint32_t adcValue, int32_t *ret_temp) {
  if (adcValue <= 20) {
    return NTC_LOWER;
  } else if (adcValue >= 4020) {
    return NTC_HIGHER;
  }
  float rntc = (float)_NTC_R_SERIES * (((float)_NTC_ADC_MAX / (float)adcValue) - 1.0f);
  float temp;
  temp = rntc / (float)_NTC_R_NOMINAL;
  temp = logf(temp);
  temp /= (float)_NTC_BETA;
  temp += 1.0f / ((float)_NTC_TEMP_NOMINAL + 273.15f);
  temp = 1.0f / temp;
  temp -= 273.15f;
  *ret_temp = (int32_t)(((temp * 9.0f / 5.0f) + 32.f) * 1000);
  return NTC_OK;
}
