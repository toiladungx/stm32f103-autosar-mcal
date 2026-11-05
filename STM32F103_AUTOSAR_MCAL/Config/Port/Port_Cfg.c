/**********************************************************
 * @file    Port_cfg.c
 * @brief   Port Driver Configuration Source File
 * @details File này chứa phần hiện thực các tham số cấu hình 
 *          cho từng chân Port của driver Port theo chuẩn AUTOSAR,
 *          dùng cho STM32F103.
 *
 * @version 1.0
 * @date    28-9-2026
 * @author  Nguyen Duc Dung
 **********************************************************/

/* ====================================================== */
/*                        INCLUDES                        */
/* ====================================================== */
#include "Port_cfg.h"


/* ====================================================== */
/*               PIN CONFIGURATION ARRAY                  */
/* ====================================================== */
/**
 * @brief Bảng cấu hình chi tiết cho từng chân phần cứng
 */
const Port_PinConfigType Port_PinConfigurations[PORT_CONFIGURED_PINS_COUNT] = {
    /* Pin 0 (PORT_CONF_PIN_LED): PA0 - DIO, Output, 50MHz, cho phép đổi chiều & mode */
    {
        .portNum             = PORT_ID_A,
        .pinNum              = 0U,
        .mode                = PORT_PIN_MODE_DIO,
        .direction           = PORT_PIN_OUT,
        .speed               = GPIO_Speed_50MHz,
        .pull                = PORT_PIN_PULL_NONE,
        .level               = PORT_PIN_LEVEL_HIGH,
        .directionChangeable = 1U,
        .modeChangeable      = 1U
    },

    /* Pin 1 (PORT_CONF_PIN_ADC): PA1 - ADC, Input, không đổi chiều/mode lúc chạy */
    {
        .portNum             = PORT_ID_A,
        .pinNum              = 1U,
        .mode                = PORT_PIN_MODE_ADC,
        .direction           = PORT_PIN_IN,
        .speed               = GPIO_Speed_2MHz,
        .pull                = PORT_PIN_PULL_NONE,
        .level               = PORT_PIN_LEVEL_LOW,
        .directionChangeable = 0U,
        .modeChangeable      = 0U
    },

    /* Pin 2 (PORT_CONF_PIN_RELAY): PB0 - DIO, Output, 10MHz, cố định chiều/mode */
    {
        .portNum             = PORT_ID_B,
        .pinNum              = 0U,
        .mode                = PORT_PIN_MODE_DIO,
        .direction           = PORT_PIN_OUT,
        .speed               = GPIO_Speed_10MHz,
        .pull                = PORT_PIN_PULL_NONE,
        .level               = PORT_PIN_LEVEL_LOW,
        .directionChangeable = 0U,
        .modeChangeable      = 0U
    },

    /* Pin 3 (PORT_CONF_PIN_PWM): PC13 - PWM (Timer Alternate Function), 50MHz */
    {
        .portNum             = PORT_ID_C,
        .pinNum              = 13U,
        .mode                = PORT_PIN_MODE_PWM,
        .direction           = PORT_PIN_OUT,
        .speed               = GPIO_Speed_50MHz,
        .pull                = PORT_PIN_PULL_NONE,
        .level               = PORT_PIN_LEVEL_LOW,
        .directionChangeable = 1U,
        .modeChangeable      = 1U
    }
};

/* ========================================================================== */
/*                      ROOT CONFIGURATION STRUCTURE                          */
/* ========================================================================== */
/**
 * @brief Biến cấu hình gốc truyền vào hàm Port_Init()
 */
const Port_ConfigType Port_Config = {
    .pinConfigs = Port_PinConfigurations,
    .numConfiguredPins = PORT_CONFIGURED_PINS_COUNT
};