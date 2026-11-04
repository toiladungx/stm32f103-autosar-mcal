#include "stm32f10x.h"
#include "stm32f10x_gpio.h"
#include "stm32f10x_rcc.h"
#include "stm32f10x_can.h"

static uint8_t g_rx_can_data[8];
static uint8_t g_rx_can_len = 0;
static uint32_t g_rx_can_id = 0;

void Delay(volatile uint32_t nCount)
{
    for (; nCount != 0; nCount--);
}

void CAN1_Init(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;
    CAN_InitTypeDef CAN_InitStructure;
    CAN_FilterInitTypeDef CAN_FilterInitStructure;

    RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE);
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_CAN1, ENABLE);

    /* CAN1 TX/RX on PA11/PA12 */
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11 | GPIO_Pin_12;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOA, &GPIO_InitStructure);

    CAN_DeInit(CAN1);

    CAN_InitStructure.CAN_TTCM = DISABLE;
    CAN_InitStructure.CAN_ABOM = DISABLE;
    CAN_InitStructure.CAN_AWUM = DISABLE;
    CAN_InitStructure.CAN_NART = DISABLE;
    CAN_InitStructure.CAN_RFLM = DISABLE;
    CAN_InitStructure.CAN_TXFP = DISABLE;

    CAN_InitStructure.CAN_Mode = CAN_Mode_LoopBack;
    CAN_InitStructure.CAN_SJW = CAN_SJW_1tq;
    CAN_InitStructure.CAN_BS1 = CAN_BS1_6tq;
    CAN_InitStructure.CAN_BS2 = CAN_BS2_7tq;
    CAN_InitStructure.CAN_Prescaler = 6;

    CAN_Init(CAN1, &CAN_InitStructure);

    /* Accept all frames */
    CAN_FilterInitStructure.CAN_FilterNumber = 0;
    CAN_FilterInitStructure.CAN_FilterMode = CAN_FilterMode_IdMask;
    CAN_FilterInitStructure.CAN_FilterScale = CAN_FilterScale_32bit;
    CAN_FilterInitStructure.CAN_FilterIdHigh = 0x0000;
    CAN_FilterInitStructure.CAN_FilterIdLow = 0x0000;
    CAN_FilterInitStructure.CAN_FilterMaskIdHigh = 0x0000;
    CAN_FilterInitStructure.CAN_FilterMaskIdLow = 0x0000;
    CAN_FilterInitStructure.CAN_FilterFIFOAssignment = CAN_FIFO0;
    CAN_FilterInitStructure.CAN_FilterActivation = ENABLE;
    CAN_FilterInit(&CAN_FilterInitStructure);
}

void CAN1_SendFrame(uint32_t id, uint8_t *data, uint8_t len)
{
    CanTxMsg TxMessage;
    uint8_t txMailbox;

    TxMessage.StdId = id;
    TxMessage.ExtId = 0;
    TxMessage.IDE = CAN_ID_STD;
    TxMessage.RTR = CAN_RTR_Data;
    TxMessage.DLC = len;

    for (uint8_t i = 0; i < len; i++)
    {
        TxMessage.Data[i] = data[i];
    }

    txMailbox = CAN_Transmit(CAN1, &TxMessage);

    while (CAN_TransmitStatus(CAN1, txMailbox) == CAN_TxStatus_Pending)
    {
        /* wait */
    }
}

void CAN1_ReceiveLoopback(void)
{
    CanRxMsg RxMessage;

    if (CAN_MessagePending(CAN1, CAN_FIFO0) != 0)
    {
        CAN_Receive(CAN1, CAN_FIFO0, &RxMessage);

        if (RxMessage.IDE == CAN_ID_STD)
        {
            g_rx_can_id = RxMessage.StdId;
            g_rx_can_len = RxMessage.DLC;

            for (uint8_t i = 0; i < g_rx_can_len; i++)
            {
                g_rx_can_data[i] = RxMessage.Data[i];
            }

            /* Nếu data dài hơn 8 thì cắt bớt; DLC chuẩn là <= 8 */
            for (uint8_t i = g_rx_can_len; i < 8; i++)
            {
                g_rx_can_data[i] = 0;
            }
        }
    }
}

int main(void)
{
    uint8_t canData[8];

    /* LED on PC13 */
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);

    GPIO_InitTypeDef GPIO_InitStructure;
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_13;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOC, &GPIO_InitStructure);

    CAN1_Init();

    canData[0] = 0x11;
    canData[1] = 0x22;
    canData[2] = 0x33;
    canData[3] = 0x44;
    canData[4] = 0x55;
    canData[5] = 0x66;
    canData[6] = 0x77;
    canData[7] = 0x88;

    while (1)
    {
        GPIO_SetBits(GPIOC, GPIO_Pin_13);
        CAN1_SendFrame(0x123, canData, 8);
        CAN1_ReceiveLoopback();

        if (g_rx_can_len != 0)
        {
            /* Có frame nhận về, mảng chứa data ở g_rx_can_data */
            /* ví dụ: g_rx_can_data[0] ... g_rx_can_data[7] */
        }

        Delay(2000000);

        GPIO_ResetBits(GPIOC, GPIO_Pin_13);
        Delay(2000000);
    }
}