#ifndef IIC_H
#define IIC_H

#include "hc32_ll.h"

/* I2C unit define */
#define I2C1_UNIT              (CM_I2C1)
#define I2C1_FCG               (FCG1_PERIPH_I2C1)

/* Define port and pin for SDA and SCL */
#define I2C1_SCL_PORT          (GPIO_PORT_E)
#define I2C1_SCL_PIN           (GPIO_PIN_01)
#define I2C1_SDA_PORT          (GPIO_PORT_I)
#define I2C1_SDA_PIN           (GPIO_PIN_12)
#define I2C1_SCL_FUNC          (GPIO_FUNC_49)
#define I2C1_SDA_FUNC          (GPIO_FUNC_48)

#define I2C1_BAUDRATE        (100000UL)
#define I2C1_TIMEOUT         (0x40000U)

/* Define for EEPROM AT24C02 */
#define EE_24CXX_DEV_ADDR               (0x50U)
#define EE_24CXX_MEM_ADDR_LEN           (2U)
#define EE_24CXX_PAGE_SIZE              (128U)
#define EE_24CXX_CAPACITY               (65536U)



/**
 * @brief i2c1接口初始化
 * @param none
 * @retval int32_t  是否成功
 */
int32_t i2c1_init(void);
void i2c1_deinit(void);
int32_t i2c1_write(uint16_t u16Addr, const uint8_t *pu8Buf, uint32_t u32Len);
//int32_t i2c1_read(uint16_t u16Addr, uint8_t *pu8Buf, uint32_t u32Len);
//int32_t i2c1_waitidle(void);

#endif