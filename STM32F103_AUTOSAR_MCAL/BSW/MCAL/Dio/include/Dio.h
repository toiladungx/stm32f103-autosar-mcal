/**********************************************************
 * @file    Dio.h
 * @brief   Digital Input/Output (DIO) Driver Header File
 * @details File này chứa các định nghĩa về kiểu dữ liệu và
 *          khai báo các API của DIO Driver tuân theo chuẩn AUTOSAR.
 *          Driver này được thiết kế để điều khiển GPIO của STM32F103.
 *
 * @version 1.0.0
 * @date    21-Sep-2026
 * @author  Nguyen Duc Dung
 **********************************************************/

#ifndef _DIO_H_
#define _DIO_H_

#include  "Std_Types.h"         // Các kiểu dữ liệu chuẩn AUTOSAR     
#include "stm32f10x_gpio.h"     // Thư viện chuẩn của STM32F103   


/**********************************************************
 * Định nghĩa các ID cho cổng GPIO (Dio Port ID)
 **********************************************************/
#define DIO_PORT_A      0
#define DIO_PORT_B      1
#define DIO_PORT_C      2
#define DIO_PORT_D      3


#define DIO_MODULE_ID                       0x78U
#define DIO_INSTANCE_ID                     0x00U

#define DIO_READCHANNEL_SERVICE_ID          0x00
#define DIO_WRITECHANNEL_SERVICE_ID         0x01
#define DIO_READPORT_SERVICE_ID             0x02
#define DIO_WRITEPORT_SERVICE_ID            0x03
#define DIO_READCHANNELGROUP_SERVICE_ID     0x04
#define DIO_WRITECHANNELGROUP_SERVICE_ID    0x05
#define DIO_FLIPCHANNEL_SERVICE_ID          0x11
//#define DIO_GETVERSIONINFO_SERVICE_ID       0x12
#define DIO_MASKEDWRITEPORT_SERVICE_ID      0x13


//Development Errors Code
#define DIO_E_PARAM_INVALID_CHANNEL_ID      0x0A
#define DIO_E_PARAM_INVALID_PORT_ID         0x14
#define DIO_E_PARAM_INVALID_GROUP           0x1F
#define DIO_E_PARAM_POINTER                 0x20


// GPIO A Channels
#define DIO_CHANNEL_0  0 
#define DIO_CHANNEL_1  1 
#define DIO_CHANNEL_2  2 
#define DIO_CHANNEL_3  3 
#define DIO_CHANNEL_4  4 
#define DIO_CHANNEL_5  5 
#define DIO_CHANNEL_6  6 
#define DIO_CHANNEL_7  7 
#define DIO_CHANNEL_8  8 
#define DIO_CHANNEL_9  9 
#define DIO_CHANNEL_10 10
#define DIO_CHANNEL_11 11
#define DIO_CHANNEL_12 12
#define DIO_CHANNEL_13 13
#define DIO_CHANNEL_14 14
#define DIO_CHANNEL_15 15

// GPIO B Channels
#define DIO_CHANNEL_16 16
#define DIO_CHANNEL_17 17
#define DIO_CHANNEL_18 18
#define DIO_CHANNEL_19 19
#define DIO_CHANNEL_20 20
#define DIO_CHANNEL_21 21
#define DIO_CHANNEL_22 22
#define DIO_CHANNEL_23 23
#define DIO_CHANNEL_24 24
#define DIO_CHANNEL_25 25
#define DIO_CHANNEL_26 26
#define DIO_CHANNEL_27 27
#define DIO_CHANNEL_28 28
#define DIO_CHANNEL_29 29
#define DIO_CHANNEL_30 30
#define DIO_CHANNEL_31 31

// GPIO C Channels
#define DIO_CHANNEL_32 32
#define DIO_CHANNEL_33 33
#define DIO_CHANNEL_34 34
#define DIO_CHANNEL_35 35
#define DIO_CHANNEL_36 36
#define DIO_CHANNEL_37 37
#define DIO_CHANNEL_38 38
#define DIO_CHANNEL_39 39
#define DIO_CHANNEL_40 40
#define DIO_CHANNEL_41 41
#define DIO_CHANNEL_42 42
#define DIO_CHANNEL_43 43
#define DIO_CHANNEL_44 44
#define DIO_CHANNEL_45 45
#define DIO_CHANNEL_46 46
#define DIO_CHANNEL_47 47

// GPIO D Channels
#define DIO_CHANNEL_48 48
#define DIO_CHANNEL_49 49
#define DIO_CHANNEL_50 50
#define DIO_CHANNEL_51 51
#define DIO_CHANNEL_52 52
#define DIO_CHANNEL_53 53
#define DIO_CHANNEL_54 54
#define DIO_CHANNEL_55 55
#define DIO_CHANNEL_56 56
#define DIO_CHANNEL_57 57
#define DIO_CHANNEL_58 58
#define DIO_CHANNEL_59 59
#define DIO_CHANNEL_60 60
#define DIO_CHANNEL_61 61
#define DIO_CHANNEL_62 62
#define DIO_CHANNEL_63 63

#define STD_LOW     0x00
#define STD_HIGH    0x01

typedef uint8  Dio_ChannelType;
typedef uint8  Dio_PortType;
typedef uint8  Dio_LevelType;
typedef uint16 Dio_PortLevelType;

typedef struct {
    uint32       mask;
    uint8        offset;
    Dio_PortType port;
} Dio_ChannelGroupType;

Dio_LevelType Dio_ReadChannel(Dio_ChannelType ChannelId);

void Dio_WriteChannel(Dio_ChannelType ChannelId, Dio_LevelType Level);

Dio_PortLevelType Dio_ReadPort(Dio_PortType PortId);

void Dio_WritePort(Dio_PortType PortId, Dio_PortLevelType Level);

Dio_PortLevelType Dio_ReadChannelGroup(const Dio_ChannelGroupType* ChannelGroupIdPtr);

void Dio_WriteChannelGroup(const Dio_ChannelGroupType* ChannelGroupIdPtr, Dio_PortLevelType Level);

//void Dio_GetVersionInfo(Std_VersionInfoType* VersionInfo);

Dio_LevelType Dio_FlipChannel(Dio_ChannelType ChannelId);

void Dio_MaskedWritePort(Dio_PortType PortId, Dio_PortLevelType Level, Dio_PortLevelType Mask);

#endif // _DIO_H_