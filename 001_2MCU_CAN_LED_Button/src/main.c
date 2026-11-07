#include "stm32f10x.h"
#include "stm32f10x_can.h"
#include "stm32f10x_gpio.h"
#include "stm32f10x_rcc.h"

CanTxMsg TX_Message;
CanRxMsg RX_Message;

//#define NODE1

void delay(uint32_t ms)
{
    for(uint32_t i = 0; i < ms * 4000; i ++)
    {
        __NOP();
    }
}

#ifdef NODE1

void CAN_Send_Init(void)
{
    GPIO_InitTypeDef GPIO_InitStruct;
    CAN_InitTypeDef CAN_InitStruct;

    RCC_APB1PeriphClockCmd(RCC_APB1Periph_CAN1, ENABLE);
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);

    // CANTX
    GPIO_InitStruct.GPIO_Pin = GPIO_Pin_12;
    GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOA, &GPIO_InitStruct);

    // CANRX
    GPIO_InitStruct.GPIO_Pin = GPIO_Pin_11;
    GPIO_InitStruct.GPIO_Mode = GPIO_Mode_IPU;
    GPIO_Init(GPIOA, &GPIO_InitStruct);

    CAN_DeInit(CAN1);

    CAN_InitStruct.CAN_TTCM = DISABLE;
    CAN_InitStruct.CAN_ABOM = DISABLE;
    CAN_InitStruct.CAN_AWUM = DISABLE;
    CAN_InitStruct.CAN_NART = DISABLE;
    CAN_InitStruct.CAN_RFLM = DISABLE;
    CAN_InitStruct.CAN_TXFP = DISABLE;
    CAN_InitStruct.CAN_Mode = CAN_Mode_Normal;

    CAN_InitStruct.CAN_SJW = CAN_SJW_1tq;
    CAN_InitStruct.CAN_BS1 = CAN_BS1_12tq;
    CAN_InitStruct.CAN_BS2 = CAN_BS2_5tq;
    CAN_InitStruct.CAN_Prescaler = 4;
    
    CAN_Init(CAN1, &CAN_InitStruct);
}

void CAN_ButtonInit(void)
{
    GPIO_InitTypeDef GPIO_InitStruct;

    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);

    GPIO_InitStruct.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1 | GPIO_Pin_10 | GPIO_Pin_11;
    GPIO_InitStruct.GPIO_Mode = GPIO_Mode_IPU;
    GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOB, &GPIO_InitStruct);
}

#else

void CAN_Receive_Init(void)
{
    GPIO_InitTypeDef GPIO_InitStruct;
    CAN_InitTypeDef CAN_InitStruct;
    CAN_FilterInitTypeDef CAN_FilterInitStruct;

    RCC_APB1PeriphClockCmd(RCC_APB1Periph_CAN1, ENABLE);
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);

    // CANTX
    GPIO_InitStruct.GPIO_Pin = GPIO_Pin_12;
    GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOA, &GPIO_InitStruct);
    
    // CANRX
    GPIO_InitStruct.GPIO_Pin = GPIO_Pin_11;
    GPIO_InitStruct.GPIO_Mode = GPIO_Mode_IPU;
    GPIO_Init(GPIOA, &GPIO_InitStruct);

    CAN_DeInit(CAN1);

    CAN_InitStruct.CAN_TTCM = DISABLE;
    CAN_InitStruct.CAN_ABOM = DISABLE;
    CAN_InitStruct.CAN_AWUM = DISABLE;
    CAN_InitStruct.CAN_NART = DISABLE;
    CAN_InitStruct.CAN_RFLM = DISABLE;
    CAN_InitStruct.CAN_TXFP = DISABLE;
    CAN_InitStruct.CAN_Mode = CAN_Mode_Normal;

    CAN_InitStruct.CAN_SJW = CAN_SJW_1tq;
    CAN_InitStruct.CAN_BS1 = CAN_BS1_12tq;
    CAN_InitStruct.CAN_BS2 = CAN_BS2_5tq;
    CAN_InitStruct.CAN_Prescaler = 4;

    CAN_Init(CAN1, &CAN_InitStruct);

    CAN_FilterInitStruct.CAN_FilterNumber = 0;
    CAN_FilterInitStruct.CAN_FilterMode = CAN_FilterMode_IdMask;
    CAN_FilterInitStruct.CAN_FilterScale = CAN_FilterScale_32bit;
    CAN_FilterInitStruct.CAN_FilterIdHigh = 0x0000;
    CAN_FilterInitStruct.CAN_FilterIdLow = 0x0000;
    CAN_FilterInitStruct.CAN_FilterMaskIdHigh = 0x0000;
    CAN_FilterInitStruct.CAN_FilterMaskIdLow = 0x0000;
    CAN_FilterInitStruct.CAN_FilterFIFOAssignment = CAN_Filter_FIFO0;
    CAN_FilterInitStruct.CAN_FilterActivation = ENABLE;
    CAN_FilterInit(&CAN_FilterInitStruct);
}

void CAN_LEDInit(void)
{
    GPIO_InitTypeDef GPIO_InitStruct;

    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);

    GPIO_InitStruct.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_5;
    GPIO_InitStruct.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;

    GPIO_Init(GPIOA, &GPIO_InitStruct);

    GPIO_ResetBits(GPIOA, GPIO_Pin_0 | GPIO_Pin_5);
}

#endif


int main(void)
{

#ifdef NODE1
    CAN_Send_Init();
    CAN_ButtonInit();

    TX_Message.IDE = CAN_Id_Standard;
    TX_Message.RTR = CAN_RTR_Data;
    TX_Message.DLC = 1;

    while(1)
    {
        // Gửi msg Bật đèn
        if(GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_0) == Bit_RESET)
        {
            TX_Message.StdId = 0x123;
            TX_Message.Data[0] = 1;
            CAN_Transmit(CAN1, &TX_Message);
            while(GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_0) == Bit_RESET);
        }

        if(GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_10) == Bit_RESET)
        {
            TX_Message.StdId = 0x123;
            TX_Message.Data[0] = 2;
            CAN_Transmit(CAN1, &TX_Message);
            while(GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_10) == Bit_RESET);
        }

        // Gửi msg Tắt đèn 
        if(GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_1) == Bit_RESET)
        {
            TX_Message.StdId = 0x456;
            TX_Message.Data[0] = 1;
            CAN_Transmit(CAN1, &TX_Message);
            while(GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_1) == Bit_RESET);
        }

        if(GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_11) == Bit_RESET)
        {
            TX_Message.StdId = 0x456;
            TX_Message.Data[0] = 2;
            CAN_Transmit(CAN1, &TX_Message);
            while(GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_11) == Bit_RESET);
        }
    }

#else
    CAN_Receive_Init();
    CAN_LEDInit();

    while(1)
    {
        if(CAN_MessagePending(CAN1, CAN_FIFO0) > 0)
        {
            CAN_Receive(CAN1, CAN_FIFO0, &RX_Message);
            if(RX_Message.StdId == 0x123)
            {
                if(RX_Message.Data[0] == 1)
                {
                    GPIO_SetBits(GPIOA, GPIO_Pin_5);
                }else if(RX_Message.Data[0] == 2)
                {
                    GPIO_SetBits(GPIOA, GPIO_Pin_0);
                }
            }else if(RX_Message.StdId == 0x456)
            {
                if(RX_Message.Data[0] == 1)
                {
                    GPIO_ResetBits(GPIOA, GPIO_Pin_5);
                }else if(RX_Message.Data[0] == 2)
                {
                    GPIO_ResetBits(GPIOA, GPIO_Pin_0);
                }
            }
        }
    }
#endif
}