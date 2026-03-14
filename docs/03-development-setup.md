# Development Environment Setup

## Overview

Since you don't have physical RISC-V hardware (like E6A), we'll use QEMU to emulate a RISC-V system. QEMU provides an excellent development and testing environment that closely mimics real hardware behavior.

---

## 🖥️ Emulation Options for RISC-V E6A

### Why QEMU is the Best Option

| Option | Pros | Cons | Recommendation |
|--------|------|------|----------------|
| **QEMU** | Free, excellent debugging, widely used | Not cycle-accurate | ✅ **Primary choice** |
| **Spike** | Official RISC-V simulator, accurate | Less peripheral support | Good for ISA testing |
| **Renode** | Multi-node simulation, good for IoT | Steeper learning curve | Advanced scenarios |
| **Verilator** | Cycle-accurate, uses Verilog | Slow, complex setup | Hardware verification |

### QEMU virt Machine vs. Real E6A

The QEMU `virt` machine provides:
- Similar RV32IMAC instruction set
- UART for console I/O
- Timer (CLINT) for scheduling
- Interrupt controller (PLIC)
- Configurable memory

**Key Difference**: Real E6A has M-mode only; QEMU virt can support M/S/U modes. We'll configure our OS to work in M-mode only to stay compatible with E6A.

---

## 🛠️ Installation Guide (Windows)

### Step 1: Install RISC-V GNU Toolchain

#### Option A: Pre-built Binaries (Recommended)

1. Download from SiFive:
   - Visit: https://www.sifive.com/software
   - Download: "GNU Toolchain — Embedded"
   - Or direct from: https://github.com/sifive/freedom-tools/releases

2. Extract to a folder, e.g., `C:\riscv-toolchain`

3. Add to PATH:
   ```cmd
   setx PATH "%PATH%;C:\riscv-toolchain\bin"
   ```

4. Verify installation:
   ```cmd
   riscv64-unknown-elf-gcc --version
   ```

#### Option B: Using MSYS2

```bash
# Install MSYS2 from https://www.msys2.org/
# Open MSYS2 MINGW64 terminal

pacman -Syu
pacman -S mingw-w64-x86_64-riscv64-unknown-elf-gcc
pacman -S mingw-w64-x86_64-riscv64-unknown-elf-gdb
```

### Step 2: Install QEMU

#### Option A: Pre-built (Recommended)

1. Download QEMU for Windows:
   - Visit: https://www.qemu.org/download/#windows
   - Or: https://qemu.weilnetz.de/w64/

2. Install and add to PATH:
   ```cmd
   setx PATH "%PATH%;C:\Program Files\qemu"
   ```

3. Verify installation:
   ```cmd
   qemu-system-riscv32 --version
   ```

#### Option B: Using Chocolatey

```powershell
choco install qemu
```

### Step 3: Install Make (Build Tool)

```powershell
# Using Chocolatey
choco install make

# Or using MSYS2
pacman -S make
```

### Step 4: Install GDB (Debugger)

The RISC-V toolchain includes GDB:
```cmd
riscv64-unknown-elf-gdb --version
```

---

## 🛠️ Installation Guide (Linux)

### Ubuntu/Debian

```bash
# Update package list
sudo apt update

# Install RISC-V toolchain
sudo apt install gcc-riscv64-unknown-elf
sudo apt install binutils-riscv64-unknown-elf
sudo apt install gdb-multiarch

# Install QEMU
sudo apt install qemu-system-misc

# Verify
riscv64-unknown-elf-gcc --version
qemu-system-riscv32 --version
```

### Arch Linux

```bash
sudo pacman -S riscv64-elf-gcc riscv64-elf-binutils
sudo pacman -S qemu-system-riscv
```

### Building from Source (if needed)

```bash
# Clone RISC-V GNU Toolchain
git clone https://github.com/riscv-collab/riscv-gnu-toolchain
cd riscv-gnu-toolchain

# Install dependencies
sudo apt install autoconf automake autotools-dev curl python3 \
    libmpc-dev libmpfr-dev libgmp-dev gawk build-essential \
    bison flex texinfo gperf libtool patchutils bc zlib1g-dev \
    libexpat-dev ninja-build

# Configure for embedded (newlib)
./configure --prefix=/opt/riscv --with-arch=rv32imac --with-abi=ilp32
make -j$(nproc)

# Add to PATH
echo 'export PATH=/opt/riscv/bin:$PATH' >> ~/.bashrc
source ~/.bashrc
```

---

## 🚀 QEMU Usage for kAsH-OS

### Basic Command

```bash
# Run bare-metal RISC-V 32-bit program
qemu-system-riscv32 -machine virt \
    -cpu rv32 \
    -m 128M \
    -nographic \
    -bios none \
    -kernel kernel.elf
```

### Command Options Explained

| Option | Description |
|--------|-------------|
| `-machine virt` | Use QEMU's generic RISC-V virtual machine |
| `-cpu rv32` | 32-bit RISC-V CPU (matches E6A) |
| `-m 128M` | 128MB RAM (adjustable) |
| `-nographic` | No graphical window, use console |
| `-bios none` | No bootloader, start directly at kernel |
| `-kernel file` | Load kernel ELF/binary |
| `-s` | Enable GDB server on port 1234 |
| `-S` | Freeze CPU at startup (for debugging) |

### Debugging with GDB

Terminal 1 - Start QEMU with GDB server:
```bash
qemu-system-riscv32 -machine virt -cpu rv32 -m 128M -nographic \
    -bios none -kernel kernel.elf -s -S
```

Terminal 2 - Connect GDB:
```bash
riscv64-unknown-elf-gdb kernel.elf
(gdb) target remote localhost:1234
(gdb) break kmain
(gdb) continue
```

