# kAsH-OS: RISC-V Learning Operating System

A bare-metal operating system for RISC-V architecture, designed as a learning project to understand RISC-V architecture and RTOS concepts in depth.

## 🎯 Project Goals

1. **Understand RISC-V Architecture** - Registers, instruction set, privilege modes, memory management
2. **Master RTOS Concepts** - Task scheduling, context switching, inter-task communication
3. **Build a Functional RTOS** - From bootloader to a working multi-tasking system
4. **Document Everything** - Create comprehensive notes for future reference

## 📁 Project Structure

```
kAsH-OS/
├── docs/                    # Documentation and learning notes
│   ├── 01-riscv-architecture.md
│   ├── 02-rtos-concepts.md
│   ├── 03-development-setup.md
│   └── learning-roadmap.md
├── src/                     # Source code
│   ├── boot/               # Bootloader and startup code
│   ├── kernel/             # Kernel core
│   │   ├── scheduler/      # Task scheduler
│   │   ├── memory/         # Memory management
│   │   ├── ipc/           # Inter-process communication
│   │   └── drivers/       # Hardware drivers
│   └── include/            # Header files
├── tools/                   # Build tools and scripts
├── tests/                   # Test code
└── examples/               # Example applications
```

## 🛠️ Target Hardware

- **Architecture**: RISC-V RV32IMAC (32-bit with Integer, Multiply, Atomic, Compressed extensions)
- **Target Board**: QEMU virt machine (for development) / SiFive boards
- **Memory Model**: Bare-metal, no MMU initially

## 📚 Learning Phases

| Phase | Topic | Estimated Time |
|-------|-------|----------------|
| 1 | RISC-V Architecture Fundamentals | 2-3 weeks |
| 2 | Development Environment Setup | 1 week |
| 3 | Boot Process & Startup Code | 2 weeks |
| 4 | Basic Kernel & UART Driver | 2 weeks |
| 5 | Interrupt Handling | 2 weeks |
| 6 | Memory Management | 3 weeks |
| 7 | Task Management & Context Switching | 3-4 weeks |
| 8 | Scheduler Implementation | 3-4 weeks |
| 9 | IPC Mechanisms | 2-3 weeks |
| 10 | Advanced Topics & Optimization | 4+ weeks |

**Total Estimated Time: 6-9 months** (part-time, ~10-15 hours/week)

## 🚀 Quick Start

```bash
# Clone the repository
git clone https://github.com/KashifSid38/kAsh-OS.git

# Install RISC-V toolchain (see docs/03-development-setup.md)

# Build the OS
make

# Run in QEMU
make run
```

## 📖 Documentation

- [Learning Roadmap](docs/learning-roadmap.md) - Detailed learning plan
- [RISC-V Architecture](docs/01-riscv-architecture.md) - Architecture reference
- [RTOS Concepts](docs/02-rtos-concepts.md) - OS theory and concepts
- [Development Setup](docs/03-development-setup.md) - Environment configuration
- [MMU & Privilege Modes](docs/04-mmu-and-privilege-modes.md) - Virtual memory and user/kernel separation

## 🔗 Resources

### RISC-V
- [RISC-V Specifications](https://riscv.org/technical/specifications/)
- [RISC-V Unprivileged ISA](https://github.com/riscv/riscv-isa-manual)
- [RISC-V Privileged ISA](https://github.com/riscv/riscv-isa-manual)

### OS Development
- [OSDev Wiki](https://wiki.osdev.org/)
- [Writing an OS in Rust (RISC-V)](https://osblog.stephenmarz.com/)
- [xv6-riscv](https://github.com/mit-pdos/xv6-riscv)

### RTOS
- [FreeRTOS](https://www.freertos.org/)
- [Zephyr RTOS](https://www.zephyrproject.org/)

## 📝 License

This project is for educational purposes. MIT License.

## 👤 Author

Learning project by [Your Name]
