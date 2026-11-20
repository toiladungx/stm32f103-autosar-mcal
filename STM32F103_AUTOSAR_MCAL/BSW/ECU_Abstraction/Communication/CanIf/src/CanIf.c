/*************************************************************
 * @file    CanIf.c
 * @brief   AUTOSAR CAN Interface (CanIf) – Lớp trừu tượng giao tiếp CAN 
 *
 * @details Tầng ECU Abstraction kết nối PduR (phía trên) với
 *          CAN Driver (phía dưới). Chịu trách nhiệm:
 *
 *          1. Chuyển đổi dữ liệu:
 *             PduInfoType (AUTOSAR chung) → Can_PduType (CAN riêng)
 *
 *          2. Tra bảng cấu hình:
 *             CanIf_TxPduCfg[] map CanIfTxPduId → CAN ID + HTH + DLC
 *
 *          3. Callback ngược:
 *             CAN Driver báo TX xong → CanIf → PduR → COM
 *
 *          Luồng TX chi tiết:
 *          ┌────────┐  CanIf_Transmit(id, &info)   ┌─────────┐
 *          │  PduR  │ ──────────────────────────►  │  CanIf  │
 *          └────────┘                              └────┬────┘
 *                                                       │ Can_Write(hth, &pdu)
 *                                                 ┌─────▼──────┐
 *                                                 │ CAN Driver │
 *                                                 │  (bxCAN)   │
 *                                                 └────────────┘
 *
 * @version 1.0
 * @date    18-11-2026
 * @author  Nguyen Duc Dung
 *************************************************************/
#include <stddef.h>
#include "CanIf.h"
#include "CanIf_Cfg.h"
#include "Can.h"

/* ===========================================================
 * Biến trạng thái nội bộ
 * ===========================================================*/
boolean CanIf_Initialized = FALSE;

/* Mảng lưu vết TxPduId tương ứng với từng Hth để báo TxConfirmation */
static PduIdType s_txPduInFlight[CANIF_NUM_TX_PDUS];

/* ===========================================================
 * CanIf_Init – Khởi tạo module CAN Interface
 * ===========================================================*/
void CanIf_Init(void)
{
    /* 1. Xóa sạch bảng theo dõi các PDU đang truyền */
    for (uint8 i = 0u; i < CANIF_NUM_TX_PDUS; i++)
    {
        s_txPduInFlight[i] = (PduIdType)0u;
    }

    /* 2. Đánh dấu CanIf đã sẵn sàng hoạt động */
    CanIf_Initialized = TRUE;
}

