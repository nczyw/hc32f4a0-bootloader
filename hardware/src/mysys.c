#include "mysys.h"

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
__WEAKDEF void BSP_CLK_Init(){
    stc_clock_xtal_init_t stcXtalInit;
    stc_clock_pll_init_t stcPLLHInit;

    CLK_SetClockDiv(CLK_BUS_CLK_ALL,
                    (CLK_PCLK0_DIV1 | CLK_PCLK1_DIV2 | CLK_PCLK2_DIV4 |
                     CLK_PCLK3_DIV4 | CLK_PCLK4_DIV2 | CLK_EXCLK_DIV2 |
                     CLK_HCLK_DIV1));
    (void)CLK_XtalStructInit(&stcXtalInit);
    /* Config Xtal and enable Xtal */
    stcXtalInit.u8Mode   = CLK_XTAL_MD_OSC;
    stcXtalInit.u8Drv    = CLK_XTAL_DRV_ULOW;
    stcXtalInit.u8State  = CLK_XTAL_ON;
    stcXtalInit.u8StableTime = CLK_XTAL_STB_2MS;

    GPIO_AnalogCmd(BSP_XTAL_PORT, BSP_XTAL_PIN, ENABLE);
    (void)CLK_XtalInit(&stcXtalInit);

    (void)CLK_PLLStructInit(&stcPLLHInit);
    /* VCO = (8/1)*100 = 800MHz*/
    stcPLLHInit.u8PLLState      = CLK_PLL_ON;
    stcPLLHInit.PLLCFGR         = 0UL;
    stcPLLHInit.PLLCFGR_f.PLLM  = 1UL - 1UL;
    stcPLLHInit.PLLCFGR_f.PLLN  = 100UL - 1UL;
    stcPLLHInit.PLLCFGR_f.PLLP  = 4UL - 1UL;
    stcPLLHInit.PLLCFGR_f.PLLQ  = 4UL - 1UL;
    stcPLLHInit.PLLCFGR_f.PLLR  = 4UL - 1UL;
    stcPLLHInit.PLLCFGR_f.PLLSRC = CLK_PLL_SRC_XTAL;
    (void)CLK_PLLInit(&stcPLLHInit);

    /* SRAM Read/Write wait cycle setting */
    SRAM_SetWaitCycle(SRAM_SRAMH, SRAM_WAIT_CYCLE0, SRAM_WAIT_CYCLE0);
    SRAM_SetWaitCycle((SRAM_SRAM123 | SRAM_SRAM4 | SRAM_SRAMB), SRAM_WAIT_CYCLE1, SRAM_WAIT_CYCLE1);
    /* 0-wait @ 40MHz */
    EFM_SetWaitCycle(EFM_WAIT_CYCLE5);
    /* 4 cycles for 200 ~ 250MHz */
    GPIO_SetReadWaitCycle(GPIO_RD_WAIT4);
    CLK_SetSysClockSrc(CLK_SYSCLK_SRC_PLL);
    GPIO_SetDebugPort(GPIO_PIN_SWO,DISABLE);
    GPIO_SetDebugPort(GPIO_PIN_TDI,DISABLE);
    GPIO_SetDebugPort(GPIO_PIN_TRST,DISABLE);
}

/**
 * @brief  BSP Xtal32 initialize.
 * @param  None
 * @retval int32_t:
 *         - LL_OK: XTAL32 enable successfully
 *         - LL_ERR_TIMEOUT: XTAL32 enable timeout.
 */
