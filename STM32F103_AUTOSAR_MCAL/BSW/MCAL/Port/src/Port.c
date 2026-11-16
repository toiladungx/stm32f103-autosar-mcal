/**********************************************************
 * @file    Port.c
 * @brief   Port Driver Source File (AUTOSAR Standard)
 * @details File này hiện thực các API chuẩn AUTOSAR cho Port Driver,
 *          dành cho STM32F103 sử dụng SPL. Driver cho phép cấu hình,
 *          đổi mode, đổi chiều, refresh và đọc thông tin version của port/pin.
 *
 * @version 1.0
 * @date    23-9-2026
 * @author  Nguyen Duc Dung
 **********************************************************/

 
/* ====================================================== */
/*                     INCLUDES                           */
/* ====================================================== */

#include "Port.h"
#include "Port_Cfg.h"
#include "stm32f10x_rcc.h"

#if(PORT_DEV_ERROR_DETECT == STD_ON)
#include "Det.h"
#endif



/* ====================================================== */
/*              STATIC / INTERNAL VARIABLES               */
/* ====================================================== */

// Trạng thái khởi tạo
static uint8 Port_Initialized = 0;

// Con trỏ lưu bảng cấu hình sau khi Init
static const Port_ConfigType *Port_ConfigPtr = NULL;



/* ====================================================== */
/*              INTERNAL HELPER FUNCTION                  */
/* ====================================================== */

// Cấu hình phần cứng cho 1 pin GPIO dựa trên struct cấu hình
static void Port_ApplyPinConfig(const Port_PinConfigType *pinConfigPtr)
{
    GPIO_InitTypeDef GPIO_InitStruct;
    GPIO_TypeDef *gpioPort = NULL;
    uint16_t pinMask = 0;
    
    if(pinConfigPtr == NULL)
    {
        return;
    }

    // Xác định PORT phần cứng và bật Clock RCC tương ứng
    switch(pinConfigPtr->portNum)
    {
        case PORT_ID_A:
            gpioPort = GPIOA;
            RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
            break;
        case PORT_ID_B:
            gpioPort = GPIOB;
            RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
            break;
        case PORT_ID_C:
            gpioPort = GPIOC;
            RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);
            break;
        case PORT_ID_D:
            gpioPort = GPIOD;
            RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOD, ENABLE);
            break;
        default:
            return;
    }

    // Tính Pin Mask trực tiếp (1 << PinNum)
    pinMask = (uint16)(1U << (pinConfigPtr->pinNum));

    // Nạp thông số chân và tốc độ vào struct
    GPIO_InitStruct.GPIO_Pin = pinMask;
    GPIO_InitStruct.GPIO_Speed = pinConfigPtr->speed;

    // Cấu hình GPIO_Mode theo Mode chức năng, Direction và Pull
    if(pinConfigPtr->mode == PORT_PIN_MODE_DIO)
    {
        if(pinConfigPtr->direction == PORT_PIN_OUT)
        {
            if(pinConfigPtr->pull == PORT_PIN_PULL_UP)
            {
                GPIO_InitStruct.GPIO_Mode = GPIO_Mode_Out_PP;
            }else{
                GPIO_InitStruct.GPIO_Mode = GPIO_Mode_Out_OD;
            }
        }else{ //PORT_PIN_IN
            if(pinConfigPtr->pull == PORT_PIN_PULL_UP)
            {
                GPIO_InitStruct.GPIO_Mode = GPIO_Mode_IPU;
            }else if(pinConfigPtr->pull == PORT_PIN_PULL_DOWN)
            {
                GPIO_InitStruct.GPIO_Mode = GPIO_Mode_IPD;
            }else{
                GPIO_InitStruct.GPIO_Mode = GPIO_Mode_IN_FLOATING;
            }
        }
    }else if (pinConfigPtr->mode == PORT_PIN_MODE_ADC)
    {
        // Chế độ Analog (chỉ nhận Input)
        if (pinConfigPtr->direction == PORT_PIN_IN)
        {
            GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AIN;
        }else{
            return; //Không hợp lệ nếu cấu hình ADC mà chọn Output
        }
    }else if(pinConfigPtr->mode == PORT_PIN_MODE_PWM)
    {
        // Chế độ AF Push-Pull cho PWM (Timer CH)
        GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AF_PP;
    }
    else{
        // Các mode khác như ADC, PWM, SPI ... mở rộng thêm tùy MCU
        return;
    }

    // Ghi cấu hình xuống thanh ghi phần cứng
    GPIO_Init(gpioPort, &GPIO_InitStruct);

    // Nếu là output, xuất mức logic khởi tạo (HIGH/LOW)
    if(pinConfigPtr->direction == PORT_PIN_OUT)
    {
        if(pinConfigPtr->level == PORT_PIN_LEVEL_HIGH)
        {
            GPIO_SetBits(gpioPort, pinMask);
        }
        else
        {
            GPIO_ResetBits(gpioPort, pinMask);
        }
    }
}



/* ====================================================== */
/*                  FUNCTION DEFINITIONS                  */
/* ====================================================== */

