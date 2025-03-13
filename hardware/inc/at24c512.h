#ifndef AT24C512_H
#define AT24C512_H
#include "iic.h"

/* Define for EEPROM AT24C02 */
#define EE_24CXX_DEV_ADDR               (0x50U)
#define EE_24CXX_MEM_ADDR_LEN           (2U)
#define EE_24CXX_PAGE_SIZE              (128U)
#define EE_24CXX_CAPACITY               (65536U)

void at24c512_init(void);

int32_t at24c512_read(uint16_t MemAddr,uint8_t *dataBuf, uint32_t dataBufLen);

int32_t at24c512_write(uint16_t MemAddr, const uint8_t *dataBuf, uint32_t dataBufLen);

#endif