/**********************************************************
 * @file    Can.c
 * @brief   AUTOSAR CAN Driver – bxCAN nội bộ STM32F103 (SPL)
 *
 * @details Module MCAL điều khiển bộ CAN tích hợp (bxCAN) trong
 *          STM32F103 thông qua thư viện SPL (Standard Peripheral
 *          Library).
 *
 *          ╔═══════════════════════════════════════════════╗
 *          ║           KIẾN TRÚC PHẦN CỨNG                 ║
 *          ╠═══════════════════════════════════════════════╣
 *          ║                                               ║
 *          ║  STM32F103                                    ║
 *          ║  ┌─────────────────────────────────────┐      ║
 *          ║  │  APB1 Bus (36 MHz)                  │      ║
 *          ║  │  ┌──────────────────┐               │      ║
 *          ║  │  │  bxCAN (CAN1)    │               │      ║
 *          ║  │  │  @ 0x40006400    │               │      ║
 *          ║  │  │                  │               │      ║
 *          ║  │  │  ┌─Mailbox TX─┐  │  PA12 ──► TX  │      ║
 *          ║  │  │  │ MB0 MB1 MB2│  │  PA11 ◄── RX  │      ║
 *          ║  │  │  └────────────┘  │               │      ║
 *          ║  │  │  ┌─Filter────┐   │               │      ║
 *          ║  │  │  │ 14 banks  │   │               │      ║
 *          ║  │  │  └───────────┘   │               │      ║
 *          ║  │  │  ┌─FIFO RX───┐   │               │      ║
 *          ║  │  │  │ FIFO0/1   │   │               │      ║
 *          ║  │  │  └───────────┘   │               │      ║
 *          ║  │  └──────────────────┘               │      ║
 *          ║  └─────────────────────────────────────┘      ║
 *          ╚═══════════════════════════════════════════════╝
 *
 *          Luồng truyền (TX):
 *            1. CanIf gọi Can_Write(HTH, &pdu)
 *            2. Can_Write() chuyển Can_PduType → CanTxMsg (SPL)
 *            3. Gọi CAN_Transmit(CAN1, &txMsg) → nạp vào mailbox
 *            4. bxCAN tự động truyền lên CAN bus
 *            5. Can_MainFunction_Write() polling kiểm tra hoàn tất
 *            6. Khi xong → gọi CanIf_TxConfirmation() báo lên trên
 *
 *          Cấu hình bit timing (500 kbps):
 *            APB1 = 36 MHz
 *            Prescaler = 4 → Time Quantum = 4/36MHz ≈ 111ns
 *            1 bit = (1 + BS1 + BS2) TQ = (1 + 11 + 6) × 111ns = 2µs
 *            Bit rate = 1/2µs = 500 kbps
 *
 * @note    Dùng CAN_Mode_LoopBack cho test trên Renode.
 *          Khi chạy trên phần cứng thật, đổi sang CAN_Mode_Normal.
 *
 * @version 1.0
 * @date    15-11-2026
 * @author  Nguyen Duc Dung
 **********************************************************/

#include "Can.h"
#include "CanIf.h"             // CanIf_TxConfirmation() – callback lên tầng trên
#include "stm32f10x_rcc.h"
#include "stm32f10x_gpio.h"
#include "stm32f10x_can.h"
#include "misc.h"              // NVIC configuration
#include <stddef.h>


/* ========================================================
 * Định nghĩa phần cứng và hằng số cấu hình
 * --------------------------------------------------------
 * CAN_PERIPH    : Peripheral bxCAN sử dụng (CAN1)
 * CAN_GPIO_PORT : Port GPIO cho chân CAN (GPIOA)
 * CAN_RX_PIN    : PA11 – Chân nhận CAN
 * CAN_TX_PIN    : PA12 – Chân truyền CAN
 * CAN_TX_SLOTS   : Số slot TX (bxCAN có 3 mailbox)
 * ========================================================*/
#define CAN_PERIPH        CAN1
#define CAN_GPIO_PORT     GPIOA
#define CAN_RX_PIN        GPIO_Pin_11
#define CAN_TX_PIN        GPIO_Pin_12

#define CAN_TX_SLOTS      6u


/* ========================================================
 * Biến trạng thái nội bộ
 * --------------------------------------------------------
 * s_inited       : Cờ đánh dấu đã khởi tạo chưa
 * s_swPduHandle  : Lưu swPduHandle cho mỗi slot (để callback)
 * s_txPending    : Cờ đánh dấu slot đang chờ TX hoàn tất
 * s_txMailbox    : Mailbox ID do CAN_Transmit() trả về
 * ========================================================*/
static boolean            s_inited = FALSE;
static volatile PduIdType s_swPduHandle[CAN_TX_SLOTS];
static volatile boolean   s_txPending[CAN_TX_SLOTS];
static volatile uint8     s_txMailbox[CAN_TX_SLOTS];



/* =========================================================
 * Can_Init – Khởi tạo toàn bộ CAN Driver & Phần cứng bxCAN
 * =========================================================*/
