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
    DDL_PrintfInit(BSP_PRINTF_DEVICE,BSP_PRINTF_BAUDRATE,BSP_PRINTF_Preinit);
    init_gpio();
    //SDCardInit();
//    LL_PERIPH_WP(LL_PERIPH_ALL);
//    char  file[512] = {0};
  for (;;) {
    //    if(IAP_FileFind(file) == FR_OK) {
    //      int result = strcmp(file,"0:/update/rayyi-tech_LeakTest/a.bin");
    //      if(result == 0){
            
          //  IAP_Init(file);	//应用程序升级
    //      }
			    
	//	}
    //    Updating();
	//	
	//	IAP_JumpToApp(IAP_BOOT_SIZE);
     
    DeUpdate();
    DDL_Printf("hello world\r\n"); 
		DDL_DelayMS(500);
    Updating();
    DDL_DelayMS(500);
  }
}