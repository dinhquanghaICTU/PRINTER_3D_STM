#include "stm32f10x.h"
#include "uart.h"
#include "led.h"
#include "FreeRTOS.h"
#include "task.h"

static void debug_send_hex32(uint32_t value)
{
    static const char hex[] = "0123456789ABCDEF";

    uart_debug_send_string("0x");
    for (int shift = 28; shift >= 0; shift -= 4) {
        uart_debug_send_char(hex[(value >> shift) & 0x0FUL]);
    }
}

static void halt_with_blink(void)
{
    taskDISABLE_INTERRUPTS();

    while (1) {
        led_on();
        for (volatile uint32_t i = 0; i < 400000u; i++) {
        }
        led_off();
        for (volatile uint32_t i = 0; i < 400000u; i++) {
        }
    }
}

static void fault_halt(const char *name)
{
    uart_debug_send_string(name);
    uart_debug_send_string("\r\nCFSR ");
    debug_send_hex32(SCB->CFSR);
    uart_debug_send_string("\r\nHFSR ");
    debug_send_hex32(SCB->HFSR);
    uart_debug_send_string("\r\n");

    halt_with_blink();
}

void HardFault_Handler(void)
{
    fault_halt("hardfault");
}

void MemManage_Handler(void)
{
    fault_halt("memmanage");
}

void BusFault_Handler(void)
{
    fault_halt("busfault");
}

void UsageFault_Handler(void)
{
    fault_halt("usagefault");
}

void vApplicationMallocFailedHook(void)
{
    uart_debug_send_string("malloc failed\r\n");
    halt_with_blink();
}

void vApplicationStackOverflowHook(TaskHandle_t task, char *task_name)
{
    (void)task;
    (void)task_name;

    uart_debug_send_string("stack overflow\r\n");
    halt_with_blink();
}

void vAssertCalled(const char *file, uint32_t line)
{
    (void)file;

    uart_debug_send_string("assert failed line ");
    debug_send_hex32(line);
    uart_debug_send_string("\r\n");

    halt_with_blink();
}
