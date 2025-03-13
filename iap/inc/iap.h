#ifndef IAP_H
#define IAP_H

#include "hc32_ll.h"
#include "sd_diskio.h"



#define IAP_BOOT_SIZE    0x20000
#define APPADDR          IAP_BOOT_SIZE

#define AppWriteDoneADDR      (0)        //APP写入完成标志地址


extern uint8_t  AppWriteDone;                  //APP写入完成标志
extern uint8_t  AppInfError;                    //APP有非法内容


extern func_ptr_t JumpToApp;    //函数指针
extern uint8_t SDCardLoaded;    //SD卡是否已经加载

/**
 * @brief           加载SD卡
 * 
 * @return uint8_t  返回0，表示加载成功
 */
extern uint8_t SDCARDLoad(void);

/**
 * @brief IAP初始化，读取指定的文件名字，更新程序
 * 
 * @param path 指定的文件名字
 * @return uint8_t 返回0表示OK
 */
extern uint8_t IAP_Init(const TCHAR *path);

/**
 * @brief  查找更新文件
 * 
 * @param fileinfo 查找到的更新文件信息
 * @return FRESULT 查找是否成功
 */
extern FRESULT IAP_FileFind(char * file);

/**
 * @brief  Jump from boot to app function.
 * @param  [in] u32Addr                 APP address
 * @retval LL_ERR                       APP address error
 */
extern int32_t IAP_JumpToApp(uint32_t u32Addr);

/**
 * @brief  IAP peripheral de-initialize.
 * @param  None
 * @retval None
 */
extern void IAP_PeriphDeinit(void);


/**
 * @brief  Systick De-Initialize.
 * @param  None
 * @retval None
 */
extern void SysTick_DeInit(void);

/**
 * @brief  IAP clock De-Initialize.
 * @param  None
 * @retval None
 */
extern void IAP_CLK_DeInit(void);

extern uint8_t Clean_updateflag(void);
#endif
