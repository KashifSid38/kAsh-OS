# kAsH-OS Learning Roadmap

## 📅 Complete Learning Plan with Time Estimates

This document provides a detailed breakdown of the learning journey to build a RISC-V RTOS from scratch.

---

## Phase 1: RISC-V Architecture Fundamentals (2-3 weeks)

### Week 1-2: Core Architecture
- [ ] **Day 1-3**: RISC-V history and design philosophy
  - Why RISC-V? Open ISA benefits
  - Comparison with ARM, x86
  - Modular extension system (RV32I, M, A, F, D, C)

- [ ] **Day 4-7**: Register architecture
  - 32 general-purpose registers (x0-x31)
  - Program Counter (PC)
  - Special registers (zero, ra, sp, gp, tp, etc.)
  - ABI naming conventions

- [ ] **Day 8-10**: Instruction formats
  - R-type, I-type, S-type, B-type, U-type, J-type
  - Immediate encoding
  - Instruction encoding/decoding

- [ ] **Day 11-14**: Core instruction set (RV32I)
  - Arithmetic: ADD, SUB, AND, OR, XOR, SLT
  - Loads/Stores: LW, SW, LB, SB, LH, SH
  - Branches: BEQ, BNE, BLT, BGE
  - Jumps: JAL, JALR

### Week 3: Privilege Architecture
- [ ] **Day 15-17**: Privilege levels
  - Machine mode (M-mode) - highest privilege
  - Supervisor mode (S-mode) - OS kernel
  - User mode (U-mode) - applications
  - Privilege level transitions

- [ ] **Day 18-21**: Control and Status Registers (CSRs)
  - mstatus, mtvec, mepc, mcause
  - mie, mip (interrupt registers)
  - CSR instructions: CSRRW, CSRRS, CSRRC

**Deliverables:**
- [ ] Summary notes on RISC-V architecture
- [ ] Handwritten register reference card
- [ ] Understanding of privilege modes

---

## Phase 2: Development Environment Setup (1 week)

### Week 4: Tools Installation
- [ ] **Day 1-2**: Install RISC-V GNU Toolchain
  - riscv64-unknown-elf-gcc
  - riscv64-unknown-elf-gdb
  - riscv64-unknown-elf-objdump

- [ ] **Day 3-4**: Install QEMU
  - qemu-system-riscv32
  - qemu-system-riscv64
  - Understanding QEMU virt machine

- [ ] **Day 5-7**: Setup development workflow
  - Makefile creation
  - GDB debugging setup
  - VS Code integration

**Deliverables:**
- [ ] Working toolchain
- [ ] Hello World bare-metal program running in QEMU
- [ ] Debugging workflow established

---

## Phase 3: Boot Process & Startup Code (2 weeks)

### Week 5: Understanding Boot Process
- [ ] **Day 1-3**: RISC-V boot sequence
  - Reset vector
  - Boot ROM behavior
  - Memory map understanding

- [ ] **Day 4-7**: Linker scripts
  - Memory regions definition
  - Section placement (.text, .data, .bss, .rodata)
  - Stack setup

### Week 6: Startup Assembly
- [ ] **Day 8-10**: Writing startup.S
  - Stack pointer initialization
  - BSS clearing
  - Global pointer setup
  - Jump to C main

- [ ] **Day 11-14**: First C code execution
  - Minimal C runtime setup
  - Calling C functions from assembly
  - Return handling

**Deliverables:**
- [ ] Working linker script
- [ ] Startup assembly code
- [ ] Boot to C main() function

---

## Phase 4: Basic Kernel & UART Driver (2 weeks)

### Week 7: UART Driver
- [ ] **Day 1-3**: Understanding UART
  - UART protocol basics
  - 16550 UART (QEMU virt)
  - Memory-mapped I/O

- [ ] **Day 4-7**: Implementing UART driver
  - uart_init()
  - uart_putc() - character output
  - uart_getc() - character input
  - printf() implementation

### Week 8: Basic Kernel Structure
- [ ] **Day 8-10**: Kernel organization
  - kmain() function
  - Basic kernel initialization sequence
  - Panic handling

- [ ] **Day 11-14**: Memory layout
  - Kernel memory map
  - Stack allocation
  - Heap area definition

**Deliverables:**
- [ ] Working UART driver
- [ ] printf() functionality
- [ ] Basic kernel that prints to console

---

## Phase 5: Interrupt Handling (2 weeks)

### Week 9: Exception Handling
- [ ] **Day 1-3**: RISC-V exception model
  - Synchronous exceptions
  - Exception causes (mcause)
  - Exception handling flow

- [ ] **Day 4-7**: Trap handler
  - trap_vector in assembly
  - Saving/restoring context
  - Dispatching to C handlers

### Week 10: Interrupts
- [ ] **Day 8-10**: External interrupts
  - PLIC (Platform Level Interrupt Controller)
  - Interrupt enable/disable
  - Interrupt priorities

- [ ] **Day 11-14**: Timer interrupts
  - CLINT (Core Local Interruptor)
  - mtime, mtimecmp registers
  - Periodic timer setup

**Deliverables:**
- [ ] Exception handler framework
- [ ] Timer interrupt working
- [ ] Foundation for preemptive scheduling

---

## Phase 6: Memory Management (3 weeks)

### Week 11-12: Heap Management
- [ ] **Day 1-7**: Simple allocator
  - First-fit allocator
  - malloc() / free() implementation
  - Memory fragmentation understanding

- [ ] **Day 8-14**: Better allocators
  - Buddy allocator concept
  - Slab allocator concept
  - Choose and implement one

