#include "mysys.h"
#include "uart_debug.h"
#include "mygpio.h"
#include "iap.h"
#include "flash.h"
#include "at24c512.h"
#include <string.h>
int main(void)
{
  LL_PERIPH_WE(LL_PERIPH_ALL);
  EFM_FWMC_Cmd(ENABLE);
  EFM_SequenceSectorOperateCmd(FLASH_SECTOR0_NUM, FLASH_SECTOR_NUM, ENABLE);
  BSP_CLK_Init();
  BSP_XTAL32_Init();
  DDL_PrintfInit(UART_DEBUG_UNIT,UART_DEBUG_BAUDRATE,BSP_PRINTF_Preinit);
  at24c512_init();
  init_gpio();
  SystemCoreClockUpdate();
  at24c512_read(AppWriteDoneADDR,(uint8_t *)&AppWriteDone,sizeof(AppWriteDone));
  console_output("Bootloader running.\r\n");

  DeUpdate(); //关闭指示灯

  char  file[512] = {0};
  if(IAP_FileFind(file) == FR_OK) {       //先去看看是否需要更新
    if(file[0] != 0){                     //表示需要更新
      console_output("File name is ");
      console_output(file);
      console_output("\r\n");
      IAP_Init(file);                     //应用程序更新
      Clean_updateflag();                 //清除更新标志
    }
    else{
      console_output("No need to update\r\n");
    }
  }
  DDL_DelayMS(1);
  while(1){
    IAP_JumpToApp(IAP_BOOT_SIZE);     //直接Jump
    memset(file,0,sizeof(file));    //清空文件缓存
    if(IAP_FileFind(file) == FR_OK){   //APP段没有正确的数据，必须要强制更新,忽略更新标志，直接查找更新文件
      if(file[0] != 0){                     //表示需要更新
        console_output("File name is ");
        console_output(file);
        console_output("\r\n");
        IAP_Init(file);                     //应用程序更新
        Clean_updateflag();                 //清除更新标志
      }
    }
    DDL_DelayMS(2000);    //如果强制更新失败，那就等待2秒再执行
  }
}
