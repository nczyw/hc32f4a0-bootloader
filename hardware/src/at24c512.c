#include "at24c512.h"

void at24c512_init(void)
{
    i2c1_init();
}

int32_t at24c512_read(uint16_t MemAddr, uint8_t *dataBuf, uint32_t dataBufLen)
{
    return i2c_read(I2C1_UNIT,EE_24CXX_DEV_ADDR,MemAddr,EE_24CXX_MEM_ADDR_LEN,dataBuf,dataBufLen,I2C1_TIMEOUT);
}

int32_t at24c512_write(uint16_t MemAddr, const uint8_t *dataBuf, uint32_t dataBufLen)
{
    return i2c_write(I2C1_UNIT,EE_24CXX_DEV_ADDR,MemAddr,EE_24CXX_MEM_ADDR_LEN,dataBuf,dataBufLen,I2C1_TIMEOUT);
}
