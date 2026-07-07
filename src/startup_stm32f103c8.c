#include <stdint.h>

/* stack top = 0x20000000 + 20KB */
#define STACK_TOP 0x20005000

extern int main(void);

/* linker symbols */
extern uint32_t _etext, _sdata, _edata, _sbss, _ebss;

/* handlers */
void Reset_Handler(void);
void Default_Handler(void);
void SysTick_Handler(void);
void USART1_IRQHandler(void);
void USART2_IRQHandler(void);
void USART3_IRQHandler(void);

/* vector table */
__attribute__((section(".isr_vector")))
void (* const vector_table[])(void) = {
    (void (*)(void))STACK_TOP, // 0 MSP
    Reset_Handler,             // 1 Reset
    Default_Handler,           // 2 NMI
    Default_Handler,           // 3 HardFault
    Default_Handler,           // 4 MemManage
    Default_Handler,           // 5 BusFault
    Default_Handler,           // 6 UsageFault
    0, 0, 0, 0,                // 7–10
    Default_Handler,           // 11 SVCall
    Default_Handler,           // 12 DebugMonitor
    0,                         // 13
    Default_Handler,           // 14 PendSV
    SysTick_Handler,           // 15 SysTick
    
    // External Interrupts
    Default_Handler,           // 16 WWDG
    Default_Handler,           // 17 PVD
    Default_Handler,           // 18 TAMPER
    Default_Handler,           // 19 RTC
    Default_Handler,           // 20 FLASH
    Default_Handler,           // 21 RCC
    Default_Handler,           // 22 EXTI0
    Default_Handler,           // 23 EXTI1
    Default_Handler,           // 24 EXTI2
    Default_Handler,           // 25 EXTI3
    Default_Handler,           // 26 EXTI4
    Default_Handler,           // 27 DMA1_Channel1
    Default_Handler,           // 28 DMA1_Channel2
    Default_Handler,           // 29 DMA1_Channel3
    Default_Handler,           // 30 DMA1_Channel4
    Default_Handler,           // 31 DMA1_Channel5
    Default_Handler,           // 32 DMA1_Channel6
    Default_Handler,           // 33 DMA1_Channel7
    Default_Handler,           // 34 ADC1_2
    Default_Handler,           // 35 USB_HP_CAN_TX
    Default_Handler,           // 36 USB_LP_CAN_RX0
    Default_Handler,           // 37 CAN_RX1
    Default_Handler,           // 38 CAN_SCE
    Default_Handler,           // 39 EXTI9_5
    Default_Handler,           // 40 TIM1_BRK
    Default_Handler,           // 41 TIM1_UP
    Default_Handler,           // 42 TIM1_TRG_COM
    Default_Handler,           // 43 TIM1_CC
    Default_Handler,           // 44 TIM2
    Default_Handler,           // 45 TIM3
    Default_Handler,           // 46 TIM4
    Default_Handler,           // 47 I2C1_EV
    Default_Handler,           // 48 I2C1_ER
    Default_Handler,           // 49 I2C2_EV
    Default_Handler,           // 50 I2C2_ER
    Default_Handler,           // 51 SPI1
    Default_Handler,           // 52 SPI2
    USART1_IRQHandler,         // 53 USART1 
    USART2_IRQHandler,         // 54 USART2
    USART3_IRQHandler,         // 55 USART3
    Default_Handler,           // 56 EXTI15_10
    Default_Handler,           // 57 RTCAlarm
    Default_Handler,           // 58 USBWakeup
};

/* reset */
void Reset_Handler(void)
{
    uint32_t *src = &_etext;
    uint32_t *dst = &_sdata;

    while (dst < &_edata)
        *dst++ = *src++;

    dst = &_sbss;
    while (dst < &_ebss)
        *dst++ = 0;

    main();

    while (1);
}

void Default_Handler(void)
{
    while (1);
}

// Weak aliases
void SysTick_Handler(void) __attribute__((weak, alias("Default_Handler")));
void USART1_IRQHandler(void) __attribute__((weak, alias("Default_Handler")));
void USART2_IRQHandler(void) __attribute__((weak, alias("Default_Handler")));
void USART3_IRQHandler(void) __attribute__((weak, alias("Default_Handler")));