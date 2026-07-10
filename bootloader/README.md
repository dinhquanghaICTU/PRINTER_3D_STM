# Bootloader Template

This folder is for the standalone STM32F103 bootloader.

Memory map:

```text
0x08000000 - 0x08003FFF  Bootloader
0x08004000 - 0x08009BFF  App A
0x08009C00 - 0x0800F7FF  App B
0x0800F800 - 0x0800FFFF  OTA metadata
```

The bootloader should stay small and bare-metal. Do not put FreeRTOS tasks here.

Application OTA flow:

```text
App receives firmware
App writes the inactive slot
App verifies image
App writes OTA metadata as PENDING
App resets MCU
Bootloader verifies pending slot
Bootloader jumps to App A or App B
```

TODO:

- Add metadata flash erase/write helpers.
- Add CRC verification for metadata and image.
- Add confirmed/testing/rollback logic.
- Add a separate Makefile target for bootloader builds.
