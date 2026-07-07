#include "stm32f10x.h"
#include "stm32f10x_gpio.h"
#include "stm32f10x_rcc.h"
#include "stdio.h"

static void delay_ms(uint32_t ms)
{
    while (ms--) {
        for (volatile uint32_t i = 0; i < 8000u; i++) {
        }
    }
}

int main(void)
{
    printf("Hello, World!\n");

    // while (1) {}
}
