#include "uart_debug.h"

static void UART_DEBUG_RxError_IrqCallback(void);
void init_debug(void)
{
    stc_usart_uart_init_t stcUartInit;
    stc_irq_signin_config_t stcIrqSigninConfig;

    /* Configure USART RX/TX pin. */
    GPIO_SetFunc(UART_DEBUG_RX_PORT, UART_DEBUG_RX_PIN, UART_DEBUG_RX_GPIO_FUNC);
    GPIO_SetFunc(UART_DEBUG_TX_PORT, UART_DEBUG_TX_PIN, UART_DEBUG_TX_GPIO_FUNC);

    /* Enable peripheral clock */
    UART_DEBUG_FCG(ENABLE);

    /* Initialize UART. */
    (void)USART_UART_StructInit(&stcUartInit);
    stcUartInit.u32ClockDiv = USART_CLK_DIV64;
    stcUartInit.u32Baudrate = UART_DEBUG_BAUDRATE;
    stcUartInit.u32OverSampleBit = USART_OVER_SAMPLE_8BIT;
    if (LL_OK != USART_UART_Init(UART_DEBUG_UNIT, &stcUartInit, NULL)) {
        for (;;) {
        }
    }

    /* Register error IRQ handler && configure NVIC. */
    stcIrqSigninConfig.enIRQn = UART_DEBUG_RX_ERR_IRQn;
    stcIrqSigninConfig.enIntSrc = UART_DEBUG_RX_ERR_INT_SRC;
    stcIrqSigninConfig.pfnCallback = &UART_DEBUG_RxError_IrqCallback;
    (void)INTC_IrqSignIn(&stcIrqSigninConfig);
    NVIC_ClearPendingIRQ(stcIrqSigninConfig.enIRQn);
    NVIC_SetPriority(stcIrqSigninConfig.enIRQn, DDL_IRQ_PRIO_DEFAULT);
    NVIC_EnableIRQ(stcIrqSigninConfig.enIRQn);

    /* Enable RX/TX function */
    USART_FuncCmd(UART_DEBUG_UNIT, (USART_RX | USART_INT_RX | USART_TX), ENABLE);
}

void deint_debug(void)
{
    USART_DeInit(UART_DEBUG_UNIT);
    UART_DEBUG_FCG(DISABLE);
    GPIO_SetFunc(UART_DEBUG_RX_PORT, UART_DEBUG_RX_PIN, GPIO_FUNC_0);
    GPIO_SetFunc(UART_DEBUG_TX_PORT, UART_DEBUG_TX_PIN, GPIO_FUNC_0);
}

static void UART_DEBUG_RxError_IrqCallback(void)
{
    (void)USART_ReadData(UART_DEBUG_UNIT);

    USART_ClearStatus(UART_DEBUG_UNIT, (USART_FLAG_PARITY_ERR | USART_FLAG_FRAME_ERR | USART_FLAG_OVERRUN));
}

void uart_debug_send(const char *str)
{
    //rt_enter_critical(); 
	while (*str!='\0'){ 
        while (RESET == USART_GetStatus(UART_DEBUG_UNIT, USART_FLAG_TX_EMPTY)){}
        USART_WriteData(UART_DEBUG_UNIT, *str++); 
	} 
	//rt_exit_critical();
    
}

__WEAKDEF int32_t _write(int fd, char data[], int32_t size)
{
    int32_t i = -1;
    //rt_enter_critical(); 
    (void)fd;
    for(i = 0; i < size; i++){
        while (RESET == USART_GetStatus(UART_DEBUG_UNIT, USART_FLAG_TX_EMPTY)){}
        USART_WriteData(UART_DEBUG_UNIT, data[i]);
    }
    //rt_exit_critical();
    return i ? i : -1;
}