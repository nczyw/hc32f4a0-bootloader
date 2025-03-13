#include "mygpio.h"
/**
 * @brief Initialize GPIO pins
 * @param None
 * @retval None
 */
void init_gpio(void){
    stc_gpio_init_t stcGpioInit;

    //开始配置输出
    GPIO_StructInit(&stcGpioInit);
    stcGpioInit.u16PinState = PIN_STAT_RST;
    stcGpioInit.u16PinDir = PIN_DIR_OUT;
    GPIO_Init(SYSRUN_G_PORT, SYSRUN_G_PIN, &stcGpioInit);  
/*
    GPIO_Init(SELECTAIR_PORT, SELECTAIR_PIN, &stcGpioInit);
    GPIO_Init(POSITIVEPREHOLD_PORT, POSITIVEPREHOLD_PIN, &stcGpioInit);
    GPIO_Init(NEGATIVEPREHOLD_PORT, NEGATIVEPREHOLD_PIN, &stcGpioInit);
    GPIO_Init(TESTEND_PORT, TESTEND_PIN, &stcGpioInit);
    GPIO_Init(CONNECTAIR_PORT, CONNECTAIR_PIN, &stcGpioInit);
    GPIO_Init(TESTOK_PORT, TESTOK_PIN, &stcGpioInit);
    GPIO_Init(INDICATIONOK_PORT, INDICATIONOK_PIN, &stcGpioInit);
    GPIO_Init(OT8_PORT, OT8_PIN, &stcGpioInit);

    //开始配置输入
    GPIO_StructInit(&stcGpioInit);
    stcGpioInit.u16PullUp   = PIN_PU_ON;
    stcGpioInit.u16PinDir   = PIN_DIR_IN;
    GPIO_Init(AIR_OK_PORT, AIR_OK_PIN, &stcGpioInit);
    GPIO_Init(INSERT_PORT, INSERT_PIN, &stcGpioInit);
    GPIO_Init(START_PORT, START_PIN, &stcGpioInit);
    GPIO_Init(IN4_PORT, IN4_PIN, &stcGpioInit);
*/
}

void set_output(uint8_t u8Port, uint16_t u16Pin, uint8_t u8Value)
{
    if (u8Value){
        GPIO_SetPins(u8Port, u16Pin);
    }
    else{
        GPIO_ResetPins(u8Port, u16Pin);
    }
}
