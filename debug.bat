@echo off
echo Starting QEMU with GDB server on port 1234...
echo CPU is PAUSED - connect GDB to continue
echo.
echo In Git Bash run:  make gdb
echo.
"C:\Program Files\qemu\qemu-system-riscv32.exe" -machine virt -cpu rv32 -m 128M -nographic -bios none -kernel build\kernel.elf -s -S
