/**********************************************************
 * @file    Port.h
 * @brief   Port Driver Header File (AUTOSAR Standard)
 * @details File này chứa các định nghĩa về kiểu dữ liệu,
 *          macro ánh xạ GPIO, và khai báo các API chuẩn
 *          AUTOSAR cho Port Driver. Driver này được thiết
 *          kế cho STM32F103 sử dụng SPL.
 * @version 1.0
 * @date    23-9-2026
 * @author  Nguyen Duc Dung
 **********************************************************/

#ifndef _PORT_H_
#define _PORT_H_

#include "Std_Types.h"      // Các kiểu dữ liệu chuẩn AUTOSAR
// #include "Port_Cfg.h"
#include "stm32f10x.h"
#include "stm32f10x_gpio.h"

/**********************************************************
 * Thông tin Module và Instance
 **********************************************************/
#define PORT_VENDOR_ID      0x4D2U
#define PORT_MODULE_ID      0x51U
#define PORT_INSTANCE_ID    0x00U


/**********************************************************
 * AUTOSAR DET SERVICE IDs
 **********************************************************/
#define PORT_INIT_SERVICE_ID                    0x00U
#define PORT_SET_PIN_DIRECTION_SERVICE_ID       0x01U
#define PORT_REFRESH_PORT_DIRECTION_SERVICE_ID  0x02U
#define PORT_GET_VERSION_INFO_SERVICE_ID        0x03U
#define PORT_SET_PIN_MODE_SERVICE_ID            0x04U


/**********************************************************
 * AUTOSAR DET ERROR CODES
 **********************************************************/
#define PORT_E_PARAM_PIN                0x0AU
#define PORT_E_DIRECTION_UNCHANGEABLE   0x0BU
#define PORT_E_INIT_FAILED              0x0CU
#define PORT_E_PARAM_INVALID_MODE       0x0DU
#define PORT_E_MODE_UNCHANGEABLE        0x0EU
#define PORT_E_UNINIT                   0x0FU
#define PORT_E_PARAM_POINTER            0x10U


 /**********************************************************
 * Định nghĩa các giá trị PortId cho các cổng GPIO
 **********************************************************/
#define PORT_ID_A   0U   //GPIOA
#define PORT_ID_B   1U   //GPIOB
#define PORT_ID_C   2U   //GPIOC
#define PORT_ID_D   3U   //GPIOD


/**********************************************************
 * Định nghĩa các chế độ mode và trạng thái cho pin
 **********************************************************/
// Port Pin Mode
#define PORT_PIN_MODE_DIO   0U
#define PORT_PIN_MODE_ADC   1U
#define PORT_PIN_MODE_PWM   2U
#define PORT_PIN_MODE_SPI   3U
#define PORT_PIN_MODE_CAN   4U
#define PORT_PIN_MODE_LIN   5U

// Internal Pull-up/Pull-down
#define PORT_PIN_PULL_NONE  0U
#define PORT_PIN_PULL_UP    1U
#define PORT_PIN_PULL_DOWN  2U

// Initial Output Levels
#define PORT_PIN_LEVEL_LOW  0U
#define PORT_PIN_LEVEL_HIGH 1U


/**********************************************************
 * Định nghĩa kiểu dữ liệu của Port Driver AUTOSAR
 **********************************************************/

 typedef uint8 Port_PinType;

 typedef enum
{
    PORT_PIN_IN     = 0x00,     //Input
    PORT_PIN_OUT    = 0x01      //Output
}Port_PinDirectionType;
 
typedef uint8 Port_PinModeType;

typedef struct
{
    uint8 portNum;                      // PORT_ID_A, PORT_ID_B, ...
    uint8 pinNum;                       // Số chân: 0 -> 15
    Port_PinModeType mode;              // Chế độ: DIO, ADC, PWM...
    Port_PinDirectionType direction;    // PORT_PIN_IN hoặc PORT_PIN_OUT
    GPIOSpeed_TypeDef speed;            // GPIO_Speed_10MHz, 2MHz, 50MHz
    uint8 level;                        // Giá trị mức ban đầu nếu output (LOW/HIGH)
    uint8 pull;                         // PULL_NONE, PULL_UP, PULL_DOWN
    uint8 directionChangeable;          // Cho phép đổi chiều runtime
    uint8 modeChangeable;               // Cho phép đổi mode runtime
}Port_PinConfigType;

typedef struct
{
    const Port_PinConfigType *pinConfigs;   // Con trỏ tới danh sách các pin
    uint16 numConfiguredPins;               // Số lượng chân cần cấu hình
}Port_ConfigType;


/**********************************************************
 * Khai báo prototype các hàm API Port Driver AUTOSAR
 **********************************************************/
void Port_Init (const Port_ConfigType *ConfigPtr);

void Port_SetPinDirection(Port_PinType Pin, Port_PinDirectionType Direction);

void Port_RefreshPortDirection(void);

//void Port_GetVersionInfo(Std_VersionInfoType *versioninfo);

void Port_SetPinMode(Port_PinType Pin, Port_PinModeType Mode);

#endif // _PORT.H_