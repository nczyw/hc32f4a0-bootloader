#ifndef MYGPIO_H
#define MYGPIO_H

#include "hc32_ll.h"

#define SYSRUN_G_PORT   (GPIO_PORT_C)
#define SYSRUN_G_PIN    (GPIO_PIN_13)

#define DeUpdate()         GPIO_SetPins(GPIO_PORT_C,GPIO_PIN_13)
#define Updating()         GPIO_ResetPins(GPIO_PORT_C,GPIO_PIN_13)

/**
 * @brief Initialize GPIO pins
 * @param None
 * @retval None
 */
void init_gpio(void);

#endif