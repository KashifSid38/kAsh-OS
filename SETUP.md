# kAsH-OS Development Environment Setup

This guide sets up the RISC-V toolchain and QEMU for building and running kAsH-OS. It works on both Windows and macOS.

**Windows note:** The Makefile auto-detects the Zephyr SDK RISC-V toolchain at `~/zephyr-sdk-0.16.8/` and QEMU at `C:\Program Files\qemu\` — if both are installed, `make all` and `make run` work with no overrides needed.

---

## macOS

```bash
# Install Homebrew if you don't have it
/bin/bash -c "$(curl -fsSL https://raw.githubusercontent.com/Homebrew/install/HEAD/install.sh)"

# RISC-V GNU toolchain (installs riscv64-unknown-elf-gcc, gdb, objdump, etc.)
brew install riscv-gnu-toolchain

# QEMU with RISC-V support
brew install qemu
```

Verify:
```bash
riscv64-unknown-elf-gcc --version
qemu-system-riscv32 --version
```

Then build and run:
```bash
make all
make run
```

---

## Windows

The cleanest option is the **xPack RISC-V GCC** toolchain — it's standalone, per-user, and has no Zephyr or Zephyr SDK dependency.

### Option A: xPack (recommended)

1. Install [Node.js LTS](https://nodejs.org/) (needed for xpm package manager)
2. Install xpm:
   ```powershell
   npm install --global xpm@latest
   ```
3. Install the RISC-V toolchain globally:
   ```powershell
   xpm install --global @xpack-dev-tools/riscv-none-elf-gcc@latest
   ```
   This installs tools like `riscv-none-elf-gcc`, `riscv-none-elf-gdb`, etc.

4. Add the xPack bin directory to your PATH. xpm will print the path after install — it looks like:
   ```
   %APPDATA%\xPacks\@xpack-dev-tools\riscv-none-elf-gcc\<version>\bin
   ```
   Add this to your System/User PATH environment variable.

5. Install QEMU for Windows from: https://www.qemu.org/download/#windows  
   (Direct link: qemu-w64 installer from https://qemu.weilnetz.de/w64/ )  
   Add the QEMU install directory (e.g., `C:\Program Files\qemu`) to PATH.

Verify in a new terminal (Git Bash or PowerShell):
```bash
riscv-none-elf-gcc --version
qemu-system-riscv32 --version
```

Build and run (xPack uses `riscv-none-elf-` prefix, not `riscv64-unknown-elf-`):
```bash
make all CROSS_COMPILE=riscv-none-elf-
make run CROSS_COMPILE=riscv-none-elf-
```

To avoid typing `CROSS_COMPILE=...` every time, set it in your shell:
```bash
# Git Bash / bash
export CROSS_COMPILE=riscv-none-elf-
make all
make run
```

### Option B: Pre-built riscv64-unknown-elf (SiFive/freedomstudio)

SiFive provides a standalone toolchain with the `riscv64-unknown-elf-` prefix:
- Download from: https://github.com/sifive/freedom-tools/releases
- Extract and add the `bin/` directory to PATH

With this option, `make all` and `make run` work without any override (Makefile defaults to `riscv64-unknown-elf-`).

---

## Using Git Bash on Windows

All `make` commands should be run in **Git Bash** (not PowerShell or cmd). Git Bash ships with `make` if you install it via "Git for Windows" with the "Additional tools" option, or install `make` separately via `winget install GnuWin32.Make` or from https://gnuwin32.sourceforge.net/packages/make.htm.

---

## Makefile Override Reference

| Variable | Default | Purpose |
|----------|---------|---------|
| `CROSS_COMPILE` | `riscv64-unknown-elf-` | Toolchain prefix |
| `QEMU` | `qemu-system-riscv32` | QEMU binary name/path |

Override on the command line:
```bash
make run CROSS_COMPILE=riscv-none-elf- QEMU=/usr/local/bin/qemu-system-riscv32
```

Or export in your shell profile to avoid repeating:
```bash
export CROSS_COMPILE=riscv-none-elf-
```

---

## Verify Everything Works

```bash
# Build
make all

# Run in QEMU (Ctrl+A then X to exit)
make run

# Debug with GDB — two terminals needed:
# Terminal 1:
make debug
# Terminal 2:
make gdb
```

Expected boot output:
```
  _          _          ___  ____
 | | __     / \        / ___||  _ \
 | |/ /    / _ \       \___ \| |_) |
 |   <    / ___ \   _  ___) |  __/
 |_|\_\  /_/   \_\ (_)|____/|_|

kAsH-OS v0.1.0
...
UART echo ready. Type something!
```

---

## Troubleshooting

**`make: riscv64-unknown-elf-gcc: Command not found`**  
The toolchain is not in PATH. Check the install step and restart your terminal.

**`qemu-system-riscv32: Command not found`**  
QEMU is not in PATH. On Windows, ensure the QEMU install directory is in PATH. On macOS, `brew install qemu` should handle it.

**`make` not found on Windows**  
Install Git for Windows and enable the Unix tools option, or install GnuWin32 make separately.

**Build error: `-no-pie` not recognized**  
Older GCC versions may not support `-no-pie`. Try removing it from `CFLAGS` in the Makefile — it's a hardening flag that's not required for bare-metal.
