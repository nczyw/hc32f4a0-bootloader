#include "sd_diskio.h"
#include "mygpio.h"
/* Block size is 512 bytes */
#define SD_DEFAULT_BLOCK_SIZE (512U)
/* SD read/write timeout time */
#define SD_RW_TIMEOUT_TIME (30000UL)

/* Local variable definitions ('static') */
static volatile DSTATUS SdStat = (DSTATUS)STA_NOINIT;

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
    SdCard_CD_Init(SDIOC_CD_PORT, SDIOC_CD_PIN);
    GPIO_SetFunc(SDIOC_CK_PORT,  SDIOC_CK_PIN,  GPIO_FUNC_9);
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
    else {
        
        if(SdCard_Insert() != 0){
            
            //SD卡没有插入
            result = 2 ;
        }
        else{
            
            int32_t result = SD_Init(&SdHandle);
            result = 3 ;
            if(result == LL_ERR){
                
            }
            else if(result == LL_ERR_INVD_PARAM){
                
            }
            else if(result == LL_ERR_TIMEOUT){
                
            }
            else if(result == LL_ERR_INVD_MD){
                
            }
            else{
                
                result = 0 ;
                
            }
        }
    }
    return result;
}

/**
* @brief Get SD card state.
* @param None
* @retval An en_result_t enumeration value:
* - Ok: Data transfer is acting
* - Error: No data transfer is acting
*/
static uint8_t SDCard_GetCardTransState(void)
{
    uint8_t enRet;
    en_sd_card_state_t enCardState;
    enRet = SD_GetCardState(&SdHandle, &enCardState);
    if (0 == enRet){
        if (SD_CARD_STAT_TRANS == enCardState){
            enRet = 0;
        }
    }
    return enRet;
}

/**
* @brief Check the SD card status.
* @retval DSTATUS: Operation status
*/
static DSTATUS SD_CheckStatus(void)
{
    SdStat = (DSTATUS)STA_NOINIT;
    en_pin_state_t enRet;
    enRet = SdCard_Insert();
    if (PIN_RESET == enRet)
    {
        SdStat &= (DSTATUS)(~(DSTATUS)STA_NOINIT);
    }
    return SdStat;
}


DSTATUS SDCard_status(void)
{
    return SD_CheckStatus();
}

DSTATUS SDCard_initialize(void)
{
    SdStat = (DSTATUS)STA_NOINIT;
    if (0 == SDCardInit()){
        SdStat = SD_CheckStatus();
    }
    return SdStat;
}
/**
* @brief Reads Sector(s)
* @param buff: Pointer to data buffer used to store read data
* @param sector: Sector address (LBA)
* @param count: Number of sectors to read (1..128)
* @retval DRESULT: Operation result
*/
DRESULT SDCard_read(BYTE *buff, LBA_t sector, UINT count)
{
    if(!count) return RES_PARERR;       //参数错误
    DRESULT res = RES_ERROR;
    if (0 == SD_ReadBlocks(&SdHandle, (uint32_t)sector, (uint16_t)count, (uint8_t *)buff,SD_RW_TIMEOUT_TIME)){
    /* Wait until the read operation is finished */
        while (0 != SDCard_GetCardTransState());
        res = RES_OK;
    }
    return res;
}
/**
* @brief Writes Sector(s)
* @param buff: Pointer to data to be written
* @param sector: Sector address (LBA)
* @param count: Number of sectors to write (1..128)
* @retval DRESULT: Operation result
*/
DRESULT SDCard_write(const BYTE *buff, LBA_t sector, UINT count)
{
    if(!count) return RES_PARERR;       //参数错误
    DRESULT res = RES_ERROR;
    if (0 == SD_WriteBlocks(&SdHandle, (uint32_t)sector, (uint16_t)count, (uint8_t *)buff,SD_RW_TIMEOUT_TIME)){
    /* Wait until the Write operation is finished */
        while (0 != SDCard_GetCardTransState());
        res = RES_OK;
    }
    return res;
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
      *(WORD*)buff = SdHandle.stcSdCardInfo.u32BlockSize / SD_DEFAULT_BLOCK_SIZE;
    	res = RES_OK;
      break;

    default:
      res = RES_PARERR;
    }

    return res;
}
DWORD get_fattime (void)
{
	return 0;
}