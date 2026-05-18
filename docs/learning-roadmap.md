# kAsH-OS Learning Roadmap

The phased plan for building kAsH-OS and using it to develop systems-engineering judgement. Read `GUIDE.md` for the operating principles that govern *how* I work through these phases.

## Premise

- **Cadence:** ~6 hrs/week (5–8 realistic), two sessions of ~3 hrs each.
- **Total horizon:** ~20 months to a multitasking QEMU OS with virtual memory, syscalls, userland, simple FS, and SMP. Hardware port deferred — decide on a board after the QEMU OS is solid.
- Every phase ships a runnable artifact AND at least one journal/retrospective entry. No phase ships only code.
- Buffer ~30% slack per phase for debugging time. Stuck-on-bugs is the default, not the exception.

## Status (2026-05-16)

The repo has working scaffold: boot, UART, banner, trap-handler dispatch, echo loop. The code was AI-scaffolded during an earlier exploration phase; Phase 0 below addresses that honestly. **No phase is "complete" until I can explain every line of its code in my own words without referring to AI** (see `GUIDE.md` for the full definition of done).

---

## Phase 0 — Reset & internalize the existing scaffold (2-3 weeks)

Read every line of `Makefile`, `linker.ld`, `src/boot/startup.S`, `src/kernel/main.c`, `src/kernel/uart.c`, `src/include/*.h`. For each non-trivial source file, write a `<file>.notes.md` sibling that explains in my own words what each block does and why each choice was made. When I can write the notes without consulting AI, I own the file.

**Deliverables**

- `*.notes.md` sibling files for `startup.S`, `main.c`, `uart.c`, `linker.ld`, and `Makefile`.
- Journal entries (per session) capturing what surprised me and what's still murky.
- A re-built kernel running in QEMU that I can explain end-to-end, including: what `mstatus` does, what `mtvec` is, why BSS gets cleared, what the linker script's section ordering implies, what each UART register does.
- Concept note: "What happens between QEMU `-kernel` and my `kmain` running?" in `docs/concepts/`.

**Why first.** Pretending to start from a green field would re-create the same illusion. Owning what's already there is the only honest starting point.

---

## Phase 1 — RISC-V deep dive: traps and timers (3-4 weeks)

Read the RISC-V Privileged Spec sections on traps and CSRs. The current trap handler partially saves caller-saved registers; finish the save/restore correctly and verify with deliberate test exceptions. Set up CLINT timer interrupts via `mtimecmp` / `mtime`. Hook timer ticks into a logged tick counter visible over UART.

**Deliverables**

- Trap handler that correctly saves/restores all caller-saved registers and returns cleanly via `mret`.
- A deliberate divide-by-zero / illegal-instruction test that shows the handler decoding it correctly.
- CLINT timer firing every ~10 ms, logged.
- Concept note: "RISC-V trap delivery, in my own words" in `docs/concepts/`.

---

## Phase 2 — Memory management (5-6 weeks)

Big phase. The current code is RV32 with no MMU. Decide whether to switch to RV64 + Sv39 here (recommended — Sv39 is the canonical RISC-V virtual memory and matches xv6) or stay RV32 with PMP only.

Implement a physical page allocator, kernel page tables, basic kmalloc.

**Decision point.** RV32 → RV64 transition probably happens here. Document the trade-off in the journal *before* deciding. Consider: how much existing code changes? Does staying RV32 force me to learn fewer real-world OS concepts?

**Deliverables**

- Physical frame allocator (bitmap or freelist; my choice, explained in journal).
- If RV64: kernel mapped via Sv39 page tables, identity or higher-half (my choice, explained).
- If RV32: PMP regions configured to protect kernel code and stack.
- `kmalloc` + `kfree` (simple first-fit is fine; better algorithms can wait).
- Concept note on virtual memory translation (or PMP mechanics if I stay RV32).

**Risk.** This phase kills most hobby OSes. If stuck >8 weeks, stop and read xv6's `vm.c` carefully — but only after seriously attempting the design myself.

---

## Phase 3 — Multitasking (4-5 weeks)

Kernel threads first with cooperative `yield()`, then preemptive scheduling driven by the timer interrupt from Phase 1. Context-switch in assembly. Two threads visibly time-slicing over UART.

**Deliverables**

