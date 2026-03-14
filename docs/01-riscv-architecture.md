# RISC-V Architecture Reference

## Overview

RISC-V is an open-source Instruction Set Architecture (ISA) based on RISC principles. This document covers the architecture fundamentals needed for OS development.

---

## 🎯 Target: RISC-V E-Series (E6A and similar)

The SiFive E-series (like E6A, E21, E31, E76) are microcontroller-class cores designed for embedded applications. Key characteristics:

- **RV32IMAC** - 32-bit with Integer, Multiply, Atomic, Compressed extensions
- **Machine mode only** (M-mode) - simplest privilege model
- **No MMU** - Physical Memory Protection (PMP) only
- **Tight integration** - Designed for real-time, low-power applications

### E6A Specifications
- Pipeline: 3-stage in-order
- Performance: Up to 1.61 DMIPS/MHz
- Area: ~0.023 mm² (in 28nm)
- Power: Ultra-low power consumption

---

## 📊 Register Architecture

### General Purpose Registers (x0-x31)

| Register | ABI Name | Description | Saver |
|----------|----------|-------------|-------|
| x0 | zero | Hardwired zero | - |
| x1 | ra | Return address | Caller |
| x2 | sp | Stack pointer | Callee |
| x3 | gp | Global pointer | - |
| x4 | tp | Thread pointer | - |
| x5 | t0 | Temporary | Caller |
| x6-x7 | t1-t2 | Temporaries | Caller |
| x8 | s0/fp | Saved/Frame pointer | Callee |
| x9 | s1 | Saved register | Callee |
| x10-x11 | a0-a1 | Arguments/Return values | Caller |
| x12-x17 | a2-a7 | Arguments | Caller |
| x18-x27 | s2-s11 | Saved registers | Callee |
| x28-x31 | t3-t6 | Temporaries | Caller |

### Special Registers

```
PC (Program Counter) - Current instruction address
```

---

## 📝 Instruction Formats

### RV32I Base Instruction Formats

```
R-type: [funct7 | rs2 | rs1 | funct3 | rd | opcode]
        [  7   |  5  |  5  |   3    |  5 |   7   ]

I-type: [    imm[11:0]    | rs1 | funct3 | rd | opcode]
        [       12        |  5  |   3    |  5 |   7   ]

S-type: [imm[11:5] | rs2 | rs1 | funct3 | imm[4:0] | opcode]
        [    7     |  5  |  5  |   3    |    5     |   7   ]

B-type: [imm[12|10:5] | rs2 | rs1 | funct3 | imm[4:1|11] | opcode]
        [      7      |  5  |  5  |   3    |      5      |   7   ]

U-type: [         imm[31:12]          | rd | opcode]
        [            20               |  5 |   7   ]

J-type: [   imm[20|10:1|11|19:12]     | rd | opcode]
        [            20               |  5 |   7   ]
```

---

## 🔧 Key Instructions for OS Development

### Arithmetic
```asm
add  rd, rs1, rs2    # rd = rs1 + rs2
addi rd, rs1, imm    # rd = rs1 + imm
sub  rd, rs1, rs2    # rd = rs1 - rs2
```

### Logic
```asm
and  rd, rs1, rs2    # rd = rs1 & rs2
or   rd, rs1, rs2    # rd = rs1 | rs2
xor  rd, rs1, rs2    # rd = rs1 ^ rs2
sll  rd, rs1, rs2    # rd = rs1 << rs2 (logical)
srl  rd, rs1, rs2    # rd = rs1 >> rs2 (logical)
sra  rd, rs1, rs2    # rd = rs1 >> rs2 (arithmetic)
```

### Memory Access
```asm
lw   rd, offset(rs1)  # Load word
sw   rs2, offset(rs1) # Store word
lb   rd, offset(rs1)  # Load byte
sb   rs2, offset(rs1) # Store byte
lh   rd, offset(rs1)  # Load halfword
sh   rs2, offset(rs1) # Store halfword
```

