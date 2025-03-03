#include "MCP342x_wrap.h"
#include "MCP342x.h"
#include "Wire.h"

#ifdef LOG_TAG
#define LOG_TAG "MCP342x_wrap"
#include "elog.h"
#endif 

MCP342x adc;

void MCP342xInit(i2c_handle_type *hi2c, uint8_t i2c_address) {
  wireSetHandle(hi2c);
  adc = MCP342x(i2c_address);
}

uint8_t MCP342x_generalCallReset() { return adc.generalCallReset(); }

MCP342x::Channel convertChannel(MCP342x_channel_t channel) {
  switch (channel) {
    case MCP342x_CHANNEL_1:
      return MCP342x::channel1;
    case MCP342x_CHANNEL_2:
      return MCP342x::channel2;
    case MCP342x_CHANNEL_3:
      return MCP342x::channel3;
    case MCP342x_CHANNEL_4:
      return MCP342x::channel4;
    default:
      return MCP342x::channel1;
  }
}

MCP342x_error_t convertError(MCP342x::error_t error) { return static_cast<MCP342x_error_t>(error); }

MCP342x_error_t MCP342x_convertAndRead(MCP342x_channel_t channel, long *result) {
  const MCP342x::Mode mode = MCP342x::oneShot;
  const MCP342x::Resolution resolution = MCP342x::resolution18;
  const MCP342x::Gain gain = MCP342x::gain8;
  const unsigned long timeout = 1000;

  MCP342x::Config status;

  MCP342x::error_t err = adc.convertAndRead(convertChannel(channel), mode, resolution, gain, timeout, *result, status);

  return convertError(err);
}