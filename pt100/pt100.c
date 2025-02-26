#include "pt100.h"
#include "FreeRTOS.h"
#include "task.h"

void PT100_Init(Pt100I2cParam_t *pt100) {
  // i2c_config(&pt100->i2c_handle);

  MCP342xInit(&pt100->i2c_handle, pt100->mcp_i2c_addr);

  MCP342x_generalCallReset();
  vTaskDelay(5);
}

MCP342x_error_t PT100_MCP_ReadAndCalcTemp(Pt100I2cParam_t *pt100, MCP342xChannel_t channel, int32_t *pt100_temp) {
  long adc_val = 0;

  MCP342x_error_t err = MCP342x_convertAndRead(channel, &adc_val);

  if (err != errorNone) {
    return err;
  }

  unsigned long Vdiff_uv = ((adc_val * 125) >> 3) >> pt100->adc_gain;

  float pt_ideal_resist = (100.0 + 2920 * Vdiff_uv * 0.000001) / (1.0 - 0.000001 * Vdiff_uv * 10);
  // float pt_real_resist = 0.985451 * pt_ideal_resist - 0.00020694;

  // 使用公式 T = (-A + sqrt(A^2 - 4 * B * (1 - R/R0))) / (2 * B)
  float ratio = pt_ideal_resist / R0;
  float discriminant = A * A - 4.0f * B * (1.0f - ratio);

  if (discriminant < 0) {
    *pt100_temp = 0xffff;  // 返回一个无效温度值
    return errorDiscriminant;  
  }

  float temperature = (-A + sqrtf(discriminant)) / (2.0f * B);

  *pt100_temp = (int32_t)roundf(temperature * 1000);

  return errorNone;
}