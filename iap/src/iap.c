#include "iap.h"

func_ptr_t JumpToApp = NULL;

/**
 * @brief           加载SD卡
 * 
 * @return uint8_t  返回0，表示加载成功
 */
uint8_t SDCARDLoad(void)
{
    
    if(SDCardInit() == LL_OK){  //如果初始化成功
        
        if(SD_GetCardCSD(&SdHandle,&CsdInformation) == LL_OK){  //获取SD卡信息
            return 0 ;  //加载成功
        }
    }
    return 1 ;
}

/**
 * @brief IAP初始化，读取指定的文件名字，更新程序
 * 
 * @param path 指定的文件名字
 * @return uint8_t 返回0表示OK
 */
uint8_t IAP_Init(const TCHAR *path)
{
	FATFS       fs;
    FIL         fsrc;         
    FRESULT     res;
    UINT        br;
    uint8_t     APPBuff[512] = {0};      //APP程序数据缓存
    if(SDCARDLoad() != LL_OK){          //如果SD卡加载失败,函数直接返回
        return 1 ;
    }
	if(f_mount(&fs,"0:",1) != FR_OK){		//挂载失败
		return 1 ;
	}
	res = f_open(&fsrc,path,FA_READ|FA_OPEN_EXISTING);	//打开文件
	if(res == FR_OK){	//文件打开成功 
        uint32_t ReadCount = fsrc.obj.objsize / sizeof(APPBuff);       //计算要读多少次
				uint32_t filesize = fsrc.obj.objsize;
        if(filesize % sizeof(APPBuff) != 0){ //表示有不完整的数据
            ReadCount++;    
        }
        for (uint32_t i = 0; i < ReadCount; i++)
        {
            res = f_read(&fsrc,APPBuff,sizeof(APPBuff),&br); //读取升级文件
            if(res == FR_OK){       //如果读取成功
                if(i == 0){     //第一次读取数据时，判断栈顶是否合法，不合法，要直接退出不准更新
                    uint32_t u32StackTop = *((uint32_t *)APPBuff);      //获取栈顶地址
                    if ((u32StackTop <= SRAM_BASE) || (u32StackTop > (SRAM_BASE + SRAM_SIZE))) {    //判断地址是否合法
                        f_close(&fsrc);f_mount(&fs,"0:",0);return 1 ;   //返回错误
                    }
                    Updating();
                    //栈顶地址合法后，再清空APP段的数据
                    FLASH_EraseSector(IAP_BOOT_SIZE,EFM_END_ADDR + 1 - IAP_BOOT_SIZE);    //清空APP程序段
                }
                if(FLASH_WriteData(i * sizeof(APPBuff) + 0 + IAP_BOOT_SIZE ,APPBuff,br) != LL_OK){  //写入flash失败
                    f_close(&fsrc);f_mount(&fs,"0:",0);return 1 ;   //返回错误
                }
            }
            else {
                f_close(&fsrc);f_mount(&fs,"0:",0);return 1 ;   //返回错误
            }
        }
        f_close(&fsrc);f_mount(&fs,"0:",0); //APP程序写入flash完成，再写入完成标志
         uint32_t WriteOk = 0x01 ;           //APP写入完成标志
		if(FLASH_WriteData(FLASH_SIZE - 4 , (uint8_t *)&WriteOk , sizeof(WriteOk)) != LL_OK){   //写入完成标志失败，也要当成失败
            return 1 ;
        }
        
	}
    else {
        f_mount(&fs,"0:",0);return 1 ;
    }
	return 0 ;
}

/**
 * @brief  查找更新文件
 * 
 * @param fileinfo 查找到的更新文件信息
 * @return FRESULT 查找是否成功
 */
FRESULT IAP_FileFind(char * file)
{
    
    //先加载SD卡
    if(SDCARDLoad() != LL_OK){  //SD卡加载失败
        
        return FR_DISK_ERR ;
    }
    
    //先打开更新配置文件
    FATFS       fs;
    FIL         fsrc;         
    FRESULT     res;
    UINT        br;
    uint8_t     namebuffer[256] = {0};
    
    res = f_mount(&fs,"0:",1);      //挂载驱动器
    if(res != FR_OK){
        return res;
    }
    res = f_open(&fsrc,"0:/update/config",FA_READ|FA_OPEN_EXISTING);	//打开文件
    if(res == FR_OK){   //文件打开成功
        res = f_read(&fsrc,namebuffer,sizeof(namebuffer),&br); //读取配置文件
        f_close(&fsrc);
    }
    else {
        f_mount(&fs,"0:",0);      //卸载驱动器
        return res;
    }
    TCHAR dim[512] = {"0:/update/"};
    namebuffer[br] = 0x2f;  //添加一个左斜杠
    strcat((char *)dim,(char *)namebuffer); //连接字符串
    FILINFO  fileinfo ;
    DIR dir;
    int len = 0 ;
    res = f_opendir(&dir, dim);
    if (res != FR_OK) {
        return res;
    }
    //开始遍历目录下的所有程序
    while (1){
        res = f_readdir(&dir, &fileinfo);   //读取文件信息
        if ((res != FR_OK) || (fileinfo.fname[0] == 0)) {
            // End of directory or error, break loop
            f_closedir(&dir);f_mount(&fs,"0:",0);
            break;
        }
        else if(fileinfo.fattrib & AM_HID){     //这是隐藏文件，不处理
            continue;
        }
        else if(fileinfo.fattrib & AM_DIR){    //这是目录，不处理
            continue;
        }
        else {      //这正常的文件名
            len = strlen(fileinfo.fname);  //获取文件名长度
            const TCHAR dx[5] = {".bin"};
            if(fileinfo.fname[len - 4] == dx[0] && fileinfo.fname[len - 3] == dx[1] && fileinfo.fname[len - 2] == dx[2] && fileinfo.fname[len - 1] == dx[3]){ //后缀名检查
                strcat((char *)dim,(char *)fileinfo.fname);
                memcpy((void *)file,(void *)dim,512);
                f_closedir(&dir);f_mount(&fs,"0:",0);
                return FR_OK;
            }
        }
    } 
    f_closedir(&dir);f_mount(&fs,"0:",0);
    return FR_DISK_ERR;
}

