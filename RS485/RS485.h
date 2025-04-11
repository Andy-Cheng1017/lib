#ifndef __RS485_H_
#define __RS485_H_
#include <stdint.h>
#include <stdbool.h>

#if defined(AT_START_F407) || defined(AT_START_F403A)
#include "at32f403a_407.h"
#elif defined AT_START_F403
#include "at32f403.h"
#elif defined AT_START_F413
#include "at32f413.h"
#elif defined AT_START_F415
#include "at32f415.h"
#elif defined AT_START_F421
#include "at32f421.h"
#elif defined AT_START_F425
#include "at32f425.h"
#elif defined AT_START_F435_437
#include "at32f435_437.h"
#endif

#define DECLARE_RS485_BUFFERS(name, data_max)  \
    static uint8_t name##_rx_circle_buf[(data_max) * 2];  \
    static uint8_t name##_tx_circle_buf[(data_max) * 2];  \
    static uint8_t name##_rx_pkg[(data_max)];            \
    static uint8_t name##_tx_pkg[(data_max)];            \
    static uint8_t name##_rx_Data[(data_max)];           \
    static uint8_t name##_tx_Data[(data_max)]

#define RS485_BUFFERS_INIT(name)            \
    .rx_circle_buf = name##_rx_circle_buf,   \
    .tx_circle_buf = name##_tx_circle_buf,   \
    .rx_pkg = name##_rx_pkg,                 \
    .tx_pkg = name##_tx_pkg,                 \
    .rx_Data = name##_rx_Data,               \
    .tx_Data = name##_tx_Data

typedef enum {
  BR_4800 = 4800,
  BR_9600 = 9600,
  BR_19200 = 19200,
  BR_38400 = 38400,
  BR_57600 = 57600,
  BR_115200 = 115200,
  BR_230400 = 230400,
  BR_460800 = 460800,
  BR_921600 = 921600,
} baud_rate_t;

typedef enum {
  READ_COILS = 0x01,
  READ_DISCRETE_INPUTS = 0x02,
  READ_HOLDING_REGISTERS = 0x03,
  READ_INPUT_REGISTERS = 0x04,
  WRITE_SINGLE_COIL = 0x05,
  WRITE_SINGLE_REGISTER = 0x06,
  WRITE_MULTIPLE_COILS = 0x0F,
  WRITE_MULTIPLE_REGISTERS = 0x10,
} RsFunc_t;

typedef enum {
  RS485_OK = 0,
  UNPKG_FINISH,
  OTHER_SLAVE_ADDR,
  UNPKG_OVER_PACKGE_SIZE,
  UNPKG_TOO_SHORT,
  CRC_ERROR,
  ILLIGAL_FUNC,
  ILLIGAL_DATA_ADDR,
  ILLIGAL_DATA_VALUE,
  SLAVE_DEVICE_FAILURE,
  ENCODE_FOR_NUMBER,
  ENCODE_FOR_SINGLE_DATA,
  WRITE_REGISTERS_ERROR,
} RsError_t;

typedef enum {
  MASTER,
  SLAVE,
} rs485_mode_t;

typedef struct {
  uint16_t data_max_size;
  uint16_t circle_buffer_max_size;
  uint16_t pkg_max_size;

  usart_type *UART;
  rs485_mode_t Mode;
  baud_rate_t BaudRate;
  usart_stop_bit_num_type StopBit;
  usart_data_bit_num_type DataBit;
  uint8_t ip_addr;
  bool root;

  uint8_t *rx_circle_buf;
  uint8_t rx_idex;
  bool rx_pkg_cplt_f;
  bool rx_pkg_start_f;
  uint8_t rx_first_pkg;
  uint8_t *rx_pkg;
  uint8_t decd_idex;

  RsFunc_t rx_Func;
  uint8_t *rx_Data;
  uint8_t rx_Data_len;

  uint8_t *tx_circle_buf;
  uint8_t tx_idex;
  uint8_t *tx_pkg;
  uint8_t encd_idex;

  RsFunc_t tx_Func;
  uint8_t *tx_Data;
  uint8_t tx_Data_len;

  uint16_t reg_hdle_stat;
  uint16_t reg_hdle_end;
  uint16_t reg_hdle_num;

} Rs485_t;

typedef uint32_t (*rs485_handler_t)(RsFunc_t func, uint16_t addr, uint16_t data, uint8_t len, bool root);

bool RsRegHdle(Rs485_t *rs485, rs485_handler_t handler);

void RsInit(Rs485_t *rs485);
void RS485_Re_Config(Rs485_t *rs485);

void RS485_Rx_Cplt_ISR(Rs485_t *rs485);

void RS485_Tx_Data_ISR(Rs485_t *rs485);
RsError_t RS485_Rx_Data_ISR(Rs485_t *rs485);

RsError_t RsUnpkg(Rs485_t *rs485, RsFunc_t *upk_func, uint8_t *upk_data, uint8_t *upk_data_len);

RsError_t RsSlaveDecdEncd(Rs485_t *rs485, RsFunc_t rx_Func, uint8_t *rx_Data, uint8_t rx_Data_len, RsFunc_t *tx_Func, uint8_t *tx_Data,
                          uint8_t *tx_Data_len);

RsError_t RsMasterDecd(Rs485_t *rs485, RsFunc_t rx_Func, uint8_t *rx_Data, uint8_t rx_Data_len);

RsError_t RsMasterEncd(Rs485_t *rs485, RsFunc_t Func, uint16_t stat_addr, uint16_t num, uint16_t *data, uint8_t Data_len, uint8_t *tx_Data,
                       uint8_t *tx_Data_len);

RsError_t RsPkg(Rs485_t *rs485, uint8_t DstIpAddr, RsFunc_t pkg_func, uint8_t *pkg_data, uint8_t pkg_data_len);

uint32_t crc8_block_calculate(uint8_t *pbuffer, uint32_t length);

bool RsChkAvailable(Rs485_t *rs485);

RsError_t RS485Read(Rs485_t *rs485);

RsError_t RS485ReadHandler(Rs485_t *rs485);

RsError_t RS485WriteHandler(Rs485_t *rs485, uint16_t *data, uint8_t data_len);

RsError_t RS485Write(Rs485_t *rs485);

#endif
