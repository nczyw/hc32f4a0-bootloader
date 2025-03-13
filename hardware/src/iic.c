#include "iic.h"

static int32_t BSP_I2C_Init(CM_I2C_TypeDef *I2Cx , uint32_t baudrate); 
int32_t i2c1_init(void)
{
    stc_gpio_init_t stcGpioInit;
    /* Configuration I2C GPIO */
    (void)GPIO_StructInit(&stcGpioInit);
    (void)GPIO_Init(I2C1_SCL_PORT, I2C1_SCL_PIN, &stcGpioInit);
    (void)GPIO_Init(I2C1_SDA_PORT, I2C1_SDA_PIN, &stcGpioInit);
    GPIO_SetFunc(I2C1_SCL_PORT, I2C1_SCL_PIN, I2C1_SCL_FUNC);
    GPIO_SetFunc(I2C1_SDA_PORT, I2C1_SDA_PIN, I2C1_SDA_FUNC);
    /* Enable I2C Peripheral*/
    FCG_Fcg1PeriphClockCmd(I2C1_FCG, ENABLE);
    return BSP_I2C_Init(I2C1_UNIT,I2C1_BAUDRATE);
}
void i2c1_deinit(void)
{
    /* Initialize I2C port*/
    GPIO_SetFunc(I2C1_SCL_PORT, I2C1_SCL_PIN, GPIO_FUNC_0);
    GPIO_SetFunc(I2C1_SDA_PORT, I2C1_SDA_PORT, GPIO_FUNC_0);
    (void)I2C_DeInit(I2C1_UNIT);
}
int32_t i2c2_init(void)
{
    stc_gpio_init_t stcGpioInit;
    /* Configuration I2C GPIO */
    (void)GPIO_StructInit(&stcGpioInit);
    (void)GPIO_Init(I2C2_SCL_PORT, I2C2_SCL_PIN, &stcGpioInit);
    (void)GPIO_Init(I2C2_SDA_PORT, I2C2_SDA_PIN, &stcGpioInit);
    GPIO_SetFunc(I2C2_SCL_PORT, I2C2_SCL_PIN, I2C2_SCL_FUNC);
    GPIO_SetFunc(I2C2_SDA_PORT, I2C2_SDA_PIN, I2C2_SDA_FUNC);
    /* Enable I2C Peripheral*/
    FCG_Fcg1PeriphClockCmd(I2C2_FCG, ENABLE);
    return BSP_I2C_Init(I2C2_UNIT,I2C2_BAUDRATE);
}
void i2c2_deinit(void)
{
     /* Initialize I2C port*/
     GPIO_SetFunc(I2C2_SCL_PORT, I2C2_SCL_PIN, GPIO_FUNC_0);
     GPIO_SetFunc(I2C2_SDA_PORT, I2C2_SDA_PORT, GPIO_FUNC_0);
     (void)I2C_DeInit(I2C2_UNIT);
}
int32_t i2c3_init(void)
{
    stc_gpio_init_t stcGpioInit;
    /* Configuration I2C GPIO */
    (void)GPIO_StructInit(&stcGpioInit);
    (void)GPIO_Init(I2C3_SCL_PORT, I2C3_SCL_PIN, &stcGpioInit);
    (void)GPIO_Init(I2C3_SDA_PORT, I2C3_SDA_PIN, &stcGpioInit);
    GPIO_SetFunc(I2C3_SCL_PORT, I2C3_SCL_PIN, I2C3_SCL_FUNC);
    GPIO_SetFunc(I2C3_SDA_PORT, I2C3_SDA_PIN, I2C3_SDA_FUNC);
    /* Enable I2C Peripheral*/
    FCG_Fcg1PeriphClockCmd(I2C3_FCG, ENABLE);
    return BSP_I2C_Init(I2C3_UNIT,I2C3_BAUDRATE);
}
void i2c3_deinit(void)
{
    /* Initialize I2C port*/
    GPIO_SetFunc(I2C3_SCL_PORT, I2C3_SCL_PIN, GPIO_FUNC_0);
    GPIO_SetFunc(I2C3_SDA_PORT, I2C3_SDA_PORT, GPIO_FUNC_0);
    (void)I2C_DeInit(I2C3_UNIT);
}
int32_t i2c_write(CM_I2C_TypeDef *I2Cx, uint16_t DevAddr, uint16_t MemAddr, uint16_t MemAddrLen, const uint8_t *dataBuf, uint32_t dataBufLen, uint32_t u32TimeOut)
{
    int32_t i32Ret;

    uint16_t MemAddrTemp;
    if(MemAddrLen == 1U){       //单字节内存地址
        MemAddrTemp = MemAddr;
    }
    else{
        MemAddrTemp = (uint16_t)((((uint32_t)MemAddr >> 8) & 0xFFUL) + (((uint32_t)MemAddr << 8) & 0xFF00UL));
    }

    I2C_SWResetCmd(I2Cx, ENABLE);
    I2C_SWResetCmd(I2Cx, DISABLE);
    i32Ret = I2C_Start(I2Cx, u32TimeOut);
    if (LL_OK == i32Ret) {
        i32Ret = I2C_TransAddr(I2Cx, DevAddr, I2C_DIR_TX, u32TimeOut);

        if (LL_OK == i32Ret) {
            i32Ret = I2C_TransData(I2Cx, (const uint8_t *)&MemAddrTemp, MemAddrLen, u32TimeOut);
            if (LL_OK == i32Ret) {
                i32Ret = I2C_TransData(I2Cx, dataBuf, dataBufLen, u32TimeOut);
            }
        }
    }
    (void)I2C_Stop(I2Cx, u32TimeOut);
    return i32Ret;
}
int32_t i2c_read(CM_I2C_TypeDef *I2Cx, uint16_t DevAddr, uint16_t MemAddr, uint16_t MemAddrLen, uint8_t *dataBuf, uint32_t dataBufLen,uint32_t u32TimeOut)
{
    int32_t i32Ret;
    uint16_t MemAddrTemp;
    if(MemAddrLen == 1U){       //单字节内存地址
        MemAddrTemp = MemAddr;
    }
    else{
        MemAddrTemp = (uint16_t)((((uint32_t)MemAddr >> 8) & 0xFFUL) + (((uint32_t)MemAddr << 8) & 0xFF00UL));
    }
    I2C_SWResetCmd(I2Cx, ENABLE);
    I2C_SWResetCmd(I2Cx, DISABLE);
    i32Ret = I2C_Start(I2Cx, u32TimeOut);
    if (LL_OK == i32Ret) {
        i32Ret = I2C_TransAddr(I2Cx, DevAddr, I2C_DIR_TX, u32TimeOut);
        if (LL_OK == i32Ret) {
            i32Ret = I2C_TransData(I2Cx, (const uint8_t *)&MemAddrTemp, MemAddrLen, u32TimeOut);
            if (LL_OK == i32Ret) {
                i32Ret = I2C_Restart(I2Cx, u32TimeOut);
                if (LL_OK == i32Ret) {
                    if (1UL == dataBufLen) {
                        I2C_AckConfig(I2Cx, I2C_NACK);
                    }

                    i32Ret = I2C_TransAddr(I2Cx, DevAddr, I2C_DIR_RX, u32TimeOut);
                    if (LL_OK == i32Ret) {
                        i32Ret = I2C_MasterReceiveDataAndStop(I2Cx, dataBuf, dataBufLen, u32TimeOut);
                    }
                    I2C_AckConfig(I2Cx, I2C_ACK);
                }
            }
        }
    }

    if (LL_OK != i32Ret) {
        (void)I2C_Stop(I2Cx, I2C1_TIMEOUT);
    }

    return i32Ret;
}

