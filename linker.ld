/*
 * kAsH-OS Linker Script
 * Author: bitwizebard
 * Target: RISC-V RV32IMAC (QEMU virt machine)
 */

/* Entry point - first function to execute */
ENTRY(_start)

/* Memory layout for QEMU virt machine */
MEMORY
{
    /* RAM starts at 0x80000000 for QEMU virt */
    RAM (rwx) : ORIGIN = 0x80000000, LENGTH = 128M
}

/* Stack size */
__stack_size = 0x4000;  /* 16KB stack */

SECTIONS
{
    /* Code section - starts at RAM origin */
    .text : {
        PROVIDE(_text_start = .);
        *(.text.init)       /* Startup code first */
        *(.text .text.*)    /* All other code */
        PROVIDE(_text_end = .);
    } > RAM

    /* Read-only data */
    .rodata : {
        . = ALIGN(4);
        PROVIDE(_rodata_start = .);
        *(.rodata .rodata.*)
        *(.srodata .srodata.*)
        PROVIDE(_rodata_end = .);
    } > RAM

    /* Initialized data */
    .data : {
        . = ALIGN(4);
        PROVIDE(_data_start = .);
        *(.data .data.*)
        *(.sdata .sdata.*)
        PROVIDE(_data_end = .);
    } > RAM

    /* Uninitialized data (BSS) */
    .bss : {
        . = ALIGN(4);
        PROVIDE(_bss_start = .);
        *(.bss .bss.*)
        *(.sbss .sbss.*)
        *(COMMON)
        PROVIDE(_bss_end = .);
    } > RAM

    /* Stack - grows downward */
    .stack (NOLOAD) : {
        . = ALIGN(16);
        PROVIDE(_stack_bottom = .);
        . = . + __stack_size;
        PROVIDE(_stack_top = .);
    } > RAM

    /* Heap - everything after stack */
    .heap (NOLOAD) : {
        . = ALIGN(4);
        PROVIDE(_heap_start = .);
        /* Heap grows upward to end of RAM */
    } > RAM

    PROVIDE(_heap_end = ORIGIN(RAM) + LENGTH(RAM));

    /* Memory size info */
    PROVIDE(_memory_start = ORIGIN(RAM));
    PROVIDE(_memory_end = ORIGIN(RAM) + LENGTH(RAM));
}
