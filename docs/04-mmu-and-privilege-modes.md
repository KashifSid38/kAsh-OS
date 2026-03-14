# MMU and Privilege Modes in RISC-V

## Overview

This document explains RISC-V memory management options and how to achieve user/kernel space separation.

---

## ❌ E-Series (E6A) Limitations

**The SiFive E-series (E6A, E21, E31, E76) does NOT have MMU support.**

### E-Series Characteristics
- **Privilege Modes**: M-mode only (no S-mode or U-mode)
- **Memory Protection**: Physical Memory Protection (PMP) only
- **Virtual Memory**: Not supported
- **Use Case**: Bare-metal RTOS, microcontroller applications

### What PMP Can Do
PMP provides basic memory protection without virtual addresses:
- Define up to 16 memory regions
- Set read/write/execute permissions per region
- Protect kernel code from accidental overwrite
- **Cannot** provide process isolation or virtual address spaces

```c
// PMP Example: Protect kernel code as read-only
void setup_pmp(void) {
    // Region 0: Kernel code (0x80000000 - 0x80010000) = Read + Execute
    csr_write(pmpaddr0, 0x80010000 >> 2);  // Top of region
    csr_write(pmpcfg0, PMP_R | PMP_X | PMP_TOR);  // Read + Execute, Top-of-Range
}
```

---

## ✅ For MMU Support: Target S/U-Series or QEMU

### Option 1: Use QEMU virt with Full Privilege Support

QEMU's `virt` machine supports full RISC-V privilege architecture:

```bash
# 64-bit with MMU support
qemu-system-riscv64 -machine virt -cpu rv64 -m 256M \
    -nographic -bios none -kernel kernel.elf
```

### Option 2: Target Different Hardware

| Core | Example Boards | MMU | Price Range |
|------|---------------|-----|-------------|
| SiFive U74 | HiFive Unmatched | Sv39 | $500+ |
| T-Head C906 | Allwinner D1 boards | Sv39 | $15-50 |
| StarFive JH7110 | VisionFive 2 | Sv39 | $50-100 |

---

## 🔧 RISC-V Privilege Architecture (with MMU)

### Privilege Levels

```
┌─────────────────────────────────────────────┐
│           Machine Mode (M-mode)             │  ← Highest privilege
│  - First code to run after reset            │
│  - Full hardware access                     │
│  - Handles traps from lower modes           │
├─────────────────────────────────────────────┤
│          Supervisor Mode (S-mode)           │  ← Kernel/OS runs here
│  - Manages virtual memory                   │
│  - Handles syscalls from user mode          │
│  - Runs kernel code                         │
├─────────────────────────────────────────────┤
│             User Mode (U-mode)              │  ← Lowest privilege
│  - User applications run here               │
│  - Cannot access kernel memory              │
│  - Uses syscalls for OS services            │
└─────────────────────────────────────────────┘
```

### CSRs for Each Mode

| Mode | Status | Trap Vec | EPC | Cause | Scratch |
|------|--------|----------|-----|-------|---------|
| M | mstatus | mtvec | mepc | mcause | mscratch |
| S | sstatus | stvec | sepc | scause | sscratch |

---

## 💾 RISC-V Virtual Memory (Sv39)

### Sv39: 39-bit Virtual Address Space

```
Virtual Address (39 bits):
┌──────────┬──────────┬──────────┬──────────┐
│  VPN[2]  │  VPN[1]  │  VPN[0]  │  Offset  │
│  9 bits  │  9 bits  │  9 bits  │ 12 bits  │
└──────────┴──────────┴──────────┴──────────┘

Physical Address (56 bits max):
┌────────────────────────────────┬──────────┐
│         PPN (44 bits)          │  Offset  │
│                                │ 12 bits  │
└────────────────────────────────┴──────────┘
```

### Page Table Entry (PTE)

```
63    54 53    28 27    19 18    10 9   8 7 6 5 4 3 2 1 0
┌───────┬────────┬────────┬────────┬─────┬─┬─┬─┬─┬─┬─┬─┬─┐
│Reserved│ PPN[2] │ PPN[1] │ PPN[0] │ RSW │D│A│G│U│X│W│R│V│
└───────┴────────┴────────┴────────┴─────┴─┴─┴─┴─┴─┴─┴─┴─┘

V = Valid
R = Readable
W = Writable
X = Executable
U = User-accessible
G = Global
A = Accessed
D = Dirty
```

