#include "stm32f10x.h"
#include "stm32f10x_gpio.h"
#include "stm32f10x_rcc.h"

// Hàm delay đơn giản (không dùng timer, không chính xác)
void Delay(volatile uint32_t nCount) {
    for(; nCount != 0; nCount--);
}

int main(void) {
    // 1. Cấp clock cho Port C
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);

    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_USART1 | RCC_APB2Periph_AFIO, ENABLE);
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_CAN1, ENABLE);


    // 2. Cấu hình chân PC13 là Output Push-Pull, tốc độ 50MHz
    GPIO_InitTypeDef GPIO_InitStructure;
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_13;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOC, &GPIO_InitStructure);

    // 3. Vòng lặp nháy LED trên PC13
    while (1) {
        // Tắt LED (vì PC13 trên board Bluepill thường tích cực mức thấp: 1 = tắt, 0 = sáng)
        GPIO_SetBits(GPIOC, GPIO_Pin_13);
        Delay(0x000FFFFF); // Chờ

        // Bật LED
        GPIO_ResetBits(GPIOC, GPIO_Pin_13);
        Delay(0x000FFFFF); // Chờ

        

        //sysbus LogPeripheralAccess sysbus.gpioPortC
    }
}
