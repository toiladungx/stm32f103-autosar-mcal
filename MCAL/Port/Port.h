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
#include "stm32f10x_gpio.h" // Thư viện chuẩn của STM32F103

/**********************************************************
 * Định nghĩa các giá trị PortId cho các cổng GPIO
 **********************************************************/
#define PORT_ID_A           0   //GPIOA
#define PORT_ID_B           1   //GPIOB
#define PORT_ID_C           2   //GPIOC
#define PORT_ID_D           3   //GPIOD



/**********************************************************
 * Định nghĩa kiểu dữ liệu của Port Driver AUTOSAR
 **********************************************************/
/**/
typedef uint8 Port_PinType;

/**/
typedef enum
{
    PORT_PIN_IN     = 0x00,     //Input
    PORT_PIN_OUT    = 0x01      //Output
}Port_PinDirectionType;

/*
*/
typedef uint8 Port_PinModeType;

typedef struct
{
    uint8 PortNum;                      // 0=A, 1=B, 2=C, 3=D
    uint8 PinNum;                       // 0 -> 15
    Port_PinModeType Mode;              // Chế độ chức năng
    Port_PinDirectionType Direction;    // Chiều ban đầu
    uint8 Speed;                        // Tốc độ hoạt động
    uint8 DirectionChangeable;          // 1: Cho phép đổi chiều runtime
    uint8 Level;                        // Giá trị mức ban đầu nếu output
    uint8 Pull;                         // Kiểu pull: none, up hay down
    uint8 ModeChangeable;               // 1: Cho phép đổi mode runtime
}Port_PinConfigType;

typedef struct
{
    const Port_PinConfigType *PinConfigs;   // Con trỏ tới mảng cấu hình pin
    uint16 PinCount;                        // Số lượng chân cấu hình
}Port_ConfigType;


#endif // _PORT.H_