#include "led.h"
#include "stm32f10x_rcc.h"
#include "stm32f10x_gpio.h"



void led_init(void)
{
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);

    GPIO_InitTypeDef gpio;
    gpio.GPIO_Pin = GPIO_Pin_13;
    gpio.GPIO_Mode = GPIO_Mode_Out_PP;
    gpio.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOC, &gpio);

    GPIO_SetBits(GPIOC, GPIO_Pin_13);
}

void led_on(void)
{
    GPIO_ResetBits(GPIOC, GPIO_Pin_13);
}

void led_off(void)
{
    GPIO_SetBits(GPIOC, GPIO_Pin_13);
}



