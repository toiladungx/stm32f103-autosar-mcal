/**********************************************************
 * @file    Pwm.c
 * @brief   Pulse Width Modulation (PWM) Driver Source File
 * @details Hiện thực các API của PWM Driver chuẩn AUTOSAR cho STM32F103, sử dụng SPL.
 *          Quản lý chức năng PWM, không cấu hình chân GPIO.
 *
 * @version 1.0
 * @date    05-Oct-2026
 * @author  Nguyen Duc Dung
 **********************************************************/

#include "Pwm.h"

#if(PWM_DEV_ERROR_DETECT == STD_ON)
#include "Det.h"
#endif

/**********************************************************
 * Static Variables & Defines
 **********************************************************/
// Lưu trữ cấu hình driver PWM hiện tại
static const Pwm_ConfigType *Pwm_CurrentConfigPtr = NULL;

// Trạng thái đã khởi tạo của driver PWM
static uint8 Pwm_IsInitialized = 0U;
static uint8 Pwm_IsPowerStatePrepared = 0U;
static Pwm_PowerStateType Pwm_CurrentPowerState = PWM_FULL_POWER;
static Pwm_PowerStateType Pwm_TargetPowerState = PWM_FULL_POWER;

// Ghi giá trị compare vào CCR tương ứng kênh phần cứng
static void Pwm_WriteCompareValue(const Pwm_ChannelConfigType *channelConfig, uint16 compareValue)
{
    if(channelConfig == NULL)
    {
        return;
    }

    switch(channelConfig->channel)
    {
        case 1U:
            channelConfig->TIMx->CCR1 = compareValue;
            break;
        case 2U:
            channelConfig->TIMx->CCR2 = compareValue;
            break;
        case 3U:
            channelConfig->TIMx->CCR3 = compareValue;
            break;
        case 4U:
            channelConfig->TIMx->CCR4 = compareValue;
            break;
        default:
            break;
    }
}


/**********************************************************
 * Function Definitions
 **********************************************************/
void Pwm_Init(const Pwm_ConfigType *ConfigPtr)
{

#if (PWM_DEV_ERROR_DETECT == STD_ON)
    if(Pwm_IsInitialized != 0U)
    {
        Det_ReportError(PWM_MODULE_ID, PWM_INSTANCE_ID, PWM_INIT_SERVICE_ID, PWM_E_ALREADY_INITIALIZED);
        return;
    }

    if(ConfigPtr == NULL)
    {
        Det_ReportError(PWM_MODULE_ID, PWM_INSTANCE_ID, PWM_INIT_SERVICE_ID, PWM_E_PARAM_POINTER);
        return;
    }
#endif

    Pwm_CurrentConfigPtr = ConfigPtr;

    for(uint8 i = 0; i < ConfigPtr->NumChannels; i++)
    {
        const Pwm_ChannelConfigType *channelConfig = &ConfigPtr->Channels[i];

        // Cấu hình chu kỳ cho Timer (ARR)
        channelConfig->TIMx->ARR = channelConfig->defaultPeriod;

        uint32 compareVal = Pwm_ComputeCompareValue(channelConfig->defaultPeriod, channelConfig->defaultDutyCycle);

        // Giá trị compare ban đầu theo duty cycle mặc định
        Pwm_WriteCompareValue(channelConfig, compareVal);

        // Bật Timer
        TIM_Cmd(channelConfig->TIMx, ENABLE);

        // Nếu là TIM1 (advanced), cần bật thêm cờ xuất xung MOE
        if(channelConfig->TIMx == TIM1)
        {
            TIM_CtrlPWMOutputs(TIM1, ENABLE);
        }
    }

    Pwm_CurrentPowerState = PWM_FULL_POWER;
    Pwm_TargetPowerState = PWM_FULL_POWER;
    Pwm_IsPowerStatePrepared = 0U;
    Pwm_IsInitialized = 1U;
}



void Pwm_DeInit(void)
{
#if (PWM_DEV_ERROR_DETECT == STD_ON)
    if(Pwm_IsInitialized == 0U)
    {
        Det_ReportError(PWM_MODULE_ID, PWM_INSTANCE_ID, PWM_DEINIT_SERVICE_ID, PWM_E_UNINIT);
        return;
    }
#endif

    for(uint8 i = 0; i < Pwm_CurrentConfigPtr->NumChannels; i++)
    {
        const Pwm_ChannelConfigType *channelConfig = &Pwm_CurrentConfigPtr->Channels[i];
        
        // Nếu TIM1 thì tắt ngõ ra nâng cao
        if(channelConfig->TIMx == TIM1)
        {
            TIM_CtrlPWMOutputs(TIM1, DISABLE);
        }
        
        // Tắt Timer 
        TIM_Cmd(channelConfig->TIMx, DISABLE);

        // Reset thanh ghi Duty Cycle về 0 / ilde
        Pwm_WriteCompareValue(channelConfig, 0U);
    }

    Pwm_CurrentConfigPtr = NULL;
    Pwm_CurrentPowerState = PWM_FULL_POWER;
    Pwm_TargetPowerState = PWM_FULL_POWER;
    Pwm_IsPowerStatePrepared = 0U;
    Pwm_IsInitialized = 0U;
}



