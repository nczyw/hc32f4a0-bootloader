/**
 *******************************************************************************
 * @file  sdio.h
 * @brief This file contains all the functions prototypes of the SDIO card
 *        driver library.
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
#ifndef __SDIO_H__
#define __SDIO_H__

/* C binding of definitions if building with C++ compiler */
#ifdef __cplusplus
extern "C"
{
#endif

/*******************************************************************************
 * Include files
 ******************************************************************************/
#include "hc32_ll_sdioc.h"
#include "hc32_ll_dma.h"
#include "hc32_ll_utility.h"

/**
 * @addtogroup LL_SDIOC_LIB
 * @{
 */

/**
 * @addtogroup SDIOC_SDIO_Card
 * @{
 */

/*******************************************************************************
 * Global type definitions ('typedef')
 ******************************************************************************/
/**
 * @defgroup SDIO_Global_Types SDIO Global Types
 * @{
 */

/**
 * @brief SDIO CCCR structure
 */
typedef struct {
    uint8_t          u8CccrVersion;             /*!< CCCR version */
    uint8_t          u8SdioVersion;             /*!< SDIO version */

    en_flag_status_t enSupportDirectCmd;        /*!< Support Direct Commands(CMD52) */
    en_flag_status_t enSupportMultiBlock;       /*!< Support Multi-Block Transfer(CMD53) */
    en_flag_status_t enSupportLowSpeedCard;     /*!< Low-Speed Card */
    en_flag_status_t enSupportLowSpeed4Bit;     /*!< 4bit Mode Support for Low-Speed Card */

    en_flag_status_t enBusWidth;                /*!< SDIO bus width */
    en_flag_status_t enSupportPowerCtrl;        /*!< Support Master Power Control */
    en_flag_status_t enSupportHighSpeed;        /*!< Support High-Speed */
} stc_sdio_cccr_t;

/**
 * @brief SDIO common CIS structure
 */
typedef struct {
    uint16_t     u16ManufacturerCode;
    uint16_t     u16ManufacturerInfo;
    uint16_t     u16Func0BlockSize;
    uint32_t     u32MaxTransSpeed;
} stc_sdio_cis_t;

typedef struct stc_sdio_card stc_sdio_card_t;
typedef struct stc_sdio_host stc_sdio_host_t;
typedef struct stc_sdio_func_tuple stc_sdio_func_tuple_t;

/**
 * @brief SDIO function CIS tuple structure
 */
struct stc_sdio_func_tuple {
    uint8_t u8Code;
    uint8_t u8Size;
    uint8_t *pu8Data;

    stc_sdio_func_tuple_t *pstcNext;
};

/**
 * @brief SDIO function structure
 */
typedef struct {
    stc_sdio_card_t       *pstcCard;            /*!< The function belongs to this SDIO card */

    uint8_t               u8FuncNum;            /*!< Function number */
    uint8_t               u8FuncCode;           /*!< Standard SDIO function interface code  */
    uint16_t              u16ManufacturerCode;  /*!< Manufacturer code */
    uint16_t              u16ManufacturerInfo;  /*!< Manufacturer information */
    uint16_t              u16MaxBlockSize;      /*!< Maximum block size */
    uint16_t              u16CurrBlockSize;     /*!< Current block size */
    uint32_t              u32EnableTimeoutVal;  /*!< Max enable timeout in msec */
    stc_sdio_func_tuple_t *pstcTuples;          /*!< Tuple chain */
} stc_sdio_func_t;

/**
 * @brief SDIO card structure
 */
struct stc_sdio_card {
    stc_sdio_host_t *pstcHost;              /*!< The card belongs to this host */

    uint16_t        u16RelativeCardAddr;    /*!< Card address */
    uint8_t         u8TotalFuncNum;         /*!< Total number of SDIO functions */
    stc_sdio_cccr_t stcCccr;                /*!< Common card info */
    stc_sdio_cis_t  stcCis;                 /*!< Common tuple info */
    stc_sdio_func_t *apstcSdioFunc[8];      /*!< SDIO functions */
};

/**
 * @brief SDIO host structure
 */
struct stc_sdio_host {
    CM_SDIOC_TypeDef    *SDIOCx;        /*!< Pointer to SDIOC registers base address            */
    stc_sdioc_init_t    stcSdiocInit;   /*!< SDIOC Initialize structure @ref stc_sdioc_init_t   */

    CM_DMA_TypeDef      *DMAx;          /*!< Pointer to DMA registers base address              */
    uint8_t             u8DmaTxCh;      /*!< Specifies the DMA channel used to send             */
    uint8_t             u8DmaRxCh;      /*!< Specifies the DMA channel used to receive          */

    uint8_t             *pu8Buffer;     /*!< Pointer to SD Tx/Rx transfer Buffer                */
    uint32_t            u32Len;         /*!< SD Tx/Rx Transfer length                           */
    uint32_t            u32Context;     /*!< SD transfer context                                */
    uint32_t            u32ErrorCode;   /*!< SD Card Error codes                                */

    stc_sdio_card_t     *pstcCard;      /*!< The card attach this host */
};

/**
 * @}
 */

/*******************************************************************************
 * Global pre-processor symbols/macros ('#define')
 ******************************************************************************/
/**
 * @defgroup SDIO_Global_Macros SDIO Global Macros
 * @{
 */

/**
 * @defgroup SDIO_Card_Common_Control_Registers SDIO Card Common Control Registers(CCCR)
 * @{
 */
#define SDIO_REG_CCCR_CCCR_REV          (0x00UL)  /*!< CCCR/SDIO Revision */
#define SDIO_REG_CCCR_SD_REV            (0x01UL)  /*!< SD Specification Revision */
#define SDIO_REG_CCCR_IO_EN             (0x02UL)  /*!< I/O Enable */
#define SDIO_REG_CCCR_IO_RDY            (0x03UL)  /*!< I/O Ready */
#define SDIO_REG_CCCR_INT_EN            (0x04UL)  /*!< Int Enable */
#define SDIO_REG_CCCR_INT_PEND          (0x05UL)  /*!< Int Pending */
#define SDIO_REG_CCCR_IO_ABORT          (0x06UL)  /*!< I/O Abort */
#define SDIO_REG_CCCR_BUS_IF_CTRL       (0x07UL)  /*!< Bus Interface Control */
#define SDIO_REG_CCCR_CARD_CAPABILITY   (0x08UL)  /*!< Card Capability */
#define SDIO_REG_CCCR_CIS_PTR           (0x09UL)  /* common CIS pointer (3 bytes, 0x09~0x0B) */
#define SDIO_REG_CCCR_BUS_SUSPEND       (0x0CUL)  /*!< Bus Suspend */
#define SDIO_REG_CCCR_FUNC_SEL          (0x0DUL)  /*!< Function Select */
#define SDIO_REG_CCCR_EXEC_FLAG         (0x0EUL)  /*!< Exec Flags */
#define SDIO_REG_CCCR_RDY_FLAG          (0x0FUL)  /*!< Ready Flags */
#define SDIO_REG_CCCR_FN0_BLOCK_SIZE    (0x10UL)  /*!< FN0 Block Size (2bytes, 0x10~0x11) */
#define SDIO_REG_CCCR_POWER_CTRL        (0x12UL)  /*!< Power Control */
#define SDIO_REG_CCCR_HIGH_SPEED        (0x13UL)  /*!< High-Speed */
/**
 * @}
 */

/**
 * @defgroup CCCR_Version CCCR Version
 * @{
 */
#define SDIO_CCCR_REV_1_00              (0U)      /* CCCR/FBR Version 1.00 */
#define SDIO_CCCR_REV_1_10              (1U)      /* CCCR/FBR Version 1.10 */
#define SDIO_CCCR_REV_1_20              (2U)      /* CCCR/FBR Version 1.20 */
/**
 * @}
 */

/**
 * @defgroup SDIO_Version CCCR Version
 * @{
 */
#define SDIO_SDIO_REV_1_00              (0U)      /* SDIO Spec Version 1.00 */
#define SDIO_SDIO_REV_1_10              (1U)      /* SDIO Spec Version 1.10 */
#define SDIO_SDIO_REV_1_20              (2U)      /* SDIO Spec Version 1.20(unreleased) */
#define SDIO_SDIO_REV_2_00              (3U)      /* SDIO Spec Version 2.00 */
/**
 * @}
 */

/**
 * @defgroup CCCR_Register_Bus_Interface_Control_Bit CCCR Register Bus_Interface_Control Bits
 * @{
 */
#define SDIO_BUS_IF_CTRL_WIDTH_1BIT     (0x00U)   /*!< Bus Width: 1bit */
#define SDIO_BUS_IF_CTRL_WIDTH_4BIT     (0x02U)   /*!< Bus Width: 2bit */
#define SDIO_BUS_IF_CTRL_ECSI           (0x20U)   /*!< Enable Continuous SPI Interrupt */
#define SDIO_BUS_IF_CTRL_SCSI           (0x40U)   /*!< Support Continuous SPI Interrupt */
/**
 * @}
 */

/**
 * @defgroup CCCR_Register_Card_Capability_Bits CCCR Register Card_Capability Bits
 * @{
 */
#define SDIO_CARD_CAPABILITY_SDC        (0x01U)   /*!< Support Direct Command(CMD52) */
#define SDIO_CARD_CAPABILITY_SMB        (0x02U)   /*!< Support Multiple Block Transfer (CMD53) */
#define SDIO_CARD_CAPABILITY_SRW        (0x04U)   /*!< Support Read-Wait */
#define SDIO_CARD_CAPABILITY_SBS        (0x08U)   /*!< Support Bus Control */
#define SDIO_CARD_CAPABILITY_S4MI       (0x10U)   /*!< Support Block Gap Interrupt */
#define SDIO_CARD_CAPABILITY_E4MI       (0x20U)   /*!< Enable Block Gap Interrupt */
#define SDIO_CARD_CAPABILITY_LSC        (0x40U)   /*!< Low-Speed Card */
#define SDIO_CARD_CAPABILITY_4BLS       (0x80U)   /*!< 4-bit Mode Support for Low-speed Card */
/**
 * @}
 */

/**
 * @defgroup CCCR_Register_Power Control_Bits CCCR Register Power_Control Bits
 * @{
 */
#define SDIO_POWER_CTRL_SMPC            (0x01U)   /*!< Support Master Power Control */
#define SDIO_POWER_CTRL_EMPC            (0x02U)   /*!< Enable Master Power Control */
/**
 * @}
 */

/**
 * @defgroup CCCR_Register_High_Speed_Bits CCCR Register High_Speed Bits
 * @{
 */
#define SDIO_HIGH_SPEED_SHS             (0x01U)   /*!< Support High-Speed mode */
#define SDIO_HIGH_SPEED_EHS             (0x02U)   /*!< Enable High-Speed mode */
/**
 * @}
 */

/*!< Base of function f's FBRs */
#define SDIO_REG_FBR_BASE(f)            ((uint32_t)(f) * 0x100UL)

/*
 * Function_Basic_Registers Function Basic Registers (FBR)
 */
#define SDIO_REG_FBR_STD_FUNC_IF        (0x00UL)  /*!< Standard Function Interface */
#define SDIO_REG_FBR_STD_IF_EXT         (0x01UL)  /*!< Extended Standard Function Interface */
#define SDIO_REG_FBR_POWER              (0x02UL)  /*!< Power Control */
#define SDIO_REG_FBR_CIS                (0x09UL)  /*!< CIS pointer (3 bytes, 0x09~0x0B ) */
#define SDIO_REG_FBR_CSA                (0x0CUL)  /*!< CSA pointer (3 bytes, 0x0C~0x0E) */
#define SDIO_REG_FBR_CSA_DATA           (0x0FUL)  /*!< Data access window to Function Code Storage Area(CSA) */
#define SDIO_REG_FBR_BLOCK_SIZE         (0x10UL)  /*!< I/O block size (2 bytes, 0x10~0x11) */
/**
 * @defgroup FBR_Register_Standard_Function_Interface_Bits FBR Register Standard_Function_Interface Bits
 * @{
 */
#define SDIO_FBR_SUPPORTS_CSA           (0x40U)   /* supports Code Storage Area */
#define SDIO_FBR_ENABLE_CSA             (0x80U)   /* enable Code Storage Area */
/**
 * @}
 */

/**
 * @defgroup FBR_Register_Power_Control_Bits FBR Register Power_Control Bits
 * @{
 */
#define SDIO_FBR_POWER_SPS              (0x01U)   /* Supports Power Selection */
#define SDIO_FBR_POWER_EPS              (0x02U)   /* Enable (low) Power Selection */
/**
 * @}
 */

/**
 * @defgroup SDIO_CIS_Tuple_Code SDIO CIS Tuple Code
 * @{
 */
#define CISTPL_NULL                     (0x00U)
#define CISTPL_CHECKSUM                 (0x10U)
#define CISTPL_VERS_1                   (0x15U)
#define CISTPL_ALTSTR                   (0x16U)
#define CISTPL_MANFID                   (0x20U)
#define CISTPL_FUNCID                   (0x21U)
#define CISTPL_FUNCE                    (0x22U)
#define CISTPL_SDIO_STD                 (0x91U)
#define CISTPL_SDIO_EXT                 (0x92U)
#define CISTPL_END                      (0xFFU)
/**
 * @}
 */

/**
 * @defgroup SDIO_Device_ID SDIO Device ID
 * @{
 */
#define SDIO_ANY_FUNC_ID                (0xFFU)
#define SDIO_ANY_MAN_ID                 (0xFFFFU)
#define SDIO_ANY_PROD_ID                (0xFFFFU)
/**
 * @}
 */

/**
 * @defgroup SDIO_Max_Functions SDIO Max Functions
 * @{
 */
#define SDIO_MAX_FUNCTIONS              (7U)
/**
 * @}
 */

/**
 * @defgroup SDIO_Transfer_Context SDIO Transfer Context
 * @{
 */
#define SDIO_CONTEXT_NONE               (0x00UL)  /*!< None                             */
#define SDIO_CONTEXT_RD_SINGLE_BLOCK    (0x01UL)  /*!< Read single block operation      */
#define SDIO_CONTEXT_RD_MULTI_BLOCK     (0x02UL)  /*!< Read multiple blocks operation   */
#define SDIO_CONTEXT_WR_SINGLE_BLOCK    (0x10UL)  /*!< Write single block operation     */
#define SDIO_CONTEXT_WR_MULTI_BLOCK     (0x20UL)  /*!< Write multiple blocks operation  */
#define SDIO_CONTEXT_INT                (0x40UL)  /*!< Process in Interrupt mode        */
#define SDIO_CONTEXT_DMA                (0x80UL)  /*!< Process in DMA mode              */
/**
 * @}
 */

/**
 * @}
 */
/*******************************************************************************
 * Global variable definitions ('extern')
 ******************************************************************************/

/*******************************************************************************
  Global function prototypes (definition in C source)
 ******************************************************************************/
/**
 * @addtogroup SDIO_Global_Functions
 * @{
 */
int32_t SDIO_Init(stc_sdio_host_t *pstcHost);
void SDIO_IRQHandler(stc_sdio_host_t *pstcHost);
void SDIO_TxCompleteCallback(stc_sdio_host_t *pstcHost);
void SDIO_RxCompleteCallback(stc_sdio_host_t *pstcHost);
void SDIO_ErrorCallback(stc_sdio_host_t *pstcHost);
void SDIO_DmaTransConfig(const stc_sdio_host_t *pstcHost, uint8_t u8Ch, uint32_t u32SrcAddr,
                         uint32_t u32DestAddr, uint16_t u16BlockSize, uint16_t u16TransCount);

int32_t SDIO_SetHighSpeed(stc_sdio_card_t *pstcCard);
int32_t SDIO_SetBusWidth(stc_sdio_card_t *pstcCard);

int32_t SDIO_SetBlockSize(stc_sdio_func_t *pstcFunc, uint16_t u16BlockSize);
int32_t SDIO_IOReadByte(stc_sdio_func_t *pstcFunc, uint32_t u32Addr, uint8_t *pu8Data);
int32_t SDIO_IOWriteByte(stc_sdio_func_t *pstcFunc, uint32_t u32Addr, uint8_t u8Data);
int32_t SDIO_IORwExtended(stc_sdio_func_t *pstcFunc,
                          uint32_t u32RwFlag,
                          uint32_t u32Addr,
                          uint32_t u32OperateCode,
                          uint8_t  *pu8Data,
                          uint16_t u16BlockCount,
                          uint16_t u16BlockSize);
int32_t SDIO_IORwExtended_INT(stc_sdio_func_t *pstcFunc,
                              uint32_t u32RwFlag,
                              uint32_t u32Addr,
                              uint32_t u32OperateCode,
                              uint8_t  *pu8Data,
                              uint16_t u16BlockCount,
                              uint16_t u16BlockSize);
int32_t SDIO_IORwExtended_DMA(stc_sdio_func_t *pstcFunc,
                              uint32_t u32RwFlag,
                              uint32_t u32Addr,
                              uint32_t u32OperateCode,
                              uint8_t  *pu8Data,
                              uint16_t u16BlockCount,
                              uint16_t u16BlockSize);
/**
 * @}
 */

/**
 * @}
 */

/**
 * @}
 */

#ifdef __cplusplus
}
#endif

#endif /* __SDIO_H__ */

/*******************************************************************************
 * EOF (not truncated)
 ******************************************************************************/
