#include <string.h>
#include "RS485.h"

#define MAX_BUF_MASK (MAX_CIRCLE_BUFFER_SIZE - 1)
#define MAX_HANDLER_COUNT 10

#define UPDATE_IDX(idx) ((idx) = (((idx) + 1) & MAX_BUF_MASK))
#define DOWMDATE_IDX(idx) ((idx) = (((idx) - 1) & MAX_BUF_MASK))

RsError_t err;
typedef struct {
  uint16_t start_addr;
  uint16_t end_addr;
  rs485_handler_t handler;
} rs485_handler_entry_t;

static rs485_handler_entry_t g_handler_table[MAX_HANDLER_COUNT];
static int g_handler_count = 0;

bool RsRegHdle(Rs485_t *rs485, rs485_handler_t handler) {
  if (g_handler_count >= MAX_HANDLER_COUNT) {
    return false;
  }
  g_handler_table[g_handler_count].start_addr = rs485->reg_hdle_stat;
  g_handler_table[g_handler_count].end_addr = rs485->reg_hdle_end;
  g_handler_table[g_handler_count].handler = handler;
  g_handler_count++;
  return true;
}

#ifndef CRC_MODULE_ENABLED
uint32_t crc8_block_calculate(uint8_t *pbuffer, uint32_t length) {
  // software crc8
}
#endif  // !CRC_MODULE_ENABLED
#ifdef CRC_MODULE_ENABLED
uint32_t crc8_block_calculate(uint8_t *pbuffer, uint32_t length) {
  crc_data_reset();
  uint32_t index = 0;

  for (index = 0; index < length; index++) {
    (*(uint8_t *)&CRC->dt) = pbuffer[index];
  }

  return (CRC->dt);
}
#endif  // CRC_MODULE_ENABLED

IRQn_Type Get_USART_IRQn(usart_type *uart) {
  switch ((uint32_t)uart) {
    case (uint32_t)USART1:
      return USART1_IRQn;
    case (uint32_t)USART2:
      return USART2_IRQn;
#if defined(AT_START_F415) || defined(AT_START_F407)
    case (uint32_t)USART3:
      return USART3_IRQn;
#endif
#if defined(AT_START_F407)
    case (uint32_t)UART4:
      return UART4_IRQn;
    case (uint32_t)UART5:
      return UART5_IRQn;
    case (uint32_t)USART6:
      return USART6_IRQn;
    case (uint32_t)UART7:
      return UART7_IRQn;
    case (uint32_t)UART8:
      return UART8_IRQn;
#endif
    default:
      return USART1_IRQn;
  }
}

void RsInit(Rs485_t *rs485) {
  // usart_reset(rs485->UART);
  usart_init(rs485->UART, (uint32_t)rs485->BaudRate, rs485->DataBit, rs485->StopBit);

  usart_receiver_enable(rs485->UART, TRUE);
  usart_transmitter_enable(rs485->UART, TRUE);

  usart_hardware_flow_control_set(rs485->UART, USART_HARDWARE_FLOW_NONE);

  usart_parity_selection_config(rs485->UART, USART_PARITY_NONE);
  nvic_priority_group_config(NVIC_PRIORITY_GROUP_4);

  usart_enable(rs485->UART, TRUE);

  usart_interrupt_enable(rs485->UART, USART_IDLE_INT, TRUE);
  usart_interrupt_enable(rs485->UART, USART_RDBF_INT, TRUE);
  nvic_irq_enable(Get_USART_IRQn(rs485->UART), 0, 0);

  // usart_interrupt_enable(rs485->UART, USART_TDBE_INT, TRUE);

  memset(rs485->rx_circle_buf, 0, MAX_CIRCLE_BUFFER_SIZE);
  rs485->rx_circle_buf[0] = 0x7E;
  rs485->rx_idex = 0;
  rs485->rx_pkg_cplt_f = FALSE;
  rs485->decd_idex = 0;

  memset(rs485->tx_circle_buf, 0, MAX_CIRCLE_BUFFER_SIZE);
  rs485->tx_circle_buf[0] = 0x7E;
  rs485->tx_idex = 0;
  rs485->encd_idex = 0;
}

