/**
 *******************************************************************************
 * @file  sdio.c
 * @brief This file provides firmware functions to manage the SDIO card.
 @verbatim
   Change Logs:
   Date             Author          Notes
   2023-09-30       CDT             First version
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
#include <string.h>
#include <stdlib.h>
#include "sdio.h"

/**
 * @addtogroup LL_SDIOC_LIB
 * @{
 */

/**
 * @defgroup SDIOC_SDIO_Card SDIOC SDIO Card
 * @{
 */

/*******************************************************************************
 * Local type definitions ('typedef')
 ******************************************************************************/

/*******************************************************************************
 * Local pre-processor symbols/macros ('#define')
 ******************************************************************************/

/**
 * @defgroup SDIO_Local_Macros SDIO Local Macros
 * @{
 */

/* SDIO CCCR Register Bus_Interface_Control Bus Width Mask */
#define SDIO_BUS_IF_CTRL_WIDTH_MASK             (0xFCU)

/* SDIO CMD53 max bytes tranfsereed */
#define SDIO_CMD53_MAX_BYTES                    (512U)

/* SDIO host VDD voltage window */
#define SDIO_HOST_OCR_32_33_V                   (1UL << 20U) /* The 3.2 - 3.3 voltage window. */
#define SDIO_HOST_OCR_33_34_V                   (1UL << 21U) /* The 3.3 - 3.4 voltage window. */
#define SDIO_HOST_VDD_VOLT_WIN                  (SDIO_HOST_OCR_32_33_V | SDIO_HOST_OCR_33_34_V)
/**
 * @}
 */

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
 * @defgroup SDIO_Local_Functions SDIO Local Functions
 * @{
 */

/**
 * @brief  Read and parse SDIO CCCR.
 * @param  [in] pstcCard                Pointer to a @ref stc_sdio_card_t structure
 * @retval int32_t:
 *           - LL_OK: Operate successfully
 *           - LL_ERR: Operate unsuccessfully
 *           - LL_ERR_INVD_PARAM: NULL == pstcCard or NULL == pstcCard->apstcSdioFunc[0]
 */
static int32_t SDIO_ReadCccr(stc_sdio_card_t *pstcCard)
{
    int32_t i32Ret;
    uint8_t u8Data;

    if ((NULL == pstcCard) || (NULL == pstcCard->apstcSdioFunc[0])) {
        i32Ret = LL_ERR_INVD_PARAM;
    } else {
        (void)memset(&pstcCard->stcCccr, 0, sizeof(stc_sdio_cccr_t));

        /* CCCR/SDIO Version */
        i32Ret = SDIO_IOReadByte(pstcCard->apstcSdioFunc[0], SDIO_REG_CCCR_CCCR_REV, &u8Data);
        if (LL_OK != i32Ret) {
            return LL_ERR;
        } else {
            pstcCard->stcCccr.u8CccrVersion = u8Data & 0x0FU;
            if (pstcCard->stcCccr.u8CccrVersion > SDIO_CCCR_REV_1_20) {
                DDL_Printf("Unrecognised register CCCR/SDIO_Revision: CCCR=%d\r\n", pstcCard->stcCccr.u8CccrVersion);
                return LL_ERR;
            } else {
                DDL_Printf("Register CCCR/SDIO_Revision: CCCR=%d\r\n", pstcCard->stcCccr.u8CccrVersion);
            }

            pstcCard->stcCccr.u8SdioVersion = (u8Data & 0xF0U) >> 4;
            DDL_Printf("Register CCCR/SDIO_Revision: SDIO=%d\r\n", pstcCard->stcCccr.u8SdioVersion);
        }

        /* Register CCCR Card Capability: SMB/LSC/4BLS */
        i32Ret = SDIO_IOReadByte(pstcCard->apstcSdioFunc[0], SDIO_REG_CCCR_CARD_CAPABILITY, &u8Data);
        if (LL_OK != i32Ret) {
            return LL_ERR;
        } else {
            if (0U != (u8Data & SDIO_CARD_CAPABILITY_SMB)) {
                pstcCard->stcCccr.enSupportMultiBlock = SET;
            }

            if (0U != (u8Data & SDIO_CARD_CAPABILITY_LSC)) {
                pstcCard->stcCccr.enSupportLowSpeedCard = SET;
            }

            if (0U != (u8Data & SDIO_CARD_CAPABILITY_4BLS)) {
                pstcCard->stcCccr.enSupportLowSpeed4Bit = SET;
            }

            if (0U != (u8Data & SDIO_CARD_CAPABILITY_4BLS)) {
                pstcCard->stcCccr.enBusWidth = SET;
            }
        }

        /* Register CCCR Power Control: SMPC */
        if (pstcCard->stcCccr.u8CccrVersion >= SDIO_CCCR_REV_1_10) {
            i32Ret = SDIO_IOReadByte(pstcCard->apstcSdioFunc[0], SDIO_REG_CCCR_POWER_CTRL, &u8Data);
            if (LL_OK != i32Ret) {
                return LL_ERR;
            } else {
                if (0U != (u8Data & SDIO_POWER_CTRL_SMPC)) {
                    pstcCard->stcCccr.enSupportPowerCtrl = SET;
                }
            }
        }

        /* Register High-Speed: SMPC */
        if (pstcCard->stcCccr.u8CccrVersion >= SDIO_CCCR_REV_1_20) {
            i32Ret = SDIO_IOReadByte(pstcCard->apstcSdioFunc[0], SDIO_REG_CCCR_HIGH_SPEED, &u8Data);
            if (LL_OK != i32Ret) {
                return LL_ERR;
            } else {
                if ((u8Data & SDIO_HIGH_SPEED_SHS) != 0U) {
                    pstcCard->stcCccr.enSupportHighSpeed = SET;
                }
            }
        }
        i32Ret = LL_OK;
    }

    return i32Ret;
}

/**
 * @brief  Parse the tuple of function 0.
 * @param  [in] pstcCard                Pointer to a @ref stc_sdio_card_t structure
 * @param  [in] pu8TupleBody            Pointer to tuple body data
 * @param  [in] u8TupleLink             Length of tuple data
 * @retval int32_t:
 *           - LL_OK: Operate successfully
 *           - LL_ERR: Operate unsuccessfully
 *           - LL_ERR_INVD_PARAM: NULL == pstcCard or NULL == pstcCard->apstcSdioFunc[0] or
                                  NULL == pu8TupleBody or 0U == u8TupleLink
 */
static int32_t SDIO_CisTupleFunc0(stc_sdio_card_t *pstcCard, const uint8_t *pu8TupleBody, uint8_t u8TupleLink)
{
    int32_t i32Ret;
    const uint32_t u32SpeedUnit[8] = {
        10000UL, 100000UL, 1000000UL, 10000000UL, 0UL, 0UL, 0UL, 0UL
    };
    const uint8_t u8SpeedVal[16] = {
        0U, 10U, 12U, 13U, 15U, 20U, 25U, 30U, 35U, 40U, 45U, 50U, 55U, 60U, 70U, 80U
    };

    if ((NULL == pstcCard) || (NULL == pstcCard->apstcSdioFunc[0]) || (NULL == pu8TupleBody) || (0U == u8TupleLink)) {
        i32Ret = LL_ERR_INVD_PARAM;
    } else {
        if ((u8TupleLink < 0x04U) || (0U != pu8TupleBody[0])) {
            return LL_ERR;
        }

        /* TPLFE_FN0_BLK_SIZE */
        pstcCard->stcCis.u16Func0BlockSize = (uint16_t)pu8TupleBody[1] | ((uint16_t)pu8TupleBody[2] << 8);
        pstcCard->apstcSdioFunc[0]->u16MaxBlockSize = pstcCard->stcCis.u16Func0BlockSize;
        DDL_Printf("Maximum Block Size: %d\r\n", pstcCard->stcCis.u16Func0BlockSize);

        /* TPLFE_MAX_TRAN_SPEED */
        pstcCard->stcCis.u32MaxTransSpeed = u8SpeedVal[(pu8TupleBody[3] >> 3) & 15U] * u32SpeedUnit[pu8TupleBody[3] & 7U];
        DDL_Printf("Maximum Transfer Speed: %d\r\n", (unsigned int)pstcCard->stcCis.u32MaxTransSpeed);
        i32Ret = LL_OK;
    }

    return i32Ret;
}

/**
 * @brief  Parse the tuple of function 1~7.
 * @param  [in] pstcFunc                Pointer to a @ref stc_sdio_func_t structure
 * @param  [in] pu8TupleBody            Pointer to tuple body data
 * @param  [in] u8TupleLink             Length of tuple data
 * @retval int32_t:
 *           - LL_OK: Operate successfully
 *           - LL_ERR: Operate unsuccessfully
 *           - LL_ERR_INVD_PARAM: NULL == pstcFunc or NULL == pstcFunc->pstcCard or
 *                                NULL == pu8TupleBody or 0U == u8TupleLink
 */
static int32_t SDIO_CisTupleFunc(stc_sdio_func_t *pstcFunc, const uint8_t *pu8TupleBody, uint8_t u8TupleLink)
{
    int32_t i32Ret;
    uint8_t u8Version;
    uint8_t u8MinSize;

    if ((NULL == pstcFunc) || (NULL == pstcFunc->pstcCard) || (NULL == pu8TupleBody) || (0U == u8TupleLink)) {
        i32Ret = LL_ERR_INVD_PARAM;
    } else {
        u8Version = pstcFunc->pstcCard->stcCccr.u8SdioVersion;
        u8MinSize = (u8Version == SDIO_SDIO_REV_1_00) ? 28U : 42U;

        if ((u8TupleLink < u8MinSize) || (1U != pu8TupleBody[0])) {
            return LL_ERR;
        }

        /* TPLFE_MAX_BLK_SIZE */
        pstcFunc->u16MaxBlockSize = (uint16_t)pu8TupleBody[12] | ((uint16_t)pu8TupleBody[13] << 8);
        DDL_Printf("Maximum Block Size: %d\r\n", pstcFunc->u16MaxBlockSize);

        /* TPLFE_ENABLE_TIMEOUT_VAL, present in ver 1.1 and above */
        if (u8Version > SDIO_SDIO_REV_1_00) {
            pstcFunc->u32EnableTimeoutVal = ((uint32_t)pu8TupleBody[28] | ((uint32_t)pu8TupleBody[29] << 8)) * 10UL;
        } else {
            pstcFunc->u32EnableTimeoutVal = 1000UL; /* 1000ms */
        }
        i32Ret = LL_OK;
    }

    return i32Ret;
}

/**
 * @brief  Read and parse CIS.
 * @param  [in] pstcFunc                Pointer to a @ref stc_sdio_func_t structure
 * @retval int32_t:
 *           - LL_OK: Operate successfully
 *           - LL_ERR: Operate unsuccessfully
 *           - LL_ERR_INVD_PARAM: NULL == pstcFunc or NULL == pstcFunc->pstcCard or
 *                                NULL == pstcFunc->pstcCard->apstcSdioFunc[0]
 */
static int32_t SDIO_ReadCis(stc_sdio_func_t *pstcFunc)
{
    uint8_t i;
    int32_t i32Ret;
    uint8_t u8Data;
    uint8_t u8Len;
    uint8_t u8TplCode;
    uint8_t u8TplLink;
    uint32_t u32CisPtr = 0;
    uint32_t u32RegAddr;
    stc_sdio_func_tuple_t *pstcCurrent;
    stc_sdio_func_tuple_t **pstcPrev;
    stc_sdio_card_t *pstcCard;
    stc_sdio_func_t *pstcFunc0;

    if ((NULL == pstcFunc) || (NULL == pstcFunc->pstcCard) || (NULL == pstcFunc->pstcCard->apstcSdioFunc[0])) {
        return LL_ERR_INVD_PARAM;
    } else {
        pstcCard = pstcFunc->pstcCard;
        pstcFunc0 = pstcCard->apstcSdioFunc[0];

        for (i = 0U; i < 3U; i++) {
            u32RegAddr = SDIO_REG_FBR_BASE(pstcFunc->u8FuncNum) + SDIO_REG_FBR_CIS + (uint32_t)i;
            i32Ret = SDIO_IOReadByte(pstcFunc0, u32RegAddr, &u8Data);
            if (LL_OK != i32Ret) {
                return LL_ERR;
            }

            u32CisPtr |= (uint32_t)u8Data << (i * 8U);
        }

        DDL_Printf("\r\n[CIS] Function=%d, ptr=0x%08X\r\n", pstcFunc->u8FuncNum, (unsigned int)u32CisPtr);

        pstcPrev = &pstcFunc->pstcTuples;

        for (;;) {
            i32Ret = SDIO_IOReadByte(pstcFunc0, u32CisPtr++, &u8TplCode);
            if (LL_OK != i32Ret) {
                break;
            }

            i32Ret = SDIO_IOReadByte(pstcFunc0, u32CisPtr++, &u8TplLink);
            if (LL_OK != i32Ret) {
                break;
            }

            if ((u8TplCode == CISTPL_END) || (u8TplLink == 0xFFU)) {
                break;
            }

            if (u8TplCode == CISTPL_NULL) {
                continue;
            }

            pstcCurrent = malloc(sizeof(stc_sdio_func_tuple_t) + u8TplLink);
            if (NULL == pstcCurrent) {
                return LL_ERR;
            }

            pstcCurrent->pu8Data = &((uint8_t *)pstcCurrent)[sizeof(stc_sdio_func_tuple_t)];
            (void)memset(pstcCurrent->pu8Data, 0, u8TplLink);

            for (i = 0U; i < u8TplLink; i++) {
                i32Ret = SDIO_IOReadByte(pstcFunc0, u32CisPtr + (uint32_t)i, &u8Data);
                if (LL_OK != i32Ret) {
                    break;
                }

                pstcCurrent->pu8Data[i] = u8Data;
            }

            if (LL_OK != i32Ret) {
                free(pstcCurrent);
                break;
            }

            switch (u8TplCode) {
                case CISTPL_MANFID:
                    if (u8TplLink < 4U) {
                        DDL_Printf("CISTPL_MANFID tplink error");
                    }  else {
                        if (0U != pstcFunc->u8FuncNum) {
                            pstcFunc->u16ManufacturerCode = (uint16_t)pstcCurrent->pu8Data[0];
                            pstcFunc->u16ManufacturerCode |= (uint16_t)pstcCurrent->pu8Data[1] << 8;
                            pstcFunc->u16ManufacturerInfo = (uint16_t)pstcCurrent->pu8Data[2];
                            pstcFunc->u16ManufacturerInfo |= (uint16_t)pstcCurrent->pu8Data[3] << 8;
                        } else {
                            pstcCard->stcCis.u16ManufacturerCode = (uint16_t)pstcCurrent->pu8Data[0];
                            pstcCard->stcCis.u16ManufacturerCode |= (uint16_t)pstcCurrent->pu8Data[1] << 8;
                            pstcCard->stcCis.u16ManufacturerInfo = (uint16_t)pstcCurrent->pu8Data[2];
                            pstcCard->stcCis.u16ManufacturerInfo |= (uint16_t)pstcCurrent->pu8Data[3] << 8;
                        }
                        DDL_Printf("Manufacturer Code: 0x%04X\r\n", pstcCard->stcCis.u16ManufacturerCode);
                        DDL_Printf("Manufacturer Information: 0x%04X\r\n", pstcCard->stcCis.u16ManufacturerInfo);
                    }

                    free(pstcCurrent);
                    break;
                case CISTPL_FUNCE:
                    if (0U != pstcFunc->u8FuncNum) {
                        i32Ret = SDIO_CisTupleFunc(pstcFunc, pstcCurrent->pu8Data, u8TplLink);
                    } else {
                        i32Ret = SDIO_CisTupleFunc0(pstcCard, pstcCurrent->pu8Data, u8TplLink);
                    }

                    if (LL_OK != i32Ret) {
                        DDL_Printf("CISTPL_FUNCE error: size %u type %u", u8TplLink, pstcCurrent->pu8Data[0]);
                    }

                    break;
                case CISTPL_VERS_1:
                    if (u8TplLink < 2U) {
                        DDL_Printf("CISTPL_VERS_1 error: tplink %u", u8TplLink);
                    }

                    /* Print Product Information */
                    for (i = 2U; pstcCurrent->pu8Data[i] != 0xFFU; i += (u8Len + 1U)) {
                        u8Len = (uint8_t)strlen((char *)pstcCurrent->pu8Data + i);
                        if (0U != u8Len) {
                            DDL_Printf(" %s", pstcCurrent->pu8Data + i);
                        }
                    }
                    DDL_Printf("\r\n");
                    break;
                default:
                    pstcCurrent->pstcNext = NULL;
                    pstcCurrent->u8Code = u8TplCode;
                    pstcCurrent->u8Size = u8TplLink;
                    *pstcPrev = pstcCurrent;
                    pstcPrev = &pstcCurrent->pstcNext;
                    DDL_Printf("CIS tuple code 0x%X, length %d\r\n", u8TplCode, u8TplLink);
                    break;
            }

            u32CisPtr += u8TplLink;
        }

        if (0U != pstcFunc->u8FuncNum) {
            *pstcPrev = pstcFunc0->pstcTuples;
        }
    }

    return i32Ret;
}

/**
 * @brief  Free CIS.
 * @param  [in] pstcFunc                Pointer to a @ref stc_sdio_func_t structure
 * @retval int32_t:
 *           - LL_OK: Operate successfully
 *           - LL_ERR: Operate unsuccessfully
 *           - LL_ERR_INVD_PARAM: NULL == pstcFunc or NULL == pstcFunc->pstcCard or
 *                                NULL == pstcFunc->pstcCard->apstcSdioFunc[0]
 */
static int32_t SDIO_FreeCis(stc_sdio_func_t *pstcFunc)
{
    int32_t i32Ret;
    stc_sdio_card_t *pstcCard;
    stc_sdio_func_tuple_t *pstcTmp;
    stc_sdio_func_tuple_t *pstcTuple;

    if ((NULL == pstcFunc) || (NULL == pstcFunc->pstcCard) || (NULL == pstcFunc->pstcCard->apstcSdioFunc[0])) {
        i32Ret = LL_ERR_INVD_PARAM;
    } else  {
        pstcCard = pstcFunc->pstcCard;
        pstcTuple = pstcFunc->pstcTuples;

        while ((NULL != pstcTuple) && ((pstcTuple != pstcCard->apstcSdioFunc[0]->pstcTuples) || (0U == pstcFunc->u8FuncNum))) {
            pstcTmp = pstcTuple->pstcNext;
            free(pstcTuple);
            pstcTuple = pstcTmp;
        }

        pstcFunc->pstcTuples = NULL;
        i32Ret = LL_OK;
    }

    return i32Ret;
}

/**
 * @brief  Read FBR.
 * @param  [in] pstcFunc                Pointer to a @ref stc_sdio_func_t structure
 * @retval int32_t:
 *           - LL_OK: Operate successfully
 *           - LL_ERR: Operate unsuccessfully
 *           - LL_ERR_INVD_PARAM: NULL == pstcFunc or NULL == pstcFunc->pstcCard or
 *                                NULL == pstcFunc->pstcCard->apstcSdioFunc[0]
 */
static int32_t SDIO_ReadFbr(stc_sdio_func_t *pstcFunc)
{
    int32_t i32Ret;
    uint8_t u8Data;
    uint32_t u32Addr;
    stc_sdio_func_t *pstcFunc0;

    if ((NULL == pstcFunc) || (NULL == pstcFunc->pstcCard) || (NULL == pstcFunc->pstcCard->apstcSdioFunc[0])) {
        i32Ret = LL_ERR_INVD_PARAM;
    } else {
        pstcFunc0 = pstcFunc->pstcCard->apstcSdioFunc[0];

        u32Addr = SDIO_REG_FBR_BASE(pstcFunc->u8FuncNum) + SDIO_REG_FBR_STD_FUNC_IF;
        i32Ret = SDIO_IOReadByte(pstcFunc0, u32Addr, &u8Data);
        if (LL_OK != i32Ret) {
            return LL_ERR;
        }

        u8Data &= 0x0FU;

        if (u8Data == 0x0FU) {
            u32Addr = SDIO_REG_FBR_BASE(pstcFunc->u8FuncNum) + SDIO_REG_FBR_STD_IF_EXT;
            i32Ret = SDIO_IOReadByte(pstcFunc0, u32Addr, &u8Data);
            if (LL_OK != i32Ret) {
                return LL_ERR;
            }
        }

        pstcFunc->u8FuncCode = u8Data;
        i32Ret = LL_OK;
    }

    return i32Ret;
}

/**
 * @brief  Initialize SDIO function.
 * @param  [in] pstcCard                Pointer to a @ref stc_sdio_card_t structure
 * @param  [in] u8FuncNum               Function number
 * @retval int32_t:
 *           - LL_OK: Operate successfully
 *           - LL_ERR: Operate unsuccessfully
 *           - LL_ERR_INVD_PARAM: NULL == pstcCard or NULL == pstcCard->apstcSdioFunc[0]
 */
static int32_t SDIO_InitFunc(stc_sdio_card_t *pstcCard, uint8_t u8FuncNum)
{
    int32_t i32Ret;
    stc_sdio_func_t *pstcFunc;

    if ((NULL == pstcCard) || (NULL == pstcCard->apstcSdioFunc[0])) {
        return LL_ERR_INVD_PARAM;
    } else {
        DDL_ASSERT(u8FuncNum <= SDIO_MAX_FUNCTIONS);

        pstcFunc = malloc(sizeof(stc_sdio_func_t));
        if (NULL == pstcFunc) {
            DDL_Printf("Failed to malloc stc_sdio_func_t\r\n");
            return LL_ERR;
        }

        (void)memset(pstcFunc, 0, sizeof(stc_sdio_func_t));

        pstcFunc->pstcCard = pstcCard;
        pstcFunc->u8FuncNum = u8FuncNum;

        i32Ret = SDIO_ReadFbr(pstcFunc);
        if (LL_OK != i32Ret) {
            goto error;
        }

        i32Ret = SDIO_ReadCis(pstcFunc);
        if (LL_OK != i32Ret) {
            goto error;
        }

        if (0U == pstcFunc->u16ManufacturerInfo) {
            pstcFunc->u16ManufacturerCode = pstcCard->stcCis.u16ManufacturerCode;
            pstcFunc->u16ManufacturerInfo = pstcCard->stcCis.u16ManufacturerInfo;
        }

        pstcCard->apstcSdioFunc[u8FuncNum] = pstcFunc;
    }

    return LL_OK;

error:
    (void)SDIO_FreeCis(pstcFunc);
    free(pstcFunc);
    pstcCard->apstcSdioFunc[u8FuncNum] = NULL;
    return i32Ret;
}

/**
 * @brief  Initialize SDIO card.
 * @param  [in] pstcHost                Pointer to a @ref stc_sdio_host_t structure
 * @retval int32_t:
 *           - LL_OK: Initialize successfully
 *           - LL_ERR: Initialize unsuccessfully
 *           - LL_ERR_INVD_PARAM: NULL == pstcHost
 */
static int32_t SDIO_InitCard(stc_sdio_host_t *pstcHost)
{
    uint8_t i;
    int32_t i32Ret;
    uint32_t u32Response;
    uint8_t u8TotalFuncNum;
    stc_sdio_card_t *pstcCard;
    uint16_t u16RcaVal;
    uint32_t u32ValidOcr;
    uint32_t u32Count = 0UL;
    uint32_t u32CardReady;

    if (NULL == pstcHost) {
        return LL_ERR_INVD_PARAM;
    } else {
        /* CMD0: GO_IDLE_STATE */
        i32Ret = SDMMC_CMD0_GoIdleState(pstcHost->SDIOCx, &pstcHost->u32ErrorCode);
        if (LL_OK != i32Ret) {
            return LL_ERR;
        }

        /* Wait for reset to completed */
        DDL_DelayMS(1U);

        /* Wait card ready */
        for (;;) {
            if (u32Count++ >= SDMMC_DATA_TIMEOUT) {
                return LL_ERR_TIMEOUT;
            }

            i32Ret = SDMMC_CMD5_IOSendOperateCond(pstcHost->SDIOCx, 0UL, &pstcHost->u32ErrorCode);
            if (LL_OK == i32Ret) {
                /* Get command response */
                (void)SDIOC_GetResponse(pstcHost->SDIOCx, SDIOC_RESP_REG_BIT0_31, &u32Response);
                u32CardReady = (((u32Response >> 31U) == 1UL) ? 1UL : 0UL);
                if (u32CardReady > 0UL) {
                    break;
                }
            }
        }

        u32ValidOcr = (u32Response & SDIO_HOST_VDD_VOLT_WIN);
        if (0UL == u32ValidOcr) {
            DDL_Printf(" Host VDD voltage window is incompatible with card");
            return LL_ERR;
        } else {
            i32Ret = SDMMC_CMD5_IOSendOperateCond(pstcHost->SDIOCx, u32ValidOcr, &pstcHost->u32ErrorCode);
            if (LL_OK != i32Ret) {
                return LL_ERR;
            }

            /* Get the total function number */
            (void)SDIOC_GetResponse(pstcHost->SDIOCx, SDIOC_RESP_REG_BIT0_31, &u32Response);
            u8TotalFuncNum = (uint8_t)((u32Response & 0x70000000UL) >> 28);
        }

        /* Create SDIO card instance */
        pstcCard = malloc(sizeof(stc_sdio_card_t));
        if (NULL == pstcCard) {
            DDL_Printf("Failted to malloc stc_sdio_card_t \r\n");
            return LL_ERR;
        } else {
            (void)memset(pstcCard, 0, sizeof(stc_sdio_card_t));

            pstcCard->u8TotalFuncNum = u8TotalFuncNum;
            pstcCard->pstcHost = pstcHost;
            pstcHost->pstcCard = pstcCard;
        }

        /* Create SDIO function 0 instance */
        pstcCard->apstcSdioFunc[0] = malloc(sizeof(stc_sdio_func_t));
        if (NULL == pstcCard->apstcSdioFunc[0]) {
            DDL_Printf("Failed to malloc stc_sdio_func_t \r\n");
            i32Ret = LL_ERR;
            goto error1;
        } else {
            (void)memset(pstcCard->apstcSdioFunc[0], 0, sizeof(stc_sdio_func_t));
            pstcCard->apstcSdioFunc[0]->pstcCard = pstcCard;
            pstcCard->apstcSdioFunc[0]->u8FuncNum = 0U;
        }

        /* Send CMD3 SET_REL_ADDR with argument 0 for SDIO Card publishes its RCA */
        i32Ret = SDMMC_CMD3_SendRelativeAddr(pstcHost->SDIOCx, &u16RcaVal, &pstcHost->u32ErrorCode);
        if (LL_OK != i32Ret) {
            DDL_Printf("Failed to get RCA address \r\n");
            i32Ret = LL_ERR;
            goto error2;
        } else {
            pstcCard->u16RelativeCardAddr = u16RcaVal;
        }

        /* Send CMD7 with the returned RCA to select the card */
        i32Ret = SDMMC_CMD7_SelectDeselectCard(pstcHost->SDIOCx,
                                               ((uint32_t)pstcCard->u16RelativeCardAddr << 16), &pstcHost->u32ErrorCode);
        if (LL_OK != i32Ret) {
            DDL_Printf("Failed to select card \r\n");
            i32Ret = LL_ERR;
            goto error2;
        }

        i32Ret = SDIO_ReadCccr(pstcCard);
        if (LL_OK != i32Ret) {
            DDL_Printf("Failed to read CCCR \r\n");
            i32Ret = LL_ERR;
            goto error2;
        }

        i32Ret = SDIO_ReadCis(pstcCard->apstcSdioFunc[0]);
        if (LL_OK != i32Ret) {
            DDL_Printf("Failed to read CIS \r\n");
            i32Ret = LL_ERR;
            goto error2;
        }

        i32Ret = SDIO_SetHighSpeed(pstcCard);
        if (LL_OK != i32Ret) {
            DDL_Printf("Failed to set high speed \r\n");
            i32Ret = LL_ERR;
            goto error2;
        }

        i32Ret = SDIO_SetBusWidth(pstcCard);
        if (LL_OK != i32Ret) {
            DDL_Printf("Failed to set bus width \r\n");
            i32Ret = LL_ERR;
            goto error2;
        }

        for (i = 1U; i < u8TotalFuncNum + 1U; i++) {
            i32Ret = SDIO_InitFunc(pstcCard, i);
            if (LL_OK != i32Ret) {
                DDL_Printf("Failed to initialize function \r\n");
                i32Ret = LL_ERR;
                goto error3;
            }
        }

        return LL_OK;
    }

error3:
    if (NULL != pstcHost->pstcCard) {
        for (i = 1U; i <= pstcHost->pstcCard->u8TotalFuncNum; i++) {
            if (NULL != pstcHost->pstcCard->apstcSdioFunc[i]) {
                (void)SDIO_FreeCis(pstcHost->pstcCard->apstcSdioFunc[i]);
                free(pstcHost->pstcCard->apstcSdioFunc[i]);
                pstcHost->pstcCard->apstcSdioFunc[i] = NULL;
            }
        }
    }
error2:
    if ((NULL != pstcHost->pstcCard) && (NULL != pstcHost->pstcCard->apstcSdioFunc[0])) {
        (void)SDIO_FreeCis(pstcHost->pstcCard->apstcSdioFunc[0]);
        free(pstcHost->pstcCard->apstcSdioFunc[0]);
        pstcHost->pstcCard->apstcSdioFunc[0] = NULL;
    }
error1:
    if (NULL != pstcHost->pstcCard) {
        free(pstcHost->pstcCard);
        pstcHost->pstcCard = NULL;
    }

    return i32Ret;
}

/**
 * @brief  Read or Write the SDIO Card FIFO.
 * @param  [in] pstcHost                Pointer to a @ref stc_sdio_host_t structure
 * @param  [in] pstcDataConfig          Pointer to a @ref stc_sdioc_data_config_t structure
 * @param  [out] pu8Data                Pointer to the value of read/write fifo
 * @param  [in] u32Timeout              The timeout time
 * @retval int32_t:
 *           - LL_OK: Read or Write the FIFO success
 *           - LL_ERR: Refer to u32ErrorCode for the reason of error
 *           - LL_ERR_INVD_PARAM: An invalid parameter was write to the send command
 *           - LL_ERR_TIMEOUT: Send command timeout
 */
static int32_t SDIO_ReadWriteFifo(stc_sdio_host_t *pstcHost, const stc_sdioc_data_config_t *pstcDataConfig,
                                  uint8_t pu8Data[], uint32_t u32Timeout)
{
    __IO uint32_t u32Count;
    int32_t i32Ret = LL_OK;
    uint32_t u32Index = 0UL;

    /* The u32Timeout is expressed in ms */
    u32Count = u32Timeout * (HCLK_VALUE / 20000UL);
    while (RESET == SDIOC_GetIntStatus(pstcHost->SDIOCx, (SDIOC_INT_FLAG_DEBE | SDIOC_INT_FLAG_DCE |
                                                          SDIOC_INT_FLAG_DTOE | SDIOC_INT_FLAG_TC))) {
        if (SDIOC_TRANS_DIR_TO_CARD != pstcDataConfig->u16TransDir) {
            /* Read buffer data */
            if (SET == SDIOC_GetHostStatus(pstcHost->SDIOCx, SDIOC_HOST_FLAG_BRE)) {
                (void)SDIOC_ReadBuffer(pstcHost->SDIOCx, (uint8_t *)&pu8Data[u32Index],
                                       (uint32_t)(pstcDataConfig->u16BlockSize));
                u32Index += pstcDataConfig->u16BlockSize;
            }
        } else {
            /* Write buffer data */
            if (SET == SDIOC_GetHostStatus(pstcHost->SDIOCx, SDIOC_HOST_FLAG_BWE)) {
                (void)SDIOC_WriteBuffer(pstcHost->SDIOCx, (uint8_t *)&pu8Data[u32Index],
                                        (uint32_t)(pstcDataConfig->u16BlockSize));
                u32Index += pstcDataConfig->u16BlockSize;
            }
        }
        if (0UL == u32Count) {
            SDIOC_ClearIntStatus(pstcHost->SDIOCx, SDIOC_INT_STATIC_FLAGS);
            return LL_ERR_TIMEOUT;
        }
        u32Count--;
    }

    /* Get error state */
    if (SET == SDIOC_GetIntStatus(pstcHost->SDIOCx, SDIOC_INT_FLAG_DEBE)) {
        SDIOC_ClearIntStatus(pstcHost->SDIOCx, SDIOC_INT_STATIC_FLAGS);
        pstcHost->u32ErrorCode |= SDMMC_ERR_DATA_STOP_BIT;
        return LL_ERR;
    } else if (SET == SDIOC_GetIntStatus(pstcHost->SDIOCx, SDIOC_INT_FLAG_DCE)) {
        SDIOC_ClearIntStatus(pstcHost->SDIOCx, SDIOC_INT_STATIC_FLAGS);
        pstcHost->u32ErrorCode |= SDMMC_ERR_DATA_CRC_FAIL;
        return LL_ERR;
    } else if (SET == SDIOC_GetIntStatus(pstcHost->SDIOCx, SDIOC_INT_FLAG_DTOE)) {
        SDIOC_ClearIntStatus(pstcHost->SDIOCx, SDIOC_INT_STATIC_FLAGS);
        pstcHost->u32ErrorCode |= SDMMC_ERR_DATA_TIMEOUT;
        return LL_ERR;
    } else {
        /* Empty FIFO if there is still any data */
        if (SDIOC_TRANS_DIR_TO_CARD != pstcDataConfig->u16TransDir) {
            u32Count = u32Timeout * (HCLK_VALUE / 20000UL);
            while (SET == SDIOC_GetHostStatus(pstcHost->SDIOCx, SDIOC_HOST_FLAG_BRE)) {
                (void)SDIOC_ReadBuffer(pstcHost->SDIOCx, (uint8_t *)&pu8Data[u32Index],
                                       (uint32_t)(pstcDataConfig->u16BlockSize));
                u32Index += pstcDataConfig->u16BlockSize;
                if (0UL == u32Count) {
                    SDIOC_ClearIntStatus(pstcHost->SDIOCx, SDIOC_INT_STATIC_FLAGS);
                    return LL_ERR_TIMEOUT;
                }
                u32Count--;
            }
        }
        /* Clear all the error and completed flags */
        SDIOC_ClearIntStatus(pstcHost->SDIOCx, SDIOC_INT_STATIC_FLAGS);
    }

    return i32Ret;
}
/**
 * @}
 */

/**
 * @defgroup SDIO_Global_Functions SDIO Global Functions
 * @{
 */

/**
 * @brief  Initialize SDIO.
 * @param  [in] pstcHost                Pointer to a @ref stc_sdio_host_t structure
 * @retval int32_t:
 *           - LL_OK: SDIO Initialize successfully
 *           - LL_ERR: Refer to u32ErrorCode for the reason of error
 *           - LL_ERR_INVD_PARAM: pstcHost == NULL
 *           - LL_ERR_INVD_MD: The Bus clock frequency is too high
 */
int32_t SDIO_Init(stc_sdio_host_t *pstcHost)
{
    int32_t i32Ret;
    stc_sdioc_init_t stcSdiocInit;
    uint16_t u16ClkDiv = 0U;

    if (NULL == pstcHost) {
        i32Ret = LL_ERR_INVD_PARAM;
    } else {
        /* Check the SDIOC clock is over 25Mhz or 50Mhz */
        i32Ret = SDIOC_VerifyClockDiv(SDIOC_MD_SD, pstcHost->stcSdiocInit.u8SpeedMode, pstcHost->stcSdiocInit.u16ClockDiv);
        if (LL_OK != i32Ret) {
            return LL_ERR_INVD_MD;
        }

        /* Default SDIOC configuration for SDIO card initialization */
        i32Ret = SDIOC_GetOptimumClockDiv(SDIOC_OUTPUT_CLK_FREQ_400K, &u16ClkDiv);
        if (LL_OK != i32Ret) {
            return LL_ERR_INVD_MD;
        }
        (void)SDIOC_StructInit(&stcSdiocInit);
        stcSdiocInit.u16ClockDiv   = u16ClkDiv;
        stcSdiocInit.u32Mode       = pstcHost->stcSdiocInit.u32Mode;
        stcSdiocInit.u8CardDetect  = pstcHost->stcSdiocInit.u8CardDetect;
        (void)SDIOC_Init(pstcHost->SDIOCx, &stcSdiocInit);

        /* Set Power State to ON */
        SDIOC_PowerCmd(pstcHost->SDIOCx, ENABLE);

        /* Wait for the SDIOC to initialization */
        DDL_DelayMS(2U);

        /* Initialize SDIO card */
        i32Ret = SDIO_InitCard(pstcHost);
    }

    return i32Ret;
}

/**
 * @brief  This function handles SDIO card interrupt request.
 * @param  [in] pstcHost                Pointer to a @ref stc_sdio_host_t structure
 * @retval None
 */
void SDIO_IRQHandler(stc_sdio_host_t *pstcHost)
{
    en_functional_state_t enTransState;
    en_functional_state_t enReceiveState;

    enTransState   = SDIOC_GetIntEnableState(pstcHost->SDIOCx, SDIOC_INT_BWRSEN);
    enReceiveState = SDIOC_GetIntEnableState(pstcHost->SDIOCx, SDIOC_INT_BRRSEN);
    /* Check for SDIO interrupt flags */
    if (RESET != SDIOC_GetIntStatus(pstcHost->SDIOCx, SDIOC_INT_FLAG_TC)) {
        SDIOC_ClearIntStatus(pstcHost->SDIOCx, SDIOC_INT_FLAG_TC);
        SDIOC_IntCmd(pstcHost->SDIOCx, (SDIOC_INT_DEBESEN | SDIOC_INT_DCESEN | SDIOC_INT_DTOESEN |
                                        SDIOC_INT_TCSEN   | SDIOC_INT_BRRSEN | SDIOC_INT_BWRSEN), DISABLE);
        if ((0UL != (pstcHost->u32Context & SDIO_CONTEXT_INT)) || (0UL != (pstcHost->u32Context & SDIO_CONTEXT_DMA))) {
            SDIOC_ClearIntStatus(pstcHost->SDIOCx, SDIOC_INT_STATIC_FLAGS);
            if ((0UL != (pstcHost->u32Context & SDIO_CONTEXT_WR_SINGLE_BLOCK)) ||
                (0UL != (pstcHost->u32Context & SDIO_CONTEXT_WR_MULTI_BLOCK))) {
                SDIO_TxCompleteCallback(pstcHost);
            } else {
                SDIO_RxCompleteCallback(pstcHost);
            }
        }
    } else if ((RESET != SDIOC_GetHostStatus(pstcHost->SDIOCx, SDIOC_HOST_FLAG_BWE)) &&
               (DISABLE != enTransState)) {
        (void)SDIOC_WriteBuffer(pstcHost->SDIOCx, pstcHost->pu8Buffer, 4UL);
        pstcHost->pu8Buffer += 4U;
        pstcHost->u32Len -= 4U;
        if (0UL == pstcHost->u32Len) {
            SDIOC_IntCmd(pstcHost->SDIOCx, SDIOC_INT_BWRSEN, DISABLE);
        }
    } else if ((RESET != SDIOC_GetHostStatus(pstcHost->SDIOCx, SDIOC_HOST_FLAG_BRE)) &&
               (DISABLE != enReceiveState)) {
        (void)SDIOC_ReadBuffer(pstcHost->SDIOCx, pstcHost->pu8Buffer, 4UL);
        pstcHost->pu8Buffer += 4U;
        pstcHost->u32Len -= 4U;
        if (0UL == pstcHost->u32Len) {
            SDIOC_IntCmd(pstcHost->SDIOCx, SDIOC_INT_BRRSEN, DISABLE);
        }
    } else if (RESET != SDIOC_GetIntStatus(pstcHost->SDIOCx, (SDIOC_INT_DEBESEN |
                                                              SDIOC_INT_DCESEN | SDIOC_INT_DTOESEN))) {
        /* Set LL_ERR code */
        if (RESET != SDIOC_GetIntStatus(pstcHost->SDIOCx, SDIOC_INT_DEBESEN)) {
            pstcHost->u32ErrorCode |= SDMMC_ERR_DATA_STOP_BIT;
        }
        if (RESET != SDIOC_GetIntStatus(pstcHost->SDIOCx, SDIOC_INT_DCESEN)) {
            pstcHost->u32ErrorCode |= SDMMC_ERR_DATA_CRC_FAIL;
        }
        if (RESET != SDIOC_GetIntStatus(pstcHost->SDIOCx, SDIOC_INT_DTOESEN)) {
            pstcHost->u32ErrorCode |= SDMMC_ERR_DATA_TIMEOUT;
        }

        /* Clear All flags */
        SDIOC_ClearIntStatus(pstcHost->SDIOCx, SDIOC_INT_STATIC_FLAGS);
        /* Disable all interrupts */
        SDIOC_IntCmd(pstcHost->SDIOCx, (SDIOC_INT_DEBESEN | SDIOC_INT_DCESEN | SDIOC_INT_DTOESEN |
                                        SDIOC_INT_TCSEN   | SDIOC_INT_BRRSEN | SDIOC_INT_BWRSEN), DISABLE);
        if ((0UL != (pstcHost->u32Context & SDIO_CONTEXT_INT)) ||
            (0UL != (pstcHost->u32Context & SDIO_CONTEXT_DMA))) {
            SDIO_ErrorCallback(pstcHost);
        } else {
        }
    } else {
    }
}

/**
 * @brief  SDIO Tx completed callbacks
 * @param  [in] pstcHost                Pointer to a @ref stc_sdio_host_t structure
 * @retval None
 */
__WEAKDEF void SDIO_TxCompleteCallback(stc_sdio_host_t *pstcHost)
{
    (void)pstcHost;
    /* NOTE: This function SDIO_TxCpltCallback can be implemented in the user file */
}

/**
 * @brief  SDIO Rx completed callbacks
 * @param  [in] pstcHost                Pointer to a @ref stc_sdio_host_t structure
 * @retval None
 */
__WEAKDEF void SDIO_RxCompleteCallback(stc_sdio_host_t *pstcHost)
{
    (void)pstcHost;
    /* NOTE: This function SDIO_TxCpltCallback can be implemented in the user file */
}

/**
 * @brief  SDIO error callbacks
 * @param  [in] pstcHost                Pointer to a @ref stc_sdio_host_t structure
 * @retval None
 */
__WEAKDEF void SDIO_ErrorCallback(stc_sdio_host_t *pstcHost)
{
    (void)pstcHost;
    /* NOTE: This function SDIO_TxCpltCallback can be implemented in the user file */
}

/**
 * @brief  Configure the Dma transfer parameters.
 * @param  [in] pstcHost                Pointer to a @ref stc_sdio_host_t structure
 * @param  [in] u8Ch                    DMA transfer channel
 * @param  [in] u32SrcAddr              Source Address
 * @param  [in] u32DestAddr             Destination Address
 * @param  [in] u16BlockSize            Block Size
 * @param  [in] u16TransCount           Transfer Count
 * @retval None
 */
void SDIO_DmaTransConfig(const stc_sdio_host_t *pstcHost, uint8_t u8Ch, uint32_t u32SrcAddr,
                         uint32_t u32DestAddr, uint16_t u16BlockSize, uint16_t u16TransCount)
{
    /* Stop Configure channel */
    (void)DMA_ChCmd(pstcHost->DMAx, u8Ch, DISABLE);
    DMA_ClearTransCompleteStatus(pstcHost->DMAx, (uint32_t)(0x1UL << u8Ch));

    /* Config DMA source and destination address */
    (void)DMA_SetSrcAddr(pstcHost->DMAx, u8Ch, u32SrcAddr);
    (void)DMA_SetDestAddr(pstcHost->DMAx, u8Ch, u32DestAddr);
    /* Config DMA block size and transfer count */
    (void)DMA_SetBlockSize(pstcHost->DMAx, u8Ch, u16BlockSize);
    (void)DMA_SetTransCount(pstcHost->DMAx, u8Ch, u16TransCount);
}

/**
 * @brief  Set high speed.
 * @param  [in] pstcCard                Pointer to a @ref stc_sdio_card_t structure
 * @retval int32_t:
 *           - LL_OK: Operate successfully
 *           - LL_ERR: Operate unsuccessfully
 *           - LL_ERR_INVD_PARAM: NULL == pstcCard or NULL == pstcCard->pstcHost or NULL == pstcCard->apstcSdioFunc[0]
 */
int32_t SDIO_SetHighSpeed(stc_sdio_card_t *pstcCard)
{
    int32_t i32Ret;
    uint8_t u8Speed;

    if ((NULL == pstcCard) || (NULL == pstcCard->pstcHost) || (NULL == pstcCard->apstcSdioFunc[0])) {
        i32Ret = LL_ERR_INVD_PARAM;
    } else {
        if (RESET == pstcCard->stcCccr.enSupportHighSpeed) {
            return LL_ERR;
        }

        i32Ret = SDIO_IOReadByte(pstcCard->apstcSdioFunc[0], SDIO_REG_CCCR_HIGH_SPEED, &u8Speed);
        if (LL_OK != i32Ret) {
            return LL_ERR;
        }

        u8Speed |= SDIO_HIGH_SPEED_EHS;

        i32Ret = SDIO_IOWriteByte(pstcCard->apstcSdioFunc[0], SDIO_REG_CCCR_HIGH_SPEED, u8Speed);
        if (LL_OK != i32Ret) {
            return LL_ERR;
        }

        /* Set the clock division and speed mode of SDIOC */
        SDIOC_SetSpeedMode(pstcCard->pstcHost->SDIOCx, pstcCard->pstcHost->stcSdiocInit.u8SpeedMode);
        SDIOC_SetClockDiv(pstcCard->pstcHost->SDIOCx, pstcCard->pstcHost->stcSdiocInit.u16ClockDiv);
        i32Ret = LL_OK;
    }

    return i32Ret;
}

/**
 * @brief  Set bus width according with host bus width.
 * @param  [in] pstcCard                Pointer to a @ref stc_sdio_card_t structure
 * @retval int32_t:
 *           - LL_OK: Operate successfully
 *           - LL_ERR: Operate unsuccessfully
 *           - LL_ERR_INVD_PARAM: NULL == pstcCard or NULL == pstcCard->pstcHost or NULL == pstcCard->apstcSdioFunc[0]
 */
int32_t SDIO_SetBusWidth(stc_sdio_card_t *pstcCard)
{
    int32_t i32Ret;
    uint8_t u8BusWidth;

    if ((NULL == pstcCard) || (NULL == pstcCard->pstcHost) || (NULL == pstcCard->apstcSdioFunc[0])) {
        i32Ret = LL_ERR_INVD_PARAM;
    } else {
        if ((SET == pstcCard->stcCccr.enSupportLowSpeedCard) && (RESET == pstcCard->stcCccr.enBusWidth)) {
            return LL_ERR;
        }

        i32Ret = SDIO_IOReadByte(pstcCard->apstcSdioFunc[0], SDIO_REG_CCCR_BUS_IF_CTRL, &u8BusWidth);
        if (LL_OK != i32Ret) {
            return LL_ERR;
        }

        u8BusWidth &= SDIO_BUS_IF_CTRL_WIDTH_MASK;
        if (SDIOC_BUS_WIDTH_1BIT == pstcCard->pstcHost->stcSdiocInit.u8BusWidth) {
            u8BusWidth |= SDIO_BUS_IF_CTRL_WIDTH_1BIT;   /* 1 Bit */
        } else if (SDIOC_BUS_WIDTH_4BIT == pstcCard->pstcHost->stcSdiocInit.u8BusWidth) {
            u8BusWidth |= SDIO_BUS_IF_CTRL_WIDTH_4BIT;   /* 4 Bit */
        } else {
            return LL_ERR_INVD_PARAM;
        }

        i32Ret = SDIO_IOWriteByte(pstcCard->apstcSdioFunc[0], SDIO_REG_CCCR_BUS_IF_CTRL, u8BusWidth);
        if (LL_OK != i32Ret) {
            return LL_ERR;
        }

        /* Set the bus width of SDIOC */
        (void)SDIOC_SetBusWidth(pstcCard->pstcHost->SDIOCx, pstcCard->pstcHost->stcSdiocInit.u8BusWidth);
        i32Ret = LL_OK;
    }

    return i32Ret;
}

/**
 * @brief  Set the block size for the specified function.
 * @param  [in] pstcFunc                Pointer to a @ref stc_sdio_func_t structure
 * @param  [in] u16BlockSize            Block size
 * @retval int32_t:
 *           - LL_OK: Operate successfully
 *           - LL_ERR: Operate unsuccessfully
 *           - LL_ERR_INVD_PARAM: NULL == pstcFunc or NULL == pstcFunc->pstcCard or
 *                                NULL == pstcFunc->pstcCard->apstcSdioFunc[0] or
 *                                u16BlockSize > pstcFunc->u16MaxBlockSize
 */
int32_t SDIO_SetBlockSize(stc_sdio_func_t *pstcFunc, uint16_t u16BlockSize)
{
    int32_t i32Ret;
    uint32_t u32Addr;
    stc_sdio_func_t *pstcFunc0;

    if ((NULL == pstcFunc) || (NULL == pstcFunc->pstcCard) || \
        (NULL == pstcFunc->pstcCard->apstcSdioFunc[0]) || (u16BlockSize > pstcFunc->u16MaxBlockSize)) {
        i32Ret = LL_ERR_INVD_PARAM;
    } else {
        pstcFunc0 = pstcFunc->pstcCard->apstcSdioFunc[0];

        u32Addr = SDIO_REG_FBR_BASE(pstcFunc->u8FuncNum) + SDIO_REG_FBR_BLOCK_SIZE;
        i32Ret = SDIO_IOWriteByte(pstcFunc0, u32Addr, (uint8_t)(u16BlockSize & 0xFFU));
        if (LL_OK != i32Ret) {
            return LL_ERR;
        }

        i32Ret = SDIO_IOWriteByte(pstcFunc0, u32Addr + 1UL, (uint8_t)((u16BlockSize >> 8) & 0xFFU));
        if (LL_OK != i32Ret) {
            return LL_ERR;
        }

        pstcFunc->u16CurrBlockSize = u16BlockSize;
        i32Ret = LL_OK;
    }

    return i32Ret;
}

/**
 * @brief  Read byte.
 * @param  [in] pstcFunc                Pointer to a @ref stc_sdio_func_t structure
 * @param  [in] u32Addr                 Register address
 * @param  [in] pu8Data                 Pointer to data buffer
 * @retval int32_t:
 *           - LL_OK: Operate successfully
 *           - LL_ERR: Operate unsuccessfully
 *           - LL_ERR_INVD_PARAM: NULL == pstcFunc or NULL == pstcFunc->pstcCard or NULL == pstcFunc->pstcCard->pstcHost
 *                                or NULL == pu8Data
 */
int32_t SDIO_IOReadByte(stc_sdio_func_t *pstcFunc, uint32_t u32Addr, uint8_t *pu8Data)
{
    int32_t i32Ret;
    stc_sdio_host_t *pstcHost;
    stc_sdio_cmd52_arg_t stcCmdArg;

    if ((NULL == pstcFunc) || (NULL == pstcFunc->pstcCard) || \
        (NULL == pstcFunc->pstcCard->pstcHost) || (NULL == pu8Data)) {
        i32Ret = LL_ERR_INVD_PARAM;
    } else {
        pstcHost = pstcFunc->pstcCard->pstcHost;

        stcCmdArg.u32RwFlag = SDIO_CMD52_ARG_RD;
        stcCmdArg.u8FuncNum = pstcFunc->u8FuncNum;
        stcCmdArg.u32RegAddr = u32Addr;
        stcCmdArg.u32RawFlag = SDIO_CMD52_ARG_RAW_FLAG_0;
        i32Ret = SDMMC_CMD52_IORwDirect(pstcHost->SDIOCx, &stcCmdArg, 0U, pu8Data,
                                        &pstcHost->u32ErrorCode);
    }

    return i32Ret;
}

/**
 * @brief  Write byte.
 * @param  [in] pstcFunc                Pointer to a @ref stc_sdio_func_t structure
 * @param  [in] u32Addr                 Register address
 * @param  [in] u8Data                  Data to write
 * @retval int32_t:
 *           - LL_OK: Operate successfully
 *           - LL_ERR: Operate unsuccessfully
 *           - LL_ERR_INVD_PARAM: NULL == pstcFunc or NULL == pstcFunc->pstcCard or NULL == pstcFunc->pstcCard->pstcHost
 */
int32_t SDIO_IOWriteByte(stc_sdio_func_t *pstcFunc, uint32_t u32Addr, uint8_t u8Data)
{
    int32_t i32Ret;
    stc_sdio_host_t *pstcHost;
    stc_sdio_cmd52_arg_t stcCmdArg;

    if ((NULL == pstcFunc) || (NULL == pstcFunc->pstcCard) || (NULL == pstcFunc->pstcCard->pstcHost)) {
        i32Ret = LL_ERR_INVD_PARAM;
    } else {
        pstcHost = pstcFunc->pstcCard->pstcHost;

        stcCmdArg.u32RwFlag = SDIO_CMD52_ARG_WR;
        stcCmdArg.u8FuncNum = pstcFunc->u8FuncNum;
        stcCmdArg.u32RegAddr = u32Addr;
        stcCmdArg.u32RawFlag = SDIO_CMD52_ARG_RAW_FLAG_0;
        i32Ret = SDMMC_CMD52_IORwDirect(pstcHost->SDIOCx, &stcCmdArg, u8Data, NULL,
                                        &pstcHost->u32ErrorCode);
    }

    return i32Ret;
}

/**
 * @brief  Transfer data using block mode by polling.
 * @param  [in] pstcFunc                Pointer to a @ref stc_sdio_func_t structure
 * @param  [in] u32RwFlag               Read or write
 *         This parameter can be one of the macros group @ref SDIO_CMD53_Arguments_RW_Flag
 *           @arg SDIO_CMD53_ARG_RD: Read
 *           @arg SDIO_CMD53_ARG_WR: Write
 * @param  [in] u32Addr                 Register address
 * @param  [in] u32OperateCode          Operate code
 *         This parameter can be one of the macros group @ref SDIO_CMD53_Arguments_Operate_Code
 *           @arg SDIO_CMD53_ARG_OP_CODE_ADDR_FIX: Address fix
 *           @arg SDIO_CMD53_ARG_OP_CODE_ADDR_INC: Address increase
 * @param  [in] pu8Data                 Pointer to data buffer
 * @param  [in] u16BlockCount           Block count
 * @param  [in] u16BlockSize            Block size
 * @retval int32_t:
 *           - LL_OK: Operate successfully
 *           - LL_ERR: Operate unsuccessfully
 *           - LL_ERR_INVD_PARAM: NULL == pstcFunc or NULL == pstcFunc->pstcCard or NULL == pstcFunc->pstcCard->pstcHost
 *                                or NULL == pu8Data or 0U == u16BlockCount or 0U == u16BlockSize
 */
int32_t SDIO_IORwExtended(stc_sdio_func_t *pstcFunc,
                          uint32_t u32RwFlag,
                          uint32_t u32Addr,
                          uint32_t u32OperateCode,
                          uint8_t  *pu8Data,
                          uint16_t u16BlockCount,
                          uint16_t u16BlockSize)
{
    int32_t  i32Ret;
    uint32_t u32ErrStatus;
    stc_sdio_host_t *pstcHost;
    stc_sdio_cmd53_arg_t stcCmdArg;
    stc_sdioc_data_config_t stcDataCfg;

    if ((NULL == pstcFunc) || (NULL == pstcFunc->pstcCard) || (NULL == pstcFunc->pstcCard->pstcHost) || \
        (NULL == pu8Data) || (0U == u16BlockCount) || (0U == u16BlockSize)) {
        return LL_ERR_INVD_PARAM;
    } else {
        pstcHost = pstcFunc->pstcCard->pstcHost;

        pstcHost->pu8Buffer = pu8Data;
        pstcHost->u32Len = (uint32_t)u16BlockCount * (uint32_t)u16BlockSize;
        pstcHost->u32Context = SDIO_CONTEXT_NONE;
        pstcHost->u32ErrorCode = SDMMC_ERR_NONE;

        if (SDIO_CMD53_ARG_WR == u32RwFlag) {
            pstcHost->u32Context |= (u16BlockCount > 1U) ? SDIO_CONTEXT_WR_MULTI_BLOCK : SDIO_CONTEXT_WR_SINGLE_BLOCK;
        } else {
            pstcHost->u32Context |= (u16BlockCount > 1U) ? SDIO_CONTEXT_RD_MULTI_BLOCK : SDIO_CONTEXT_RD_SINGLE_BLOCK;
        }

        /* Configure the SDIOC data transfer */
        stcDataCfg.u16BlockSize  = u16BlockSize;
        stcDataCfg.u16BlockCount = u16BlockCount;
        stcDataCfg.u16TransDir   = (u32RwFlag == SDIO_CMD53_ARG_WR) ? SDIOC_TRANS_DIR_TO_CARD : SDIOC_TRANS_DIR_TO_HOST;
        stcDataCfg.u16AutoCmd12  = SDIOC_AUTO_SEND_CMD12_DISABLE;
        stcDataCfg.u16TransMode  = (u16BlockCount > 1U) ? SDIOC_TRANS_MD_MULTI : SDIOC_TRANS_MD_SINGLE;
        stcDataCfg.u16DataTimeout = SDIOC_DATA_TIMEOUT_CLK_2E27;
        (void)SDIOC_ConfigData(pstcHost->SDIOCx, &stcDataCfg);

        /* IO_RW_EXTENDED Command */
        stcCmdArg.u8FuncNum = pstcFunc->u8FuncNum;
        stcCmdArg.u32RwFlag = u32RwFlag;
        stcCmdArg.u32RegAddr = u32Addr;
        stcCmdArg.u32OperateCode = u32OperateCode;
        if ((1UL == u16BlockCount) && (u16BlockSize <= SDIO_CMD53_MAX_BYTES)) {
            stcCmdArg.u32Count = (SDIO_CMD53_MAX_BYTES == u16BlockSize) ? 0UL : (uint32_t)u16BlockSize;
            stcCmdArg.u32BlockMode = SDIO_CMD53_ARG_TRANS_MD_BYTE;
        } else {
            stcCmdArg.u32Count = u16BlockCount;
            stcCmdArg.u32BlockMode = SDIO_CMD53_ARG_TRANS_MD_BLOCK;
        }
        i32Ret = SDMMC_CMD53_IORwExtended(pstcHost->SDIOCx, &stcCmdArg, &u32ErrStatus);
        if (LL_OK != i32Ret) {
            SDIOC_ClearIntStatus(pstcHost->SDIOCx, SDIOC_INT_STATIC_FLAGS);
            return LL_ERR;
        }

        /* Get data */
        i32Ret = SDIO_ReadWriteFifo(pstcHost, &stcDataCfg, (uint8_t *)pu8Data, SDMMC_DATA_TIMEOUT);
        if (LL_OK != i32Ret) {
            return LL_ERR;
        }
    }

    return LL_OK;
}

/**
 * @brief  Transfer data using block mode by interrupt.
 * @param  [in] pstcFunc                Pointer to a @ref stc_sdio_func_t structure
 * @param  [in] u32RwFlag               Read or write
 *         This parameter can be one of the macros group @ref SDIO_CMD53_Arguments_RW_Flag
 *           @arg SDIO_CMD53_ARG_RD: Read
 *           @arg SDIO_CMD53_ARG_WR: Write
 * @param  [in] u32Addr                 Register address
 * @param  [in] u32OperateCode          Operate code
 *         This parameter can be one of the macros group @ref SDIO_CMD53_Arguments_Operate_Code
 *           @arg SDIO_CMD53_ARG_OP_CODE_ADDR_FIX: Address fix
 *           @arg SDIO_CMD53_ARG_OP_CODE_ADDR_INC: Address increase
 * @param  [in] pu8Data                 Pointer to data buffer
 * @param  [in] u16BlockCount           Block count
 * @param  [in] u16BlockSize            Block size
 * @retval int32_t:
 *           - LL_OK: Operate successfully
 *           - LL_ERR: Operate unsuccessfully
 *           - LL_ERR_INVD_PARAM: NULL == pstcFunc or NULL == pstcFunc->pstcCard or NULL == pstcFunc->pstcCard->pstcHost
 *                                or NULL == pu8Data or 0U == u16BlockCount or 0U == u16BlockSize
 */
int32_t SDIO_IORwExtended_INT(stc_sdio_func_t *pstcFunc,
                              uint32_t u32RwFlag,
                              uint32_t u32Addr,
                              uint32_t u32OperateCode,
                              uint8_t  *pu8Data,
                              uint16_t u16BlockCount,
                              uint16_t u16BlockSize)
{
    int32_t  i32Ret;
    uint32_t u32ErrStatus;
    stc_sdio_host_t *pstcHost;
    stc_sdio_cmd53_arg_t stcCmdArg;
    stc_sdioc_data_config_t stcDataCfg;

    if ((NULL == pstcFunc) || (NULL == pstcFunc->pstcCard) || (NULL == pstcFunc->pstcCard->pstcHost) || \
        (NULL == pu8Data) || (0U == u16BlockCount) || (0U == u16BlockSize)) {
        return LL_ERR_INVD_PARAM;
    } else {
        pstcHost = pstcFunc->pstcCard->pstcHost;

        pstcHost->pu8Buffer = pu8Data;
        pstcHost->u32Len = (uint32_t)u16BlockCount * (uint32_t)u16BlockSize;
        pstcHost->u32Context = SDIO_CONTEXT_INT;
        pstcHost->u32ErrorCode = SDMMC_ERR_NONE;

        SDIOC_ClearIntStatus(pstcHost->SDIOCx, (SDIOC_INT_FLAG_BWR | SDIOC_INT_FLAG_BRR));

        /* Enable SDIOC interrupt */
        if (SDIO_CMD53_ARG_WR == u32RwFlag) {
            pstcHost->u32Context |= (u16BlockCount > 1U) ? SDIO_CONTEXT_WR_MULTI_BLOCK : SDIO_CONTEXT_WR_SINGLE_BLOCK;

            SDIOC_IntCmd(pstcHost->SDIOCx, (SDIOC_INT_DEBESEN | SDIOC_INT_DCESEN | SDIOC_INT_DTOESEN | \
                                            SDIOC_INT_TCSEN   | SDIOC_INT_BWRSEN), ENABLE);
        } else {
            pstcHost->u32Context |= (u16BlockCount > 1U) ? SDIO_CONTEXT_RD_MULTI_BLOCK : SDIO_CONTEXT_RD_SINGLE_BLOCK;

            SDIOC_IntCmd(pstcHost->SDIOCx, (SDIOC_INT_DEBESEN | SDIOC_INT_DCESEN | SDIOC_INT_DTOESEN | \
                                            SDIOC_INT_TCSEN   | SDIOC_INT_BRRSEN), ENABLE);
        }

        /* Configure the SDIOC data transfer */
        stcDataCfg.u16BlockSize  = u16BlockSize;
        stcDataCfg.u16BlockCount = u16BlockCount;
        stcDataCfg.u16TransDir   = (SDIO_CMD53_ARG_WR == u32RwFlag) ? SDIOC_TRANS_DIR_TO_CARD : SDIOC_TRANS_DIR_TO_HOST;
        stcDataCfg.u16AutoCmd12  = SDIOC_AUTO_SEND_CMD12_DISABLE;
        stcDataCfg.u16TransMode  = (u16BlockCount > 1U) ? (uint16_t)SDIOC_TRANS_MD_MULTI :
                                   (uint16_t)SDIOC_TRANS_MD_SINGLE;
        stcDataCfg.u16DataTimeout = SDIOC_DATA_TIMEOUT_CLK_2E27;
        (void)SDIOC_ConfigData(pstcHost->SDIOCx, &stcDataCfg);

        /* IO_RW_EXTENDED Command */
        stcCmdArg.u8FuncNum = pstcFunc->u8FuncNum;
        stcCmdArg.u32RwFlag = u32RwFlag;
        stcCmdArg.u32RegAddr = u32Addr;
        stcCmdArg.u32OperateCode = u32OperateCode;
        if ((1UL == u16BlockCount) && (u16BlockSize <= SDIO_CMD53_MAX_BYTES)) {
            stcCmdArg.u32Count = (SDIO_CMD53_MAX_BYTES == u16BlockSize) ? 0UL : (uint32_t)u16BlockSize;
            stcCmdArg.u32BlockMode = SDIO_CMD53_ARG_TRANS_MD_BYTE;
        } else {
            stcCmdArg.u32Count = u16BlockCount;
            stcCmdArg.u32BlockMode = SDIO_CMD53_ARG_TRANS_MD_BLOCK;
        }
        i32Ret = SDMMC_CMD53_IORwExtended(pstcHost->SDIOCx, &stcCmdArg, &u32ErrStatus);
        if (LL_OK != i32Ret) {
            SDIOC_ClearIntStatus(pstcHost->SDIOCx, SDIOC_INT_STATIC_FLAGS);
            return LL_ERR;
        }
    }

    return LL_OK;
}

/**
 * @brief  Transfer data using block mode by DMA.
 * @param  [in] pstcFunc                Pointer to a @ref stc_sdio_func_t structure
 * @param  [in] u32RwFlag               Read or write
 *         This parameter can be one of the macros group @ref SDIO_CMD53_Arguments_RW_Flag
 *           @arg SDIO_CMD53_ARG_RD: Read
 *           @arg SDIO_CMD53_ARG_WR: Write
 * @param  [in] u32Addr                 Register address
 * @param  [in] u32OperateCode          Operate code
 *         This parameter can be one of the macros group @ref SDIO_CMD53_Arguments_Operate_Code
 *           @arg SDIO_CMD53_ARG_OP_CODE_ADDR_FIX: Address fix
 *           @arg SDIO_CMD53_ARG_OP_CODE_ADDR_INC: Address increase
 * @param  [in] pu8Data                 Pointer to data buffer
 * @param  [in] u16BlockCount           Block count
 * @param  [in] u16BlockSize            Block size
 * @retval int32_t:
 *           - LL_OK: Read block(s) success
 *           - LL_ERR: Refer to u32ErrorCode for the reason of error
 *           - LL_ERR_INVD_PARAM: pstcFunc == NULL or NULL == pu8Data or pstcFunc->pstcCard == NULL or
 *                                NULL == pstcFunc->pstcCard->pstcHost or 0U == u16BlockCount or 0U == u16BlockSize
 */
int32_t SDIO_IORwExtended_DMA(stc_sdio_func_t *pstcFunc,
                              uint32_t u32RwFlag,
                              uint32_t u32Addr,
                              uint32_t u32OperateCode,
                              uint8_t  *pu8Data,
                              uint16_t u16BlockCount,
                              uint16_t u16BlockSize)
{
    int32_t  i32Ret;
    uint32_t u32ErrStatus;
    stc_sdio_host_t *pstcHost;
    stc_sdio_cmd53_arg_t stcCmdArg;
    stc_sdioc_data_config_t stcDataCfg;

    if ((NULL == pstcFunc) || (NULL == pstcFunc->pstcCard)  || (NULL == pstcFunc->pstcCard->pstcHost) || \
        (NULL == pu8Data) || (!IS_ADDR_ALIGN_WORD(pu8Data)) || (0U == u16BlockCount) || (0U == u16BlockSize)) {
        return LL_ERR_INVD_PARAM;
    } else {
        pstcHost = pstcFunc->pstcCard->pstcHost;

        pstcHost->pu8Buffer = pu8Data;
        pstcHost->u32Len = (uint32_t)u16BlockCount * (uint32_t)u16BlockCount;
        pstcHost->u32ErrorCode = SDMMC_ERR_NONE;
        pstcHost->u32Context = SDIO_CONTEXT_DMA;

        /* Enable SDIOC interrupt */
        if (SDIO_CMD53_ARG_WR == u32RwFlag) {
            pstcHost->u32Context |= (u16BlockCount > 1U) ? SDIO_CONTEXT_WR_MULTI_BLOCK : SDIO_CONTEXT_WR_SINGLE_BLOCK;

            /* Configure DMA parameters */
            SDIO_DmaTransConfig(pstcHost, pstcHost->u8DmaTxCh, (uint32_t)pu8Data, (uint32_t)(&pstcHost->SDIOCx->BUF0),
                                (u16BlockSize / 4U), u16BlockCount);
            /* Enable the DMA Channel */
            (void)DMA_ChCmd(pstcHost->DMAx, pstcHost->u8DmaTxCh, ENABLE);
        } else {
            pstcHost->u32Context |= (u16BlockCount > 1U) ? SDIO_CONTEXT_RD_MULTI_BLOCK : SDIO_CONTEXT_RD_SINGLE_BLOCK;

            /* Configure DMA parameters */
            SDIO_DmaTransConfig(pstcHost, pstcHost->u8DmaRxCh, (uint32_t)(&pstcHost->SDIOCx->BUF0), (uint32_t)pu8Data,
                                (u16BlockSize / 4U), u16BlockCount);
            /* Enable the DMA Channel */
            (void)DMA_ChCmd(pstcHost->DMAx, pstcHost->u8DmaRxCh, ENABLE);
        }

        /* Enable SDIOC transfer complete and errors interrupt */
        SDIOC_IntCmd(pstcHost->SDIOCx, (SDIOC_INT_TCSEN  | SDIOC_INT_DEBESEN | \
                                        SDIOC_INT_DCESEN | SDIOC_INT_DTOESEN), ENABLE);

        /* Configure the SDIOC data transfer */
        stcDataCfg.u16BlockSize  = u16BlockSize;
        stcDataCfg.u16BlockCount = u16BlockCount;
        stcDataCfg.u16TransDir   = (SDIO_CMD53_ARG_WR == u32RwFlag) ? SDIOC_TRANS_DIR_TO_CARD : SDIOC_TRANS_DIR_TO_HOST;
        stcDataCfg.u16AutoCmd12  = SDIOC_AUTO_SEND_CMD12_DISABLE;
        stcDataCfg.u16TransMode  = (u16BlockCount > 1U) ? SDIOC_TRANS_MD_MULTI : SDIOC_TRANS_MD_SINGLE;
        stcDataCfg.u16DataTimeout = SDIOC_DATA_TIMEOUT_CLK_2E27;
        (void)SDIOC_ConfigData(pstcHost->SDIOCx, &stcDataCfg);

        /* IO_RW_EXTENDED Command */
        stcCmdArg.u8FuncNum = pstcFunc->u8FuncNum;
        stcCmdArg.u32RwFlag = u32RwFlag;
        stcCmdArg.u32RegAddr = u32Addr;
        stcCmdArg.u32OperateCode = u32OperateCode;
        if ((1UL == u16BlockCount) && (u16BlockSize <= SDIO_CMD53_MAX_BYTES)) {
            stcCmdArg.u32Count = (SDIO_CMD53_MAX_BYTES == u16BlockSize) ? 0UL : (uint32_t)u16BlockSize;
            stcCmdArg.u32BlockMode = SDIO_CMD53_ARG_TRANS_MD_BYTE;
        } else {
            stcCmdArg.u32Count = u16BlockCount;
            stcCmdArg.u32BlockMode = SDIO_CMD53_ARG_TRANS_MD_BLOCK;
        }
        i32Ret = SDMMC_CMD53_IORwExtended(pstcHost->SDIOCx, &stcCmdArg, &u32ErrStatus);
        if (LL_OK != i32Ret) {
            SDIOC_ClearIntStatus(pstcHost->SDIOCx, SDIOC_INT_STATIC_FLAGS);
            return LL_ERR;
        }
    }

    return LL_OK;
}

/**
 * @}
 */

/**
 * @}
 */

/**
 * @}
 */

/******************************************************************************
 * EOF (not truncated)
 *****************************************************************************/
