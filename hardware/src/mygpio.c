#include "mygpio.h"
/**
 * @brief Initialize GPIO pins
 * @param None
 * @retval None
 */
void init_gpio(void){
    stc_gpio_init_t stcGpioInit;

    (void)GPIO_StructInit(&stcGpioInit);
    stcGpioInit.u16PinState = PIN_STAT_RST;
    stcGpioInit.u16PinDir = PIN_DIR_OUT;
    (void)GPIO_Init(SYSRUN_G_PORT, SYSRUN_G_PIN, &stcGpioInit);
}
