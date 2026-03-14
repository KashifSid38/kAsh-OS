/*
 * kAsH-OS UART Driver
 * Author: bitwizebard
 * 
 * UART driver for QEMU virt machine (16550 compatible)
 */

#include "uart.h"

/* UART register access macros */
#define UART_REG(offset)    (*(vuint8_t *)(UART0_BASE + (offset)))

/* Variadic argument macros (since we don't have stdarg.h) */
typedef __builtin_va_list va_list;
#define va_start(ap, last)  __builtin_va_start(ap, last)
#define va_end(ap)          __builtin_va_end(ap)
#define va_arg(ap, type)    __builtin_va_arg(ap, type)

/**
 * Initialize the UART
 */
void uart_init(void)
{
    /* Disable interrupts */
    UART_REG(UART_IER) = 0x00;
    
    /* Set baud rate (not strictly necessary for QEMU) */
    /* Enable DLAB to set divisor */
    UART_REG(UART_LCR) = UART_LCR_DLAB;
    
    /* Set divisor (for 38400 baud with 1.8432 MHz clock) */
    /* LSB */
    UART_REG(0) = 0x03;
    /* MSB */
    UART_REG(1) = 0x00;
    
    /* 8 bits, no parity, 1 stop bit */
    UART_REG(UART_LCR) = UART_LCR_8N1;
    
    /* Enable FIFO, clear TX/RX */
    UART_REG(UART_FCR) = UART_FCR_ENABLE | UART_FCR_RXCLR | UART_FCR_TXCLR;
    
    /* Enable received data available interrupt (optional) */
    /* UART_REG(UART_IER) = UART_IER_RDI; */
}

/**
 * Check if transmit buffer is ready
 */
static int uart_tx_ready(void)
{
    return UART_REG(UART_LSR) & UART_LSR_THRE;
}

/**
 * Send a single character
 */
void uart_putc(char c)
{
    /* Wait for transmit buffer to be empty */
    while (!uart_tx_ready()) {
        /* Busy wait */
    }
    
    /* Send character */
    UART_REG(UART_THR) = c;
}

/**
 * Check if data is available to read
 */
int uart_data_ready(void)
{
    return UART_REG(UART_LSR) & UART_LSR_DR;
}

/**
 * Receive a single character
 */
char uart_getc(void)
{
    /* Wait for data to be available */
    while (!uart_data_ready()) {
        /* Busy wait */
    }
    
    /* Read and return character */
    return UART_REG(UART_RBR);
}

/**
 * Send a null-terminated string
 */
void uart_puts(const char *s)
{
    while (*s) {
        if (*s == '\n') {
            uart_putc('\r');  /* Add carriage return */
        }
        uart_putc(*s++);
    }
}

/**
 * Print a hexadecimal number
 */
void uart_put_hex(uint32_t value)
{
    const char hex_digits[] = "0123456789abcdef";
    char buffer[9];
    int i;
    
    /* Convert to hex string (reverse order) */
    for (i = 7; i >= 0; i--) {
        buffer[i] = hex_digits[value & 0xF];
        value >>= 4;
    }
    buffer[8] = '\0';
    
    /* Print "0x" prefix */
    uart_puts("0x");
    
    /* Skip leading zeros (but keep at least one digit) */
    i = 0;
    while (i < 7 && buffer[i] == '0') {
        i++;
    }
    
    uart_puts(&buffer[i]);
}

/**
 * Print a decimal number
 */
void uart_put_dec(int32_t value)
{
    char buffer[12];  /* Enough for -2147483648 */
    int i = 10;
    int negative = 0;
    
    buffer[11] = '\0';
    
    if (value < 0) {
        negative = 1;
        value = -value;
    }
    
    /* Handle zero specially */
    if (value == 0) {
        uart_putc('0');
        return;
    }
    
    /* Convert to decimal string (reverse order) */
    while (value > 0) {
        buffer[i--] = '0' + (value % 10);
        value /= 10;
    }
    
    if (negative) {
        buffer[i--] = '-';
    }
    
    uart_puts(&buffer[i + 1]);
}

/**
 * Print an unsigned decimal number
 */
static void uart_put_udec(uint32_t value)
{
    char buffer[12];
    int i = 10;
    
    buffer[11] = '\0';
    
    if (value == 0) {
        uart_putc('0');
        return;
    }
    
    while (value > 0) {
        buffer[i--] = '0' + (value % 10);
        value /= 10;
    }
    
    uart_puts(&buffer[i + 1]);
}

/**
 * Simple printf implementation
 * Supports: %d, %u, %x, %s, %c, %p, %%
 */
void kprintf(const char *fmt, ...)
{
    va_list args;
    va_start(args, fmt);
    
    while (*fmt) {
        if (*fmt == '%') {
            fmt++;
            switch (*fmt) {
                case 'd':
                case 'i':
                    uart_put_dec(va_arg(args, int32_t));
                    break;
                    
                case 'u':
                    uart_put_udec(va_arg(args, uint32_t));
                    break;
                    
                case 'x':
                case 'X':
                    uart_put_hex(va_arg(args, uint32_t));
                    break;
                    
                case 'p':
                    uart_put_hex((uint32_t)va_arg(args, void *));
                    break;
                    
                case 's': {
                    const char *s = va_arg(args, const char *);
                    uart_puts(s ? s : "(null)");
                    break;
                }
                
                case 'c':
                    uart_putc((char)va_arg(args, int));
                    break;
                    
                case '%':
                    uart_putc('%');
                    break;
                    
                default:
                    /* Unknown format, print as-is */
                    uart_putc('%');
                    uart_putc(*fmt);
                    break;
            }
        } else if (*fmt == '\n') {
            uart_putc('\r');
            uart_putc('\n');
        } else {
            uart_putc(*fmt);
        }
        fmt++;
    }
    
    va_end(args);
}
