/**
  **************************************************************************
  * @file     flash.h
  * @brief    flash header file
  **************************************************************************
  *                       Copyright notice & Disclaimer
  *
  * The software Board Support Package (BSP) that is made available to
  * download from Artery official website is the copyrighted work of Artery.
  * Artery authorizes customers to use, copy, and distribute the BSP
  * software and its related documentation for the purpose of design and
  * development in conjunction with Artery microcontrollers. Use of the
  * software is governed by this copyright notice and the following disclaimer.
  *
  * THIS SOFTWARE IS PROVIDED ON "AS IS" BASIS WITHOUT WARRANTIES,
  * GUARANTEES OR REPRESENTATIONS OF ANY KIND. ARTERY EXPRESSLY DISCLAIMS,
  * TO THE FULLEST EXTENT PERMITTED BY LAW, ALL EXPRESS, IMPLIED OR
  * STATUTORY OR OTHER WARRANTIES, GUARANTEES OR REPRESENTATIONS,
  * INCLUDING BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY,
  * FITNESS FOR A PARTICULAR PURPOSE, OR NON-INFRINGEMENT.
  *
  **************************************************************************
  */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __FLASH_H__
#define __FLASH_H__

#ifdef __cplusplus
extern "C" {
#endif

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

/** @addtogroup AT32F407_periph_examples
  * @{
  */

/** @addtogroup 407_FLASH_write_read
  * @{
  */


/** @defgroup FLASH_write_read_functions
  * @{
  */

void flash_read(uint32_t read_addr, uint16_t *p_buffer, uint16_t num_read);
error_status flash_write_nocheck(uint32_t write_addr, uint16_t *p_buffer, uint16_t num_write);
error_status flash_write(uint32_t write_addr,uint16_t *p_Buffer, uint16_t num_write);

/**
  * @}
  */

/**
  * @}
  */

/**
  * @}
  */

#ifdef __cplusplus
}
#endif

#endif

