### Page Table Structure (3-Level)

```
                    ┌──────────────┐
      satp ────────►│   Root PT    │ (512 entries)
                    │  (Level 2)   │
                    └──────┬───────┘
                           │ VPN[2]
                           ▼
                    ┌──────────────┐
                    │   Level 1    │ (512 entries)
                    │  Page Table  │
                    └──────┬───────┘
                           │ VPN[1]
                           ▼
                    ┌──────────────┐
                    │   Level 0    │ (512 entries)
                    │  Page Table  │
                    └──────┬───────┘
                           │ VPN[0]
                           ▼
                    ┌──────────────┐
                    │  4KB Page    │
                    │   (Frame)    │
                    └──────────────┘
```

### Page Sizes

| Level | Page Size | Name |
|-------|-----------|------|
| 0 | 4 KB | Regular page |
| 1 | 2 MB | Megapage |
| 2 | 1 GB | Gigapage |

---

## 🔐 User/Kernel Separation Implementation

### Address Space Layout (Example)

```
Virtual Address Space (39-bit = 512 GB):

0xFFFFFFFF_FFFFFFFF ┌───────────────────┐
                    │                   │
                    │   Kernel Space    │  (Top 256 GB)
                    │   (S-mode only)   │
                    │                   │
0xFFFFFFC0_00000000 ├───────────────────┤  ← Kernel base
                    │                   │
                    │    Unmapped       │
                    │                   │
0x00000040_00000000 ├───────────────────┤
                    │                   │
                    │    User Space     │  (Bottom 256 GB)
                    │   (U-mode OK)     │
                    │                   │
0x00000000_00000000 └───────────────────┘
```

### Key Steps for User/Kernel Separation

1. **Set up page tables** with different permissions:
   ```c
   // Kernel pages: no U bit
   pte = (ppn << 10) | PTE_R | PTE_W | PTE_X | PTE_V;
   
   // User pages: with U bit
   pte = (ppn << 10) | PTE_R | PTE_W | PTE_X | PTE_U | PTE_V;
   ```

2. **Enable paging** via satp register:
   ```c
   // Sv39 mode (8), ASID (0), PPN of root page table
   satp = (8ULL << 60) | (root_ppn);
   csr_write(satp, satp_val);
   sfence_vma();  // Flush TLB
   ```

3. **Switch to S-mode** from M-mode:
   ```asm
   # Set MPP to S-mode (01)
   li t0, (1 << 11)
   csrw mstatus, t0
   
   # Set return address
   la t0, s_mode_entry
   csrw mepc, t0
   
   # Return to S-mode
   mret
   ```

4. **Switch to U-mode** from S-mode:
   ```asm
   # Set SPP to U-mode (0)
   li t0, 0
   csrw sstatus, t0
   
   # Set return address to user code
   la t0, user_entry
   csrw sepc, t0
   
   # Return to U-mode
   sret
   ```

---

## 📊 Project Path Options

### Option A: Stay with E6A (M-mode only)
- ✅ Simpler, faster to learn
- ✅ Good for RTOS concepts
- ❌ No MMU, limited isolation
- **Best for**: Understanding scheduling, IPC, drivers

### Option B: Add MMU Support (Recommended for full OS)
- ✅ True user/kernel separation
- ✅ Process isolation
- ✅ Virtual memory
- ❌ More complex
- **Best for**: Understanding complete OS architecture

### Recommended Approach

**Phase 1-6**: Build RTOS core on QEMU virt (M-mode, RV32)
- Boot, UART, interrupts, scheduler, memory allocator

**Phase 7+**: Add MMU support (switch to RV64)
- Enable Sv39 paging
- Implement user/kernel separation
- Add process support

---

## 🔗 Resources

- [RISC-V Privileged Spec](https://github.com/riscv/riscv-isa-manual) - Chapter 4: Supervisor-Level ISA
- [xv6-riscv](https://github.com/mit-pdos/xv6-riscv) - MIT's teaching OS with full Sv39 implementation
- [rCore Tutorial](https://rcore-os.github.io/rCore-Tutorial-Book-v3/) - Rust OS tutorial with RISC-V MMU
