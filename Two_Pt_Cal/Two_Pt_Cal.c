#include "Two_Pt_Cal.h"
#include <stddef.h>
#include <math.h>

void Cal_Init(CalParam_t *params) {
  if (params == NULL) {
    return;
  }
  params->offset = 0.0f;
  params->slope = 1.0f;
}

/* 校正參數計算函式
 * 根據 raw 與 ideal 數值計算校正斜率與偏移量
 */
ErrCal_t Cal_CalcParams(CalParam_t *params) {
  if (params == NULL) return CAL_ERR_NULL_PARAM;
  if (params->raw_l == NULL || params->raw_h == NULL || params->ideal_l == NULL || params->ideal_h == NULL) return CAL_ERR_NULL_POINTER;

  if (params->data_type == DATA_TYPE_INT16) {
    int16_t raw_l = *(int16_t *)(params->raw_l);
    int16_t raw_h = *(int16_t *)(params->raw_h);
    int16_t ideal_l = *(int16_t *)(params->ideal_l);
    int16_t ideal_h = *(int16_t *)(params->ideal_h);

    // 檢查輸入是否有效，避免除以零
    if (raw_l == raw_h) return CAL_ERR_INVALID_INPUT;

    params->slope = (float)(ideal_h - ideal_l) / (raw_h - raw_l);
    params->offset = (float)ideal_l - params->slope * raw_l;
  } else if (params->data_type == DATA_TYPE_INT32) {
    int32_t raw_l = *(int32_t *)(params->raw_l);
    int32_t raw_h = *(int32_t *)(params->raw_h);
    int32_t ideal_l = *(int32_t *)(params->ideal_l);
    int32_t ideal_h = *(int32_t *)(params->ideal_h);

    // 檢查輸入是否有效，避免除以零
    if (raw_l == raw_h) return CAL_ERR_INVALID_INPUT;

    params->slope = (float)(ideal_h - ideal_l) / (raw_h - raw_l);
    params->offset = (float)ideal_l - params->slope * raw_l;
  } else {
    return CAL_ERR_UNKNOWN_DATATYPE;
  }

  return CAL_SUCCESS;
}

/* 校正應用函式
 * 根據已計算的 slope 與 offset 將原始值進行校正
 * 對於 16 位元校正，輸入值與返回值均為 16 位元數值（不過此處返回類型仍為 int32_t，可依需求調整）
 */
int32_t Cal_Apply(const CalParam_t *params, int32_t raw) {
  if (params == NULL) return raw;  // 若參數無效，返回原始值

  float result = params->slope * raw + params->offset;
  // if (params->data_type == DATA_TYPE_INT16) {
  //   // 將結果轉換成 16 位元數值（經四捨五入）
  //   int16_t corrected = (int16_t)roundf(result);
  //   return corrected;
  // } else if (params->data_type == DATA_TYPE_INT32) {
    // 將結果轉換成 32 位元數值（經四捨五入）
    int32_t corrected = (int32_t)roundf(result);
    return corrected;
  // } else {
    // 未知的資料型態，直接返回原始值
    // return raw;
  // }
}