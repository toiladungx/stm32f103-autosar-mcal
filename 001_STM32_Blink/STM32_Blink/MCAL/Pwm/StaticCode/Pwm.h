/**********************************************************
 * @file    Pwm.h
 * @brief   Pulse Width Modulation (PWM) Driver Header File
 * @details File này chứa các định nghĩa về kiểu dữ liệu và
 *          khai báo các API của PWM Driver tuân theo chuẩn AUTOSAR.
 *          Driver này được thiết kế để điều khiển Timer PWM của STM32F103
 *          sử dụng thư viện SPL.
 * @version 1.0
 * @date    9/10/2026
 * @author  Nguyen Duc Dung
 **********************************************************/

#ifndef _PWM_H_
#define _PWM_H_

#include "Std_Types.h"              //Các kiểu dữ liệu chuẩn AUTOSAR
#include "stm32f10x_tim.h"          //Thư viện SPL Timer PWM cho STM32F103


/***********************************************************
 * Định nghĩa các kiểu dữ liệu của PWM Driver
 ***********************************************************/

typedef uint8 Pwm_ChannelType;

typedef uint16 Pwm_PeriodType;

typedef enum
{
    PWM_HIGH                    = 0x00,
    PWM_LOW                     = 0x01
}Pwm_OutputStateType;

typedef enum
{
    PWM_RISING_EDGE             = 0x00,
    PWM_FALLING_EDGE            = 0x01,
    PWM_BOTH_EDGES              = 0x02 
}Pwm_EdgeNotificationType;

typedef enum
{
    PWM_VARIABLE_PERIOD         = 0x00,
    PWM_FIXED_PERIOD            = 0x01,
    PWM_FIXED_PERIOD_SHIFTED    = 0x02
}Pwm_ChannelClassType;

typedef enum
{
    PWM_SERVICE_ACCEPTED        = 0x00,
    PWM_NOT_INIT                = 0x01,
    PWM_SEQUENCE_ERROR          = 0x02,
    PWM_HW_FAILURE              = 0x03,
    PWM_POWER_STATE_NOT_SUPP    = 0x04,
    PWM_TRANS_NOT_POSSIBLE      = 0x05
}Pwm_PowerStateRequestResultType;

typedef uint8 Pwm_PowerStateType;

#define PWM_FULL_POWER          ((Pwm_PowerStateType)0x00U)

typedef struct
{  
    TIM_TypeDef                 *TIMx;                      //Timer sử dụng (TIM1, TIM2, ...)
    uint8                       channelId;                  //Channel số: (1, 2, 3, ...)
    Pwm_ChannelClassType        classType;                  //Loại kênh
    Pwm_PeriodType              defaultPeriod;              //Chu kỳ mặc định
    uint16                      defaultDutyCycle;           //Duty cycle mặc định (0x0000 - 0x8000)
    Pwm_OutputStateType         polarity;                   //Đầu ra ban đầu
    Pwm_OutputStateType         idleState;                  //Trạng thái khi idle
    void                        (*NotificationCb)(void);    //Callback notification (optional)
}Pwm_ChannelConfigType;

typedef struct
{
    const Pwm_ChannelConfigType *Channels;                  //Con trỏ trỏ đến mảng cấu hình các kênh
    uint8                       NumChannels;                //Số lượng kênh PWM được cấu hình
} Pwm_ConfigType;           

/***********************************************************
 * Khai báo các API của PWM Driver (chuẩn AUTOSAR)
************************************************************/


#endif // _PWM_H_