# =============================================================================
# kAsH-OS Makefile
# Target: RISC-V RV32IMAC
# Platform: QEMU virt machine
# =============================================================================
ifeq ($(OS),Windows_NT)
    SHELL := C:/PROGRA~1/Git/usr/bin/bash.exe
    .SHELLFLAGS := -c
endif

# Toolchain configuration
# Windows (Zephyr SDK 0.16.8):  CROSS_COMPILE=.../zephyr-sdk-0.16.8/riscv64-zephyr-elf/bin/riscv64-zephyr-elf-
# macOS (brew):                  CROSS_COMPILE=riscv64-unknown-elf-
# See SETUP.md for full install instructions.
_ZEPHYR_GCC = C:\Users\mksiddi\zephyr-sdk-0.16.8\riscv64-zephyr-elf\bin\riscv64-zephyr-elf-gcc.exe
_ZEPHYR_BIN = C:/Users/mksiddi/zephyr-sdk-0.16.8/riscv64-zephyr-elf/bin/riscv64-zephyr-elf-
ifneq ($(wildcard $(_ZEPHYR_GCC)),)
CROSS_COMPILE ?= $(_ZEPHYR_BIN)
else
CROSS_COMPILE ?= riscv64-unknown-elf-
endif
CC      = $(CROSS_COMPILE)gcc
AS      = $(CROSS_COMPILE)as
LD      = $(CROSS_COMPILE)ld
OBJCOPY = $(CROSS_COMPILE)objcopy
OBJDUMP = $(CROSS_COMPILE)objdump
SIZE    = $(CROSS_COMPILE)size
GDB     = $(CROSS_COMPILE)gdb

# Target name
TARGET = kernel

# Directories
SRC_DIR     = src
BOOT_DIR    = $(SRC_DIR)/boot
KERNEL_DIR  = $(SRC_DIR)/kernel
INCLUDE_DIR = $(SRC_DIR)/include
BUILD_DIR   = build

