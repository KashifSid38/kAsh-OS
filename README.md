# kAsH-OS

A bare-metal RISC-V operating system built from scratch as a structured learning project. The goal is twofold: a working multitasking OS on QEMU (eventually ported to real RISC-V hardware), and the systems-engineering judgement to make good design decisions along the way.

This is not a polished product. It is a learning artifact. Expect mistakes, rewrites, and design notes that document why each choice was made (and sometimes unmade).

## Status

Early. The current codebase boots on QEMU `virt`, initializes a 16550 UART, prints a banner, and runs an echo loop. The trap handler decodes RISC-V exception codes but does not yet do anything useful with timer or external interrupts.

This scaffold was AI-generated; Phase 0 is dedicated to re-deriving understanding of every line. See `docs/learning-roadmap.md` for the full plan and `GUIDE.md` for the operating principles that govern how I work through it.

## Quick start

See `SETUP.md` for toolchain installation (Windows + macOS).

```bash
make all      # Build the kernel ELF + binary
make run      # Run in QEMU (Ctrl+A then X to exit)
make debug    # Run in QEMU with GDB server
make gdb      # Connect GDB to a running `make debug` session (in a second terminal)
make clean    # Remove build artifacts
make help     # List all targets
```

## Repository layout

```
kAsH-OS/
├── Makefile                  # Build system with toolchain auto-detection
├── linker.ld                 # Memory layout for QEMU virt machine
├── README.md                 # This file
├── SETUP.md                  # Toolchain install (Windows + macOS)
├── GUIDE.md                  # Operating principles for the project
├── BACKLOG.md                # Parked ideas / scope-creep
├── src/
│   ├── boot/startup.S        # Reset code: stack, BSS, mtvec, jump to kmain
│   ├── kernel/main.c         # kmain, banner, trap dispatch, echo loop
│   ├── kernel/uart.c         # 16550 UART driver + kprintf
│   └── include/              # types.h, uart.h
└── docs/
    ├── learning-roadmap.md   # 10-phase, ~20-month plan at ~6 hrs/wk
    ├── journal/              # Per-session reflections (the design diary)
    └── concepts/             # Concept notes — written by me, after I've struggled with the topic
```

## License & author

Personal learning project by Kashif (`KashifSid38`). MIT licensed. No AMD IP, no confidential information; all tooling is open source.
