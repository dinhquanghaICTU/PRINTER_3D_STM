#include "stm32f10x.h"
#include "misc.h"
#include "uart.h"
#include "led.h"
#include "FreeRTOS.h"
#include "task.h"

#define BAUDRATE 115200 

static void led_task(void *argument)
{
    (void)argument;

    uart_debug_send_string("led\r\n");

    while (1) {
        led_on();
        vTaskDelay(pdMS_TO_TICKS(500));
        led_off();
        vTaskDelay(pdMS_TO_TICKS(500));
    }
}

int main(void)
{
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_4);

    uart_debug_init(BAUDRATE);
    uart_esp32_init(BAUDRATE);
    led_init();
    uart_debug_send_string("oke\r\n");

    if (xTaskCreate(led_task, "led", 128, NULL, tskIDLE_PRIORITY + 1, NULL) != pdPASS) {
        uart_debug_send_string("led task create fail\r\n");
        while (1) {
        }
    }

    uart_debug_send_string("scheduler start\r\n");
    vTaskStartScheduler();

    uart_debug_send_string("scheduler fail\r\n");
    while (1) {
    }
}
