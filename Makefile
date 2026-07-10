PROJECT := PRINTER_3D_STM
BOOTLOADER_PROJECT := bootloader

MCU := cortex-m3
LD_SCRIPT := linker/stm32f103c8.ld
STARTUP_FILE := src/startup_stm32f10x_md.s
VECT_TAB_OFFSET ?= 0x0

BOOTLOADER_ADDR := 0x08000000
APP_A_ADDR := 0x08004000
APP_B_ADDR := 0x08009C00

PREFIX := arm-none-eabi
CC := $(PREFIX)-gcc
AS := $(PREFIX)-gcc
OBJCOPY := $(PREFIX)-objcopy
SIZE := $(PREFIX)-size
ST_FLASH ?= st-flash

BUILD_DIR := build
OBJ_DIR := $(BUILD_DIR)/obj
BOOTLOADER_BUILD_DIR := build_bootloader
BOOTLOADER_OBJ_DIR := $(BOOTLOADER_BUILD_DIR)/obj
APP_A_BUILD_DIR := build_app_a
APP_A_OBJ_DIR := $(APP_A_BUILD_DIR)/obj
APP_B_BUILD_DIR := build_app_b
APP_B_OBJ_DIR := $(APP_B_BUILD_DIR)/obj

SRC_DIRS := \
	src \
	Libraries/STM32F10x_StdPeriph_Driver/src

INC_DIRS := \
	src \
	app/inc \
	hardware/inc \
	middle/inc \
	Libraries/CMSIS/Include \
	Libraries/CMSIS/Device/ST/STM32F10x/Include \
	Libraries/STM32F10x_StdPeriph_Driver/inc \
	third_party/FreeRTOS-Kernel/include \
	third_party/FreeRTOS-Kernel/portable/GCC/ARM_CM3

INCLUDES := $(addprefix -I,$(INC_DIRS))

DRIVER_SOURCES := \
	app/src/app.c \
	hardware/src/uart.c \
	hardware/src/led.c \
	middle/src/ota.c

FREERTOS_SOURCES := \
	third_party/FreeRTOS-Kernel/tasks.c \
	third_party/FreeRTOS-Kernel/queue.c \
	third_party/FreeRTOS-Kernel/list.c \
	third_party/FreeRTOS-Kernel/timers.c \
	third_party/FreeRTOS-Kernel/event_groups.c \
	third_party/FreeRTOS-Kernel/stream_buffer.c \
	third_party/FreeRTOS-Kernel/portable/GCC/ARM_CM3/port.c \
	third_party/FreeRTOS-Kernel/portable/MemMang/heap_4.c