### Week 13: Memory Protection (Optional for RTOS)
- [ ] **Day 15-21**: Physical Memory Protection (PMP)
  - PMP registers
  - Region configuration
  - Access permissions

**Deliverables:**
- [ ] Working heap allocator
- [ ] Memory allocation tests
- [ ] Understanding of memory protection

---

## Phase 7: Task Management & Context Switching (3-4 weeks)

### Week 14-15: Task Control Block
- [ ] **Day 1-7**: Task structure
  - Task Control Block (TCB) design
  - Task states (Ready, Running, Blocked, Suspended)
  - Task stack allocation

- [ ] **Day 8-14**: Task creation
  - task_create() function
  - Initial context setup
  - Task list management

### Week 16-17: Context Switching
- [ ] **Day 15-21**: Context switch implementation
  - Saving all registers to TCB
  - Loading registers from new TCB
  - Stack pointer switching

- [ ] **Day 22-28**: Testing context switch
  - Two-task switching
  - Verifying register preservation
  - Stack integrity checks

**Deliverables:**
- [ ] TCB implementation
- [ ] task_create() working
- [ ] Manual context switching between tasks

---

## Phase 8: Scheduler Implementation (3-4 weeks)

### Week 18-19: Basic Scheduler
- [ ] **Day 1-7**: Round-robin scheduler
  - Ready queue implementation
  - schedule() function
  - Timer-based preemption

- [ ] **Day 8-14**: Scheduler integration
  - Hooking scheduler to timer interrupt
  - Idle task
  - Task yield

### Week 20-21: Priority Scheduling
- [ ] **Day 15-21**: Priority-based scheduling
  - Priority levels
  - Priority queues
  - Priority inheritance (optional)

- [ ] **Day 22-28**: Advanced features
  - Time slicing
  - Real-time scheduling concepts
  - Rate Monotonic Scheduling (RMS)

**Deliverables:**
- [ ] Working preemptive scheduler
- [ ] Multiple tasks running concurrently
- [ ] Priority-based task selection

---

## Phase 9: IPC Mechanisms (2-3 weeks)

### Week 22-23: Synchronization Primitives
- [ ] **Day 1-7**: Mutex implementation
  - Spinlocks
  - Blocking mutex
  - Priority inversion problem

- [ ] **Day 8-14**: Semaphores
  - Counting semaphores
  - Binary semaphores
  - Producer-consumer problem

### Week 24: Message Passing
- [ ] **Day 15-21**: Message queues
  - Queue data structure
  - Blocking send/receive
  - Non-blocking variants

**Deliverables:**
- [ ] Mutex implementation
- [ ] Semaphore implementation
- [ ] Message queue implementation

---

## Phase 10: Advanced Topics (4+ weeks)

### Ongoing: Polish and Extend
- [ ] **Device drivers**: SPI, I2C, GPIO
- [ ] **File system**: Simple FAT or custom
- [ ] **Shell**: Command-line interface
- [ ] **Debugging**: Better debug output, assertions
- [ ] **Testing**: Unit tests, integration tests
- [ ] **Documentation**: API documentation
- [ ] **Real hardware**: Port to physical RISC-V board

---

## 📊 Time Summary

| Phase | Topic | Hours | Calendar Time |
|-------|-------|-------|---------------|
| 1 | RISC-V Architecture | 30-45 | 2-3 weeks |
| 2 | Dev Environment | 10-15 | 1 week |
| 3 | Boot Process | 20-30 | 2 weeks |
| 4 | Kernel & UART | 20-30 | 2 weeks |
| 5 | Interrupts | 20-30 | 2 weeks |
| 6 | Memory Management | 30-45 | 3 weeks |
| 7 | Task Management | 40-60 | 3-4 weeks |
| 8 | Scheduler | 40-60 | 3-4 weeks |
| 9 | IPC | 25-40 | 2-3 weeks |
| 10 | Advanced | 50+ | 4+ weeks |
| **Total** | | **285-415** | **24-30 weeks** |

**Assuming 10-15 hours/week: 6-9 months**

---

## 🎯 Milestones

1. **M1**: Boot and print "Hello, RISC-V!" (Week 6)
2. **M2**: Handle timer interrupts (Week 10)
3. **M3**: Two tasks switching manually (Week 17)
4. **M4**: Preemptive multitasking (Week 21)
5. **M5**: Full RTOS with IPC (Week 24)

---

## 💡 How Cline Can Help

1. **Code Implementation**: Write startup code, drivers, kernel modules
2. **Debugging**: Help debug assembly and C code issues
3. **Documentation**: Create detailed notes and explanations
4. **Code Review**: Review your implementations
5. **Testing**: Help create test cases
6. **Explanations**: Deep-dive into any concept
7. **Architecture Decisions**: Discuss design choices

---

## 📝 Weekly Workflow

1. **Study**: Read documentation/tutorials (2-3 hours)
2. **Implement**: Write code with Cline's help (4-6 hours)
3. **Debug**: Test and fix issues (2-3 hours)
4. **Document**: Write notes in docs/ (1-2 hours)
5. **Commit**: Push to git repository (ongoing)

---

## 🔄 Progress Tracking

Update this section as you progress:

- [ ] Phase 1 Complete - Date: ____
- [ ] Phase 2 Complete - Date: ____
- [ ] Phase 3 Complete - Date: ____
- [ ] Phase 4 Complete - Date: ____
- [ ] Phase 5 Complete - Date: ____
- [ ] Phase 6 Complete - Date: ____
- [ ] Phase 7 Complete - Date: ____
- [ ] Phase 8 Complete - Date: ____
- [ ] Phase 9 Complete - Date: ____
- [ ] Phase 10 Complete - Date: ____
