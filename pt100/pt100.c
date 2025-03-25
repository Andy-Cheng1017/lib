#include <math.h>
#include <stdbool.h>
#include "pt100.h"
#include "FreeRTOS.h"
#include "task.h"

#define R0 100.0f
#define A 3.9083e-3f
#define B -5.775e-7f

#define DEFAULT_BRIGE_RESISTOR 100
#define DEFAULT_CURRENT_LIMIT_RESISTOR 1500
#define DEFAULT_SOURCE_VOLTAGE 3

void PT100_Init(Pt100I2cParam_t *pt100) {
  MCP342xInit(&pt100->i2c_handle, pt100->mcp_i2c_addr);
  MCP342x_generalCallReset();

  if (!pt100->brige_resistor) {
    pt100->brige_resistor = DEFAULT_BRIGE_RESISTOR;
  }
  if (!pt100->current_limit_resistor) {
    pt100->current_limit_resistor = DEFAULT_CURRENT_LIMIT_RESISTOR;
  }
  if (!pt100->source_voltage) {
    pt100->source_voltage = DEFAULT_SOURCE_VOLTAGE;
  }
  vTaskDelay(5);
}

MCP342x_error_t PT100_MCP_ReadAndCalcTemp(Pt100I2cParam_t *pt100, MCP342xChannel_t channel, int32_t *pt100_temp) {
  long adc_val = 0;

  MCP342x_error_t err = MCP342x_convertAndRead(channel, &adc_val);

  if (err != errorNone) {
    return err;
  }

  unsigned long Vdiff_uv = ((adc_val * 125) >> 3) >> pt100->adc_gain;

  /* Formula about Voltage difference to PT100 resistance:

                        2 * Vdiff * ( 3 * CURRENT_LIMIT_RESISTOR + BRIGE_RESISTOR ) + SOURCE_VOLTAGE * BRIGE_RESISTOR
   PT100_Resistance =  ---------------------------------------------------------------------------------------------------
                         SOURCE_VOLTAGE - ( ( CURRENT_LIMIT_RESISTOR + BRIGE_RESISTOR) / BRIGE_RESISTOR ) * 2 * Vdiff
  */

  // float pt_ideal_resist = (100.0 + 2920 * Vdiff_uv * 0.000001) / (1.0 - 0.000001 * Vdiff_uv * 10);
  float pt_ideal_resist =
      (2 * Vdiff_uv * 0.000001 * (3 * pt100->current_limit_resistor + pt100->brige_resistor) + pt100->source_voltage * pt100->brige_resistor) /
      (pt100->source_voltage - ((pt100->current_limit_resistor + pt100->brige_resistor) / pt100->brige_resistor) * 2 * Vdiff_uv * 0.000001);

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