### Branches
```asm
beq  rs1, rs2, offset  # Branch if equal
bne  rs1, rs2, offset  # Branch if not equal
blt  rs1, rs2, offset  # Branch if less than
bge  rs1, rs2, offset  # Branch if greater or equal
bltu rs1, rs2, offset  # Branch if less than (unsigned)
bgeu rs1, rs2, offset  # Branch if greater or equal (unsigned)
```

### Jumps
```asm
jal  rd, offset    # Jump and link (rd = PC+4, PC = PC+offset)
jalr rd, rs1, imm  # Jump and link register (rd = PC+4, PC = rs1+imm)
```

### CSR Instructions (Critical for OS)
```asm
csrrw  rd, csr, rs1   # CSR read/write
csrrs  rd, csr, rs1   # CSR read and set bits
csrrc  rd, csr, rs1   # CSR read and clear bits
csrrwi rd, csr, imm   # CSR read/write immediate
csrrsi rd, csr, imm   # CSR read and set bits immediate
csrrci rd, csr, imm   # CSR read and clear bits immediate
```

### System Instructions
```asm
ecall              # Environment call (syscall)
ebreak             # Environment break (debugger)
mret               # Return from machine mode trap
wfi                # Wait for interrupt
```

---

## 🔐 Privilege Architecture

### Machine Mode (M-mode)
The highest privilege level, always present. For embedded RISC-V like E6A, often the ONLY mode.

### Key Control and Status Registers (CSRs)

#### Machine Status (mstatus)
```
Bits:
  MIE  [3]    - Machine Interrupt Enable
  MPIE [7]    - Previous MIE value
  MPP  [12:11] - Previous privilege mode
```

#### Machine Trap Vector (mtvec)
```
Base address for trap handler
MODE [1:0]:
  0 = Direct (all traps go to BASE)
  1 = Vectored (interrupts go to BASE + 4*cause)
```

#### Machine Exception Program Counter (mepc)
```
Address of instruction that caused trap (for return)
```

#### Machine Cause (mcause)
```
Bit 31: Interrupt (1) or Exception (0)
Bits [30:0]: Exception/Interrupt code
```

#### Machine Trap Value (mtval)
```
Additional trap information (bad address, instruction)
```

#### Machine Interrupt Enable (mie)
```
MEIE [11] - External interrupt enable
MTIE [7]  - Timer interrupt enable
MSIE [3]  - Software interrupt enable
```

#### Machine Interrupt Pending (mip)
```
MEIP [11] - External interrupt pending
MTIP [7]  - Timer interrupt pending
MSIP [3]  - Software interrupt pending
```

---

## ⏰ Timer and Interrupt Architecture

### CLINT (Core Local Interruptor)
Memory-mapped registers for timer and software interrupts:

| Offset | Register | Description |
|--------|----------|-------------|
| 0x0000 | msip | Machine software interrupt pending |
| 0x4000 | mtimecmp | Timer compare (64-bit) |
| 0xBFF8 | mtime | Timer counter (64-bit) |

### PLIC (Platform Level Interrupt Controller)
For external interrupts:

| Register | Description |
|----------|-------------|
| priority | Interrupt priority (0-7) |
| pending | Pending interrupts bitmap |
| enable | Enable interrupts bitmap |
| threshold | Priority threshold |
| claim/complete | Claim and complete interrupts |

---

## 💾 Memory Map (QEMU virt machine)

```
0x00000000 - 0x00000FFF : Debug ROM
0x00001000 - 0x00011FFF : Boot ROM
0x00100000 - 0x00100FFF : Test area
0x02000000 - 0x0200FFFF : CLINT
0x0C000000 - 0x0FFFFFFF : PLIC
0x10000000 - 0x10000FFF : UART0
0x10001000 - 0x10001FFF : VirtIO
0x80000000 - 0xFFFFFFFF : RAM (configurable)
```

---

## 🔗 Resources

- [RISC-V Unprivileged Spec](https://github.com/riscv/riscv-isa-manual)
- [RISC-V Privileged Spec](https://github.com/riscv/riscv-isa-manual)
- [SiFive E-Series Manual](https://www.sifive.com/documentation)
- [RISC-V Assembly Programmer's Manual](https://github.com/riscv-non-isa/riscv-asm-manual)
