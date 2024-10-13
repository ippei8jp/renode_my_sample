#include "stm32f4xx.h"
#include <stm32f4xx_rcc.h>
#include <stm32f4xx_usart.h>
#include <stm32f4xx_gpio.h>
#include "stm32f4xx_exti.h"
#include "stm32f4xx_syscfg.h"

#include <stdio.h>
#include <stdbool.h>
#include <stdarg.h>

// ===============================================================
// タイマ周り
static volatile uint32_t TimingDelay;

void Delay(uint32_t nTime){
    TimingDelay = nTime;
    while(TimingDelay != 0);
}

void SysTick_Handler(void){
    if (TimingDelay != 0x00) {
        TimingDelay--;
    }
}

// ===============================================================
// EXTI 割り込みハンドラ
void EXTI0_IRQHandler(void)
{
    // Checks whether the interrupt from EXTI0 or not
    if (EXTI_GetITStatus(EXTI_Line0)) {
        printf("EXTI0 Interrupted!\n");

        // Clears the EXTI line pending bit
        EXTI_ClearITPendingBit(EXTI_Line0);
    }
}

void EXTI1_IRQHandler(void)
{
    // Checks whether the interrupt from EXTI1 or not
    if (EXTI_GetITStatus(EXTI_Line1)) {
        printf("EXTI1 Interrupted!\n");

        // Clears the EXTI line pending bit
        EXTI_ClearITPendingBit(EXTI_Line1);
    }
}

void EXTI2_IRQHandler(void)
{
    // Checks whether the interrupt from EXTI2 or not
    if (EXTI_GetITStatus(EXTI_Line2)) {
        printf("EXTI2 Interrupted!\n");

        // Clears the EXTI line pending bit
        EXTI_ClearITPendingBit(EXTI_Line2);
    }
}

// ===============================================================
int main(void)
{
    // クロック供給
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOD, ENABLE);
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_SYSCFG, ENABLE);
    RCC_AHB1PeriphClockCmd(RCC_APB1Periph_USART2, ENABLE);

    // GPIOの設定
    GPIO_InitTypeDef GPIO_InitStructure;

    // Pin PA0/PA1/PA2 入力モード
    GPIO_StructInit(&GPIO_InitStructure);
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1 | GPIO_Pin_2;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
    GPIO_Init(GPIOA, &GPIO_InitStructure);

    // Pin PD12/PD13/PD14 出力モード
    GPIO_StructInit(&GPIO_InitStructure);
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12 | GPIO_Pin_13 | GPIO_Pin_14;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
    GPIO_Init(GPIOD, &GPIO_InitStructure);

    // EXTI割り込みソースの選択 PA0→EXTI0/PA1→EXTI1/PA2→EXTI2
    SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOA, EXTI_PinSource0);
    SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOA, EXTI_PinSource1);
    SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOA, EXTI_PinSource2);

    // EXTI設定
    EXTI_InitTypeDef EXTI_InitStruct;
    EXTI_StructInit(&EXTI_InitStruct);
    EXTI_InitStruct.EXTI_Line = EXTI_Line0 | EXTI_Line1 | EXTI_Line2;
    EXTI_InitStruct.EXTI_LineCmd = ENABLE;
    EXTI_InitStruct.EXTI_Mode = EXTI_Mode_Interrupt;
    EXTI_InitStruct.EXTI_Trigger = EXTI_Trigger_Rising;
    EXTI_Init(&EXTI_InitStruct);

    // NVIC設定
    NVIC_InitTypeDef NVIC_InitStruct;

    // EXTI0
    NVIC_InitStruct.NVIC_IRQChannel = EXTI0_IRQn;
    NVIC_InitStruct.NVIC_IRQChannelCmd = ENABLE;
    // EXTI0_IRQn has Most important interrupt
    NVIC_InitStruct.NVIC_IRQChannelPreemptionPriority = 0x00;
    NVIC_InitStruct.NVIC_IRQChannelSubPriority = 0x00;
    NVIC_Init(&NVIC_InitStruct);

    // EXTI1
    NVIC_InitStruct.NVIC_IRQChannel = EXTI1_IRQn;
    NVIC_InitStruct.NVIC_IRQChannelCmd = ENABLE;
    // EXTI0_IRQn has Most important interrupt
    NVIC_InitStruct.NVIC_IRQChannelPreemptionPriority = 0x00;
    NVIC_InitStruct.NVIC_IRQChannelSubPriority = 0x00;
    NVIC_Init(&NVIC_InitStruct);

    // EXTI2
    NVIC_InitStruct.NVIC_IRQChannel = EXTI2_IRQn;
    NVIC_InitStruct.NVIC_IRQChannelCmd = ENABLE;
    // EXTI0_IRQn has Most important interrupt
    NVIC_InitStruct.NVIC_IRQChannelPreemptionPriority = 0x00;
    NVIC_InitStruct.NVIC_IRQChannelSubPriority = 0x00;
    NVIC_Init(&NVIC_InitStruct);

    // UARTの設定
    USART_ClockInitTypeDef USART_ClockInitStruct;
    USART_ClockStructInit(&USART_ClockInitStruct);
    USART_ClockInit(USART2,&USART_ClockInitStruct);

    USART_InitTypeDef USART_InitStruct;
    USART_StructInit(&USART_InitStruct);
    USART_Init(USART2,&USART_InitStruct);

    // SysTick Timer 初期化
    if (SysTick_Config(SystemCoreClock / 10000)) {
        printf("SysTick init error\n");
        while (1);
    }

    // Hello World
    printf("Hello World!\n");

#if 0
    // scanf test
    int data;
    printf("input data :\n");
    scanf("%d", &data);
    printf("data=%d\n", data);
#endif

    while (1){
        static int  count  = 0;

        // Button
        uint8_t btn0 = GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_0);
        uint8_t btn1 = GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_1);
        uint8_t btn2 = GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_2);

        printf("Loop %5d   %3s  %3s  %3s\n",  count, 
                                            btn0 ? "ON": "OFF",
                                            btn1 ? "ON": "OFF",
                                            btn2 ? "ON": "OFF"
        );
        count++;


#if 0
        static bool ledval = false;
        // toggle LED
        GPIO_WriteBit(GPIOD, GPIO_Pin_12, (ledval) ? Bit_SET : Bit_RESET);
        ledval = !ledval;
#else
        // LED 
        static int led_cnt = 0;
        switch (led_cnt) {
            case 0:
                GPIO_WriteBit(GPIOD, GPIO_Pin_12, Bit_SET);
                led_cnt++;
                break;
            case 1:
                GPIO_WriteBit(GPIOD, GPIO_Pin_13, Bit_SET);
                led_cnt++;
                break;
            case 2:
                GPIO_WriteBit(GPIOD, GPIO_Pin_14, Bit_SET);
                led_cnt++;
                break;
            case 3:
                GPIO_WriteBit(GPIOD, GPIO_Pin_12, Bit_RESET);
                led_cnt++;
                break;
            case 4:
                GPIO_WriteBit(GPIOD, GPIO_Pin_13, Bit_RESET);
                led_cnt++;
                break;
            case 5:
                GPIO_WriteBit(GPIOD, GPIO_Pin_14, Bit_RESET);
                led_cnt = 0;
                break;
            default:
                led_cnt = 0;
        }
#endif
        // wait 1000msec
        Delay (1000);
    }

    return 0;
}







