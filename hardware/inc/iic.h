#ifndef IIC_H
#define IIC_H

#include "hc32_ll.h"

/* I2C1 unit define */
#define I2C1_UNIT              (CM_I2C1)
#define I2C1_FCG               (FCG1_PERIPH_I2C1)

/* Define port and pin for SDA and SCL */
#define I2C1_SCL_PORT          (GPIO_PORT_E)
#define I2C1_SCL_PIN           (GPIO_PIN_01)
#define I2C1_SDA_PORT          (GPIO_PORT_I)
#define I2C1_SDA_PIN           (GPIO_PIN_12)
#define I2C1_SCL_FUNC          (GPIO_FUNC_51)
#define I2C1_SDA_FUNC          (GPIO_FUNC_50)

#define I2C1_BAUDRATE           (100000UL)
#define I2C1_TIMEOUT            (0x40000U)




/* I2C2 unit define */
#define I2C2_UNIT              (CM_I2C2)
#define I2C2_FCG               (FCG1_PERIPH_I2C2)

/* Define port and pin for SDA and SCL */
#define I2C2_SCL_PORT          (GPIO_PORT_G)
#define I2C2_SCL_PIN           (GPIO_PIN_10)
#define I2C2_SDA_PORT          (GPIO_PORT_G)
#define I2C2_SDA_PIN           (GPIO_PIN_09)
#define I2C2_SCL_FUNC          (GPIO_FUNC_51)
#define I2C2_SDA_FUNC          (GPIO_FUNC_50)

#define I2C2_BAUDRATE           (100000UL)
#define I2C2_TIMEOUT            (0x40000U)



/* I2C2 unit define */
#define I2C3_UNIT              (CM_I2C3)
#define I2C3_FCG               (FCG1_PERIPH_I2C3)

/* Define port and pin for SDA and SCL */
#define I2C3_SCL_PORT          (GPIO_PORT_B)
#define I2C3_SCL_PIN           (GPIO_PIN_06)
#define I2C3_SDA_PORT          (GPIO_PORT_B)
#define I2C3_SDA_PIN           (GPIO_PIN_05)
#define I2C3_SCL_FUNC          (GPIO_FUNC_53)
#define I2C3_SDA_FUNC          (GPIO_FUNC_52)

#define I2C3_BAUDRATE           (100000UL)
#define I2C3_TIMEOUT            (0x40000U)







/**
 * @brief i2c1接口初始化
 * @param none
 * @retval int32_t  是否成功
 */
int32_t i2c1_init   (void);
void    i2c1_deinit (void);

int32_t i2c2_init   (void);
void    i2c2_deinit (void);

int32_t i2c3_init   (void);
void    i2c3_deinit (void);

int32_t i2c_write(CM_I2C_TypeDef * I2Cx,uint16_t DevAddr , uint16_t MemAddr,uint16_t MemAddrLen , const uint8_t *dataBuf, uint32_t dataBufLen,uint32_t u32TimeOut);
int32_t i2c_read (CM_I2C_TypeDef * I2Cx,uint16_t DevAddr , uint16_t MemAddr,uint16_t MemAddrLen , uint8_t *dataBuf, uint32_t dataBufLen,uint32_t u32TimeOut);




#endif