/**
 * @brief  Jump from boot to app function.
 * @param  [in] u32Addr                 APP address
 * @retval LL_ERR                       APP address error
 */
int32_t IAP_JumpToApp(uint32_t u32Addr)
{
    uint32_t u32StackTop = *((__IO uint32_t *)u32Addr);
    uint32_t WriteOk = *((__IO uint32_t *)(FLASH_SIZE - 4));     //获取APP程序是否写入完成标志
    if(WriteOk == 0x01){        //如果写成0x01,表示SD卡程序已经顺利写进flash,可以执行跳转检查
        /* Check stack top pointer. */
        if ((u32StackTop > SRAM_BASE) && (u32StackTop <= (SRAM_BASE + SRAM_SIZE))) {
			//__disable_irq();
            IAP_PeriphDeinit();
			/* for(int i=0; i<144; i++)
			{
					NVIC_DisableIRQ((IRQn_Type)i);
					NVIC_ClearPendingIRQ((IRQn_Type)i);
					//enIrqResign(i);
			}
			__enable_irq(); */
            /* Jump to user application */
            uint32_t JumpAddr = *(__IO uint32_t *)(u32Addr + 4U);
            JumpToApp = (func_ptr_t)JumpAddr;
            /* Initialize user application's Stack Pointer */
			//__set_CONTROL(0);
			__set_MSP(u32StackTop);
			//__set_PSP(u32StackTop);		
            JumpToApp();
        }
    }
    return LL_ERR;
}

/**
 * @brief  IAP peripheral de-initialize.
 * @param  None
 * @retval None
 */
void IAP_PeriphDeinit(void)
{
    /* De-Init Peripheral */
    SysTick_DeInit();
    IAP_CLK_DeInit();
    GPIO_DeInit();
    SD_DeInit(&SdHandle);       //关闭SD卡功能
    /* Peripheral registers write protected */
    EFM_FWMC_Cmd(DISABLE);
    LL_PERIPH_WP(LL_PERIPH_ALL);
}



/**
 * @brief  Systick De-Initialize.
 * @param  None
 * @retval None
 */
void SysTick_DeInit(void)
{
    SysTick->CTRL  = 0UL;
    SysTick->LOAD  = 0UL;
    SysTick->VAL   = 0UL;
}

/**
 * @brief  IAP clock De-Initialize.
 * @param  None
 * @retval None
 */
void IAP_CLK_DeInit(void)
{
    CLK_SetSysClockSrc(CLK_SYSCLK_SRC_MRC);
    CLK_SetClockDiv(CLK_BUS_CLK_ALL, (CLK_PCLK0_DIV1 | CLK_PCLK1_DIV1 | CLK_PCLK2_DIV1 |
                                      CLK_PCLK3_DIV1 | CLK_PCLK4_DIV1 | CLK_EXCLK_DIV1 | CLK_HCLK_DIV1));
    CLK_PLLCmd(DISABLE);
    CLK_XtalCmd(DISABLE);
    /* Highspeed SRAM set to 0 Read/Write wait cycle */
    SRAM_SetWaitCycle(SRAM_SRAMH, SRAM_WAIT_CYCLE0, SRAM_WAIT_CYCLE0);
    /* SRAM1_2_3_4_backup set to 0 Read/Write wait cycle */
    SRAM_SetWaitCycle((SRAM_SRAM123 | SRAM_SRAM4 | SRAM_SRAMB), SRAM_WAIT_CYCLE0, SRAM_WAIT_CYCLE0);
    /* 0-wait @ 40MHz */
    EFM_SetWaitCycle(EFM_WAIT_CYCLE0);
    /* 0 cycles for 50MHz */
    GPIO_SetReadWaitCycle(GPIO_RD_WAIT0);
}