void Port_Init(const Port_ConfigType *ConfigPtr)
{
#if(PORT_DEV_ERROR_DETECT == STD_ON)
    if(ConfigPtr == NULL)
    {
        Det_ReportError(PORT_MODULE_ID, PORT_INSTANCE_ID, PORT_INIT_SERVICE_ID, PORT_E_INIT_FAILED);
        return;
    }

    if(ConfigPtr->pinConfigs == NULL)
    {
        Det_ReportError(PORT_MODULE_ID, PORT_INSTANCE_ID, PORT_INIT_SERVICE_ID, PORT_E_INIT_FAILED);
        return;
    }
#endif

    // Duyệt từng chân trong mảng cấu hình và nạp vào phần cứng
    for(uint16 i = 0U; i < ConfigPtr->numConfiguredPins; i++)
    {
        Port_ApplyPinConfig(&ConfigPtr->pinConfigs[i]);
    }

    Port_ConfigPtr = ConfigPtr;
    Port_Initialized = 1U;
}



void Port_SetPinDirection(Port_PinType Pin, Port_PinDirectionType Direction)
{
#if(PORT_DEV_ERROR_DETECT == STD_ON)
    if(Port_Initialized == 0U)
    {
        Det_ReportError(PORT_MODULE_ID, PORT_INSTANCE_ID, PORT_SET_PIN_DIRECTION_SERVICE_ID, PORT_E_UNINIT);
        return;
    }

    if(Pin >= Port_ConfigPtr->numConfiguredPins)
    {
        Det_ReportError(PORT_MODULE_ID, PORT_INSTANCE_ID, PORT_SET_PIN_DIRECTION_SERVICE_ID, PORT_E_PARAM_PIN);
        return;
    }

    if(Port_ConfigPtr->pinConfigs[Pin].directionChangeable == 0)
    {
        Det_ReportError(PORT_MODULE_ID, PORT_INSTANCE_ID, PORT_SET_PIN_DIRECTION_SERVICE_ID, PORT_E_DIRECTION_UNCHANGEABLE);
        return;
    }
#endif

    // Tạo một bản sao cấu hình tạm thời của chân để cập nhật chiều mới
    Port_PinConfigType tempPinConfig = Port_ConfigPtr->pinConfigs[Pin];
    tempPinConfig.direction = Direction;

    // Áp dụng cấu hình mới xuống thanh ghi phần cứng
    Port_ApplyPinConfig(&tempPinConfig);
}


void Port_RefreshPortDirection(void)
{
#if(PORT_DEV_ERROR_DETECT == STD_ON)
    if(Port_Initialized == 0U)
    {
        Det_ReportError(PORT_MODULE_ID, PORT_INSTANCE_ID, PORT_REFRESH_PORT_DIRECTION_SERVICE_ID, PORT_E_UNINIT);
        return;
    }
#endif

    for(uint16 i = 0U; i < Port_ConfigPtr->numConfiguredPins; i++)
    {
        // Chỉ refresh các chân không cho phép đổi chiều lúc chạy
        if(Port_ConfigPtr->pinConfigs[i].directionChangeable == 0)
        {
            Port_ApplyPinConfig(&Port_ConfigPtr->pinConfigs[i]);
        }
    }
}



void Port_SetPinMode(Port_PinType Pin, Port_PinModeType Mode)
{
#if(PORT_DEV_ERROR_DETECT == STD_ON)
    if(Port_Initialized == 0U)
    {
        Det_ReportError(PORT_MODULE_ID, PORT_INSTANCE_ID, PORT_SET_PIN_MODE_SERVICE_ID, PORT_E_UNINIT);
        return;
    }

    if(Pin >= Port_ConfigPtr->numConfiguredPins)
    {
        Det_ReportError(PORT_MODULE_ID, PORT_INSTANCE_ID, PORT_SET_PIN_MODE_SERVICE_ID, PORT_E_PARAM_PIN);
        return;
    }

    if(Port_ConfigPtr->pinConfigs[Pin].modeChangeable == 0U)
    {
        Det_ReportError(PORT_MODULE_ID, PORT_INSTANCE_ID, PORT_SET_PIN_MODE_SERVICE_ID, PORT_E_MODE_UNCHANGEABLE);
        return;
    }

    if ((Mode != PORT_PIN_MODE_DIO) &&
        (Mode != PORT_PIN_MODE_ADC) &&
        (Mode != PORT_PIN_MODE_PWM) &&
        (Mode != PORT_PIN_MODE_SPI) &&
        (Mode != PORT_PIN_MODE_CAN))
    {
        Det_ReportError(PORT_MODULE_ID, PORT_INSTANCE_ID, PORT_SET_PIN_MODE_SERVICE_ID, PORT_E_PARAM_INVALID_MODE);
        return;
    }
#endif

    // Tạo biến tạm, ko sửa trực tiếp vào vùng cấu hình gốc
    Port_PinConfigType tempPinConfig = Port_ConfigPtr->pinConfigs[Pin];
    tempPinConfig.mode = Mode;

    // Áp dụng cấu hình mới xuống thanh ghi phần cứng
    Port_ApplyPinConfig(&tempPinConfig);
}



// void Port_GetVersionInfo(Std_VersionInfoType *versioninfo)
// {
//     ;;
// }