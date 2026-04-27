@echo off
"C:\Program Files\qemu\qemu-system-riscv32.exe" -machine virt -cpu rv32 -m 128M -nographic -bios none -kernel build\kernel.elf
