#include "main.h"
/**
 * @brief  Main function of GPIO project
 * @param  None
 * @retval int32_t return value, if needed
 */
int main(void){
  LL_PERIPH_WE(LL_PERIPH_ALL);
  EFM_FWMC_Cmd(ENABLE);
  EFM_SequenceSectorOperateCmd(FLASH_SECTOR0_NUM, FLASH_SECTOR_NUM, ENABLE);
  BSP_CLK_Init();
  BSP_XTAL32_Init();
  //DDL_PrintfInit(BSP_PRINTF_DEVICE,BSP_PRINTF_BAUDRATE,BSP_PRINTF_Preinit);
  //DDL_Printf("BootLoader Running.\r\n");

  init_gpio();
  i2c1_init();
  init_debug();
  uart_debug_send("BootLoader Running.\r\n");
  //  uint8_t tmp = 0 ;
  //  DDL_Printf("Size of struct MyStruct: %zu bytes\n", sizeof(APP_Flag_t));
    //i2c1_write(I2C1_UNIT,EE_24CXX_DEV_ADDR,0,2,&tmp,1);
  i2c1_read(I2C1_UNIT,EE_24CXX_DEV_ADDR,AppWriteDoneADDR,EE_24CXX_MEM_ADDR_LEN,(uint8_t *)&AppWriteDone,sizeof(AppWriteDone)); //读取app相关配置
  //SDCardInit();
  //LL_PERIPH_WP(LL_PERIPH_ALL);
  DeUpdate(); //关闭指示灯
  char  file[512] = {0};
  if(IAP_FileFind(file) == FR_OK) {       //先去看看是否需要更新
    if(file[0] != 0){                     //表示需要更新
      uart_debug_send("File name is ");
      uart_debug_send(file);
      uart_debug_send("\r\n");
      IAP_Init(file);                     //应用程序更新
      Clean_updateflag();                 //清除更新标志
    }
    else{
      uart_debug_send("No need to update\r\n");
    }
  }
  DDL_DelayMS(1);
  
  while(1){
    IAP_JumpToApp(IAP_BOOT_SIZE);     //直接Jump
    memset(file,0,sizeof(file));    //清空文件缓存
    if(IAP_FileFind(file) == FR_OK) {   //APP段没有正确的数据，必须要强制更新,忽略更新标志，直接查找更新文件
      if(file[0] != 0){                     //表示需要更新
        uart_debug_send("File name is ");
        uart_debug_send(file);
        uart_debug_send("\r\n");
        IAP_Init(file);                     //应用程序更新
        Clean_updateflag();                 //清除更新标志
      }
    }
    
  //  DeUpdate();
    DDL_DelayMS(2000);    //如果强制更新失败，那就等待2秒再执行
  //  Updating();
  //  DDL_DelayMS(1000);
  }
}