- TCB struct (designed by me first, then compared against xv6's `proc` struct in a journal entry).
- `yield()` working between two threads.
- Preemptive scheduler triggered by timer interrupt.
- Two threads printing different patterns interleaved on UART.

---

## Phase 4 — Synchronization (3-4 weeks)

Spinlocks (with correct interrupt-disable semantics for the M-mode / S-mode case), sleep locks, condition variables. Deliberately introduce a race condition and a deadlock; observe the symptoms; fix them; document the experience.

**Deliverables**

- Spinlock implementation with interrupt-disable safety.
- Sleep lock implementation built on top of the scheduler from Phase 3.
- Producer-consumer demo using a sleep lock and condition variable.
- Journal entry: "the race I introduced, what I observed, how I diagnosed it" — this entry is more important than the code.

---

## Phase 5 — User mode + syscalls (3-4 weeks)

Drop to U-mode (this assumes RV64 + S-mode from Phase 2 — revisit if I stayed RV32). Install syscall handler via `ecall`. Implement `copy_to_user` / `copy_from_user`. Run the first userland binary, linked separately and loaded by the kernel.

**Deliverables**

- U-mode entry working.
- 5–10 basic syscalls: at minimum `write`, `read`, `exit`, `getpid`, `sleep`.
- A userland "hello world" binary running as a process and exiting cleanly.

---

## Phase 6 — Processes and IPC (4-5 weeks)

Process abstraction (vs raw threads). `fork` (or `spawn` if I prefer simpler semantics). Basic IPC via pipes or message queues (not both — pick one and own it). A shell-like launcher that spawns processes and waits for them.

**Deliverables**

- Process struct with full lifecycle (create, run, exit, reap).
- Working pipe OR message queue.
- Mini-shell that spawns and waits for child processes.

---

## Phase 7 — Block device + simple filesystem (4-6 weeks)

A virtio-blk driver. Then a simple filesystem — xv6-style is the well-trodden path; an even simpler design is fine if I can defend it. Persistence across QEMU reboots.

**Deliverables**

- virtio-blk driver reading and writing sectors.
- File create / read / write / delete.
- Demo: write a file, reboot QEMU, read it back.

---

## Phase 8 — SMP (3-4 weeks; optional but high-value)

Boot secondary harts, add per-CPU state, set up IPIs. Audit every spinlock from Phase 4 for SMP correctness — this is the moment that latent bugs become visible.

**Deliverables**

- 4 harts running the scheduler.
- A test that demonstrates a previously-passing single-core lock failing under SMP, then the fix.

---

## Phase 9 — Hardware port (4-6 weeks; deferred until QEMU OS is solid)

Pick a board at decision-time based on what concepts I most want to exercise on real silicon. Candidates: VisionFive 2, HiFive Unmatched (full Sv39 + multi-core), or K210 / ESP32-C3-class (cheaper, M-mode only). Adapt boot path, UART, timer, memory map.

**Deliverables**

- Same OS booting on a real RISC-V board.
- Journal entry: "what QEMU lied to me about" — the differences I had to discover the hard way.

---

## Phase 10 — Retrospective + polish (2-3 weeks)

Write the design retrospective. What did I learn? What would I redo? What design choices do I still disagree with? Consider writing up the project as a blog series — strong artifact for the IDP review.

**Deliverables**

- 5–10 page design retrospective in `docs/`.
- Optional: blog post(s) suitable for sharing.

---

## Quarterly checkpoints

Every 3 months, one weekend session for:

1. Re-read all journal entries from the quarter.
2. Refactor anything that's been bothering me.
3. Compare current code/design against xv6 for the corresponding subsystem.
4. Write a 1-page retrospective and commit it to `docs/journal/`.

Quarterly checkpoints are where engineering judgement compounds. Skipping them means shipping more code with less learning.

---

## Progress tracker (honest)

A phase is "completed" only when ALL of the following are true (per `GUIDE.md`):

1. Its deliverables are met.
2. I can explain every line of its code in my own words.
3. The journal has at least one entry per work session in the phase.
4. There is at least one concept note in `docs/concepts/` covering the phase's central idea.

| Phase | Status | Started | Completed | Notes |
|-------|--------|---------|-----------|-------|
| 0 — Internalize scaffold | not started | — | — | — |
| 1 — Traps & timers | not started | — | — | — |
| 2 — Memory management | not started | — | — | — |
| 3 — Multitasking | not started | — | — | — |
| 4 — Synchronization | not started | — | — | — |
| 5 — U-mode & syscalls | not started | — | — | — |
| 6 — Processes & IPC | not started | — | — | — |
| 7 — Block dev & FS | not started | — | — | — |
| 8 — SMP | not started | — | — | — |
| 9 — Hardware port | deferred | — | — | — |
| 10 — Retrospective | not started | — | — | — |