int32_t BSP_XTAL32_Init(void){
    stc_clock_xtal32_init_t stcXtal32Init;
    stc_fcm_init_t stcFcmInit;
    uint32_t u32TimeOut = 0UL;
    uint32_t u32Time = HCLK_VALUE / 5UL;
    
    if (CLK_XTAL32_ON == READ_REG8(CM_CMU->XTAL32CR)) {
        /* Disable xtal32 */
        (void)CLK_Xtal32Cmd(DISABLE);
    }

    /* Xtal32 config */
    (void)CLK_Xtal32StructInit(&stcXtal32Init);
    stcXtal32Init.u8State  = CLK_XTAL32_ON;
    stcXtal32Init.u8Drv    = CLK_XTAL32_DRV_MID;
    stcXtal32Init.u8Filter = CLK_XTAL32_FILTER_ALL_MD;
    GPIO_AnalogCmd(BSP_XTAL32_PORT, BSP_XTAL32_PIN, ENABLE);
    (void)CLK_Xtal32Init(&stcXtal32Init);

    /* FCM config */
    FCG_Fcg0PeriphClockCmd(FCG0_PERIPH_FCM, ENABLE);
    (void)FCM_StructInit(&stcFcmInit);
    stcFcmInit.u32RefClock       = FCM_REF_CLK_MRC;
    stcFcmInit.u32RefClockDiv    = FCM_REF_CLK_DIV8192;
    stcFcmInit.u32RefClockEdge   = FCM_REF_CLK_RISING;
    stcFcmInit.u32TargetClock    = FCM_TARGET_CLK_XTAL32;
    stcFcmInit.u32TargetClockDiv = FCM_TARGET_CLK_DIV1;
    stcFcmInit.u16LowerLimit     = (uint16_t)((XTAL32_VALUE / (MRC_VALUE / 8192U)) * 96UL / 100UL);
    stcFcmInit.u16UpperLimit     = (uint16_t)((XTAL32_VALUE / (MRC_VALUE / 8192U)) * 104UL / 100UL);
    (void)FCM_Init(CM_FCM, &stcFcmInit);
    /* Enable FCM, to ensure xtal32 stable */
    FCM_Cmd(CM_FCM, ENABLE);
    for (;;) {
        if (SET == FCM_GetStatus(CM_FCM, FCM_FLAG_END)) {
            FCM_ClearStatus(CM_FCM, FCM_FLAG_END);
            if (SET == FCM_GetStatus(CM_FCM, FCM_FLAG_ERR | FCM_FLAG_OVF)) {
                FCM_ClearStatus(CM_FCM, FCM_FLAG_ERR | FCM_FLAG_OVF);
            } else {
                (void)FCM_DeInit(CM_FCM);
                FCG_Fcg0PeriphClockCmd(FCG0_PERIPH_FCM, DISABLE);
                return LL_OK;
            }
        }
        u32TimeOut++;
        if (u32TimeOut > u32Time) {
            (void)FCM_DeInit(CM_FCM);
            FCG_Fcg0PeriphClockCmd(FCG0_PERIPH_FCM, DISABLE);
            return LL_ERR_TIMEOUT;
        }
    }
    
}

#if (LL_PRINT_ENABLE == DDL_ON)
/**
 * @brief  BSP printf device, clock and port pre-initialize.
 * @param  [in] vpDevice                Pointer to print device
 * @param  [in] u32Baudrate             Print device communication baudrate
 * @retval int32_t:
 *           - LL_OK:                   Initialize successfully.
 *           - LL_ERR:                  Initialize unsuccessfully.
 *           - LL_ERR_INVD_PARAM:       The u32Baudrate value is 0.
 */
int32_t BSP_PRINTF_Preinit(void *vpDevice, uint32_t u32Baudrate)
{
    uint32_t u32Div;
    float32_t f32Error;
    stc_usart_uart_init_t stcUartInit;
    int32_t i32Ret = LL_ERR_INVD_PARAM;

    (void)vpDevice;

    if (0UL != u32Baudrate) {
        /* Set TX port function */
        GPIO_SetFunc(BSP_PRINTF_PORT, BSP_PRINTF_PIN, BSP_PRINTF_PORT_FUNC);

        /* Enable clock  */
        FCG_Fcg3PeriphClockCmd(BSP_PRINTF_DEVICE_FCG, ENABLE);

        /* Configure UART */
        (void)USART_UART_StructInit(&stcUartInit);
        stcUartInit.u32OverSampleBit = USART_OVER_SAMPLE_8BIT;
        (void)USART_UART_Init(BSP_PRINTF_DEVICE, &stcUartInit, NULL);

        for (u32Div = 0UL; u32Div <= USART_CLK_DIV64; u32Div++) {
            USART_SetClockDiv(BSP_PRINTF_DEVICE, u32Div);
            i32Ret = USART_SetBaudrate(BSP_PRINTF_DEVICE, u32Baudrate, &f32Error);
            if ((LL_OK == i32Ret) && \
                ((-BSP_PRINTF_BAUDRATE_ERR_MAX <= f32Error) && (f32Error <= BSP_PRINTF_BAUDRATE_ERR_MAX))) {
                USART_FuncCmd(BSP_PRINTF_DEVICE, USART_TX, ENABLE);
                break;
            } else {
                i32Ret = LL_ERR;
            }
        }
    }

    return i32Ret;
}
#endif