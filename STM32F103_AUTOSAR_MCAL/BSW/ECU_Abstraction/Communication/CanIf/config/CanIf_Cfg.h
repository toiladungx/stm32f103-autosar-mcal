/**********************************************************
 * @file    CanIf_Cfg.h
 * @brief   Cấu hình ánh xạ TX/RX PDU cho CAN Interface (CanIf)
 **********************************************************/
#ifndef CANIF_CFG_H
#define CANIF_CFG_H

#include "Std_Types.h"
#include "ComStack_Types.h"
#include "Can_GeneralTypes.h"

/* ===========================================================
 * Số lượng PDU cấu hình cho chiều truyền (TX) và chiều nhận (RX)
 * ===========================================================*/
#define CANIF_NUM_TX_PDUS   (6u)
#define CANIF_NUM_RX_PDUS   (6u)

/* ===========================================================
 * Định danh tầng nhận callback ở phía trên (Upper Layer Target)
 * ===========================================================*/
typedef enum
{
    CANIF_USER_PDUR = 0u,   /* Chuyển tiếp lên PduR (COM signals) */
    CANIF_USER_CANTP        /* Chuyển tiếp lên CanTp (Diagnostics / ISO-TP) */
} CanIf_UserType;

/* ===========================================================
 * Danh sách ID các TX PDU (Dùng khi tầng trên gọi CanIf_Transmit)
 * ===========================================================*/
enum
{
    CanIfConf_Pdu_EngineCmd    = 0u,
    CanIfConf_Pdu_BrakeCmd     = 1u,
    CanIfConf_Pdu_BodyCmd      = 2u,
    CanIfConf_Pdu_DiagTx       = 3u,
    CanIfConf_Pdu_DiagRx_FC    = 4u,
    CanIfConf_Pdu_EngineStatus = 5u
};

/* ===========================================================
 * Danh sách ID các RX PDU (Dùng khi CanIf báo nhận lên tầng trên)
 * ===========================================================*/
enum
{
    CanIfConf_RxPdu_EngineCmd    = 0u,
    CanIfConf_RxPdu_BrakeCmd     = 1u,
    CanIfConf_RxPdu_BodyCmd      = 2u,
    CanIfConf_RxPdu_DiagRx       = 3u,
    CanIfConf_RxPdu_DiagTx_FC    = 4u,
    CanIfConf_RxPdu_EngineStatus = 5u
};

/* ===========================================================
 * Cấu trúc bảng cấu hình 1 kênh truyền (TX PDU Config)
 * ===========================================================*/
typedef struct
{
    PduIdType        CanIfTxPduId;  /* Mã định danh TX PDU nội bộ CanIf */
    PduIdType        UpperTxPduId;  /* Mã định danh PDU gốc của PduR/CanTp */
    Can_HwHandleType Hth;           /* Hardware Transmit Handle (Slot TX 0..5) */
    Can_IdType       CanId;         /* CAN ID vật lý trên bus */
    uint8            DlcMax;        /* Độ dài dữ liệu tối đa (0..8 byte) */
    CanIf_UserType   UserType;      /* Tầng nhận TxConfirmation (PduR hay CanTp) */
} CanIf_TxPduCfgType;

/* ===========================================================
 * Cấu trúc bảng cấu hình 1 kênh nhận (RX PDU Config)
 * ===========================================================*/
typedef struct
{
    PduIdType        CanIfRxPduId;  /* Mã định danh RX PDU nội bộ CanIf */
    PduIdType        UpperRxPduId;  /* Mã định danh PDU báo lên PduR/CanTp */
    Can_IdType       CanId;         /* CAN ID cần đối chiếu khi nhận frame */
    uint8            DlcMin;        /* Độ dài dữ liệu tối thiểu hợp lệ (0..8 byte) */
    CanIf_UserType   UserType;      /* Tầng nhận RxIndication (PduR hay CanTp) */
} CanIf_RxPduCfgType;

/* ===========================================================
 * Khai báo extern các bảng cấu hình (Định nghĩa trong CanIf_PBcfg.c)
 * ===========================================================*/
extern const CanIf_TxPduCfgType CanIf_TxPduCfg[CANIF_NUM_TX_PDUS];
extern const CanIf_RxPduCfgType CanIf_RxPduCfg[CANIF_NUM_RX_PDUS];
extern boolean CanIf_Initialized;

#endif /* CANIF_CFG_H */
