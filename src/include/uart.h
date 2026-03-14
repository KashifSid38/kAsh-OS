/*
 * kAsH-OS UART Driver Header
 * 
 * UART driver for QEMU virt machine (16550 compatible)
 */

#ifndef KASH_UART_H
#define KASH_UART_H

#include "types.h"

/* UART base address for QEMU virt machine */
#define UART0_BASE      0x10000000

/* UART register offsets (16550 compatible) */
#define UART_RBR        0x00    /* Receive Buffer Register (read) */
#define UART_THR        0x00    /* Transmit Holding Register (write) */
#define UART_IER        0x01    /* Interrupt Enable Register */
#define UART_IIR        0x02    /* Interrupt Identification Register (read) */
#define UART_FCR        0x02    /* FIFO Control Register (write) */
#define UART_LCR        0x03    /* Line Control Register */
#define UART_MCR        0x04    /* Modem Control Register */
#define UART_LSR        0x05    /* Line Status Register */
#define UART_MSR        0x06    /* Modem Status Register */
#define UART_SCR        0x07    /* Scratch Register */

/* Line Status Register bits */
#define UART_LSR_DR     0x01    /* Data Ready */
#define UART_LSR_OE     0x02    /* Overrun Error */
#define UART_LSR_PE     0x04    /* Parity Error */
#define UART_LSR_FE     0x08    /* Framing Error */
#define UART_LSR_BI     0x10    /* Break Indicator */
#define UART_LSR_THRE   0x20    /* Transmit Holding Register Empty */
#define UART_LSR_TEMT   0x40    /* Transmitter Empty */
#define UART_LSR_ERR    0x80    /* Error in FIFO */

/* Line Control Register bits */
#define UART_LCR_DLAB   0x80    /* Divisor Latch Access Bit */
#define UART_LCR_8N1    0x03    /* 8 data bits, no parity, 1 stop bit */

/* FIFO Control Register bits */
#define UART_FCR_ENABLE 0x01    /* Enable FIFOs */
#define UART_FCR_RXCLR  0x02    /* Clear receive FIFO */
#define UART_FCR_TXCLR  0x04    /* Clear transmit FIFO */

/* Interrupt Enable Register bits */
#define UART_IER_RDI    0x01    /* Receive Data Available Interrupt */
#define UART_IER_THRI   0x02    /* THR Empty Interrupt */

/* Function prototypes */

/**
 * Initialize the UART
 * Configures UART for 8N1, enables FIFO
 */
void uart_init(void);

/**
 * Send a single character
 * Blocks until the transmit buffer is ready
 */
void uart_putc(char c);

/**
 * Receive a single character
 * Blocks until a character is available
 */
char uart_getc(void);

/**
 * Check if data is available to read
 * Returns non-zero if data is ready
 */
int uart_data_ready(void);

/**
 * Send a null-terminated string
 */
void uart_puts(const char *s);

/**
 * Print a hexadecimal number
 */
void uart_put_hex(uint32_t value);

/**
 * Print a decimal number
 */
void uart_put_dec(int32_t value);

/**
 * Simple printf implementation
 * Supports: %d, %u, %x, %s, %c, %%
 */
void kprintf(const char *fmt, ...);

#endif /* KASH_UART_H */
