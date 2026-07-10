#ifndef BOOT_CONFIG_H
#define BOOT_CONFIG_H

#include <stdint.h>

#define BOOT_FLASH_BASE_ADDR        0x08000000UL
#define BOOTLOADER_START_ADDR       0x08000000UL
#define BOOTLOADER_SIZE             (16UL * 1024UL)

#define APP_A_START_ADDR            0x08004000UL
#define APP_A_SIZE                  (23UL * 1024UL)

#define APP_B_START_ADDR            0x08009C00UL
#define APP_B_SIZE                  (23UL * 1024UL)

#define OTA_METADATA_ADDR           0x0800F800UL
#define OTA_METADATA_SIZE           (2UL * 1024UL)

#define SRAM_START_ADDR             0x20000000UL
#define SRAM_SIZE                   (20UL * 1024UL)
#define SRAM_END_ADDR               (SRAM_START_ADDR + SRAM_SIZE)

#define OTA_METADATA_MAGIC          0x4F544131UL

#endif
