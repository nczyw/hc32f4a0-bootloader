/**
 *******************************************************************************
 * @file  iap/iap_boot/source/flash.c
 * @brief This file provides firmware functions to manage the Flash driver.
 @verbatim
   Change Logs:
   Date             Author          Notes
   2022-03-31       CDT             First version
 @endverbatim
 *******************************************************************************
 * Copyright (C) 2022-2025, Xiaohua Semiconductor Co., Ltd. All rights reserved.
 *
 * This software component is licensed by XHSC under BSD 3-Clause license
 * (the "License"); You may not use this file except in compliance with the
 * License. You may obtain a copy of the License at:
 *                    opensource.org/licenses/BSD-3-Clause
 *
 *******************************************************************************
 */

/*******************************************************************************
 * Include files
 ******************************************************************************/
#include "flash.h"

/*******************************************************************************
 * Local type definitions ('typedef')
 ******************************************************************************/

/*******************************************************************************
 * Local pre-processor symbols/macros ('#define')
 ******************************************************************************/

/*******************************************************************************
 * Global variable definitions (declared in header file with 'extern')
 ******************************************************************************/

/*******************************************************************************
 * Local function prototypes ('static')
 ******************************************************************************/

/*******************************************************************************
 * Local variable definitions ('static')
 ******************************************************************************/

/*******************************************************************************
 * Function implementation - global ('extern') and local ('static')
 ******************************************************************************/
/**
 * @brief  Check address alignment.
 * @param  u32Addr                      Flash address
 * @retval An en_result_t enumeration value:
 *           - LL_OK: Address aligned
 *           - LL_ERR: Address unaligned
 */
int32_t FLASH_CheckAddrAlign(uint32_t u32Addr)
{
    uint32_t u32Step = FLASH_SECTOR_SIZE;

    if (VECT_TAB_STEP > FLASH_SECTOR_SIZE) {
        u32Step = VECT_TAB_STEP;
    }
    if ((u32Addr % u32Step) != 0UL) {
        return LL_ERR;
    }

    return LL_OK;
}

/**
 * @brief  Erase flash sector.
 * @param  u32Addr                      Flash address
 * @param  u32Size                      Firmware size (0: current address sector)
 * @retval An en_result_t enumeration value:
 *           - LL_OK: Erase succeeded
 *           - LL_ERR: Erase timeout
 *           - LL_ERR_INVD_PARAM: The parameters is invalid.
 */
int32_t FLASH_EraseSector(uint32_t u32Addr, uint32_t u32Size)
{
    uint32_t i;
    uint32_t u32PageNum;
    if (u32Addr >= (FLASH_BASE + FLASH_SIZE)) {
        return LL_ERR_INVD_PARAM;
    }

    if (u32Size == 0U) {
        return EFM_SectorErase(u32Addr);
    } else {
        u32PageNum = u32Size / FLASH_SECTOR_SIZE;
        if ((u32Size % FLASH_SECTOR_SIZE) != 0UL) {
            u32PageNum += 1U;
        }
        for (i = 0; i < u32PageNum; i++) {
            if (LL_OK != EFM_SectorErase(u32Addr + (i * FLASH_SECTOR_SIZE))) {
                return LL_ERR;
            }
        }
    }

    return LL_OK;
}

/**
 * @brief  Write data to flash.
 * @param  u32Addr                      Flash address
 * @param  pu8Buff                      Pointer to the buffer to be written
 * @param  u32Len                       Buffer length
 * @retval int32_t:
 *           - LL_OK: Program successful.
 *           - LL_ERR_INVD_PARAM: The parameters is invalid.
 *           - LL_ERR_NOT_RDY: EFM if not ready.
 *           - LL_ERR_ADDR_ALIGN: Address alignment error
 */
int32_t FLASH_WriteData(uint32_t u32Addr, uint8_t *pu8Buff, uint32_t u32Len)
{
    if ((pu8Buff == NULL) || (u32Len == 0U) || ((u32Addr + u32Len) > (FLASH_BASE + FLASH_SIZE))) {
        return LL_ERR_INVD_PARAM;
    }
    if (0UL != (u32Addr % 4U)) {
        return LL_ERR_ADDR_ALIGN;
    }
    return EFM_Program(u32Addr, pu8Buff, u32Len);
}

/**
 * @brief  Read data from flash.
 * @param  u32Addr                      Flash address
 * @param  pu8Buff                      Pointer to the buffer to be reading
 * @param  u32Len                       Buffer length
 * @retval int32_t:
 *           - LL_OK: Read data succeeded
 *           - LL_ERR_INVD_PARAM: The parameters is invalid
 *           - LL_ERR_ADDR_ALIGN: Address alignment error
 */
int32_t FLASH_ReadData(uint32_t u32Addr, uint8_t *pu8Buff, uint32_t u32Len)
{
    uint32_t i;
    uint32_t u32WordLength, u8ByteRemain;
    uint32_t *pu32ReadBuff;
    __IO uint32_t *pu32FlashAddr;
    uint8_t  *pu8Byte;
    __IO uint8_t  *pu8FlashAddr;

    if ((pu8Buff == NULL) || (u32Len == 0U) || ((u32Addr + u32Len) > (FLASH_BASE + FLASH_SIZE))) {
        return LL_ERR_INVD_PARAM;
    }
    if (0UL != (u32Addr % 4U)) {
        return LL_ERR_ADDR_ALIGN;
    }

    pu32ReadBuff  = (uint32_t *)(uint32_t)pu8Buff;
    pu32FlashAddr = (uint32_t *)u32Addr;
    u32WordLength = u32Len / 4U;
    u8ByteRemain  = u32Len % 4U;
    /* Read data */
    for (i = 0UL; i < u32WordLength; i++) {
        *(pu32ReadBuff++) = *(pu32FlashAddr++);
    }
    if (0UL != u8ByteRemain) {
        pu8Byte      = (uint8_t *)pu32ReadBuff;
        pu8FlashAddr = (uint8_t *)pu32FlashAddr;
        for (i = 0UL; i < u8ByteRemain; i++) {
            *(pu8Byte++) = *(pu8FlashAddr++);
        }
    }

    return LL_OK;
}

/******************************************************************************
 * EOF (not truncated)
 *****************************************************************************/