### Useful GDB Commands

```gdb
# Breakpoints
break kmain              # Break at function
break *0x80000000        # Break at address
delete 1                 # Delete breakpoint 1

# Execution
continue                 # Continue execution
stepi                    # Single instruction step
step                     # Step into function
next                     # Step over function
finish                   # Run until return

# Inspection
info registers           # Show all registers
print $a0                # Print register a0
x/10i $pc                # Show 10 instructions at PC
x/10x $sp                # Show 10 words at SP
info mem                 # Memory regions

# Assembly
layout asm               # Show assembly view
layout regs              # Show registers
layout split             # Split view
```

---

## 📁 Project Structure Setup

Create the following directory structure:

```
kAsH-OS/
├── Makefile             # Build configuration
├── linker.ld            # Linker script
├── src/
│   ├── boot/
│   │   └── startup.S    # Assembly startup code
│   ├── kernel/
│   │   ├── main.c       # Kernel main
│   │   └── uart.c       # UART driver
│   └── include/
│       ├── types.h      # Type definitions
│       └── uart.h       # UART header
├── docs/                # Documentation
├── tools/               # Helper scripts
└── build/               # Build output (generated)
```

---

## 📝 Sample Makefile

```makefile
# Toolchain
CROSS_COMPILE = riscv64-unknown-elf-
CC = $(CROSS_COMPILE)gcc
AS = $(CROSS_COMPILE)as
LD = $(CROSS_COMPILE)ld
OBJCOPY = $(CROSS_COMPILE)objcopy
OBJDUMP = $(CROSS_COMPILE)objdump

# Flags
CFLAGS = -march=rv32imac -mabi=ilp32 -mcmodel=medany
CFLAGS += -ffreestanding -nostdlib -nostartfiles
CFLAGS += -O0 -g -Wall -Wextra
CFLAGS += -Isrc/include

ASFLAGS = -march=rv32imac -mabi=ilp32

LDFLAGS = -T linker.ld -nostdlib

# Source files
SRC_C = $(wildcard src/kernel/*.c)
SRC_S = $(wildcard src/boot/*.S)
OBJ = $(SRC_C:.c=.o) $(SRC_S:.S=.o)

# Target
TARGET = kernel

all: $(TARGET).elf $(TARGET).bin

$(TARGET).elf: $(OBJ)
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ $^

$(TARGET).bin: $(TARGET).elf
	$(OBJCOPY) -O binary $< $@

%.o: %.c
	$(CC) $(CFLAGS) -c -o $@ $<

%.o: %.S
	$(CC) $(ASFLAGS) -c -o $@ $<

# Run in QEMU
run: $(TARGET).elf
	qemu-system-riscv32 -machine virt -cpu rv32 -m 128M \
		-nographic -bios none -kernel $<

# Debug with QEMU + GDB
debug: $(TARGET).elf
	qemu-system-riscv32 -machine virt -cpu rv32 -m 128M \
		-nographic -bios none -kernel $< -s -S &
	sleep 1
	riscv64-unknown-elf-gdb $< -ex "target remote localhost:1234"

# Disassembly
disasm: $(TARGET).elf
	$(OBJDUMP) -d $< > $(TARGET).asm

clean:
	rm -f $(OBJ) $(TARGET).elf $(TARGET).bin $(TARGET).asm

.PHONY: all run debug disasm clean
```

---

## ✅ Verification Checklist

Run these commands to verify your setup:

```bash
# 1. Check toolchain
riscv64-unknown-elf-gcc --version
# Should show: riscv64-unknown-elf-gcc (SiFive...) ...

# 2. Check QEMU
qemu-system-riscv32 --version
# Should show: QEMU emulator version ...

# 3. Check supported QEMU machines
qemu-system-riscv32 -machine help
# Should list: virt, sifive_e, sifive_u, etc.

# 4. Check QEMU CPUs
qemu-system-riscv32 -cpu help
# Should list available CPU types

# 5. Test QEMU (should hang, Ctrl+A then X to exit)
qemu-system-riscv32 -machine virt -nographic
```

---

## 🔧 VS Code Configuration

### tasks.json

```json
{
    "version": "2.0.0",
    "tasks": [
        {
            "label": "Build",
            "type": "shell",
            "command": "make",
            "group": {
                "kind": "build",
                "isDefault": true
            }
        },
        {
            "label": "Run QEMU",
            "type": "shell",
            "command": "make run",
            "group": "test"
        },
        {
            "label": "Clean",
            "type": "shell",
            "command": "make clean"
        }
    ]
}
```

### launch.json (for debugging)

```json
{
    "version": "0.2.0",
    "configurations": [
        {
            "name": "Debug kAsH-OS",
            "type": "cppdbg",
            "request": "launch",
            "program": "${workspaceFolder}/kernel.elf",
            "miDebuggerServerAddress": "localhost:1234",
            "miDebuggerPath": "riscv64-unknown-elf-gdb",
            "cwd": "${workspaceFolder}",
            "MIMode": "gdb",
            "setupCommands": [
                {
                    "text": "set architecture riscv:rv32"
                }
            ]
        }
    ]
}
```

---

## 🎯 Quick Start Commands

```bash
# Build the kernel
make

# Run in QEMU
make run

# Debug with GDB
make debug

# Generate disassembly
make disasm

# Clean build artifacts
make clean
```

---

## 🔗 Resources

- [QEMU RISC-V Documentation](https://www.qemu.org/docs/master/system/target-riscv.html)
- [QEMU virt Machine](https://www.qemu.org/docs/master/system/riscv/virt.html)
- [GDB Manual](https://sourceware.org/gdb/current/onlinedocs/gdb/)
- [SiFive Freedom Tools](https://github.com/sifive/freedom-tools)
