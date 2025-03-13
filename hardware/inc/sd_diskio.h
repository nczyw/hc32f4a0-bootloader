#ifndef SD_DISKIO_H
#define SD_DISKIO_H

/* C binding of definitions if building with C++ compiler */
#ifdef __cplusplus
extern "C" 
{
#endif

/* Include files */
#include "hc32_ll.h"
#include "ff.h"
#include "diskio.h"
#include "sd.h"

#define SDIOC_SD_UINT                   (CM_SDIOC1)
#define SDIOC_SD_CLK                    (FCG1_PERIPH_SDIOC1)

#define SDIOC_CK_PORT                   (GPIO_PORT_C)
#define SDIOC_CK_PIN                    (GPIO_PIN_12)

#define SDIOC_CMD_PORT                  (GPIO_PORT_D)
#define SDIOC_CMD_PIN                   (GPIO_PIN_02)

#define SDIOC_D0_PORT                   (GPIO_PORT_C)
#define SDIOC_D0_PIN                    (GPIO_PIN_08)

#define SDIOC_D1_PORT                   (GPIO_PORT_C)
#define SDIOC_D1_PIN                    (GPIO_PIN_09)

#define SDIOC_D2_PORT                   (GPIO_PORT_A)
#define SDIOC_D2_PIN                    (GPIO_PIN_09)

#define SDIOC_D3_PORT                   (GPIO_PORT_C)
#define SDIOC_D3_PIN                    (GPIO_PIN_11)

#define SDIOC_CD_PORT                   (GPIO_PORT_D)
#define SDIOC_CD_PIN                    (GPIO_PIN_03)

#define SdCard_Insert()                   (GPIO_ReadInputPins(SDIOC_CD_PORT, SDIOC_CD_PIN))


extern stc_sd_handle_t SdHandle;        //SD卡句柄
extern stc_sd_card_csd_t CsdInformation; //CSD寄存器信息

/**
 * @brief SDCARD CD引脚初始化
 * @param u8Port port 引脚
 * @param u16Pin pin 编号
 */
void SdCard_CD_Init(uint8_t u8Port, uint16_t u16Pin);

/**
 * @brief SDIO模式的SDCard初始化
 * 
 * @return uint8_t 返回 0 表示 成功
 */
uint8_t SDCardInit(void);

/**
 * @brief SD卡状态获取
 * 
 * @return DSTATUS RES_OK 表示 OK
 */
extern DSTATUS SDCard_status(void);

/**
 * @brief   SD卡初始化
 * 
 * @return DSTATUS  RES_OK 表示OK
 */
extern DSTATUS SDCard_initialize(void);

/**
 * @brief 读取SD卡 扇区
 * 
 * @param buff      读取数据存放地址
 * @param sector    扇区编号
 * @param count     扇区数量
 * @return          DRESULT RES_OK 表示 OK
 */
extern DRESULT SDCard_read(BYTE *buff,LBA_t sector,UINT count);

/**
 * @brief 写入SD卡 扇区
 * 
 * @param buff      等待写入数据的地址
 * @param sector    扇区地址
 * @param count     扇区数量
 * @return          DRESULT  RES_OK 表示 OK
 */
extern DRESULT SDCard_write(const BYTE *buff,LBA_t sector,UINT count);

/**
 * @brief 获取SD卡相关信息
 * 
 * @param cmd       指令
 * @param buff      数据
 * @return          DRESULT  RES_OK 表示 OK
 */
extern DRESULT SDCard_ioctl(BYTE cmd , void * buff);

#ifdef __cplusplus
}
#endif

#endif /* SD_DISKIO_H */