void Pwm_SetDutyCycle(Pwm_ChannelType ChannelNumber, uint16 DutyCycle)
{
#if(PWM_DEV_ERROR_DETECT == STD_ON)
    if(Pwm_IsInitialized == 0U)
    {
        Det_ReportError(PWM_MODULE_ID, PWM_INSTANCE_ID, PWM_SET_DUTY_CYCLE_SERVICE_ID, PWM_E_UNINIT);
        return;
    }

    if(ChannelNumber >= Pwm_CurrentConfigPtr->NumChannels)
    {
        Det_ReportError(PWM_MODULE_ID, PWM_INSTANCE_ID, PWM_SET_DUTY_CYCLE_SERVICE_ID, PWM_E_PARAM_CHANNEL);
        return;
    }

    if(DutyCycle > 0x8000U)
    {
        Det_ReportError(PWM_MODULE_ID, PWM_INSTANCE_ID, PWM_SET_DUTY_CYCLE_SERVICE_ID, PWM_E_PARAM_DUTY);
        return;
    }
#endif

    // Lấy thông tin channel từ bảng config
    const Pwm_ChannelConfigType *channelConfig = &Pwm_CurrentConfigPtr->Channels[ChannelNumber];

    // Đọc chu kỳ hiện tại từ ARR và tính toán giá trị CCR mới
    uint32 currentPeriod = (uint32)channelConfig->TIMx->ARR;
    uint32 compareVal = 0U;

    if(DutyCycle == 0x8000U)
    {
        //100%: CCR = ARR + 1
        compareVal = currentPeriod + 1U;
    }
    else if(DutyCycle == 0x0000U)
    {
        //0%: CCR = 0
        compareVal = 0U;
    }
    else
    {
        //compareVal = (currentPeriod * DutyCycle) / 0x8000
        compareVal = (uint32)(((uint32)currentPeriod * DutyCycle) >> 15U);
    }

    Pwm_WriteCompareValue(channelConfig, compareVal);
}



void Pwm_SetPeriodAndDuty(Pwm_ChannelType ChannelNumber, Pwm_PeriodType Period, uint16 DutyCycle)
{
#if(PWM_DEV_ERROR_DETECT == STD_ON)
    if(Pwm_IsInitialized == 0U)
    {
        Det_ReportError(PWM_MODULE_ID, PWM_INSTANCE_ID, PWM_SET_PERIOD_AND_DUTY_SERVICE_ID, PWM_E_UNINIT);
        return;
    }

    if(ChannelNumber >= Pwm_CurrentConfigPtr->NumChannels)
    {
        Det_ReportError(PWM_MODULE_ID, PWM_INSTANCE_ID, PWM_SET_PERIOD_AND_DUTY_SERVICE_ID, PWM_E_PARAM_CHANNEL);
        return;
    }

    if(Pwm_CurrentConfigPtr->Channels[ChannelNumber].classType != PWM_VARIABLE_PERIOD)
    {
        Det_ReportError(PWM_MODULE_ID, PWM_INSTANCE_ID, PWM_SET_PERIOD_AND_DUTY_SERVICE_ID, PWM_E_PERIOD_UNCHANGEABLE);
        return;
    }

    if(DutyCycle > 0x8000U)
    {
        Det_ReportError(PWM_MODULE_ID, PWM_INSTANCE_ID, PWM_SET_PERIOD_AND_DUTY_SERVICE_ID, PWM_E_PARAM_DUTY);
        return;
    }
#endif

    // Lấy thông tin channel từ bảng config
    const Pwm_ChannelConfigType *channelConfig = &Pwm_CurrentConfigPtr->Channels[ChannelNumber];

    // Cập nhật chu kỳ mới vào thanh ghi ARR của Timer
    channelConfig->TIMx->ARR = Period;

    // Tính giá trị so sánh mới dựa vào period và duty cycle
    uint32 compareVal = 0U;
    
    if(DutyCycle == 0x8000U)
    {
        // 100%
        compareVal = (uint32)Period + 1U;
    }
    else if(DutyCycle == 0x0000U)
    {
        // 0%
        compareVal = 0U;
    }
    else
    {
        // compareVal = (Period * Duty Cycle) / 0x8000
        compareVal = (uint32)(((uint32)Period * DutyCycle) >> 15U);
    }
    
    // Ghi value vào thanh ghi CCR
    Pwm_WriteCompareValue(channelConfig, compareVal);
}



