#ifndef PT100_H
#define PT100_H

#include <stdint.h>
#include "MCP342x_wrap.h"
#include "i2c_application.h"

typedef enum {
  MCP342xCHANNEL_1 = 0,
  MCP342xCHANNEL_2,
  MCP342xCHANNEL_3,
  MCP342xCHANNEL_4
} MCP342xChannel_t;

typedef struct {
  i2c_handle_type i2c_handle;
  uint8_t mcp_i2c_addr;
  uint8_t adc_gain;

  uint16_t brige_resistor;
  uint16_t current_limit_resistor;
  uint8_t source_voltage;
} Pt100I2cParam_t;

void PT100_Init(Pt100I2cParam_t *pt100);

MCP342x_error_t PT100_MCP_ReadAndCalcTemp(Pt100I2cParam_t *pt100, MCP342xChannel_t channel, int32_t *pt100_temp);

#endif  // PT100_H
