PROJECT := PRINTER_3D_STM

MCU := cortex-m3
LD_SCRIPT := linker/stm32f103c8.ld
STARTUP_FILE := src/startup_stm32f10x_md.s

PREFIX := arm-none-eabi
CC := $(PREFIX)-gcc
AS := $(PREFIX)-gcc
OBJCOPY := $(PREFIX)-objcopy
SIZE := $(PREFIX)-size

BUILD_DIR := build
OBJ_DIR := $(BUILD_DIR)/obj

SRC_DIRS := \
	src \
	Libraries/STM32F10x_StdPeriph_Driver/src

INC_DIRS := \
	src \
	Libraries/CMSIS/Include \
	Libraries/CMSIS/Device/ST/STM32F10x/Include \
	Libraries/STM32F10x_StdPeriph_Driver/inc

INCLUDES := $(addprefix -I,$(INC_DIRS))

C_SOURCES := $(filter-out src/startup_stm32f103c8.c,$(foreach dir,$(SRC_DIRS),$(wildcard $(dir)/*.c)))
ASM_SOURCES := $(STARTUP_FILE)
C_OBJECTS := $(addprefix $(OBJ_DIR)/,$(notdir $(C_SOURCES:.c=.o)))
ASM_OBJECTS := $(addprefix $(OBJ_DIR)/,$(notdir $(ASM_SOURCES:.s=.o)))
OBJECTS := $(C_OBJECTS) $(ASM_OBJECTS)

vpath %.c $(SRC_DIRS)
vpath %.s $(dir $(STARTUP_FILE))

CFLAGS := \
	-mcpu=$(MCU) \
	-mthumb \
	-std=c11 \
	-Wall \
	-O0 \
	-g3 \
	-DSTM32F10X_MD \
	-DUSE_STDPERIPH_DRIVER \
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

.PHONY: all clean flash erase info

all: $(BUILD_DIR)/$(PROJECT).elf $(BUILD_DIR)/$(PROJECT).bin $(BUILD_DIR)/$(PROJECT).hex
	@$(SIZE) $(BUILD_DIR)/$(PROJECT).elf

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

clean:
	@echo "Cleaning"
	@rm -rf $(BUILD_DIR)

flash: $(BUILD_DIR)/$(PROJECT).bin
	@st-flash write $(BUILD_DIR)/$(PROJECT).bin 0x08000000

erase:
	@st-flash erase

info:
	@echo "Project: $(PROJECT)"
	@echo "MCU: STM32F103C8 / $(MCU)"
	@echo "C sources: $(C_SOURCES)"
	@echo "ASM sources: $(ASM_SOURCES)"
	@echo "Includes: $(INC_DIRS)"