void RS485_Re_Config(Rs485_t *rs485) {
  usart_receiver_enable(rs485->UART, FALSE);
  usart_transmitter_enable(rs485->UART, FALSE);
  RsInit(rs485);
}

void RS485_Tx_Data_ISR(Rs485_t *rs485) {
  while (UPDATE_IDX(rs485->tx_idex) != rs485->encd_idex) {
    if (usart_flag_get(rs485->UART, USART_TDBE_FLAG) == SET) {
      if (rs485->tx_circle_buf[rs485->tx_idex] == 0X7D) {
        usart_data_transmit(rs485->UART, rs485->tx_circle_buf[UPDATE_IDX(rs485->tx_idex)] ^ 0x20);
      } else {
        usart_data_transmit(rs485->UART, rs485->tx_circle_buf[rs485->tx_idex]);
      }
    } else {
      DOWMDATE_IDX(rs485->tx_idex);
      usart_interrupt_enable(rs485->UART, USART_TDBE_INT, TRUE);
      return;
    }
  }
}

RsError_t RS485_Rx_Data_ISR(Rs485_t *rs485) {
  uint16_t data = usart_data_receive(rs485->UART);

  if (rs485->rx_first_pkg == 0) rs485->rx_first_pkg = data;

  if (rs485->Mode == SLAVE && rs485->rx_first_pkg != rs485->ip_addr) return OTHER_SLAVE_ADDR;

  if (data == 0x7E || data == 0x7D) {
    rs485->rx_circle_buf[UPDATE_IDX(rs485->rx_idex)] = 0x7D;
    rs485->rx_circle_buf[UPDATE_IDX(rs485->rx_idex)] = data ^ 0x20;

  } else {
    rs485->rx_circle_buf[UPDATE_IDX(rs485->rx_idex)] = data;
  }
  return RS485_OK;
}

void RS485_Rx_Cplt_ISR(Rs485_t *rs485) {
  if (rs485->Mode == SLAVE && rs485->rx_first_pkg != rs485->ip_addr) {
    rs485->rx_first_pkg = 0;
    return;
  }
  rs485->rx_circle_buf[UPDATE_IDX(rs485->rx_idex)] = 0X7E;
  rs485->rx_pkg_cplt_f = TRUE;
  rs485->rx_first_pkg = 0;
}

RsError_t RsUnpkg(Rs485_t *rs485, RsFunc_t *upk_func, uint8_t *upk_data, uint8_t *upk_data_len) {
  int i = 0;
  memset(rs485->rx_pkg, 0, MAX_PKG_SIZE);
  while (rs485->rx_circle_buf[rs485->decd_idex] != 0x7E) UPDATE_IDX(rs485->decd_idex);

  uint16_t rx_last_7E_idx = rs485->rx_idex;
  while (rs485->rx_circle_buf[rx_last_7E_idx] != 0x7E) DOWMDATE_IDX(rx_last_7E_idx);

  if (rs485->decd_idex != rx_last_7E_idx) {
    while (rs485->rx_circle_buf[UPDATE_IDX(rs485->decd_idex)] != 0x7E) {
      if (rs485->rx_circle_buf[rs485->decd_idex] == 0x7D) {
        rs485->rx_pkg[i++] = rs485->rx_circle_buf[UPDATE_IDX(rs485->decd_idex)] ^ 0x20;
      } else {
        rs485->rx_pkg[i++] = rs485->rx_circle_buf[rs485->decd_idex];
      }

      if (i >= MAX_PKG_SIZE) return UNPKG_OVER_PACKGE_SIZE;
    }
  } else {
    rs485->rx_pkg_cplt_f = FALSE;
    return UNPKG_FINISH;
  }

  if (i < 6) return UNPKG_TOO_SHORT;

  if (rs485->Mode == SLAVE) {
    if (rs485->rx_pkg[0] != rs485->ip_addr) return OTHER_SLAVE_ADDR;
  } else {
    rs485->ip_addr = rs485->rx_pkg[0];
  }

  *upk_func = rs485->rx_pkg[1];
  for (int j = 0; j < i - 4; j++) {
    upk_data[j] = rs485->rx_pkg[j + 2];
  }
  *upk_data_len = i - 4;

  volatile uint32_t crc_value = crc8_block_calculate((uint8_t *)rs485->rx_pkg, i - 2);
  uint16_t calculatedCRC = (uint16_t)(crc_value & 0xFFFF);

  uint16_t receivedCRC = (rs485->rx_pkg[i - 1] << 8) | rs485->rx_pkg[i - 2];

  if (calculatedCRC != receivedCRC) {
    return CRC_ERROR;
  }

  return RS485_OK;
}