/**
 * @brief  BSP I2C initialize
 * @param  [in] I2Cx                Pointer to I2C instance register base.
 *                                  This parameter can be a value of the following:
 *         @arg CM_I2Cx:            I2C instance register base.
 * @retval int32_t:
 *            - LL_OK:              Configure success
 *            - LL_ERR_INVD_PARAM:  Invalid parameter
 */
static int32_t BSP_I2C_Init(CM_I2C_TypeDef *I2Cx , uint32_t baudrate)
{
    int32_t i32Ret;
    float32_t fErr;
    stc_i2c_init_t stcI2cInit;
    uint32_t I2cSrcClk;
    uint32_t I2cClkDiv;
    uint32_t I2cClkDivReg;

    I2cSrcClk = I2C_SRC_CLK;
    I2cClkDiv = I2cSrcClk / baudrate / I2C_WIDTH_MAX_IMME;
    for (I2cClkDivReg = I2C_CLK_DIV1; I2cClkDivReg <= I2C_CLK_DIV128; I2cClkDivReg++) {
        if (I2cClkDiv < (1UL << I2cClkDivReg)) {
            break;
        }
    }

    (void)I2C_DeInit(I2Cx);
    (void)I2C_StructInit(&stcI2cInit);
    stcI2cInit.u32Baudrate = baudrate;
    stcI2cInit.u32SclTime  = (uint32_t)((uint64_t)400UL * ((uint64_t)I2cSrcClk / ((uint64_t)1UL << I2cClkDivReg)) / (uint64_t)1000000000UL);  /* SCL time is about 400nS in EVB board */
    stcI2cInit.u32ClockDiv = I2cClkDivReg;
    i32Ret = I2C_Init(I2Cx, &stcI2cInit, &fErr);

    if (LL_OK == i32Ret) {
        I2C_BusWaitCmd(I2Cx, ENABLE);
        I2C_Cmd(I2Cx, ENABLE);
    }

    return i32Ret;
}