C_SOURCES := $(filter-out src/startup_stm32f103c8.c,$(foreach dir,$(SRC_DIRS),$(wildcard $(dir)/*.c))) $(DRIVER_SOURCES) $(FREERTOS_SOURCES)
ASM_SOURCES := $(STARTUP_FILE)
C_OBJECTS := $(addprefix $(OBJ_DIR)/,$(notdir $(C_SOURCES:.c=.o)))
ASM_OBJECTS := $(addprefix $(OBJ_DIR)/,$(notdir $(ASM_SOURCES:.s=.o)))
OBJECTS := $(C_OBJECTS) $(ASM_OBJECTS)

BOOTLOADER_INC_DIRS := \
	bootloader/inc \
	src \
	Libraries/CMSIS/Include \
	Libraries/CMSIS/Device/ST/STM32F10x/Include \
	Libraries/STM32F10x_StdPeriph_Driver/inc

BOOTLOADER_INCLUDES := $(addprefix -I,$(BOOTLOADER_INC_DIRS))
BOOTLOADER_C_SOURCES := \
	bootloader/src/main.c \
	bootloader/src/bootloader.c \
	bootloader/src/boot_uart.c \
	Libraries/STM32F10x_StdPeriph_Driver/src/stm32f10x_gpio.c \
	Libraries/STM32F10x_StdPeriph_Driver/src/stm32f10x_rcc.c \
	Libraries/STM32F10x_StdPeriph_Driver/src/stm32f10x_usart.c \
	src/system_stm32f10x.c
BOOTLOADER_ASM_SOURCES := $(STARTUP_FILE)
BOOTLOADER_C_OBJECTS := $(addprefix $(BOOTLOADER_OBJ_DIR)/,$(notdir $(BOOTLOADER_C_SOURCES:.c=.o)))
BOOTLOADER_ASM_OBJECTS := $(addprefix $(BOOTLOADER_OBJ_DIR)/,$(notdir $(BOOTLOADER_ASM_SOURCES:.s=.o)))
BOOTLOADER_OBJECTS := $(BOOTLOADER_C_OBJECTS) $(BOOTLOADER_ASM_OBJECTS)

APP_A_C_OBJECTS := $(addprefix $(APP_A_OBJ_DIR)/,$(notdir $(C_SOURCES:.c=.o)))
APP_A_ASM_OBJECTS := $(addprefix $(APP_A_OBJ_DIR)/,$(notdir $(ASM_SOURCES:.s=.o)))
APP_A_OBJECTS := $(APP_A_C_OBJECTS) $(APP_A_ASM_OBJECTS)

APP_B_C_OBJECTS := $(addprefix $(APP_B_OBJ_DIR)/,$(notdir $(C_SOURCES:.c=.o)))
APP_B_ASM_OBJECTS := $(addprefix $(APP_B_OBJ_DIR)/,$(notdir $(ASM_SOURCES:.s=.o)))
APP_B_OBJECTS := $(APP_B_C_OBJECTS) $(APP_B_ASM_OBJECTS)

vpath %.c $(SRC_DIRS) app/src hardware/src middle/src bootloader/src third_party/FreeRTOS-Kernel third_party/FreeRTOS-Kernel/portable/GCC/ARM_CM3 third_party/FreeRTOS-Kernel/portable/MemMang
vpath %.s $(dir $(STARTUP_FILE))

CFLAGS := \
	-mcpu=$(MCU) \
	-mthumb \
	-std=c11 \
	-Wall \
	-Os \
	-g3 \
	-DSTM32F10X_MD \
	-DUSE_STDPERIPH_DRIVER \
	-DVECT_TAB_OFFSET=$(VECT_TAB_OFFSET) \
	-ffunction-sections \
	-fdata-sections \
	$(INCLUDES)

ASFLAGS := \
	-mcpu=$(MCU) \
	-mthumb \
	-x assembler-with-cpp

LDFLAGS := \
	-mcpu=$(MCU) \
	-mthumb \
	-T$(LD_SCRIPT) \
	-Wl,-Map=$(BUILD_DIR)/$(PROJECT).map \
	-Wl,--gc-sections \
	-lc \
	-lm \
	-lnosys

BOOTLOADER_CFLAGS := \
	-mcpu=$(MCU) \
	-mthumb \
	-std=c11 \
	-Wall \
	-Os \
	-g3 \
	-DSTM32F10X_MD \
	-DUSE_STDPERIPH_DRIVER \
	-DVECT_TAB_OFFSET=0x0 \
	-ffunction-sections \
	-fdata-sections \
	$(BOOTLOADER_INCLUDES)

BOOTLOADER_LDFLAGS := \
	-mcpu=$(MCU) \
	-mthumb \
	-Tbootloader/linker/stm32f103c8_bootloader.ld \
	-Wl,-Map=$(BOOTLOADER_BUILD_DIR)/$(BOOTLOADER_PROJECT).map \
	-Wl,--gc-sections \
	-lc \
	-lm \
	-lnosys

APP_A_CFLAGS := $(filter-out -DVECT_TAB_OFFSET=%,$(CFLAGS)) -DVECT_TAB_OFFSET=0x4000
APP_B_CFLAGS := $(filter-out -DVECT_TAB_OFFSET=%,$(CFLAGS)) -DVECT_TAB_OFFSET=0x9C00

APP_A_LDFLAGS := \
	-mcpu=$(MCU) \
	-mthumb \
	-Tlinker/stm32f103c8_app_a.ld \
	-Wl,-Map=$(APP_A_BUILD_DIR)/$(PROJECT)_app_a.map \
	-Wl,--gc-sections \
	-lc \
	-lm \
	-lnosys

APP_B_LDFLAGS := \
	-mcpu=$(MCU) \
	-mthumb \
	-Tlinker/stm32f103c8_app_b.ld \
	-Wl,-Map=$(APP_B_BUILD_DIR)/$(PROJECT)_app_b.map \
	-Wl,--gc-sections \
	-lc \
	-lm \
	-lnosys

.PHONY: help all bootloader app_a app_b ota_images flash flash_legacy flash_bootloader flash_app_a flash_app_b flash_all erase clean info

help:
	@echo "Available make targets:"
	@echo "  make help              Show this help"
	@echo "  make all               Build legacy image: $(BUILD_DIR)/$(PROJECT).bin"
	@echo "  make bootloader        Build bootloader at $(BOOTLOADER_ADDR)"
	@echo "  make app_a             Build application for Slot A at $(APP_A_ADDR)"
	@echo "  make app_b             Build application for Slot B at $(APP_B_ADDR)"
	@echo "  make ota_images        Build bootloader, App A, and App B"
	@echo "  make flash             Build and flash App A only"
	@echo "  make flash_bootloader  Build and flash bootloader only"
	@echo "  make flash_app_a       Build and flash App A only"
	@echo "  make flash_app_b       Build and flash App B only"
	@echo "  make flash_all         Build and flash bootloader, App A, and App B"
	@echo "  make flash_legacy      Build and flash legacy image to 0x08000000"
	@echo "  make erase             Erase full chip with st-flash"
	@echo "  make clean             Remove all build folders"
	@echo "  make info              Print project/build configuration"

all: $(BUILD_DIR)/$(PROJECT).elf $(BUILD_DIR)/$(PROJECT).bin $(BUILD_DIR)/$(PROJECT).hex
	@$(SIZE) $(BUILD_DIR)/$(PROJECT).elf

bootloader: $(BOOTLOADER_BUILD_DIR)/$(BOOTLOADER_PROJECT).elf $(BOOTLOADER_BUILD_DIR)/$(BOOTLOADER_PROJECT).bin $(BOOTLOADER_BUILD_DIR)/$(BOOTLOADER_PROJECT).hex
	@$(SIZE) $(BOOTLOADER_BUILD_DIR)/$(BOOTLOADER_PROJECT).elf

app_a: $(APP_A_BUILD_DIR)/$(PROJECT)_app_a.elf $(APP_A_BUILD_DIR)/$(PROJECT)_app_a.bin $(APP_A_BUILD_DIR)/$(PROJECT)_app_a.hex
	@$(SIZE) $(APP_A_BUILD_DIR)/$(PROJECT)_app_a.elf

app_b: $(APP_B_BUILD_DIR)/$(PROJECT)_app_b.elf $(APP_B_BUILD_DIR)/$(PROJECT)_app_b.bin $(APP_B_BUILD_DIR)/$(PROJECT)_app_b.hex
	@$(SIZE) $(APP_B_BUILD_DIR)/$(PROJECT)_app_b.elf

ota_images: bootloader app_a app_b

$(BUILD_DIR)/$(PROJECT).elf: $(OBJECTS) | $(BUILD_DIR)
	@echo "Linking $@"
	@$(CC) $(CFLAGS) $^ $(LDFLAGS) -o $@

$(BUILD_DIR)/$(PROJECT).bin: $(BUILD_DIR)/$(PROJECT).elf
	@echo "Creating $@"
	@$(OBJCOPY) -O binary $< $@

$(BUILD_DIR)/$(PROJECT).hex: $(BUILD_DIR)/$(PROJECT).elf
	@echo "Creating $@"
	@$(OBJCOPY) -O ihex $< $@

$(OBJ_DIR)/%.o: %.c | $(OBJ_DIR)
	@echo "Compiling $<"
	@$(CC) $(CFLAGS) -c $< -o $@

$(OBJ_DIR)/%.o: %.s | $(OBJ_DIR)
	@echo "Assembling $<"
	@$(AS) $(ASFLAGS) -c $< -o $@

$(BUILD_DIR) $(OBJ_DIR):
	@mkdir -p $@

$(BOOTLOADER_BUILD_DIR)/$(BOOTLOADER_PROJECT).elf: $(BOOTLOADER_OBJECTS) | $(BOOTLOADER_BUILD_DIR)
	@echo "Linking $@"
	@$(CC) $(BOOTLOADER_CFLAGS) $^ $(BOOTLOADER_LDFLAGS) -o $@

$(BOOTLOADER_BUILD_DIR)/$(BOOTLOADER_PROJECT).bin: $(BOOTLOADER_BUILD_DIR)/$(BOOTLOADER_PROJECT).elf
	@echo "Creating $@"
	@$(OBJCOPY) -O binary $< $@

$(BOOTLOADER_BUILD_DIR)/$(BOOTLOADER_PROJECT).hex: $(BOOTLOADER_BUILD_DIR)/$(BOOTLOADER_PROJECT).elf
	@echo "Creating $@"
	@$(OBJCOPY) -O ihex $< $@

$(BOOTLOADER_OBJ_DIR)/main.o: bootloader/src/main.c | $(BOOTLOADER_OBJ_DIR)
	@echo "Compiling bootloader $<"
	@$(CC) $(BOOTLOADER_CFLAGS) -c $< -o $@

$(BOOTLOADER_OBJ_DIR)/%.o: %.c | $(BOOTLOADER_OBJ_DIR)
	@echo "Compiling bootloader $<"
	@$(CC) $(BOOTLOADER_CFLAGS) -c $< -o $@

$(BOOTLOADER_OBJ_DIR)/%.o: %.s | $(BOOTLOADER_OBJ_DIR)
	@echo "Assembling bootloader $<"
	@$(AS) $(ASFLAGS) -c $< -o $@

$(APP_A_BUILD_DIR)/$(PROJECT)_app_a.elf: $(APP_A_OBJECTS) | $(APP_A_BUILD_DIR)
	@echo "Linking $@"
	@$(CC) $(APP_A_CFLAGS) $^ $(APP_A_LDFLAGS) -o $@

$(APP_A_BUILD_DIR)/$(PROJECT)_app_a.bin: $(APP_A_BUILD_DIR)/$(PROJECT)_app_a.elf
	@echo "Creating $@"
	@$(OBJCOPY) -O binary $< $@

$(APP_A_BUILD_DIR)/$(PROJECT)_app_a.hex: $(APP_A_BUILD_DIR)/$(PROJECT)_app_a.elf
	@echo "Creating $@"
	@$(OBJCOPY) -O ihex $< $@

$(APP_A_OBJ_DIR)/%.o: %.c | $(APP_A_OBJ_DIR)
	@echo "Compiling app A $<"
	@$(CC) $(APP_A_CFLAGS) -c $< -o $@

$(APP_A_OBJ_DIR)/%.o: %.s | $(APP_A_OBJ_DIR)
	@echo "Assembling app A $<"
	@$(AS) $(ASFLAGS) -c $< -o $@

$(APP_B_BUILD_DIR)/$(PROJECT)_app_b.elf: $(APP_B_OBJECTS) | $(APP_B_BUILD_DIR)
	@echo "Linking $@"
	@$(CC) $(APP_B_CFLAGS) $^ $(APP_B_LDFLAGS) -o $@

$(APP_B_BUILD_DIR)/$(PROJECT)_app_b.bin: $(APP_B_BUILD_DIR)/$(PROJECT)_app_b.elf
	@echo "Creating $@"
	@$(OBJCOPY) -O binary $< $@

$(APP_B_BUILD_DIR)/$(PROJECT)_app_b.hex: $(APP_B_BUILD_DIR)/$(PROJECT)_app_b.elf
	@echo "Creating $@"
	@$(OBJCOPY) -O ihex $< $@

$(APP_B_OBJ_DIR)/%.o: %.c | $(APP_B_OBJ_DIR)
	@echo "Compiling app B $<"
	@$(CC) $(APP_B_CFLAGS) -c $< -o $@

$(APP_B_OBJ_DIR)/%.o: %.s | $(APP_B_OBJ_DIR)
	@echo "Assembling app B $<"
	@$(AS) $(ASFLAGS) -c $< -o $@

$(BOOTLOADER_BUILD_DIR) $(BOOTLOADER_OBJ_DIR) $(APP_A_BUILD_DIR) $(APP_A_OBJ_DIR) $(APP_B_BUILD_DIR) $(APP_B_OBJ_DIR):
	@mkdir -p $@

clean:
	@echo "Cleaning"
	@rm -rf $(BUILD_DIR) $(BOOTLOADER_BUILD_DIR) $(APP_A_BUILD_DIR) $(APP_B_BUILD_DIR)

flash: flash_app_a

flash_legacy: $(BUILD_DIR)/$(PROJECT).bin
	@$(ST_FLASH) --reset write $(BUILD_DIR)/$(PROJECT).bin 0x08000000

flash_bootloader: $(BOOTLOADER_BUILD_DIR)/$(BOOTLOADER_PROJECT).bin
	@$(ST_FLASH) --reset write $(BOOTLOADER_BUILD_DIR)/$(BOOTLOADER_PROJECT).bin $(BOOTLOADER_ADDR)

flash_app_a: $(APP_A_BUILD_DIR)/$(PROJECT)_app_a.bin
	@$(ST_FLASH) --reset write $(APP_A_BUILD_DIR)/$(PROJECT)_app_a.bin $(APP_A_ADDR)

flash_app_b: $(APP_B_BUILD_DIR)/$(PROJECT)_app_b.bin
	@$(ST_FLASH) --reset write $(APP_B_BUILD_DIR)/$(PROJECT)_app_b.bin $(APP_B_ADDR)

flash_all: ota_images
	@$(ST_FLASH) write $(APP_A_BUILD_DIR)/$(PROJECT)_app_a.bin $(APP_A_ADDR)
	@$(ST_FLASH) write $(APP_B_BUILD_DIR)/$(PROJECT)_app_b.bin $(APP_B_ADDR)
	@$(ST_FLASH) --reset write $(BOOTLOADER_BUILD_DIR)/$(BOOTLOADER_PROJECT).bin $(BOOTLOADER_ADDR)

erase:
	@$(ST_FLASH) erase

info:
	@echo "Project: $(PROJECT)"
	@echo "MCU: STM32F103C8 / $(MCU)"
	@echo "Bootloader address: $(BOOTLOADER_ADDR)"
	@echo "App A address: $(APP_A_ADDR)"
	@echo "App B address: $(APP_B_ADDR)"
	@echo "C sources: $(C_SOURCES)"
	@echo "ASM sources: $(ASM_SOURCES)"
	@echo "Includes: $(INC_DIRS)"
