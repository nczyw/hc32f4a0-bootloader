#include "main.h"
/**
 * @brief  Main function of GPIO project
 * @param  None
 * @retval int32_t return value, if needed
 */
int main(void){
    LL_PERIPH_WE(LL_PERIPH_ALL);
    BSP_CLK_Init();
    BSP_XTAL32_Init();
    init_gpio();
    LL_PERIPH_WP(LL_PERIPH_ALL);
    for (;;) {
#ifdef BOOTLOADER
        SYSRUN();
#endif
        DDL_DelayMS(100);
    }
}