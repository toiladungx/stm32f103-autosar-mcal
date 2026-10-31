/**
 * @file    Dio.c
 * @brief   Digital Input/Output (DIO) Driver Source File
 * @details File này chứa phần hiện thực các API của DIO Driver
 *          theo chuẩn AUTOSAR dành cho STM32F103. Driver này
 *          cung cấp khả năng đọc và ghi các mức logic của
 *          các chân (GPIO pin) và các cổng (GPIO port) trong
 *          vi điều khiển STM32F103.
 *
 * @version 1.0.0
 * @date    21-Sep-2026
 * @author  Nguyen Duc Dung
 */

#include "Dio.h"

#if(DIO_DEV_ERROR_DETECT == STD_ON)
#include "Det.h"
#endif



Dio_LevelType Dio_ReadChannel(Dio_ChannelType ChannelId)
{

#if(DIO_DEV_ERROR_DETECT == STD_ON)
    if(ChannelId >= 64U)
    {
        Det_ReportError(DIO_MODULE_ID, DIO_INSTANCE_ID, DIO_READCHANNEL_SERVICE_ID, DIO_E_PARAM_INVALID_CHANNEL_ID);
        return STD_LOW;
    }
#endif

    Dio_LevelType level = STD_LOW;
    GPIO_TypeDef *GPIO_Port = NULL;
    uint16_t GPIO_Pin;

    if(ChannelId < 16U)
    {
        GPIO_Port = GPIOA;
    }else if(ChannelId < 32U)
    {
        GPIO_Port = GPIOB;
    }else if(ChannelId < 48U)
    {
        GPIO_Port = GPIOC;
    }else if(ChannelId < 64U)
    {
        GPIO_Port = GPIOD;
    }

    GPIO_Pin = (uint16_t)(1U << (ChannelId % 16U));

    if(GPIO_Port != NULL)
    {
        if(GPIO_ReadInputDataBit(GPIO_Port, GPIO_Pin) == Bit_SET)
        {
            level = STD_HIGH;
        }
        else
        {
            level = STD_LOW;
        }
    }

    return level;
}



void Dio_WriteChannel(Dio_ChannelType ChannelId, Dio_LevelType Level)
{

#if(DIO_DEV_ERROR_DETECT == STD_ON)
    if(ChannelId >= 64U)
    {
        Det_ReportError(DIO_MODULE_ID, DIO_INSTANCE_ID, DIO_WRITECHANNEL_SERVICE_ID, DIO_E_PARAM_INVALID_CHANNEL_ID);
        return;
    }
#endif

    GPIO_TypeDef *GPIO_Port = NULL;
    uint16_t GPIO_Pin;

    if(ChannelId < 16U)
    {
        GPIO_Port = GPIOA;
    }else if(ChannelId < 32U)
    {
        GPIO_Port = GPIOB;
    }else if(ChannelId < 48U)
    {
        GPIO_Port = GPIOC;
    }else if(ChannelId < 64U)
    {
        GPIO_Port = GPIOD;
    }

    GPIO_Pin = (uint16_t)(1U << (ChannelId % 16U));

    if(GPIO_Port != NULL)
    {
        if(Level == STD_HIGH)
        {
            GPIO_SetBits(GPIO_Port, GPIO_Pin);
        }else{
            GPIO_ResetBits(GPIO_Port, GPIO_Pin);
        }
    }
}



Dio_PortLevelType Dio_ReadPort(Dio_PortType PortId)
{

#if(DIO_DEV_ERROR_DETECT == STD_ON)
    if(PortId >= 4U)
    {
        Det_ReportError(DIO_MODULE_ID, DIO_INSTANCE_ID, DIO_READPORT_SERVICE_ID, DIO_E_PARAM_INVALID_PORT_ID);
        return 0x0U;
    }
#endif

    GPIO_TypeDef *GPIO_Port = NULL;
    Dio_PortLevelType portLevel = 0x0U;

    switch(PortId)
    {
        case 0U:
            GPIO_Port = GPIOA;
            break;
        case 1U:
            GPIO_Port = GPIOB;
            break;
        case 2U:
            GPIO_Port = GPIOC;
            break;
        case 3U:
            GPIO_Port = GPIOD;
            break;
        default:
            GPIO_Port = NULL;
            break;
    }

    if(GPIO_Port != NULL)
    {
        portLevel = (Dio_PortLevelType)GPIO_ReadInputData(GPIO_Port);
    }

    return portLevel;
}



