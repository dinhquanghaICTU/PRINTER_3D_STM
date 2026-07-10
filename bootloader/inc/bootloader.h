#ifndef BOOTLOADER_H
#define BOOTLOADER_H

#include <stdint.h>

typedef enum {
    BOOT_SLOT_NONE = 0,
    BOOT_SLOT_A = 1,
    BOOT_SLOT_B = 2
} boot_slot_t;

typedef enum {
    OTA_STATE_IDLE = 0,
    OTA_STATE_PENDING = 1,
    OTA_STATE_TESTING = 2,
    OTA_STATE_CONFIRMED = 3,
    OTA_STATE_ROLLBACK = 4
} ota_state_t;

typedef struct {
    uint32_t magic;
    uint32_t active_slot;
    uint32_t pending_slot;
    uint32_t image_size;
    uint32_t image_crc;
    uint32_t state;
    uint32_t sequence;
    uint32_t metadata_crc;
} ota_metadata_t;

void bootloader_run(void);

#endif
