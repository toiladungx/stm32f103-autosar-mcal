#include "stm32f10x.h"
#include "stm32f10x_gpio.h"
#include "stm32f10x_rcc.h"
#include "stm32f10x_can.h"

#define NODE_1

CanTxMsg TX_Message;
CanRxMsg RX_Message;

#ifdef NODE_1

void CAN1_Init(void)
{
    GPIO_InitTypeDef GPIO_InitStruct;
    CAN_InitTypeDef CAN_InitStruct;
    CAN_FilterInitTypeDef CAN_FilterInitStruct;

    // Bật RCC
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_CAN1, ENABLE);
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);

    // CAN1 RX
    GPIO_InitStruct.GPIO_Pin = GPIO_Pin_11;
    GPIO_InitStruct.GPIO_Mode = GPIO_Mode_IPU;
    GPIO_Init(GPIOA, &GPIO_InitStruct);

    // CAN1 TX
    GPIO_InitStruct.GPIO_Pin = GPIO_Pin_12;
    GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOA, &GPIO_InitStruct);

    CAN_DeInit(CAN1);
    CAN_StructInit(&CAN_InitStruct);

    CAN_InitStruct.CAN_TTCM = DISABLE;
    CAN_InitStruct.CAN_ABOM = DISABLE;
    CAN_InitStruct.CAN_AWUM = DISABLE;
    CAN_InitStruct.CAN_NART = DISABLE;
    CAN_InitStruct.CAN_RFLM = DISABLE;
    CAN_InitStruct.CAN_TXFP = DISABLE;

    // Đặt mode Normal
    CAN_InitStruct.CAN_Mode = CAN_Mode_Normal;

    // Cấu hình Baudrate
    CAN_InitStruct.CAN_SJW = CAN_SJW_1tq;
    CAN_InitStruct.CAN_BS1 = CAN_BS1_6tq;
    CAN_InitStruct.CAN_BS2 = CAN_BS2_7tq;
    CAN_InitStruct.CAN_Prescaler = 6;
    CAN_Init(CAN1, &CAN_InitStruct);

    // CAN1 Filter - Accept all frames
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

void CAN1_Send_LED_Msg(uint8_t ledNum)
{
    TX_Message.StdId = 0x01;
    TX_Message.IDE = CAN_Id_Standard;
    TX_Message.RTR = CAN_RTR_Data;
    TX_Message.DLC = 1;
    TX_Message.Data[0] = ledNum;
    
    CAN_Transmit(CAN1, &TX_Message);
}

void CAN1_Request_Msg(void)
{
    TX_Message.StdId = 0x02;
    TX_Message.IDE = CAN_Id_Standard;
    TX_Message.RTR = CAN_RTR_Remote;
    TX_Message.DLC = 2;

    CAN_Transmit(CAN1, &TX_Message);
}


#else

void LED_Init(void)
{
    GPIO_InitTypeDef GPIO_InitStruct;

    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);

    GPIO_InitStruct.GPIO_Pin = GPIO_Pin_0;
    GPIO_InitStruct.GPIO_Pin = GPIO_Pin_1;
    GPIO_InitStruct.GPIO_Pin = GPIO_Pin_2;
    GPIO_InitStruct.GPIO_Pin = GPIO_Pin_3;
    GPIO_InitStruct.GPIO_Mode = GPIO_Mode_Out_PP; 
    GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOA, &GPIO_InitStruct);
    
    // Tắt tất cả các LED ở trạng thái ban đầu
    GPIO_ResetBits(GPIOA, GPIO_Pin_0);
    GPIO_ResetBits(GPIOA, GPIO_Pin_1);
    GPIO_ResetBits(GPIOA, GPIO_Pin_2);
    GPIO_ResetBits(GPIOA, GPIO_Pin_3);
}

