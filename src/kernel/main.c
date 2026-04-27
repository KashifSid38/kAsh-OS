/*
 * kAsH-OS Kernel Main
 * Author: bitwizebard
 * 
 * Main kernel entry point
 */

#include "uart.h"

/* External symbols from linker script */
extern uint32_t _text_start;
extern uint32_t _text_end;
extern uint32_t _data_start;
extern uint32_t _data_end;
extern uint32_t _bss_start;
extern uint32_t _bss_end;
extern uint32_t _stack_top;
extern uint32_t _stack_bottom;
extern uint32_t _heap_start;
extern uint32_t _heap_end;

/* Version information */
#define KASH_VERSION_MAJOR  0
#define KASH_VERSION_MINOR  1
#define KASH_VERSION_PATCH  0

/**
 * Print kernel banner
 */
static void print_banner(void)
{
    kprintf("\n");
    kprintf("  +----------------------------------------------------------+\n");
    kprintf("  |                                                          |\n");
    kprintf("  |   _        _      ___  _   _            ___   ____      |\n");
    kprintf("  |  | | __   / \\    / __|| | | |          / _ \\ / ___|     |\n");
    kprintf("  |  | |/ /  / _ \\   \\__ \\| |_| |  ------  | | | \\___ \\    |\n");
    kprintf("  |  |   <  / ___ \\  |___/|  _  |          | |_| |___) |   |\n");
    kprintf("  |  |_|\\_\\/_/   \\_\\       |_| |_|           \\___/|____/    |\n");
    kprintf("  |                                                          |\n");
    kprintf("  |   RISC-V Learning Operating System                      |\n");
    kprintf("  |   Version %d.%d.%d                  bitwizebard (c) 2025  |\n",
            KASH_VERSION_MAJOR, KASH_VERSION_MINOR, KASH_VERSION_PATCH);
    kprintf("  |                                                          |\n");
    kprintf("  +----------------------------------------------------------+\n");
    kprintf("\n");
}

/**
 * Print memory map information
 */
static void print_memory_info(void)
{
    kprintf("Memory Map:\n");
    kprintf("  .text:  %p - %p\n", &_text_start, &_text_end);
    kprintf("  .data:  %p - %p\n", &_data_start, &_data_end);
    kprintf("  .bss:   %p - %p\n", &_bss_start, &_bss_end);
    kprintf("  stack:  %p - %p (%u bytes)\n", 
            &_stack_bottom, &_stack_top,
            (uint32_t)&_stack_top - (uint32_t)&_stack_bottom);
    kprintf("  heap:   %p - %p\n", &_heap_start, &_heap_end);
    kprintf("\n");
}

/**
 * Trap handler - called from assembly
 */
void trap_handler(uint32_t mcause, uint32_t mepc)
{
    int is_interrupt = (mcause >> 31) & 1;
    uint32_t code = mcause & 0x7FFFFFFF;
    
    if (is_interrupt) {
        kprintf("[INT] Interrupt %u at PC=%p\n", code, mepc);
        
        switch (code) {
            case 7:  /* Machine timer interrupt */
                kprintf("  Timer interrupt\n");
                /* TODO: Handle timer interrupt */
                break;
            case 11: /* Machine external interrupt */
                kprintf("  External interrupt\n");
                /* TODO: Handle external interrupt */
                break;
            default:
                kprintf("  Unknown interrupt\n");
                break;
        }
    } else {
        kprintf("[EXC] Exception %u at PC=%p\n", code, mepc);
        
        switch (code) {
            case 0:  kprintf("  Instruction address misaligned\n"); break;
            case 1:  kprintf("  Instruction access fault\n"); break;
            case 2:  kprintf("  Illegal instruction\n"); break;
            case 3:  kprintf("  Breakpoint\n"); break;
            case 4:  kprintf("  Load address misaligned\n"); break;
            case 5:  kprintf("  Load access fault\n"); break;
            case 6:  kprintf("  Store address misaligned\n"); break;
            case 7:  kprintf("  Store access fault\n"); break;
            case 8:  kprintf("  Environment call from U-mode\n"); break;
            case 9:  kprintf("  Environment call from S-mode\n"); break;
            case 11: kprintf("  Environment call from M-mode\n"); break;
            case 12: kprintf("  Instruction page fault\n"); break;
            case 13: kprintf("  Load page fault\n"); break;
            case 15: kprintf("  Store page fault\n"); break;
            default: kprintf("  Unknown exception\n"); break;
        }
        
        /* Halt on exception */
        kprintf("System halted.\n");
        while (1) {
            __asm__ volatile("wfi");
        }
    }
}

/**
 * Kernel main entry point
 * Called from startup.S after basic initialization
 */
void kmain(void)
{
    /* Initialize UART for console output */
    uart_init();
    
    /* Print welcome banner */
    print_banner();
    
    /* Print system information */
    kprintf("kAsH-OS booting...\n\n");
    kprintf("Target: RISC-V RV32IMAC\n");
    kprintf("Platform: QEMU virt machine\n");
    kprintf("\n");
    
    /* Print memory layout */
    print_memory_info();
    
    /* TODO: Initialize more subsystems */
    kprintf("Initialization complete.\n\n");
    
    /* Simple echo loop for testing */
    kprintf("Console ready. Type something:\n");
    kprintf("> ");
    
    while (1) {
        if (uart_data_ready()) {
            char c = uart_getc();
            
            /* Echo character */
            uart_putc(c);
            
            /* Handle newline */
            if (c == '\r' || c == '\n') {
                kprintf("\n> ");
            }
        }
    }
}