RsError_t RsChkRegAddrVal(bool handler_found, uint32_t Data_return, RsFunc_t rx_Func, RsFunc_t *tx_Func, uint8_t *tx_Data, uint8_t *tx_Data_len) {
  if ((Data_return >> 16) == ILLIGAL_FUNC) {
    *tx_Func = rx_Func + 0x80;
    tx_Data[0] = 0x01;
    *tx_Data_len = 1;
    return ILLIGAL_FUNC;
  } else if (!handler_found || (Data_return >> 16) == ILLIGAL_DATA_ADDR) {
    *tx_Func = rx_Func + 0x80;
    tx_Data[0] = 0x02;
    *tx_Data_len = 1;
    return ILLIGAL_DATA_ADDR;
  } else if ((Data_return >> 16) == ILLIGAL_DATA_VALUE) {
    *tx_Func = rx_Func + 0x80;
    tx_Data[0] = 0x03;
    *tx_Data_len = 1;
    return ILLIGAL_DATA_VALUE;
  } else if ((Data_return >> 16) == SLAVE_DEVICE_FAILURE) {
    *tx_Func = rx_Func + 0x80;
    tx_Data[0] = 0x04;
    *tx_Data_len = 1;
    return SLAVE_DEVICE_FAILURE;
  }
  return RS485_OK;
}

