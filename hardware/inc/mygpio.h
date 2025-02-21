#ifndef MYGPIO_H
#define MYGPIO_H

#include "mysys.h"

#define SYSRUN_G_PORT   (GPIO_PORT_C)
#define SYSRUN_G_PIN    (GPIO_PIN_13)

#define SYSRUN()        (GPIO_TogglePins(SYSRUN_G_PORT, SYSRUN_G_PIN))

/**
 * @brief Initialize GPIO pins
 * @param None
 * @retval None
 */
void init_gpio(void);

#endif