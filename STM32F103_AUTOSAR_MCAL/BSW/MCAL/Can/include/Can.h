/**********************************************************
 * @file    Pwm.h
 * @brief   Pulse Width Modulation (PWM) Driver Header File
 * @details File này chứa các định nghĩa về kiểu dữ liệu và
 *          khai báo các API của PWM Driver tuân theo chuẩn AUTOSAR.
 *          Driver này được thiết kế để điều khiển Timer PWM của STM32F103
 *          sử dụng thư viện SPL.
 * @version 1.0
 * @date    05-Oct-2026
 * @author  Nguyen Duc Dung
 **********************************************************/

#ifndef _PWM_H_
#define _PWM_H_

#include "Pwm_Cfg.h"
#include "Std_Types.h"
#include "stm32f10x_tim.h"      //Thư viện SPL: Timer PWM cho STM32F103


#define PWM_INIT_SERVICE_ID                     0x00
#define PWM_DEINIT_SERVICE_ID                   0x01
#define PWM_SET_DUTY_CYCLE_SERVICE_ID           0x02
#define PWM_SET_PERIOD_AND_DUTY_SERVICE_ID      0x03
#define PWM_SET_OUTPUT_TO_IDLE_SERVICE_ID       0x04
#define PWM_GET_OUTPUT_STATE_SERVICE_ID         0x05
#define PWM_DISABLE_NOTIFICATION_SERVICE_ID     0x06
#define PWM_ENABLE_NOTIFICATION_SERVICE_ID      0x07
#define PWM_GET_VERSION_INFO_SERVICE_ID         0x08
#define PWM_SET_POWER_STATE_SERVICE_ID          0x09
#define PWM_GET_CURRENT_POWER_STATE_SERVICE_ID  0x0A
#define PWM_GET_TARGET_POWER_STATE_SERVICE_ID   0x0B
#define PWM_PREPARE_POWER_STATE_SERVICE_ID      0x0C


// ID
#define PWM_MODULE_ID                           0x79U
#define PWM_INSTANCE_ID                         0x00U


//Development Errors Code
#define PWM_E_INIT_FAILED                       0x10U
#define PWM_E_UNINIT                            0x11U
#define PWM_E_PARAM_CHANNEL                     0x12U
#define PWM_E_PERIOD_UNCHANGEABLE               0x13U
#define PWM_E_ALREADY_INITIALIZED               0x14U
#define PWM_E_PARAM_POINTER                     0x15U
#define PWM_E_NOT_DISENGAGED                    0x16U
#define PWM_E_POWER_STATE_NOT_SUPPORTED         0x17U
#define PWM_E_TRANSITION_NOT_POSSIBLE           0x18U
#define PWM_E_PERIPHERAL_NOT_PREPARED           0x19U
#define PWM_E_PARAM_DUTY                        0x20U



/**********************************************************
 * Định nghĩa các kiểu dữ liệu của PWM Driver
 **********************************************************/

typedef uint8  Pwm_ChannelType;

typedef uint16 Pwm_PeriodType;

typedef enum
{
    PWM_HIGH = 0x00, 
    PWM_LOW  = 0x01
} Pwm_OutputStateType;

typedef enum
{
    PWM_RISING_EDGE  = 0x00,
    PWM_FALLING_EDGE = 0x01,
    PWM_BOTH_EDGES   = 0x02
} Pwm_EdgeNotificationType;

typedef enum
{
    PWM_VARIABLE_PERIOD      = 0x00,
    PWM_FIXED_PERIOD         = 0x01,
    PWM_FIXED_PERIOD_SHIFTED = 0x02
} Pwm_ChannelClassType;

typedef enum
{
    PWM_SERVICE_ACCEPTED     = 0x00,
    PWM_NOT_INIT             = 0x01,
    PWM_SEQUENCE_ERROR       = 0x02,
    PWM_HW_FAILURE           = 0x03,
    PWM_POWER_STATE_NOT_SUPP = 0x04,
    PWM_TRANS_NOT_POSSIBLE   = 0x05
} Pwm_PowerStateRequestResultType;

typedef enum
{
    PWM_FULL_POWER = 0U,           //Trạng thái nguồn đầy đủ
    PWM_LOW_POWER  = 1U
} Pwm_PowerStateType;

typedef struct
{
    TIM_TypeDef *TIMx;              // Timer sử dụng (TIM1, TIM2, ...)
    uint8 channel;                  // Channel nào của TIM (Channel 1, 2, 3, 4)
    Pwm_ChannelClassType classType; // Loại kênh
    Pwm_PeriodType defaultPeriod;   // Chu kỳ mặc định
    uint16 defaultDutyCycle;        // Duty Cycle mặc định (0x0000 - 0x8000)
    Pwm_OutputStateType polarity;   // Xác định cực tính của xung (xung bắt đầu ở mức cao hay mức thấp)
    Pwm_OutputStateType idleState;  // Trạng thái an toàn của chân ngõ ra khi gọi hàm dừng xung Pwm_SetOutputToIdle
    void (*NotificationCb)(void);   // Cơ chế thông báo (Notification/Callback). Khi xung hoàn thành chu kỳ hoặc có ngắt xảy ra, driver sẽ gọi hàm này để báo cho tầng ứng dụng (Application Layer) biết.
} Pwm_ChannelConfigType;

typedef struct
{
    const Pwm_ChannelConfigType *Channels;
    uint8 NumChannels;
} Pwm_ConfigType;


/**********************************************************
 * Khai báo các API của PWM Driver (chuẩn AUTOSAR)
 **********************************************************/

void Pwm_Init(const Pwm_ConfigType *ConfigPtr);

void Pwm_DeInit(void);

void Pwm_SetDutyCycle(Pwm_ChannelType ChannelNumber, uint16 DutyCycle);

void Pwm_SetPeriodAndDuty(Pwm_ChannelType ChannelNumber, Pwm_PeriodType Period, uint16 DutyCycle);

void Pwm_SetOutputToIdle(Pwm_ChannelType ChannelNumber);

Pwm_OutputStateType Pwm_GetOutputState(Pwm_ChannelType ChannelNumber);

void Pwm_DisableNotification(Pwm_ChannelType ChannelNumber);

void Pwm_EnableNotification(Pwm_ChannelType ChannelNumber, Pwm_EdgeNotificationType Notification);

Std_ReturnType Pwm_SetPowerState(Pwm_PowerStateRequestResultType *Result);

Std_ReturnType Pwm_GetCurrentPowerState(Pwm_PowerStateType *CurrentPowerState, Pwm_PowerStateRequestResultType *Result);

Std_ReturnType Pwm_GetTargetPowerState(Pwm_PowerStateType *TargetPowerState, Pwm_PowerStateRequestResultType *Result);

Std_ReturnType Pwm_PreparePowerState(Pwm_PowerStateType PowerState, Pwm_PowerStateRequestResultType *Result);

//void Pwm_GetVersionInfo(Std_VersionInfoType *versioninfo);

//void Pwm_Main_PowerTransitionManager(void);

#endif // _PWM.H_