RsError_t RsSlaveDecdEncd(Rs485_t *rs485, RsFunc_t rx_Func, uint8_t *rx_Data, uint8_t rx_Data_len, RsFunc_t *tx_Func, uint8_t *tx_Data,
                          uint8_t *tx_Data_len) {
  if (rx_Func == READ_HOLDING_REGISTERS) {
    uint16_t Start_addr = rx_Data[0] << 8 | rx_Data[1];
    uint16_t Read_num = rx_Data[2] << 8 | rx_Data[3];

    tx_Data[0] = Read_num * 2;

    for (int i = 0; i < Read_num; i++) {
      uint32_t Data_return = 0;

      bool handler_found = false;
      for (int j = 0; j < g_handler_count; j++) {
        if (Start_addr + i >= g_handler_table[j].start_addr && Start_addr + i <= g_handler_table[j].end_addr) {
          Data_return = g_handler_table[j].handler(READ_HOLDING_REGISTERS, Start_addr + i, NULL, NULL, rs485->root);
          handler_found = true;
          break;
        }
      }
      tx_Data[1 + (i << 1)] = (Data_return >> 8) & 0xFF;
      tx_Data[2 + (i << 1)] = Data_return & 0xFF;

      RsError_t ret = RsChkRegAddrVal(handler_found, Data_return, rx_Func, tx_Func, tx_Data, tx_Data_len);
      if (ret != RS485_OK) return ret;
    }

    *tx_Func = READ_HOLDING_REGISTERS;
    *tx_Data_len = Read_num * 2 + 1;
  } else if (rx_Func == WRITE_SINGLE_REGISTER) {
    tx_Data[0] = rx_Data[0];
    tx_Data[1] = rx_Data[1];
    uint16_t Start_addr = rx_Data[0] << 8 | rx_Data[1];
    uint16_t Data = rx_Data[2] << 8 | rx_Data[3];

    uint32_t Data_return = 0;

    bool handler_found = false;
    for (int j = 0; j < g_handler_count; j++) {
      if (Start_addr >= g_handler_table[j].start_addr && Start_addr <= g_handler_table[j].end_addr) {
        Data_return = g_handler_table[j].handler(WRITE_SINGLE_REGISTER, Start_addr, Data, NULL, rs485->root);
        handler_found = true;
        break;
      }
    }

    RsError_t ret = RsChkRegAddrVal(handler_found, Data_return, rx_Func, tx_Func, tx_Data, tx_Data_len);
    if (ret != RS485_OK) return ret;

    tx_Data[2] = (Data_return >> 8) & 0xFF;
    tx_Data[3] = Data_return & 0xFF;

    *tx_Func = WRITE_SINGLE_REGISTER;
    *tx_Data_len = 4;
  } else if (rx_Func == WRITE_MULTIPLE_REGISTERS) {
    tx_Data[0] = rx_Data[0];
    tx_Data[1] = rx_Data[1];
    uint16_t Start_addr = rx_Data[0] << 8 | rx_Data[1];
    uint16_t Num_addr = rx_Data[2] << 8 | rx_Data[3];
    for (int i = 0; i < Num_addr; i++) {
      uint16_t Data = rx_Data[i * 2 + 5] << 8 | rx_Data[i * 2 + 6];
      uint32_t Data_return = 0;

      bool handler_found = false;
      for (int j = 0; j < g_handler_count; j++) {
        if (Start_addr + i >= g_handler_table[j].start_addr && Start_addr + i <= g_handler_table[j].end_addr) {
          Data_return = g_handler_table[j].handler(WRITE_MULTIPLE_REGISTERS, Start_addr + i, Data, NULL, rs485->root);
          handler_found = true;
          break;
        }
      }

      RsError_t ret = RsChkRegAddrVal(handler_found, Data_return, rx_Func, tx_Func, tx_Data, tx_Data_len);
      if (ret != RS485_OK) return ret;
    }

    tx_Data[2] = rx_Data[2];
    tx_Data[3] = rx_Data[3];

    *tx_Func = WRITE_MULTIPLE_REGISTERS;
    *tx_Data_len = 4;
  } else {
    *tx_Func = rx_Func + 0x80;
    tx_Data[0] = 0x01;
    *tx_Data_len = 1;
    return ILLIGAL_FUNC;
  }
  return RS485_OK;
}

