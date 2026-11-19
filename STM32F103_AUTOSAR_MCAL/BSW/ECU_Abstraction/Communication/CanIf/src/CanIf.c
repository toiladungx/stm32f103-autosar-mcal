/**********************************************************
 * @file    CanIf.c
 * @brief   AUTOSAR CAN Interface (CanIf) – Triển khai TX path
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
 **********************************************************/
