#include "iic.h"

static int32_t BSP_I2C_Init(CM_I2C_TypeDef *I2Cx); 
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
    return BSP_I2C_Init(I2C1_UNIT);
}
void i2c1_deinit(void)
{
    /* Initialize I2C port*/
    GPIO_SetFunc(I2C1_SCL_PORT, I2C1_SCL_PIN, GPIO_FUNC_0);
    GPIO_SetFunc(I2C1_SDA_PORT, I2C1_SDA_PORT, GPIO_FUNC_0);
    (void)I2C_DeInit(I2C1_UNIT);
}
int32_t i2c1_write(uint16_t u16Addr, const uint8_t *pu8Buf, uint32_t u32Len)
{
    return 0;
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
static int32_t BSP_I2C_Init(CM_I2C_TypeDef *I2Cx)
{
    int32_t i32Ret;
    float32_t fErr;
    stc_i2c_init_t stcI2cInit;
    uint32_t I2cSrcClk;
    uint32_t I2cClkDiv;
    uint32_t I2cClkDivReg;

    I2cSrcClk = I2C_SRC_CLK;
    I2cClkDiv = I2cSrcClk / I2C1_BAUDRATE / I2C_WIDTH_MAX_IMME;
    for (I2cClkDivReg = I2C_CLK_DIV1; I2cClkDivReg <= I2C_CLK_DIV128; I2cClkDivReg++) {
        if (I2cClkDiv < (1UL << I2cClkDivReg)) {
            break;
        }
    }

    (void)I2C_DeInit(I2Cx);
    (void)I2C_StructInit(&stcI2cInit);
    stcI2cInit.u32Baudrate = I2C1_BAUDRATE;
    stcI2cInit.u32SclTime  = (uint32_t)((uint64_t)400UL * ((uint64_t)I2cSrcClk / ((uint64_t)1UL << I2cClkDivReg)) / (uint64_t)1000000000UL);  /* SCL time is about 400nS in EVB board */
    stcI2cInit.u32ClockDiv = I2cClkDivReg;
    i32Ret = I2C_Init(I2Cx, &stcI2cInit, &fErr);

    if (LL_OK == i32Ret) {
        I2C_BusWaitCmd(I2Cx, ENABLE);
        I2C_Cmd(I2Cx, ENABLE);
    }

    return i32Ret;
}