void Can_Init(void)
{
    GPIO_InitTypeDef       gpioInit;
    CAN_InitTypeDef        canInit;
    CAN_FilterInitTypeDef  filterInit;
    NVIC_InitTypeDef       nvicInit;

    //1. Bật clock cho peripheral cần thiết
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE); 
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_CAN1, ENABLE);

    //2. Cấu hình PA12 (CAN_TX)
    GPIO_StructInit(&gpioInit);
    gpioInit.GPIO_Pin   = CAN_TX_PIN;
    gpioInit.GPIO_Speed = GPIO_Speed_50MHz;
    gpioInit.GPIO_Mode  = GPIO_Mode_AF_PP;
    GPIO_Init(CAN_GPIO_PORT, &gpioInit);

    //3. Cấu hình PA11 (CAN_RX)
    gpioInit.GPIO_Pin  = CAN_RX_PIN;
    gpioInit.GPIO_Mode = GPIO_Mode_IN_FLOATING;
    GPIO_Init(CAN_GPIO_PORT, &gpioInit);

    //4. Reset và cấu hình thông số cho bxCAN (500kbps)
    CAN_DeInit(CAN_PERIPH);
    CAN_StructInit(&canInit);

    canInit.CAN_Prescaler = 4u;             // Chia clock: TQ = 4/36MHz
    canInit.CAN_SJW       = CAN_SJW_1tq;    // Synchronization Jump Width: 1tq
    canInit.CAN_BS1       = CAN_BS1_11tq;   // Bit Segment 1: 11tq
    canInit.CAN_BS2       = CAN_BS2_6tq;    // Bit Segment 2: 6tq
    canInit.CAN_Mode      = CAN_Mode_Normal;

    canInit.CAN_TTCM      = DISABLE;        // Time Triggered: không
    canInit.CAN_ABOM      = ENABLE;         // Auto Bus-Off Management: tự phục hồi
    canInit.CAN_AWUM      = DISABLE;        // Auto Wake-Up: không
    canInit.CAN_NART      = DISABLE;        // Auto Retransmission: BẬT (mặc định)
    canInit.CAN_RFLM      = DISABLE;        // Receive FIFO Locked: không khóa  
    canInit.CAN_TXFP      = DISABLE;        // TX FIFO Priority: theo ID (mặc định)
    CAN_Init(CAN_PERIPH, &canInit);

    //5. Cấu hình filter (Filter 0 chấp nhận tất cả mọi CAN ID vào FIFO0)
    filterInit.CAN_FilterNumber         = 0;
    filterInit.CAN_FilterMode           = CAN_FilterMode_IdMask;
    filterInit.CAN_FilterScale          = CAN_FilterScale_32bit;
    filterInit.CAN_FilterIdHigh         = 0x0000;
    filterInit.CAN_FilterIdLow          = 0x0000;
    filterInit.CAN_FilterMaskIdHigh     = 0x0000;
    filterInit.CAN_FilterMaskIdLow      = 0x0000;
    filterInit.CAN_FilterFIFOAssignment = CAN_Filter_FIFO0;
    filterInit.CAN_FilterActivation     = ENABLE;
    CAN_FilterInit(&filterInit);

    //6. Xóa trạng thái các slot TX phần mềm
    for(uint8 i = 0; i < CAN_TX_SLOTS; i++)
    {
        s_swPduHandle[i] = 0u;
        s_txPending[i]   = FALSE;
        s_txMailbox[i]   = 0u;
    }

    //7. Bật ngắt nhận FIFO0 và cấu hình NVIC
    CAN_ITConfig(CAN_PERIPH, CAN_IT_FMP0, ENABLE);

    nvicInit.NVIC_IRQChannel                   = USB_LP_CAN1_RX0_IRQn;
    nvicInit.NVIC_IRQChannelPreemptionPriority = 1;
    nvicInit.NVIC_IRQChannelSubPriority        = 0;
    nvicInit.NVIC_IRQChannelCmd                = ENABLE;
    NVIC_Init(&nvicInit);

    //8. Đánh dấu driver đã sẵn sàng
    s_inited = TRUE;
}




/* =========================================================
 * Can_Write – API: Gửi CAN PDU
 * ---------------------------------------------------------
 * Luồng xử lý:
 *   1. Kiểm tra điều kiện tiên quyết (init, param không NULL)
 *   2. Kiểm tra slot TX tương ứng có rảnh không
 *   3. Chuyển đổi Can_PduType (AUTOSAR) -> CanTxMsg (SPL)
 *   4. Gọi CAN_Transmit() của SPL để nạp vào mailbox
 *   5. Lưu thông tin để callback khi TX hoàn tất
 *
 * Mapping kiểu dữ liệu:
 *   ┌───────────────────────┬─────────────────────────┐
 *   │ Can_PduType (AUTOSAR) │     CanTxMsg (SPL)      │
 *   ├───────────────────────┼─────────────────────────┤
 *   │ id ≤ 0x7FF            │ StdId + CAN_Id_Standard │
 *   │ id > 0x7FF            │ ExtId + CAN_Id_Extended │
 *   │ length                │ DLC                     │
 *   │ sdu[i]                │ Data[i]                 │
 *   └───────────────────────┴─────────────────────────┘
 * =========================================================*/
Can_ReturnType Can_Write(Can_HwHandleType Hth, const Can_PduType* PduInfo)
{
    //1. Kiểm tra điều kiện
    if(!s_inited || (PduInfo == NULL) || (PduInfo->sdu == NULL))
    {
        return CAN_NOT_OK;
    }

    //2. Giới hạn chỉ số slot 
}