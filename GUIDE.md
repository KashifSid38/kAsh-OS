# kAsH-OS Operating Guide

This is the north-star document for the kAsH-OS project. It is more important than any individual line of code in this repo. It exists to keep me honest over the ~20-month duration of this learning project.

**Re-read this at the start of every new phase, and any session where I feel scattered or stuck. Update it whenever a principle proves wrong or a new lesson emerges. This is a living document.**

---

## Why I'm doing this

To stop being a firmware engineer who can build anything in a super-loop and become a systems engineer who understands what's happening underneath. The OS is the artifact; the engineering judgement is the goal. Both matter — neither alone is the win.

This is also part of my 2026 Personal Development Plan with my manager at AMD. The journal entries, phase retrospectives, and quarterly reviews are evidence I can show at year-end.

---

## Definition of done — for the project

When all of the following are true:

1. Boots on QEMU `virt` machine.
2. Multitasking with a preemptive scheduler driven by a timer interrupt.
3. Virtual memory (Sv39 if I take the RV64 path, PMP otherwise — TBD in Phase 2).
4. User mode with syscalls and at least one userland binary.
5. A simple filesystem persisting across reboots.
6. SMP support, with the scheduler running across multiple harts.
7. Same OS boots on a real RISC-V board.
8. A written design journal that explains every non-obvious decision I made.
9. A final retrospective that compares my design to xv6 / OSTEP and identifies what I'd redo.

If I can't tick a box, the project isn't done. If I'm tempted to add features beyond this list, they go in `BACKLOG.md` and I move on.

---

## Definition of done — for a single phase

A phase is complete only when ALL of the following are true:

1. The phase's deliverables (per `docs/learning-roadmap.md`) are met.
2. I can explain every line of code added in the phase in my own words, without referring to AI.
3. The journal has at least one entry per work session in the phase.
4. There is at least one concept note in `docs/concepts/` covering the phase's central idea.

A phase that produces only working code, with no understanding I can articulate, is not complete. It is technical debt.

---

## Operating principles

These are the rules I commit to. They exist because past hobby OS projects (mine and others') have failed in predictable ways.

### 1. Build first. Read reference implementations second.

Always attempt the design myself before peeking at xv6 or Linux. The gap between my attempt and theirs is where the learning is. Reading the reference first robs me of that.

After my attempt: read xv6 for the same subsystem and write a comparison note. What did I miss? What did I overcomplicate? What did I get right?

### 2. Write the *why*, not the *what*.

Code shows what. The journal explains why I chose this allocator over that one, why I used a spinlock here and a sleep lock there, what I tried and discarded. If I skip this, I'll have a working OS and shaky design intuition — exactly the failure mode to avoid.

### 3. AI is a Socratic partner, not autocomplete.

This is the highest-stakes rule, given how this project started.

**Allowed**

- "Explain this concept and let me try."
- "Review this code I wrote — what did I miss?"
- "Why is my page-fault handler causing a double fault?"
- "What are the trade-offs between a buddy allocator and a slab allocator for kernel memory?"

**Not allowed**

- "Write this function for me."
- "Generate the page-table walk routine."
- "Scaffold the scheduler."

**Test:** if AI gave me code and I couldn't reproduce it from scratch a day later, or couldn't explain every line, I didn't learn it — I just shipped it. In that case: delete the code, walk away, come back when I can derive it myself.

### 4. The 2-session rule

If I've spent two full sessions stuck on the same bug or concept, stop. Write up exactly what's happening: what I tried, what I expected, what I got, what I've already ruled out. Then ask AI / read someone else's solution / post on r/osdev. Don't grind alone past two sessions; it just deepens confusion.

### 5. Each phase ships

No phase blocks the next phase indefinitely. If I'm at 1.5x the budgeted weeks for a phase and still not done, I declare "good enough for now," log the gap in `KNOWN-ISSUES.md` (which I'll create when needed), and move on. Perfectionism kills 20-month projects.

### 6. Commit small, commit often

Even broken WIP commits, prefixed `[WIP]`. The git history is part of the learning artifact. Future me will read it like a diary.

End every session: `git add -A && git commit && git push`. Start every session: `git pull`.

### 7. Separate study sessions from build sessions

A session is either *study* (read spec, watch lecture, take notes in `docs/concepts/`) or *build* (code, debug, commit). Mixing the two produces shallow notes and shallow code.

### 8. Quarterly retrospective is sacred

Every 3 months: re-read all journal entries from the quarter, refactor anything that's been bothering me, compare to xv6 for the relevant subsystem, write a 1-page retrospective. This is where engineering judgement compounds.

### 9. The journal is non-negotiable

10 minutes at the end of every work session. Use the template in `docs/journal/_template.md`. **No journal entry → session doesn't count toward the phase.**

---

## Anti-patterns to watch for

A short list of failure modes for projects like this. Read regularly.

- **Tinkering with the build system instead of writing kernel code.** Tempting on a frustrating evening; produces nothing.
- **Adding "cool features" outside the current phase.** Park in `BACKLOG.md`. Resist.
- **Reading too much theory without writing code.** Both ends are bad: pure theory or pure code. Alternate.
- **Letting AI write code I don't understand,** then hitting a bug I can't debug because the code isn't mine. The fastest way to make this project fail.
- **Comparing my hobby OS to Linux and feeling bad.** I'm learning. Linux had thousands of person-years.
- **Long gaps without re-reading my own journal.** Even 15 min on a "dead" week to skim the last entry keeps the thread alive.
- **Marking a phase "complete" when only the code works.** Per the definition of done above, this is a lie.

---

## Cadence

- **Target:** ~6 hrs/week. 5–8 realistic given full-time work.
- **Two sessions per week:** one weekday evening (~3 hrs), one weekend morning (~3 hrs). Anything shorter than 2 hrs is mostly context-reload.
- **Journal entry:** end of every session, 10 minutes, not optional.
- **Commit + push:** end of every session, no exceptions.
- **Phase retrospective:** end of each phase (~every 4–6 weeks).
- **Quarterly retrospective:** every 3 months.

---

## Canonical resources

The few I actually use, not 50 links I never open.

- **xv6 book** (MIT, free) — the reference implementation. Re-read the relevant chapter before each phase. <https://pdos.csail.mit.edu/6.S081/2023/xv6/book-riscv-rev3.pdf>
- **OSTEP — Operating Systems: Three Easy Pieces** (free online) — concept explanations. <https://pages.cs.wisc.edu/~remzi/OSTEP/>
- **RISC-V Unprivileged Spec** — primary source for ISA questions. <https://github.com/riscv/riscv-isa-manual>
- **RISC-V Privileged Spec** — primary source for traps, CSRs, modes. <https://github.com/riscv/riscv-isa-manual>
- **OpenSBI documentation** — for the boot / M-mode story.
- **xv6-riscv source** — read directly, after attempting my own version. <https://github.com/mit-pdos/xv6-riscv>
- **One forum:** r/osdev or osdev.org wiki — when truly stuck.

If I find myself collecting bookmarks instead of reading what I have, I'm procrastinating, not learning.

---

## When I update this guide

After every phase retrospective, ask: which principle proved wrong? Which one is missing? Edit and commit.

This document is the most valuable thing in the repo. The OS is the byproduct.
