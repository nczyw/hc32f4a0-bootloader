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
    //SDCardInit();
    LL_PERIPH_WP(LL_PERIPH_ALL);
    char  file[512] = {0};
  for (;;) {

        if(IAP_FileFind(file) == FR_OK) {
		//	IAP_Init(file);	//应用程序升级
		}
    //    Updating();
		DeUpdate();
	//	IAP_JumpToApp(IAP_BOOT_SIZE);
		DDL_DelayMS(500);
    Updating();
    DDL_DelayMS(500);
  }
}