RsError_t RsMasterDecd(Rs485_t *rs485, RsFunc_t rx_Func, uint8_t *rx_Data, uint8_t rx_Data_len) {
  if (rx_Func == READ_HOLDING_REGISTERS) {
    uint16_t Start_addr = rs485->reg_hdle_stat;
    for (int i = 0; i < rx_Data[0] / 2; i++) {
      uint16_t Data = rx_Data[i * 2 + 1] << 8 | rx_Data[i * 2 + 2];
      uint32_t Data_return = 0;

      bool handler_found = false;
      for (int j = 0; j < g_handler_count; j++) {
        if (Start_addr + i >= g_handler_table[j].start_addr && Start_addr + i <= g_handler_table[j].end_addr) {
          Data_return = g_handler_table[j].handler(WRITE_MULTIPLE_REGISTERS, Start_addr + i, Data, NULL, rs485->root);
          handler_found = true;
          break;
        }
      }

      RsError_t ret = RsChkRegAddrVal(handler_found, Data_return, rx_Func, NULL, NULL, NULL);
      if (ret != RS485_OK) return ret;
    }
  } else if (rx_Func == WRITE_SINGLE_REGISTER) {
    uint16_t Start_addr = rx_Data[0] << 8 | rx_Data[1];
    uint16_t Data = rx_Data[2] << 8 | rx_Data[3];

    uint32_t Data_return = 0;

    bool handler_found = false;
    for (int j = 0; j < g_handler_count; j++) {
      if (Start_addr >= g_handler_table[j].start_addr && Start_addr <= g_handler_table[j].end_addr) {
        Data_return = g_handler_table[j].handler(WRITE_SINGLE_REGISTER, Start_addr, Data, NULL, rs485->root);
        handler_found = true;
        break;
      }
    }

    RsError_t ret = RsChkRegAddrVal(handler_found, Data_return, rx_Func, NULL, NULL, NULL);
    if (ret != RS485_OK) return ret;
  } else if (rx_Func == WRITE_MULTIPLE_REGISTERS) {
    if (rx_Data[2] != rs485->tx_Data[2] || rx_Data[3] != rs485->tx_Data[3]) return WRITE_REGISTERS_ERROR;
    uint16_t Start_addr = rx_Data[0] << 8 | rx_Data[1];
    uint16_t Read_num = rx_Data[2] << 8 | rx_Data[3];
    for (int i = 0; i < Read_num * 2; i += 2) {
      uint16_t Data = rx_Data[i + 5] << 8 | rx_Data[i + 6];
      uint32_t Data_return = 0;

      bool handler_found = false;
      for (int j = 0; j < g_handler_count; j++) {
        if (Start_addr + i >= g_handler_table[j].start_addr && Start_addr + i <= g_handler_table[j].end_addr) {
          Data_return = g_handler_table[j].handler(WRITE_MULTIPLE_REGISTERS, Start_addr + i, Data, NULL, rs485->root);
          handler_found = true;
          break;
        }
      }

      RsError_t ret = RsChkRegAddrVal(handler_found, Data_return, rx_Func, NULL, NULL, NULL);
      if (ret != RS485_OK) return ret;
    }
  } else {
    return ILLIGAL_FUNC;
  }
  return RS485_OK;
}

RsError_t RsMasterEncd(Rs485_t *rs485, RsFunc_t Func, uint16_t stat_addr, uint16_t num_reg, uint16_t *data, uint8_t data_len, uint8_t *tx_Data,
                       uint8_t *tx_data_len) {
  tx_Data[0] = (stat_addr >> 8) & 0xFF;
  tx_Data[1] = stat_addr & 0xFF;
  if (Func == READ_HOLDING_REGISTERS || Func == WRITE_SINGLE_REGISTER) {
    tx_Data[2] = num_reg >> 8 & 0xFF;
    tx_Data[3] = num_reg & 0xFF;
    *tx_data_len = 4;
    // if (Func == READ_HOLDING_REGISTERS && data_len != 2) {
    //   return ENCODE_FOR_NUMBER;
    // } else if (Func == WRITE_SINGLE_REGISTER && data_len != 2) {
    //   return ENCODE_FOR_SINGLE_DATA;
    // }
  } else if (Func == WRITE_MULTIPLE_REGISTERS) {
    // if (data_len & 0X01) return ENCODE_FOR_NUMBER;
    tx_Data[2] = 0;
    tx_Data[3] = (data_len >> 1) & 0xFF;
    tx_Data[4] = data_len;
    for (int i = 0; i < data_len / 2; i++) {
      tx_Data[i * 2 + 5] = data[i] >> 8 & 0xFF;
      tx_Data[i * 2 + 6] = data[i] & 0xFF;
    }
    *tx_data_len = data_len + 5;
  }
  return RS485_OK;
}