# Source files
SRC_C = $(wildcard $(KERNEL_DIR)/*.c)
SRC_S = $(wildcard $(BOOT_DIR)/*.S)

# Object files
OBJ_C = $(patsubst $(SRC_DIR)/%.c,$(BUILD_DIR)/%.o,$(SRC_C))
OBJ_S = $(patsubst $(SRC_DIR)/%.S,$(BUILD_DIR)/%.o,$(SRC_S))
OBJ   = $(OBJ_S) $(OBJ_C)

# Compiler flags
ARCH_FLAGS = -march=rv32imac_zicsr -mabi=ilp32

CFLAGS  = $(ARCH_FLAGS)
CFLAGS += -mcmodel=medany
CFLAGS += -ffreestanding
CFLAGS += -fno-common
CFLAGS += -nostdlib
CFLAGS += -nostartfiles
CFLAGS += -fno-builtin
CFLAGS += -fno-stack-protector
CFLAGS += -fno-pie
CFLAGS += -no-pie
CFLAGS += -O0
CFLAGS += -g
CFLAGS += -Wall
CFLAGS += -Wextra
CFLAGS += -Wno-unused-parameter
CFLAGS += -I$(INCLUDE_DIR)

# Assembler flags
ASFLAGS = $(ARCH_FLAGS)
ASFLAGS += -g

# Linker flags
LDFLAGS  = -T linker.ld
LDFLAGS += -nostdlib
LDFLAGS += -nostartfiles
LDFLAGS += -Wl,--no-relax

# QEMU configuration
# Override QEMU if the binary is not in PATH, e.g.:
#   make run QEMU=/usr/local/bin/qemu-system-riscv32
QEMU ?= $(shell which qemu-system-riscv32 2>/dev/null || echo /c/PROGRA~1/qemu/qemu-system-riscv32.exe)
QEMU_FLAGS  = -machine virt
QEMU_FLAGS += -cpu rv32
QEMU_FLAGS += -m 128M
QEMU_FLAGS += -nographic
QEMU_FLAGS += -bios none
QEMU_FLAGS += -kernel $(BUILD_DIR)/$(TARGET).elf

# GDB configuration
GDB_PORT = 1234

# =============================================================================
# Build targets
# =============================================================================

.PHONY: all clean run debug gdb disasm size help

all: $(BUILD_DIR)/$(TARGET).elf $(BUILD_DIR)/$(TARGET).bin
	@echo ""
	@echo "Build complete!"
	@echo "  ELF: $(BUILD_DIR)/$(TARGET).elf"
	@echo "  BIN: $(BUILD_DIR)/$(TARGET).bin"
	@$(SIZE) $(BUILD_DIR)/$(TARGET).elf

# Create build directories
$(BUILD_DIR):
	@mkdir -p $(BUILD_DIR)/boot
	@mkdir -p $(BUILD_DIR)/kernel

# Compile C sources
$(BUILD_DIR)/kernel/%.o: $(KERNEL_DIR)/%.c | $(BUILD_DIR)
	@echo "CC    $<"
	@$(CC) $(CFLAGS) -c -o $@ $<

# Compile assembly sources
$(BUILD_DIR)/boot/%.o: $(BOOT_DIR)/%.S | $(BUILD_DIR)
	@echo "AS    $<"
	@$(CC) $(ASFLAGS) -c -o $@ $<

# Link
$(BUILD_DIR)/$(TARGET).elf: $(OBJ) linker.ld
	@echo "LD    $@"
	@$(CC) $(CFLAGS) $(LDFLAGS) -o $@ $(OBJ)

# Create binary
$(BUILD_DIR)/$(TARGET).bin: $(BUILD_DIR)/$(TARGET).elf
	@echo "BIN   $@"
	@$(OBJCOPY) -O binary $< $@

# =============================================================================
# Run targets
# =============================================================================

# winpty bridges Git Bash's terminal with Windows console apps like QEMU
WINPTY := $(shell which winpty 2>/dev/null)
QEMU_RUN := $(if $(WINPTY),winpty $(QEMU),$(QEMU))

# Run in QEMU
run: $(BUILD_DIR)/$(TARGET).elf
	@echo "Starting QEMU..."
	@echo "Press Ctrl+A then X to exit"
	@echo ""
	$(QEMU_RUN) $(QEMU_FLAGS)

# Run QEMU with GDB server (stopped at start)
debug: $(BUILD_DIR)/$(TARGET).elf
	@echo "Starting QEMU with GDB server on port $(GDB_PORT)..."
	@echo "Connect with: $(GDB) -ex 'target remote localhost:$(GDB_PORT)' $(BUILD_DIR)/$(TARGET).elf"
	@echo "Press Ctrl+A then X to exit QEMU"
	@echo ""
	$(QEMU_RUN) $(QEMU_FLAGS) -s -S

# Connect GDB to running QEMU
gdb: $(BUILD_DIR)/$(TARGET).elf
	$(GDB) $(BUILD_DIR)/$(TARGET).elf \
		-ex "target remote localhost:$(GDB_PORT)" \
		-ex "set disassemble-next-line on"

# =============================================================================
# Utility targets
# =============================================================================

# Generate disassembly
disasm: $(BUILD_DIR)/$(TARGET).elf
	@echo "Generating disassembly..."
	$(OBJDUMP) -d -S $< > $(BUILD_DIR)/$(TARGET).asm
	@echo "Disassembly saved to $(BUILD_DIR)/$(TARGET).asm"

# Show size information
size: $(BUILD_DIR)/$(TARGET).elf
	$(SIZE) -A -x $<

# Clean build artifacts
clean:
	@echo "Cleaning..."
	@rm -rf $(BUILD_DIR)
	@echo "Done"

# =============================================================================
# Help
# =============================================================================

help:
	@echo "kAsH-OS Build System"
	@echo ""
	@echo "Usage: make [target]"
	@echo ""
	@echo "Build targets:"
	@echo "  all      - Build the kernel (default)"
	@echo "  clean    - Remove build artifacts"
	@echo ""
	@echo "Run targets:"
	@echo "  run      - Run kernel in QEMU"
	@echo "  debug    - Run QEMU with GDB server"
	@echo "  gdb      - Connect GDB to running QEMU"
	@echo ""
	@echo "Utility targets:"
	@echo "  disasm   - Generate disassembly"
	@echo "  size     - Show size information"
	@echo "  help     - Show this help"
	@echo ""
	@echo "Configuration:"
	@echo "  CROSS_COMPILE=$(CROSS_COMPILE)"
	@echo "  QEMU=$(QEMU)"
