#include "stm32f10x.h"
#include "stm32f10x_gpio.h"
#include "stm32f10x_rcc.h"
#include "uart.h"
#include "led.h"

#define BAUDRATE 115200 

static void delay_ms(uint32_t ms)
{
    while (ms--) {
        for (volatile uint32_t i = 0; i < 8000u; i++) {
        }
    }
}

int main(void)
{

    char rx_buf[128];
    uart_debug_init(BAUDRATE); 
    uart_esp32_init(BAUDRATE);
    

    uart_debug_send_string("STM32 UART test start\r\n");
    while (1) {
        uart_esp32_receive_string(rx_buf, sizeof(rx_buf));
        // uart_debug_send_string("QUANGHA TEST UART_DEBUG\r\n");
        uart_debug_send_string("ESP32 -> STM32: ");
        uart_debug_send_string(rx_buf);
        uart_debug_send_string("\r\n");
        delay_ms(1000);
    }
}
