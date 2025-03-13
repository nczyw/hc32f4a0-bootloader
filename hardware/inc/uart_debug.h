#ifndef UART_DEBUG_H
#define UART_DEBUG_H

#include "hc32_ll.h"

/* USART RX/TX pin definition */
#define UART_DEBUG_RX_PORT                   (GPIO_PORT_B)   /* PH13: USART1_RX */
#define UART_DEBUG_RX_PIN                    (GPIO_PIN_00)
#define UART_DEBUG_RX_GPIO_FUNC              (GPIO_FUNC_39)

#define UART_DEBUG_TX_PORT                   (GPIO_PORT_B)   /* PH15: USART1_TX */
#define UART_DEBUG_TX_PIN                    (GPIO_PIN_01)
#define UART_DEBUG_TX_GPIO_FUNC              (GPIO_FUNC_38)

/* USART unit definition */
#define UART_DEBUG_UNIT                      (CM_USART4)
#define UART_DEBUG_FCG(n)                    (FCG_Fcg3PeriphClockCmd(FCG3_PERIPH_USART4,n))

/* USART interrupt definition */
/* USART interrupt definition */
#define UART_DEBUG_RX_ERR_IRQn               (INT000_IRQn)
#define UART_DEBUG_RX_ERR_INT_SRC            (INT_SRC_USART4_EI)

#define UART_DEBUG_BAUDRATE                  (115200UL)

void deint_debug(void);
void console_output(const char *str);

#if (LL_PRINT_ENABLE == DDL_ON)
#define BSP_PRINTF_BAUDRATE_ERR_MAX     (0.025F)
int32_t BSP_PRINTF_Preinit(void *vpDevice, uint32_t u32Baudrate);
#endif

#endif