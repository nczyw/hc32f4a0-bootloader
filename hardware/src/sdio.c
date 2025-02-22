#include "sdio.h"
#include "mygpio.h"
stc_sd_handle_t SdHandle;        //SD卡句柄
stc_sd_card_csd_t CsdInformation; //CSD寄存器信息

/**
 * @brief SDCARD CD引脚初始化
 * @param u8Port port 引脚
 * @param u16Pin pin 编号
 */
void SdCard_CD_Init(uint8_t u8Port, uint16_t u16Pin)
{
    stc_gpio_init_t stcGpioInit;
    GPIO_StructInit(&stcGpioInit);
    stcGpioInit.u16PullUp   = PIN_PU_ON;
    stcGpioInit.u16PinDir   = PIN_DIR_IN;
    GPIO_Init(u8Port, u16Pin, &stcGpioInit);
}

uint8_t SDCardInit(void)
{
    
    uint8_t result = 0 ;
    /* Enable SDIOC clock */
    FCG_Fcg1PeriphClockCmd(SDIOC_SD_CLK, ENABLE);
    
    /* SDIOC pins configuration */
    //SdCard_CD_Init(SDIOC_CD_PORT, SDIOC_CD_PIN);
    GPIO_SetFunc(SDIOC_CK_PORT,  SDIOC_CK_PIN,  GPIO_FUNC_9);
    DeUpdate();
    GPIO_SetFunc(SDIOC_CMD_PORT, SDIOC_CMD_PIN, GPIO_FUNC_9);
    GPIO_SetFunc(SDIOC_D0_PORT,  SDIOC_D0_PIN,  GPIO_FUNC_9);
    GPIO_SetFunc(SDIOC_D1_PORT,  SDIOC_D1_PIN,  GPIO_FUNC_9);
    GPIO_SetFunc(SDIOC_D2_PORT,  SDIOC_D2_PIN,  GPIO_FUNC_9);
    GPIO_SetFunc(SDIOC_D3_PORT,  SDIOC_D3_PIN,  GPIO_FUNC_9);
    
    /* Configure structure initialization */
    SdHandle.SDIOCx                     = SDIOC_SD_UINT;
    SdHandle.stcSdiocInit.u32Mode       = SDIOC_MD_SD;
    SdHandle.stcSdiocInit.u8CardDetect  = SDIOC_CARD_DETECT_CD_PIN_LVL;
    SdHandle.stcSdiocInit.u8SpeedMode   = SDIOC_SPEED_MD_HIGH;
    SdHandle.stcSdiocInit.u8BusWidth    = SDIOC_BUS_WIDTH_4BIT;
    SdHandle.stcSdiocInit.u16ClockDiv   = SDIOC_CLK_DIV2;

    SdHandle.DMAx    = NULL;
    
    /* Reset and init SDIOC */
    if (LL_OK != SDIOC_SWReset(SdHandle.SDIOCx, SDIOC_SW_RST_ALL)) {
        
        // DDL_Printf("Reset SDIOC failed!\r\n");
        result = 1 ;     // SDIO重置失败
        //while(1);
    } 
    /*else if (SET != (en_flag_status_t)SdCard_Insert()) {
     //    DDL_Printf("No SD card insertion!\r\n");
        result = 2 ;         //没有SD卡插入 
    } */
    else if (LL_OK != SD_Init(&SdHandle)) {
        
        //    DDL_Printf("SD card initialize failed!\r\n");
        result = 3 ;        //SD卡初始化失败
        //while(1);
    } 
    else {
        result = 0 ;    //这表示SD卡正常，可以运行升级功能
    }
    DeUpdate() ;
    return result;
}

DSTATUS SDCard_status(void)
{
    return RES_OK;
}

DSTATUS SDCard_initialize(void)
{
    return RES_OK;
}

DRESULT SDCard_read(BYTE *buff, LBA_t sector, UINT count)
{
    if(!count) return RES_PARERR;       //参数错误
    if(SD_ReadBlocks(&SdHandle, (uint32_t)sector, (uint16_t)count, (uint8_t *)buff, 2000U) == LL_OK) { //读取成功
        return RES_OK;
    }
    else {
        return RES_ERROR;
    }
}

DRESULT SDCard_write(const BYTE *buff, LBA_t sector, UINT count)
{
    if(!count) return RES_PARERR;       //参数错误
    if(SD_WriteBlocks(&SdHandle, (uint32_t)sector, (uint16_t)count, (uint8_t *)buff, 2000U) == LL_OK) { //读取成功
        return RES_OK;
    }
    else {
        return RES_ERROR;
    }
}

DRESULT SDCard_ioctl(BYTE cmd, void *buff)
{
    DRESULT res = RES_ERROR;
  
    switch (cmd)
    {
    /* Make sure that no pending write process */
    case CTRL_SYNC :
      res = RES_OK;
      break;

    /* Get number of sectors on the disk (DWORD) */
    case GET_SECTOR_COUNT :
      SD_GetCardCSD(&SdHandle,&CsdInformation);
      *(DWORD*)buff = SdHandle.stcSdCardInfo.u32BlockNum;
      res = RES_OK;
      break;

    /* Get R/W sector size (WORD) */
    case GET_SECTOR_SIZE :
      SD_GetCardCSD(&SdHandle,&CsdInformation);
      *(WORD*)buff = SdHandle.stcSdCardInfo.u32LogBlockSize;
      res = RES_OK;
      break;

    /* Get erase block size in unit of sector (DWORD) */
    case GET_BLOCK_SIZE :
      SD_GetCardCSD(&SdHandle,&CsdInformation);
      *(WORD*)buff = SdHandle.stcSdCardInfo.u32BlockSize;
    	res = RES_OK;
      break;

    default:
      res = RES_PARERR;
    }

    return res;
}
