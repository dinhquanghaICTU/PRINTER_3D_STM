#include "bootloader.h"
#include "boot_config.h"
#include "boot_uart.h"
#include "stm32f10x.h"

static const ota_metadata_t *metadata = (const ota_metadata_t *)OTA_METADATA_ADDR;

__attribute__((naked, noreturn)) static void start_app(uint32_t app_stack, uint32_t app_reset_handler)
{
    (void)app_stack;
    (void)app_reset_handler;

    __asm volatile (
        "msr msp, r0        \n"
        "movs r2, #0        \n"
        "msr control, r2    \n"
        "isb                \n"
        "cpsie i            \n"
        "bx r1              \n"
    );
}

static void bootloader_delay_ms(uint32_t ms)
{
    while (ms--) {
        for (volatile uint32_t i = 0; i < 8000u; i++) {
        }
    }
}

static uint32_t slot_to_addr(boot_slot_t slot)
{
    if (slot == BOOT_SLOT_A) {
        boot_uart_send_string("select addr A\r\n");
        return APP_A_START_ADDR;
    }

    if (slot == BOOT_SLOT_B) {
         boot_uart_send_string("select addr B\r\n");
        return APP_B_START_ADDR;
    }

    return 0;
}

static int is_valid_stack(uint32_t stack_addr)
{
    return (stack_addr >= SRAM_START_ADDR) && (stack_addr <= SRAM_END_ADDR);
}

static int is_valid_reset_handler(uint32_t reset_addr, uint32_t app_addr, uint32_t app_size)
{
    return (reset_addr >= app_addr) && (reset_addr < (app_addr + app_size)) && ((reset_addr & 1UL) == 1UL);
}

static int app_is_valid(uint32_t app_addr, uint32_t app_size)
{
    uint32_t stack_addr = *(volatile uint32_t *)app_addr;
    uint32_t reset_addr = *(volatile uint32_t *)(app_addr + 4UL);

    return is_valid_stack(stack_addr) && is_valid_reset_handler(reset_addr, app_addr, app_size);
}

static int slot_is_valid(boot_slot_t slot)
{
    
    if (slot == BOOT_SLOT_A) {
        boot_uart_send_string("check 1\r\n");
        return app_is_valid(APP_A_START_ADDR, APP_A_SIZE);
    }

    if (slot == BOOT_SLOT_B) {
        boot_uart_send_string("check 2\r\n");
        return app_is_valid(APP_B_START_ADDR, APP_B_SIZE);
    }
    boot_uart_send_string("check 3\r\n");

    return 0;
}

static boot_slot_t get_default_slot(void)
{   
    
    if (metadata->magic == OTA_METADATA_MAGIC) {
        // boot_uart_send_string("da vao day first\r\n");
        if ((metadata->active_slot == BOOT_SLOT_A) || (metadata->active_slot == BOOT_SLOT_B)) {
            return (boot_slot_t)metadata->active_slot;
        }
    }

    return BOOT_SLOT_A;
}

static boot_slot_t get_pending_slot(void)
{

    
    if (metadata->magic != OTA_METADATA_MAGIC) {
        boot_uart_send_string("non version  \r\n");
        return BOOT_SLOT_NONE;
    }

    if (metadata->state != OTA_STATE_PENDING) {
        boot_uart_send_string("non pending \r\n");
        return BOOT_SLOT_NONE;
    }

    if ((metadata->pending_slot == BOOT_SLOT_A) || (metadata->pending_slot == BOOT_SLOT_B)) {
        boot_uart_send_string("go to defauld \r\n");
        
        return (boot_slot_t)metadata->pending_slot;
    }

    return BOOT_SLOT_NONE;
}

static void jump_to_app(uint32_t app_addr)
{
    uint32_t app_stack = *(volatile uint32_t *)app_addr;
    uint32_t app_reset_handler = *((volatile uint32_t *)(app_addr + 4UL));

    boot_uart_send_string("Jumping to application...\r\n");
    bootloader_delay_ms(100);

    __disable_irq();

    SysTick->CTRL = 0;
    SysTick->LOAD = 0;
    SysTick->VAL = 0;

    SCB->VTOR = app_addr;

    __DSB();
    __ISB();

    start_app(app_stack, app_reset_handler);
}

void bootloader_run(void)
{
    boot_uart_init(115200);

    boot_slot_t pending_slot = get_pending_slot();
    boot_slot_t active_slot = get_default_slot();
    boot_uart_send_string("\r\nbootloader start\r\n");

    if (slot_is_valid(pending_slot)) {
        boot_uart_send_string("jump pending slot ");
        boot_uart_send_hex32(slot_to_addr(pending_slot));
        boot_uart_send_string("\r\n");
        jump_to_app(slot_to_addr(pending_slot));
    }

    if (slot_is_valid(active_slot)) {
        boot_uart_send_string("jump active slot ");
        boot_uart_send_hex32(slot_to_addr(active_slot));
        boot_uart_send_string("\r\n");
        jump_to_app(slot_to_addr(active_slot));
    }

    if ((active_slot != BOOT_SLOT_A) && slot_is_valid(BOOT_SLOT_A)) {
        boot_uart_send_string("fallback app A\r\n");
        jump_to_app(APP_A_START_ADDR);
    }

    if ((active_slot != BOOT_SLOT_B) && slot_is_valid(BOOT_SLOT_B)) {
        boot_uart_send_string("fallback app B\r\n");
        jump_to_app(APP_B_START_ADDR);
    }

    boot_uart_send_string("no valid app\r\n");
}
