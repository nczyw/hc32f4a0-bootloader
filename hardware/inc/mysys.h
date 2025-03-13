#ifndef MYSYS_H
#define MYSYS_H

#include "hc32_ll.h"

/**
 * @defgroup EV_HC32F4A0_LQFP176_XTAL_CONFIG EV_HC32F4A0_LQFP176 XTAL port/pin definition
 * @{
 */
#define BSP_XTAL_PORT                   (GPIO_PORT_H)
#define BSP_XTAL_PIN                    (GPIO_PIN_00 | GPIO_PIN_01)
/**
 * @}
 */

/**
 * @defgroup EV_HC32F4A0_LQFP176_XTAL32_CONFIG EV_HC32F4A0_LQFP176 XTAL32 port/pin definition
 * @{
 */
#define BSP_XTAL32_PORT                 (GPIO_PORT_C)
#define BSP_XTAL32_PIN                  (GPIO_PIN_14 | GPIO_PIN_15)

/**
 * @brief  BSP clock initialize.
 *         SET board system clock to PLLH@240MHz
 *         Flash: 5 wait
 *         SRAM_HS: 0 wait
 *         SRAM1_2_3_4_B: 1 wait
 *         PCLK0: 240MHz
 *         PCLK1: 120MHz
 *         PCLK2: 60MHz
 *         PCLK3: 60MHz
 *         PCLK4: 120MHz
 *         EXCLK: 120MHz
 *         HCLK:  240MHz
 * @param  None
 * @retval None
 */
void BSP_CLK_Init(void);

/**
 * @brief  BSP Xtal32 initialize.
 * @param  None
 * @retval int32_t:
 *         - LL_OK: XTAL32 enable successfully
 *         - LL_ERR_TIMEOUT: XTAL32 enable timeout.
 */
int32_t BSP_XTAL32_Init(void);



#endif
