#include "SensConvVal.h"

// 初始化感測器配置
void Conv_Init(SensConvVal_t *cfg) {
  // 計算電壓範圍
  cfg->volt_range = cfg->volt_max - cfg->volt_min;

  // 計算感測器範圍
  cfg->sens_range = cfg->sens_max - cfg->sens_min;

  // 計算 ADC 值轉換為電壓的係數
  cfg->adc_to_volt = cfg->adc_ref / (float)cfg->adc_res;
}

// // 將 ADC 值轉換為感測器數值
// ErrConv_t Conv_GetVal_ADC(SensConvVal_t *cfg, uint16_t adc_val, int32_t *ret_val) {
//   // 將 ADC 值轉換為電壓
//   float volt = (float)adc_val * cfg->adc_to_volt;

//   // 將電壓映射到感測器數值範圍
//   float sens_val = cfg->sens_min + (volt - cfg->volt_min) * (cfg->sens_range / cfg->volt_range);

//   if (sens_val < cfg->sens_min) {
//     return VAL_LOWER;
//   } else if (sens_val > cfg->sens_max) {
//     return VAL_HIGHER;
//   } else {
//     *ret_val = (int32_t)sens_val;
//     return VAL_OK;
//   }
// }

// 將 ADC 值轉換為感測器數值
ErrConv_t Conv_GetVal_Volt(SensConvVal_t *cfg, float volt, int32_t *ret_val) {
  // 將電壓映射到感測器數值範圍
  float sens_val = cfg->sens_min + (volt - cfg->volt_min) * (cfg->sens_range / cfg->volt_range);

  if (sens_val < cfg->sens_min) {
    *ret_val = (int32_t)-1;
    return VAL_LOWER;
  } else if (sens_val > cfg->sens_max) {
    *ret_val = (int32_t)0xFFFFFFFF;
    return VAL_HIGHER;
  } else {
    *ret_val = (int32_t)sens_val;
    return VAL_OK;
  }
}