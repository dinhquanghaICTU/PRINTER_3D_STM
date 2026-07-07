# PRINTER_3D_STM

Bare-metal workspace for STM32F103C8 using STM32F10x Standard Peripheral Library (SPL), the old TrueSTUDIO-style standard library.

## Build

```sh
make
```

Output files:

- `build/PRINTER_3D_STM.elf`
- `build/PRINTER_3D_STM.bin`
- `build/PRINTER_3D_STM.hex`

## Flash with ST-Link

Install `stlink` so `st-flash` is available in PATH, then run:

```sh
make flash
```

The default firmware blinks the Blue Pill LED on `PC13` using SPL APIs such as `RCC_APB2PeriphClockCmd()` and `GPIO_Init()`.
