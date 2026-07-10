# Bootloader + FreeRTOS lỗi đã gặp

## 1. Jump từ bootloader sang app bị HardFault

### Triệu chứng

- Bootloader đọc đúng vector của App A:
  - Stack: `0x20005000`
  - Reset handler: `0x08005D59`
- App có chạy tới một đoạn nhưng không vào được `led_task`.
- Debug thấy CPU rơi vào `HardFault_Handler`.
- `CFSR = 0x9200`, `HFSR = 0x40000000`, `BFAR = 0x20005008`.

### Nguyên nhân

Trong `jump_to_app()`, code cũ gọi `__set_MSP(app_stack)` ngay bên trong một hàm C bình thường.

Sau khi đổi MSP sang stack của app, compiler vẫn sinh phần kết thúc hàm như `pop {r4, r5, r6, lr}` hoặc thao tác với stack. Lúc đó stack đã là stack mới của app, chưa có dữ liệu frame của bootloader, nên CPU đọc sai vùng stack và gây HardFault.

### Cách sửa

Tách đoạn nhảy cuối sang một hàm `naked, noreturn`, sau khi set MSP thì branch thẳng sang reset handler của app, không quay lại epilogue của hàm C.

Ví dụ:

```c
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
```

Trong `jump_to_app()` chỉ chuẩn bị:

```c
__disable_irq();
SysTick->CTRL = 0;
SysTick->LOAD = 0;
SysTick->VAL = 0;
SCB->VTOR = app_addr;
__DSB();
__ISB();

start_app(app_stack, app_reset_handler);
```

## 2. `make flash_all` xong không tự vào app, phải bấm reset tay

### Triệu chứng

- Chạy `make flash_all` xong board không tự vào `led_task`.
- Bấm nút reset trên board thì lại vào app và task LED chạy bình thường.

### Nguyên nhân

Target `flash_all` cũ nạp bootloader trước:

```make
st-flash write bootloader.bin 0x08000000
st-flash write app_a.bin 0x08004000
st-flash write app_b.bin 0x08009C00
```

Khi bootloader được nạp trước, MCU có thể reset/chạy bootloader trong lúc App A/App B chưa nạp xong hoặc đang bị ghi dở. Sau khi nạp xong toàn bộ lại không có reset cuối cùng, nên board không tự chạy đúng flow mới.

### Cách sửa

Nạp app trước, bootloader sau cùng, và reset ở lệnh cuối:

```make
flash_all: ota_images
	@$(ST_FLASH) write $(APP_A_BUILD_DIR)/$(PROJECT)_app_a.bin $(APP_A_ADDR)
	@$(ST_FLASH) write $(APP_B_BUILD_DIR)/$(PROJECT)_app_b.bin $(APP_B_ADDR)
	@$(ST_FLASH) --reset write $(BOOTLOADER_BUILD_DIR)/$(BOOTLOADER_PROJECT).bin $(BOOTLOADER_ADDR)
```

Các target flash lẻ cũng nên dùng `--reset`:

```make
flash_app_a:
	@$(ST_FLASH) --reset write app_a.bin 0x08004000
```

Sau khi sửa, chạy:

```bash
make flash_all -j16
```

Board sẽ reset ở cuối quá trình nạp và bootloader sẽ nhảy vào app đúng cách.