void Dio_WritePort(Dio_PortType PortId, Dio_PortLevelType Level)
{

#if(DIO_DEV_ERROR_DETECT == STD_ON)
    if(PortId >= 4U)
    {
        Det_ReportError(DIO_MODULE_ID, DIO_INSTANCE_ID, DIO_WRITEPORT_SERVICE_ID, DIO_E_PARAM_INVALID_PORT_ID);
        return;
    }
#endif

    GPIO_TypeDef *GPIO_Port = NULL;

    switch(PortId)
    {
        case 0U:
            GPIO_Port = GPIOA;
            break;
        case 1U:
            GPIO_Port = GPIOB;
            break;
        case 2U:
            GPIO_Port = GPIOC;
            break;
        case 3U:
            GPIO_Port = GPIOD;
            break;            
        default:
            GPIO_Port = NULL;
            break;
    }

    if(GPIO_Port != NULL)
    {
        GPIO_Write(GPIO_Port, (uint16_t)Level);
    }
}



Dio_PortLevelType Dio_ReadChannelGroup(const Dio_ChannelGroupType* ChannelGroupIdPtr)
{

#if(DIO_DEV_ERROR_DETECT == STD_ON)
    if(ChannelGroupIdPtr == NULL)
    {
        Det_ReportError(DIO_MODULE_ID, DIO_INSTANCE_ID, DIO_READCHANNELGROUP_SERVICE_ID, DIO_E_PARAM_POINTER);
        return 0x0U;
    }
    
    if(ChannelGroupIdPtr->port >= 4U)
    {
        Det_ReportError(DIO_MODULE_ID, DIO_INSTANCE_ID, DIO_READCHANNELGROUP_SERVICE_ID, DIO_E_PARAM_INVALID_PORT_ID);
        return 0x0U;
    }
#endif

    Dio_PortLevelType currentPortLevel = 0x0U;
    Dio_PortLevelType channelGroupLevel = 0x0U;
    
    if(ChannelGroupIdPtr != NULL)
    {
        currentPortLevel = Dio_ReadPort(ChannelGroupIdPtr->port);

        channelGroupLevel = (Dio_PortLevelType)((currentPortLevel & (Dio_PortLevelType)ChannelGroupIdPtr->mask) >> ChannelGroupIdPtr->offset);
    }
    return channelGroupLevel;
}



void Dio_WriteChannelGroup(const Dio_ChannelGroupType* ChannelGroupIdPtr, Dio_PortLevelType Level)
{

#if(DIO_DEV_ERROR_DETECT == STD_ON)
    if(ChannelGroupIdPtr == NULL)
    {
        Det_ReportError(DIO_MODULE_ID, DIO_INSTANCE_ID, DIO_WRITECHANNELGROUP_SERVICE_ID, DIO_E_PARAM_POINTER);
        return;
    }
    
    if(ChannelGroupIdPtr->port >= 4U)
    {
        Det_ReportError(DIO_MODULE_ID, DIO_INSTANCE_ID, DIO_WRITECHANNELGROUP_SERVICE_ID, DIO_E_PARAM_INVALID_PORT_ID);
        return;
    }
#endif   

    if(ChannelGroupIdPtr != NULL)
    {
        Dio_PortLevelType currentPortLevel = 0x0U;
        Dio_PortLevelType writeValue = 0x0U;

        //Đọc giá trị port hiện tại
        currentPortLevel = Dio_ReadPort(ChannelGroupIdPtr->port);

        //Xóa giá trị tại các chân trong config
        currentPortLevel &= (Dio_PortLevelType)(~(ChannelGroupIdPtr->mask));

        //Dịch giá trị level vào vị trí
        writeValue = ((Level << ChannelGroupIdPtr->offset) & ChannelGroupIdPtr->mask);

        //Gộp
        writeValue = (Dio_PortLevelType)(currentPortLevel | writeValue);

        Dio_WritePort(ChannelGroupIdPtr->port, writeValue);
    }
}



Dio_LevelType Dio_FlipChannel(Dio_ChannelType ChannelId)
{

#if(DIO_DEV_ERROR_DETECT == STD_ON)
    if(ChannelId >= 64U)
    {
        Det_ReportError(DIO_MODULE_ID, DIO_INSTANCE_ID, DIO_FLIPCHANNEL_SERVICE_ID, DIO_E_PARAM_INVALID_CHANNEL_ID);
        return STD_LOW;
    }
#endif

    Dio_LevelType newLevel = STD_LOW;

    if(ChannelId < 64U)
    {
        if(Dio_ReadChannel(ChannelId) == STD_HIGH)
        {
            newLevel = STD_LOW;
        }else{
            newLevel = STD_HIGH;
        }
        Dio_WriteChannel(ChannelId, newLevel);
    }

    return newLevel;
}



void Dio_MaskedWritePort(Dio_PortType PortId, Dio_PortLevelType Level, Dio_PortLevelType Mask)
{
    
}