void Pwm_SetOutputToIdle(Pwm_ChannelType ChannelNumber)
{
#if(PWM_DEV_ERROR_DETECT == STD_ON)
    if(Pwm_IsInitialized == 0U)
    {
        Det_ReportError(PWM_MODULE_ID, PWM_INSTANCE_ID, PWM_SET_OUTPUT_TO_IDLE_SERVICE_ID, PWM_E_UNINIT);
        return;
    }

    if(ChannelNumber >= Pwm_CurrentConfigPtr->NumChannels)
    {
        Det_ReportError(PWM_MODULE_ID, PWM_INSTANCE_ID, PWM_SET_OUTPUT_TO_IDLE_SERVICE_ID, PWM_E_PARAM_CHANNEL);
        return;
    }
#endif

    const Pwm_ChannelConfigType *channelConfig = &Pwm_CurrentConfigPtr->Channels[ChannelNumber];

    uint32 compareVal = 0U;

    // Xác định giá trị compare để đưa ouput về đúng Idle State
    // Nếu mức idle trùng với mức active (polarity) => 100%, và ngược lại
    if(channelConfig->idleState == channelConfig->polarity)
    {
        // 100%
        compareVal = (uint32)(channelConfig->TIMx->ARR + 1U);
    }
    else
    {
        // 0%
        compareVal = 0U;
    }

    Pwm_WriteCompareValue(channelConfig, compareVal);
}



Pwm_OutputStateType Pwm_GetOutputState(Pwm_ChannelType ChannelNumber)
{
#if(PWM_DEV_ERROR_DETECT == STD_ON)
    if(Pwm_IsInitialized == 0U)
    {
        Det_ReportError(PWM_MODULE_ID, PWM_INSTANCE_ID, PWM_GET_OUTPUT_STATE_SERVICE_ID, PWM_E_UNINIT);
        return PWM_LOW;
    }

    if(ChannelNumber >= Pwm_CurrentConfigPtr->NumChannels)
    {
        Det_ReportError(PWM_MODULE_ID, PWM_INSTANCE_ID, PWM_GET_OUTPUT_STATE_SERVICE_ID, PWM_E_PARAM_CHANNEL);
        return PWM_LOW;
    }
#endif

    const Pwm_ChannelConfigType *channelConfig = &Pwm_CurrentConfigPtr->Channels[ChannelNumber];

    uint16 isOutputEnabled = 0U;

    switch (channelConfig->channel)
    {
        case 1U:
            isOutputEnabled = (channelConfig->TIMx->CCER & TIM_CCER_CC1E);
            break;
        case 2U:
            isOutputEnabled = (channelConfig->TIMx->CCER & TIM_CCER_CC2E);
            break;
        case 3U:
            isOutputEnabled = (channelConfig->TIMx->CCER & TIM_CCER_CC3E);
            break;
        case 4U:
            isOutputEnabled = (channelConfig->TIMx->CCER & TIM_CCER_CC4E);
            break;
        default:
            break;
    }

    if(isOutputEnabled != 0U)
    {
        return PWM_HIGH;
    }
    else{
        return PWM_LOW;
    }

}



void Pwm_DisableNotification(Pwm_ChannelType ChannelNumber)
{
#if(PWM_DEV_ERROR_DETECT == STD_ON)
    if(Pwm_IsInitialized == 0U)
    {
        Det_ReportError(PWM_MODULE_ID, PWM_INSTANCE_ID, PWM_DISABLE_NOTIFICATION_SERVICE_ID, PWM_E_UNINIT);
        return;
    }

    if(ChannelNumber >= Pwm_CurrentConfigPtr->NumChannels)
    {
        Det_ReportError(PWM_MODULE_ID, PWM_INSTANCE_ID, PWM_DISABLE_NOTIFICATION_SERVICE_ID, PWM_E_PARAM_CHANNEL);
        return;
    }
#endif

    const Pwm_ChannelConfigType *channelConfig = &Pwm_CurrentConfigPtr->Channels[ChannelNumber];

    switch(channelConfig->channel)
    {
        case 1U:
            TIM_ITConfig(channelConfig->TIMx, TIM_IT_CC1, DISABLE);
            break;
        case 2U:
            TIM_ITConfig(channelConfig->TIMx, TIM_IT_CC2, DISABLE);
            break;
        case 3U:
            TIM_ITConfig(channelConfig->TIMx, TIM_IT_CC3, DISABLE);
            break;
        case 4U:
            TIM_ITConfig(channelConfig->TIMx, TIM_IT_CC4, DISABLE);
            break;
        default:
            break;
    }
}