void Turn_On_LED(uint8_t ledNum)
{
    // Tắt toàn bộ 4 LED trước khi bật LED mới
    GPIO_ResetBits(GPIOA, GPIO_Pin_0);
    GPIO_ResetBits(GPIOA, GPIO_Pin_1);
    GPIO_ResetBits(GPIOA, GPIO_Pin_2);
    GPIO_ResetBits(GPIOA, GPIO_Pin_3);
    
    // Bật LED tương ứng theo số thứ tự
    switch(ledNum)
    {
        case 1: 
            GPIO_SetBits(GPIOA, GPIO_Pin_0); 
            break;
        case 2: 
            GPIO_SetBits(GPIOA, GPIO_Pin_1); 
            break;
        case 3: 
            GPIO_SetBits(GPIOA, GPIO_Pin_2); 
            break;
        case 4: 
            GPIO_SetBits(GPIOA, GPIO_Pin_3); 
            break;
        default: 
            break; 
    }
}

void CAN2_Init(void)
{
    GPIO_InitTypeDef GPIO_InitStruct;
    CAN_InitTypeDef CAN_InitStruct;
    CAN_FilterInitTypeDef CAN_FilterInitStruct;

    // Bật RCC
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_CAN1, ENABLE);
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_CAN2, ENABLE);
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);

    // CAN2 RX
    GPIO_InitStruct.GPIO_Pin = GPIO_Pin_12;
    GPIO_InitStruct.GPIO_Mode = GPIO_Mode_IPU;
    GPIO_Init(GPIOA, &GPIO_InitStruct);

    // CAN2 TX
    GPIO_InitStruct.GPIO_Pin = GPIO_Pin_13;
    GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOA, &GPIO_InitStruct);

    CAN_DeInit(CAN2);
    CAN_StructInit(&CAN_InitStruct);

    CAN_InitStruct.CAN_TTCM = DISABLE;
    CAN_InitStruct.CAN_ABOM = DISABLE;
    CAN_InitStruct.CAN_AWUM = DISABLE;
    CAN_InitStruct.CAN_NART = DISABLE;
    CAN_InitStruct.CAN_RFLM = DISABLE;
    CAN_InitStruct.CAN_TXFP = DISABLE;

    // Đặt chế độ Normal Mode
    CAN_InitStruct.CAN_Mode = CAN_Mode_Normal;

    // Cấu hình Baudrate
    CAN_InitStruct.CAN_SJW = CAN_SJW_1tq;
    CAN_InitStruct.CAN_BS1 = CAN_BS1_6tq;
    CAN_InitStruct.CAN_BS2 = CAN_BS2_7tq;
    CAN_InitStruct.CAN_Prescaler = 6;
    CAN_Init(CAN2, &CAN_InitStruct);

    // CAN2 Filter - Accept all frames
    CAN_FilterInitStruct.CAN_FilterNumber = 14;     //Filter CAN1: 0-13, CAN2: 14-27
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

void CAN2_Respond_Msg(uint8_t byte1, uint8_t byte2)
{
    TX_Message.StdId = 0x03;
    TX_Message.IDE = CAN_Id_Standard;
    TX_Message.RTR = CAN_RTR_Data;
    TX_Message.DLC = 2;
    TX_Message.Data[0] = byte1;
    TX_Message.Data[1] = byte2;

    CAN_Transmit(CAN2, &TX_Message);
}
#endif

int main(void)
{
#ifdef NODE_1

    uint8_t led_counter = 1;
    
    CAN1_Init();

    while(1)
    {
        if(CAN_MessagePending(CAN1, CAN_FIFO0) != 0)
        {
            CAN_Receive(CAN1, CAN_FIFO0, &RX_Message);

            if((RX_Message.StdId == 0x03) && (RX_Message.RTR == CAN_RTR_Data))
            {
                uint8_t received_byte1 = RX_Message.Data[0];
                uint8_t received_byte2 = RX_Message.Data[1];
            }
        }
    }

#else // NODE_2
    
    LED_Init(); 
    CAN2_Init();

    while(1)
    {    
        if(CAN_MessagePending(CAN2, CAN_FIFO0) != 0)
        {    
            CAN_Receive(CAN2, CAN_FIFO0, &RX_Message);

            if(RX_Message.StdId == 0x01 && RX_Message.RTR == CAN_RTR_Data)
            {
                uint8_t led_to_control = RX_Message.Data[0];
                Turn_On_LED(led_to_control);
            }
            else if(RX_Message.StdId == 0x02 && RX_Message.RTR == CAN_RTR_Remote)
            {
                CAN2_Send_Data_Response(0xAA, 0xBB);
            }
        }
    }
#endif
}