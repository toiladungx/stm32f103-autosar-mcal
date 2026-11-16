/**********************************************************
 * @file    Port_Cfg.h
 * @brief   Port Driver Configuration Header File
 * @details File này chứa các định nghĩa cấu hình các chân Port 
 *          cho driver Port theo chuẩn AUTOSAR, dùng trên 
 *          STM32F103 với thư viện SPL.
 *
 * @version 1.0
 * @date    28-9-2026
 * @author  Nguyen Duc Dung
 **********************************************************/

#ifndef _PORT_CFG_H_
#define _PORT_CFG_H_

/* ====================================================== */
/*                        INCLUDES                        */
/* ====================================================== */
#include "Port.h"


/* ====================================================== */
/*           PRE-COMPILE CONFIGURATION SWITCHES           */
/* ====================================================== */
//Bật/tắt tính năng kiểm tra lỗi bằng DET (Development Error Tracer)
#define PORT_DEV_ERROR_DETECT STD_ON


/* ====================================================== */
/*               PIN CONFIGURATION NUMBERS                */
/* ====================================================== */
// Số lượng chân Port được cấu hình (tùy chỉnh theo dự án)
#define PORT_CONFIGURED_PINS_COUNT  4U

// Định danh logic trực quan cho từng chân (tương ứng với vị trí trong mảng)
#define PORT_CONFIGURED_PIN_LED       ((Port_PinType)0U)
#define PORT_CONFIGURED_PIN_ADC       ((Port_PinType)1U)
#define PORT_CONFIGURED_PIN_RELAY     ((Port_PinType)2U)
#define PORT_CONFIGURED_PIN_PWM       ((Port_PinType)3U)


/* ====================================================== */
/*               EXTERNAL CONFIG DECLARATION              */
/* ====================================================== */
// Bảng cấu hình chi tiết cho từng chân
extern const Port_PinConfigType Port_PinConfigurations[PORT_CONFIGURED_PINS_COUNT];

// Struct cấu hình tổng hợp truyền vào Port_Init()
extern const Port_ConfigType Port_Config;

#endif // _PORT_CFG_H_