RsError_t RsPkg(Rs485_t *rs485, uint8_t DstIpAddr, RsFunc_t pkg_func, uint8_t *pkg_data, uint8_t pkg_data_len) {
  memset(rs485->tx_pkg, 0, MAX_PKG_SIZE);
  rs485->tx_pkg[0] = DstIpAddr;
  rs485->tx_pkg[1] = pkg_func;
  for (int i = 0; i < pkg_data_len; i++) {
    rs485->tx_pkg[i + 2] = pkg_data[i];
  }
  uint32_t crc_value = crc8_block_calculate(rs485->tx_pkg, pkg_data_len + 2);
  rs485->tx_pkg[pkg_data_len + 2] = crc_value & 0xFF;
  rs485->tx_pkg[pkg_data_len + 3] = (crc_value >> 8) & 0xFF;

  for (int i = 0; i < pkg_data_len + 4; i++) {
    if (rs485->tx_pkg[i] == 0x7E || rs485->tx_pkg[i] == 0x7D) {
      rs485->tx_circle_buf[UPDATE_IDX(rs485->encd_idex)] = 0x7D;
      rs485->tx_circle_buf[UPDATE_IDX(rs485->encd_idex)] = rs485->tx_pkg[i] ^ 0x20;
    } else {
      rs485->tx_circle_buf[UPDATE_IDX(rs485->encd_idex)] = rs485->tx_pkg[i];
    }
  }
  rs485->tx_circle_buf[UPDATE_IDX(rs485->encd_idex)] = 0x7E;
  RS485_Tx_Data_ISR(rs485);

  return RS485_OK;
}

bool RsChkAvailable(Rs485_t *rs485) { return rs485->rx_pkg_cplt_f; }

RsError_t RS485Read(Rs485_t *rs485) {
  err = RsUnpkg(rs485, &rs485->rx_Func, rs485->rx_Data, &rs485->rx_Data_len);

  if (err != RS485_OK) {
    return err;
  }

  return RS485_OK;
}

RsError_t RS485ReadHandler(Rs485_t *rs485) {
  if (rs485->Mode == SLAVE) {
    err = RsSlaveDecdEncd(rs485, rs485->rx_Func, rs485->rx_Data, rs485->rx_Data_len, &rs485->tx_Func, rs485->tx_Data, &rs485->tx_Data_len);
  } else {
    err = RsMasterDecd(rs485, rs485->rx_Func, rs485->rx_Data, rs485->rx_Data_len);
  }
  rs485->rx_Func = 0;
  rs485->rx_Data_len = 0;
  memset(rs485->rx_Data, 0, SINGLE_DATA_MAX_SIZE);

  if (err != RS485_OK) {
    return err;
  }

  return RS485_OK;
}

RsError_t RS485WriteHandler(Rs485_t *rs485, uint16_t *data, uint8_t data_len) {
  rs485->tx_Data_len = 0;
  memset(rs485->tx_Data, 0, SINGLE_DATA_MAX_SIZE);

  if (rs485->Mode == MASTER) {
    if (rs485->tx_Func == READ_HOLDING_REGISTERS) {
      err = RsMasterEncd(rs485, rs485->tx_Func, rs485->reg_hdle_stat, rs485->reg_hdle_num, NULL, NULL, rs485->tx_Data, &rs485->tx_Data_len);
    } else if (rs485->tx_Func == WRITE_SINGLE_REGISTER) {
      err = RsMasterEncd(rs485, rs485->tx_Func, rs485->reg_hdle_stat, *data, NULL, NULL, rs485->tx_Data, &rs485->tx_Data_len);
    } else if (rs485->tx_Func == WRITE_MULTIPLE_REGISTERS) {
      err = RsMasterEncd(rs485, rs485->tx_Func, rs485->reg_hdle_stat, rs485->reg_hdle_num, data, data_len, rs485->tx_Data, &rs485->tx_Data_len);
    }
  }

  if (err != RS485_OK) {
    return err;
  }

  return RS485_OK;
}

RsError_t RS485Write(Rs485_t *rs485) {
  err = RsPkg(rs485, rs485->ip_addr, rs485->tx_Func, rs485->tx_Data, rs485->tx_Data_len);

  if (err != RS485_OK) {
    return err;
  }

  return RS485_OK;
}