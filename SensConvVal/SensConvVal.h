#ifndef SENSCONVVAL_H
#define SENSCONVVAL_H

#include <stdint.h>
#include <stdio.h>

// 感測器配置結構體
typedef struct {
  // 使用者輸入的參數
  float volt_min;    // 緩衝電路最小輸出電壓（例如 0.6V）
  float volt_max;    // 緩衝電路最大輸出電壓（例如 3.0V）
  float sens_min;    // 感測器最小值（例如 0 kPa）
  float sens_max;    // 感測器最大值（例如 10000 kPa）
  uint16_t adc_res;  // ADC 解析度（例如 4095）
  float adc_ref;     // ADC 參考電壓（例如 3.3V）

  // 預先計算的值
  float volt_range;   // 電壓範圍 (volt_max - volt_min)
  float sens_range;   // 感測器範圍 (sens_max - sens_min)
  float adc_to_volt;  // ADC 值轉換為電壓的係數 (adc_ref / adc_res)
} SensConvVal_t;

typedef enum {
  VAL_OK,
  VAL_LOWER,
  VAL_HIGHER,
} ErrConv_t;

// 函式宣告
void Conv_Init(SensConvVal_t *cfg);
// ErrConv_t Conv_GetVal_ADC(SensConvVal_t *cfg, uint16_t adc_val, int32_t *ret_val);
ErrConv_t Conv_GetVal_Volt(SensConvVal_t *cfg, float volt, int32_t *ret_val);

#endif  // SENSCONVVAL_H