void Pwm_EnableNotification(Pwm_ChannelType ChannelNumber, Pwm_EdgeNotificationType Notification)
{
#if(PWM_DEV_ERROR_DETECT == STD_ON)
    if(Pwm_IsInitialized == 0U)
    {
        Det_ReportError(PWM_MODULE_ID, PWM_INSTANCE_ID, PWM_ENABLE_NOTIFICATION_SERVICE_ID, PWM_E_UNINIT);
        return;
    }

    if(ChannelNumber >= Pwm_CurrentConfigPtr->NumChannels)
    {
        Det_ReportError(PWM_MODULE_ID, PWM_INSTANCE_ID, PWM_ENABLE_NOTIFICATION_SERVICE_ID, PWM_E_PARAM_CHANNEL);
        return;
    }
#endif

    const Pwm_ChannelConfigType *channelConfig = &Pwm_CurrentConfigPtr->Channels[ChannelNumber];

    // Xóa cờ ngắt đang có và kích hoạt ngắt
    switch(channelConfig->channel)
    {
        case 1U:
            TIM_ClearITPendingBit(channelConfig->TIMx, TIM_IT_CC1);
            TIM_ITConfig(channelConfig->TIMx, TIM_IT_CC1, ENABLE);
            break;
        case 2U:
            TIM_ClearITPendingBit(channelConfig->TIMx, TIM_IT_CC2);
            TIM_ITConfig(channelConfig->TIMx, TIM_IT_CC2, ENABLE);
            break;
        case 3U:
            TIM_ClearITPendingBit(channelConfig->TIMx, TIM_IT_CC3);
            TIM_ITConfig(channelConfig->TIMx, TIM_IT_CC3, ENABLE);
            break;
        case 4U:
            TIM_ClearITPendingBit(channelConfig->TIMx, TIM_IT_CC4);
            TIM_ITConfig(channelConfig->TIMx, TIM_IT_CC4, ENABLE);
            break;
        default:
            break;
    }
}



Std_ReturnType Pwm_SetPowerState(Pwm_PowerStateRequestResultType *Result)
{
#if(PWM_DEV_ERROR_DETECT == STD_ON)
    if(Pwm_IsInitialized == 0U)
    {
        Det_ReportError(PWM_MODULE_ID, PWM_INSTANCE_ID, PWM_SET_POWER_STATE_SERVICE_ID, PWM_E_UNINIT);
        if(Result != NULL)
        {
            *Result = PWM_SERVICE_ACCEPTED;
        }
        return E_NOT_OK;
    }

    if(Pwm_IsPowerStatePrepared == 0U)
    {
        Det_ReportError(PWM_MODULE_ID, PWM_INSTANCE_ID, PWM_SET_POWER_STATE_SERVICE_ID, PWM_E_NOT_DISENGAGED);
        if(Result != NULL)
        {
            *Result = PWM_NOT_INIT;
        }
        return E_NOT_OK;
    }
#endif

    // Thực thi chuyển trạng thái phần cứng dựa trên Pwm_TargetPowerState
    if(Pwm_TargetPowerState == PWM_FULL_POWER)
    {
        // Bật lại Timer cho tất cả các kênh được cấu hình
        for(uint8 i = 0; i < Pwm_CurrentConfigPtr->NumChannels; i++)
        {
            const Pwm_ChannelConfigType *channelConfig = &Pwm_CurrentConfigPtr->Channels[i];
            TIM_Cmd(channelConfig->TIMx, ENABLE);
            
            if(channelConfig->TIMx == TIM1)
            {
                TIM_CtrlPWMOutputs(TIM1, ENABLE);
            }
        }
    }
    else // Chế độ LOW POWER
    {
        // Dừng Timer của tất cả các kênh để giảm dòng tiêu thụ
        for(uint8 i = 0; i < Pwm_CurrentConfigPtr->NumChannels; i++)
        {
            const Pwm_ChannelConfigType *channelConfig = &Pwm_CurrentConfigPtr->Channels[i];
            TIM_Cmd(channelConfig->TIMx, DISABLE);

            if(channelConfig->TIMx == TIM1)
            {
                TIM_CtrlPWMOutputs(TIM1, DISABLE);
            }
        }
    }

    // Cập nhật trạng thái nguồn
    Pwm_CurrentPowerState = Pwm_TargetPowerState;
    Pwm_IsPowerStatePrepared = 0U;
    *Result = PWM_SERVICE_ACCEPTED;
    return E_OK;
}



