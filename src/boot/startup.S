/*
 * kAsH-OS Startup Code
 * Target: RISC-V RV32IMAC
 * 
 * This is the first code that runs after reset.
 * It sets up the stack, clears BSS, and jumps to kmain.
 */

.section .text.init
.global _start
.type _start, @function

_start:
    /*
     * Disable interrupts during startup
     * Clear MIE bit in mstatus
     */
    csrci   mstatus, 0x8

    /*
     * Set up the stack pointer
     * Stack grows downward, so we start at _stack_top
     */
    la      sp, _stack_top

    /*
     * Set up the global pointer for relaxed addressing
     * This is optional but improves code efficiency
     */
    .option push
    .option norelax
    la      gp, __global_pointer$
    .option pop

    /*
     * Clear the BSS section
     * BSS contains uninitialized global variables
     * They must be zeroed before use
     */
    la      t0, _bss_start
    la      t1, _bss_end
    bgeu    t0, t1, 2f      /* Skip if BSS is empty */

1:  /* BSS clear loop */
    sw      zero, 0(t0)
    addi    t0, t0, 4
    bltu    t0, t1, 1b

2:  /* BSS clearing done */

    /*
     * Set up trap handler
     * mtvec holds the address of the trap handler
     */
    la      t0, trap_vector
    csrw    mtvec, t0

    /*
     * Call kmain (C entry point)
     * kmain should not return, but if it does, we'll hang
     */
    call    kmain

    /*
     * If kmain returns, enter infinite loop
     * This should never happen in a proper OS
     */
halt:
    wfi                     /* Wait for interrupt (low power) */
    j       halt

/*
 * Trap vector - handles all exceptions and interrupts
 * This is a basic version; will be expanded later
 */
.align 4
.global trap_vector
.type trap_vector, @function

trap_vector:
    /*
     * Save context (caller-saved registers)
     * In a full implementation, we'd save all registers
     */
    addi    sp, sp, -64
    sw      ra, 0(sp)
    sw      t0, 4(sp)
    sw      t1, 8(sp)
    sw      t2, 12(sp)
    sw      a0, 16(sp)
    sw      a1, 20(sp)
    sw      a2, 24(sp)
    sw      a3, 28(sp)
    sw      a4, 32(sp)
    sw      a5, 36(sp)
    sw      a6, 40(sp)
    sw      a7, 44(sp)
    sw      t3, 48(sp)
    sw      t4, 52(sp)
    sw      t5, 56(sp)
    sw      t6, 60(sp)

    /*
     * Call C trap handler
     * mcause and mepc are passed as arguments
     */
    csrr    a0, mcause      /* First argument: cause */
    csrr    a1, mepc        /* Second argument: exception PC */
    call    trap_handler

    /*
     * Restore context
     */
    lw      ra, 0(sp)
    lw      t0, 4(sp)
    lw      t1, 8(sp)
    lw      t2, 12(sp)
    lw      a0, 16(sp)
    lw      a1, 20(sp)
    lw      a2, 24(sp)
    lw      a3, 28(sp)
    lw      a4, 32(sp)
    lw      a5, 36(sp)
    lw      a6, 40(sp)
    lw      a7, 44(sp)
    lw      t3, 48(sp)
    lw      t4, 52(sp)
    lw      t5, 56(sp)
    lw      t6, 60(sp)
    addi    sp, sp, 64

    /* Return from trap */
    mret

/*
 * Global pointer symbol
 * Used for efficient access to global variables
 */
.section .sdata
.global __global_pointer$
__global_pointer$:
    .word 0
