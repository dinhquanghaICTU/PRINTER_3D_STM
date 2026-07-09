#include "uart.h"
#include <stdint.h>
#include "stm32f10x_gpio.h"
#include "stm32f10x_usart.h"
#include "stm32f10x_rcc.h"


void uart_debug_init(baudrate_t baudrate)
{
    GPIO_InitTypeDef gpio;
    USART_InitTypeDef usart;

    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA |
                           RCC_APB2Periph_AFIO |
                           RCC_APB2Periph_USART1,
                           ENABLE);

    gpio.GPIO_Pin = GPIO_Pin_9;
    gpio.GPIO_Mode = GPIO_Mode_AF_PP;
    gpio.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOA, &gpio);

    gpio.GPIO_Pin = GPIO_Pin_10;
    gpio.GPIO_Mode = GPIO_Mode_IN_FLOATING;
    GPIO_Init(GPIOA, &gpio);

    usart.USART_BaudRate = baudrate;
    usart.USART_WordLength = USART_WordLength_8b;
    usart.USART_StopBits = USART_StopBits_1;
    usart.USART_Parity = USART_Parity_No;
    usart.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
    usart.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
    USART_Init(USART1, &usart);

    USART_Cmd(USART1, ENABLE);
}


void uart_debug_send_char(char c)
{
    while (USART_GetFlagStatus(USART1, USART_FLAG_TXE) == RESET);
    USART_SendData(USART1, c);
}

void uart_debug_send_string(const char* str)
{
    while (*str) {
        uart_debug_send_char(*str++);
    }
}

void uart_esp32_init(baudrate_t baudrate)
{
    GPIO_InitTypeDef gpio;
    USART_InitTypeDef usart;

    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA |
                           RCC_APB2Periph_AFIO,
                           ENABLE);

    RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2, ENABLE);

    gpio.GPIO_Pin = GPIO_Pin_2;
    gpio.GPIO_Mode = GPIO_Mode_AF_PP;
    gpio.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOA, &gpio);

    gpio.GPIO_Pin = GPIO_Pin_3;
    gpio.GPIO_Mode = GPIO_Mode_IN_FLOATING;
    GPIO_Init(GPIOA, &gpio);

    usart.USART_BaudRate = baudrate;
    usart.USART_WordLength = USART_WordLength_8b;
    usart.USART_StopBits = USART_StopBits_1;
    usart.USART_Parity = USART_Parity_No;
    usart.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
    usart.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
    USART_Init(USART2, &usart);

    USART_Cmd(USART2, ENABLE);
}


void uart_esp32_send_char(char c)
{
    while (USART_GetFlagStatus(USART2, USART_FLAG_TXE) == RESET);
    USART_SendData(USART2, c);
}


void uart_esp32_send_string(const char* str)
{
    while (*str) {
        uart_esp32_send_char(*str++);
    }
}

uint8_t uart_esp32_available(void)
{
    return USART_GetFlagStatus(USART2, USART_FLAG_RXNE) == SET;
}

char uart_esp32_receive_char(void)
{
    while (USART_GetFlagStatus(USART2, USART_FLAG_RXNE) == RESET);

    return (char)USART_ReceiveData(USART2);
}

void uart_esp32_receive_string(char *buffer, uint16_t max_len)
{
    uint16_t i = 0;
    char c;

    if (buffer == 0 || max_len == 0) {
        return;
    }

    while (i < (max_len - 1)) {
        c = uart_esp32_receive_char();

        if (c == '\n') {
            break;
        }

        if (c == '\r') {
            continue;
        }

        buffer[i++] = c;
    }

    buffer[i] = '\0';
}














