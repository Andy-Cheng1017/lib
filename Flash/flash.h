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

typedef struct {
  uint16_t *p_data;
  uint16_t len;
  uint32_t offset;
} flash_region_t;

extern flash_region_t flash_regions[];
extern const uint16_t flash_region_count;

void flash_init(void);
void flash_pvm_save_all(void);

void flash_read(uint32_t read_addr, uint16_t *p_buffer, uint16_t num_read);
error_status flash_write_nocheck(uint32_t write_addr, uint16_t *p_buffer, uint16_t num_write);
error_status flash_write(uint32_t write_addr, uint16_t *p_Buffer, uint16_t num_write);

#ifdef __cplusplus
}
#endif

#endif
