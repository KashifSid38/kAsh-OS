/*
 * kAsH-OS Type Definitions
 * Author: bitwizebard
 * 
 * Standard type definitions for the kernel.
 * Since we're bare-metal, we don't have stdint.h
 */

#ifndef KASH_TYPES_H
#define KASH_TYPES_H

/* Exact-width integer types */
typedef signed char         int8_t;
typedef unsigned char       uint8_t;
typedef signed short        int16_t;
typedef unsigned short      uint16_t;
typedef signed int          int32_t;
typedef unsigned int        uint32_t;
typedef signed long long    int64_t;
typedef unsigned long long  uint64_t;

/* Size types */
typedef uint32_t            size_t;
typedef int32_t             ssize_t;
typedef int32_t             ptrdiff_t;

/* Pointer-sized integer */
typedef uint32_t            uintptr_t;
typedef int32_t             intptr_t;

/* Boolean type */
typedef int                 bool;
#define true                1
#define false               0

/* NULL pointer */
#ifndef NULL
#define NULL                ((void *)0)
#endif

/* Register type (native word size) */
typedef uint32_t            reg_t;

/* Volatile types for hardware registers */
typedef volatile uint8_t    vuint8_t;
typedef volatile uint16_t   vuint16_t;
typedef volatile uint32_t   vuint32_t;

/* Common macros */
#define ARRAY_SIZE(x)       (sizeof(x) / sizeof((x)[0]))
#define MIN(a, b)           ((a) < (b) ? (a) : (b))
#define MAX(a, b)           ((a) > (b) ? (a) : (b))
#define ALIGN(x, a)         (((x) + (a) - 1) & ~((a) - 1))
#define IS_ALIGNED(x, a)    (((x) & ((a) - 1)) == 0)

/* Bit manipulation */
#define BIT(n)              (1U << (n))
#define BITS(hi, lo)        ((~0U >> (31 - (hi))) & (~0U << (lo)))
#define GET_BIT(x, n)       (((x) >> (n)) & 1)
#define SET_BIT(x, n)       ((x) | BIT(n))
#define CLEAR_BIT(x, n)     ((x) & ~BIT(n))
#define TOGGLE_BIT(x, n)    ((x) ^ BIT(n))

/* Memory barrier */
#define memory_barrier()    __asm__ volatile ("fence" ::: "memory")
#define read_barrier()      __asm__ volatile ("fence ir, ir" ::: "memory")
#define write_barrier()     __asm__ volatile ("fence ow, ow" ::: "memory")

/* Compiler hints */
#define likely(x)           __builtin_expect(!!(x), 1)
#define unlikely(x)         __builtin_expect(!!(x), 0)
#define UNUSED(x)           (void)(x)
#define ALWAYS_INLINE       __attribute__((always_inline)) inline
#define NORETURN            __attribute__((noreturn))
#define PACKED              __attribute__((packed))
#define ALIGNED(n)          __attribute__((aligned(n)))
#define WEAK                __attribute__((weak))
#define SECTION(s)          __attribute__((section(s)))

/* Static assertion (compile-time check) */
#define STATIC_ASSERT(cond, msg) \
    typedef char static_assertion_##msg[(cond) ? 1 : -1]

#endif /* KASH_TYPES_H */
