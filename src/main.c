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
    uart_debug_init(BAUDRATE); 
    uart_esp32_init(BAUDRATE);
    
    while (1) {
        // uart_debug_send_string("QUANGHA TEST UART_DEBUG\r\n");
        uart_esp32_send_string("QUANGHA TEST UART_ESP32\r\n");
        delay_ms(1000);
    }
}
