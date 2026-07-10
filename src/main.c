#include "stm32f10x.h"
#include "stm32f10x_gpio.h"
#include "stm32f10x_rcc.h"
#include "misc.h"
#include "uart.h"
#include "led.h"
#include "ota.h"
#include "FreeRTOS.h"
#include "task.h"

#define BAUDRATE 115200 

static void led_task(void *argument)
{
    (void)argument;
    // uart_debug_send_string("led done \r\n");
    while (1) {
        led_on();
        vTaskDelay(pdMS_TO_TICKS(500));
        led_off();
        vTaskDelay(pdMS_TO_TICKS(500));
    }
}

// static void ota_task(void *argument)
// {
//     (void)argument;

//     while (1) {
//         ota_process();
//         vTaskDelay(pdMS_TO_TICKS(1));
//     }
// }

void vApplicationMallocFailedHook(void)
{
    taskDISABLE_INTERRUPTS();
    while (1) {
    }
}

void vApplicationStackOverflowHook(TaskHandle_t task, char *task_name)
{
    (void)task;
    (void)task_name;

    taskDISABLE_INTERRUPTS();
    while (1) {
    }
}

int main(void)
{
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_4);

    uart_debug_init(BAUDRATE);
    uart_esp32_init(BAUDRATE);
    led_init();
    uart_debug_send_string("FreeRTOS  meo oke start\r\n");

    if (xTaskCreate(led_task, "led", 128, NULL, tskIDLE_PRIORITY + 1, NULL) != pdPASS) {
        uart_debug_send_string("led task fail\r\n");
        while (1) {
        }
    }

    // if (xTaskCreate(ota_task, "ota", 256, NULL, tskIDLE_PRIORITY + 2, NULL) != pdPASS) {
    //     uart_debug_send_string("ota task fail\r\n");
    //     while (1) {
    //     }
    // }

    vTaskStartScheduler();

    uart_debug_send_string("scheduler fail\r\n");
    while (1) {
    }
}
