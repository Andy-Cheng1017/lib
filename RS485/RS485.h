#ifndef __RS485_H_
#define __RS485_H_
#include <stdint.h>
#include <stdbool.h>
#include "RS485_enum.h"

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

#ifndef SINGLE_DATA_MAX_SIZE
#define SINGLE_DATA_MAX_SIZE 512
#endif

#ifndef MAX_CIRCLE_BUFFER_SIZE
#define MAX_CIRCLE_BUFFER_SIZE 1024
#endif

#ifndef MAX_PKG_SIZE
#define MAX_PKG_SIZE 512
#endif

typedef struct {
  usart_type *UART;
  rs485_mode_t Mode;
  baud_rate_t BaudRate;
  usart_stop_bit_num_type StopBit;
  usart_data_bit_num_type DataBit;
  uint8_t ip_addr;
  bool root;

  uint8_t rx_circle_buf[MAX_CIRCLE_BUFFER_SIZE];
  uint8_t rx_idex;
  bool rx_pkg_cplt_f;
  bool rx_pkg_start_f;
  uint8_t rx_first_pkg;
  uint8_t rx_pkg[MAX_PKG_SIZE];
  uint8_t decd_idex;

  RsFunc_t rx_Func;
  uint8_t rx_Data[SINGLE_DATA_MAX_SIZE];
  uint8_t rx_Data_len;

  uint8_t tx_circle_buf[MAX_CIRCLE_BUFFER_SIZE];
  uint8_t tx_idex;
  uint8_t tx_pkg[MAX_PKG_SIZE];
  uint8_t encd_idex;

  RsFunc_t tx_Func;
  uint8_t tx_Data[SINGLE_DATA_MAX_SIZE];
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