Std_ReturnType Pwm_GetCurrentPowerState(Pwm_PowerStateType *CurrentPowerState, Pwm_PowerStateRequestResultType *Result)
{
#if(PWM_DEV_ERROR_DETECT == STD_ON)
    if(Pwm_IsInitialized == 0U)
    {
        Det_ReportError(PWM_MODULE_ID, PWM_INSTANCE_ID, PWM_GET_CURRENT_POWER_STATE_SERVICE_ID, PWM_E_UNINIT);
        if(Result != NULL)
        {
            *Result = PWM_NOT_INIT;
        }
        return E_NOT_OK;
    }

    if(CurrentPowerState == NULL)
    {
        Det_ReportError(PWM_MODULE_ID, PWM_INSTANCE_ID, PWM_GET_CURRENT_POWER_STATE_SERVICE_ID, PWM_E_PARAM_POINTER);
        if(Result != NULL)
        {
            *Result = PWM_SERVICE_ACCEPTED;
        }
        return E_NOT_OK;
    }
#endif

    // Lấy trạng thái nguồn hiện tại 
    *CurrentPowerState = Pwm_CurrentPowerState;
    *Result = PWM_SERVICE_ACCEPTED;
    return E_OK;
}


Std_ReturnType Pwm_GetTargetPowerState(Pwm_PowerStateType *TargetPowerState, Pwm_PowerStateRequestResultType *Result)
{
#if(PWM_DEV_ERROR_DETECT == STD_ON)
    if(Pwm_IsInitialized == 0U)
    {
        Det_ReportError(PWM_MODULE_ID, PWM_INSTANCE_ID, PWM_GET_TARGET_POWER_STATE_SERVICE_ID, PWM_E_UNINIT);
        if(Result != NULL)
        {
            *Result = PWM_NOT_INIT;
        }
        return E_NOT_OK;
    }

    if(TargetPowerState == NULL)
    {
        Det_ReportError(PWM_MODULE_ID, PWM_INSTANCE_ID, PWM_GET_TARGET_POWER_STATE_SERVICE_ID, PWM_E_PARAM_POINTER);
        if(Result != NULL)
        {
            *Result = PWM_SERVICE_ACCEPTED;
        }
        return E_NOT_OK;
    }
#endif

    // Lấy trạng thái nguồn mục tiêu
    *TargetPowerState = Pwm_TargetPowerState;
    *Result = PWM_SERVICE_ACCEPTED;
    return E_OK;
}



Std_ReturnType Pwm_PreparePowerState(Pwm_PowerStateType PowerState, Pwm_PowerStateRequestResultType *Result)
{
#if(PWM_DEV_ERROR_DETECT == STD_ON)
    if(Pwm_IsInitialized == 0U)
    {
        Det_ReportError(PWM_MODULE_ID, PWM_INSTANCE_ID, PWM_PREPARE_POWER_STATE_SERVICE_ID, PWM_E_UNINIT);
        if(Result != NULL)
        {
            *Result = PWM_NOT_INIT;
        }
        return E_NOT_OK;
    }

    if((PowerState != PWM_FULL_POWER) && (PowerState != PWM_LOW_POWER))
    {
        Det_ReportError(PWM_MODULE_ID, PWM_INSTANCE_ID, PWM_PREPARE_POWER_STATE_SERVICE_ID, PWM_E_POWER_STATE_NOT_SUPPORTED);
        if(Result != NULL)
        {
            *Result = PWM_POWER_STATE_NOT_SUPP;
        }
        return E_NOT_OK;
    }

    if(PowerState == Pwm_CurrentPowerState)
    {
        Det_ReportError(PWM_MODULE_ID, PWM_INSTANCE_ID, PWM_PREPARE_POWER_STATE_SERVICE_ID, PWM_E_TRANSITION_NOT_POSSIBLE);
        if(Result != NULL)
        {
            *Result = PWM_TRANS_NOT_POSSIBLE;
        }
        return E_NOT_OK;
    }
#endif
    
    // Lưu trạng thái mục tiêu và bật cờ sẵn sàng cho Pwm_SetPowerState
    Pwm_TargetPowerState = PowerState;
    Pwm_IsPowerStatePrepared = 1U;
    *Result = PWM_SERVICE_ACCEPTED;
    return E_OK;
}