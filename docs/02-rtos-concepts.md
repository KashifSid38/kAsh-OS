# RTOS Concepts Reference

## Overview

This document covers the core concepts of Real-Time Operating Systems that we'll implement in kAsH-OS. Understanding these concepts is crucial before diving into implementation.

---

## 🎯 What is an RTOS?

A Real-Time Operating System is an OS designed to serve real-time application requests. It must process data as it comes in, typically without buffer delays.

### Key Characteristics

| Feature | GPOS (General Purpose) | RTOS |
|---------|------------------------|------|
| **Timing** | Best effort | Deterministic |
| **Scheduling** | Fairness-focused | Priority-focused |
| **Latency** | Variable | Bounded, predictable |
| **Response** | Average good | Worst-case guaranteed |
| **Use Case** | Desktop, servers | Embedded, control systems |

### Types of Real-Time Systems

1. **Hard Real-Time**: Missing deadline = system failure
   - Example: Airbag deployment, pacemaker
   
2. **Firm Real-Time**: Missing deadline = degraded quality
   - Example: Video streaming
   
3. **Soft Real-Time**: Missing deadline = reduced value
   - Example: Data logging

---

## 📋 Task Management

### Task (Thread) Concept

A **task** is an independent unit of execution with its own:
- Program counter
- Stack
- Register context
- Priority
- State

### Task Control Block (TCB)

The TCB is the data structure that stores all task information:

```c
typedef struct {
    // Context (saved registers)
    uint32_t registers[32];     // x0-x31
    uint32_t pc;                // Program counter
    uint32_t mstatus;           // Machine status
    
    // Stack
    uint32_t *stack_pointer;
    uint32_t *stack_base;
    uint32_t stack_size;
    
    // Task metadata
    uint32_t task_id;
    uint32_t priority;
    task_state_t state;
    char name[16];
    
    // Timing
    uint32_t time_slice;
    uint32_t remaining_time;
    uint32_t total_runtime;
    
    // Linked list
    struct tcb *next;
    struct tcb *prev;
} tcb_t;
```

### Task States

```
                    ┌─────────────────┐
                    │                 │
        create()    │    CREATED      │
                    │                 │
                    └────────┬────────┘
                             │ start()
                             ▼
┌───────────────────────────────────────────────────┐
│                                                   │
│         ┌─────────────┐    schedule()   ┌─────────┴─────────┐
│         │             │◄────────────────│                   │
│         │   RUNNING   │                 │       READY       │
│         │             │────────────────►│                   │
│         └──────┬──────┘    preempt()    └─────────┬─────────┘
│                │                                  │
│                │ wait()/sleep()                   │
│                │                                  │
│                ▼                                  │
│         ┌─────────────┐                          │
│         │             │      event/timeout       │
│         │   BLOCKED   │──────────────────────────┘
│         │             │
│         └─────────────┘
│
└───────────────────────────────────────────────────┘
```

**States:**
- **CREATED**: Task initialized but not started
- **READY**: Waiting for CPU, ready to run
- **RUNNING**: Currently executing (only one per CPU)
- **BLOCKED**: Waiting for event (I/O, semaphore, timer)
- **SUSPENDED**: Explicitly paused
- **TERMINATED**: Finished execution

### Task Creation

```c
// Task function signature
typedef void (*task_func_t)(void *arg);

// Create a new task
int task_create(
    tcb_t *tcb,           // Task control block
    const char *name,     // Task name
    task_func_t func,     // Entry point
    void *arg,            // Argument to function
    uint32_t *stack,      // Stack memory
    uint32_t stack_size,  // Stack size in bytes
    uint32_t priority     // Task priority
);
```

---

## ⚙️ Context Switching

### What is Context Switching?

The process of saving the current task's state and loading another task's state. This is the core mechanism enabling multitasking.

### Context Switch Steps

1. **Save Context** (current task)
   - Push all registers to stack (or TCB)
   - Save PC (program counter)
   - Save status registers
   - Save stack pointer to TCB

2. **Switch Stack**
   - Update current task pointer
   - Load new task's stack pointer

3. **Restore Context** (new task)
   - Restore status registers
   - Restore PC
   - Pop all registers from stack
   - Return to execution

### RISC-V Context Switch Assembly

```asm
# context_switch(old_tcb, new_tcb)
# a0 = pointer to old TCB
# a1 = pointer to new TCB

context_switch:
    # Save caller-saved registers to old TCB
    sw ra,  0(a0)
    sw sp,  4(a0)
    sw s0,  8(a0)
    sw s1, 12(a0)
    sw s2, 16(a0)
    sw s3, 20(a0)
    sw s4, 24(a0)
    sw s5, 28(a0)
    sw s6, 32(a0)
    sw s7, 36(a0)
    sw s8, 40(a0)
    sw s9, 44(a0)
    sw s10, 48(a0)
    sw s11, 52(a0)
    
    # Load new task's registers
    lw ra,  0(a1)
    lw sp,  4(a1)
    lw s0,  8(a1)
    lw s1, 12(a1)
    lw s2, 16(a1)
    lw s3, 20(a1)
    lw s4, 24(a1)
    lw s5, 28(a1)
    lw s6, 32(a1)
    lw s7, 36(a1)
    lw s8, 40(a1)
    lw s9, 44(a1)
    lw s10, 48(a1)
    lw s11, 52(a1)
    
    ret  # Return to new task
```

### Context Switch Overhead

- **Time**: Typically 1-10 microseconds
- **Cost factors**: Register saves/loads, cache effects, TLB flushes
- **Optimization**: Minimize context size, avoid unnecessary switches

---

## 📊 Scheduling Algorithms

### 1. Round-Robin (Time-Slicing)

Each task gets equal time slice, tasks rotate in circular order.

```
Time: →→→→→→→→→→→→→→→→→→→→→→→→→→→→→→→→
      |  Task A  |  Task B  |  Task C  |  Task A  |
      |  10ms    |  10ms    |  10ms    |  10ms    |
```

**Pros**: Fair, simple, no starvation
**Cons**: Not priority-aware, high context switch overhead

```c
tcb_t *schedule_round_robin(void) {
    tcb_t *next = current_task->next;
    while (next != current_task) {
        if (next->state == READY) {
            return next;
        }
        next = next->next;
    }
    return idle_task;
}
```

### 2. Priority-Based (Fixed Priority)

Higher priority tasks run first. Tasks at same priority use round-robin.

```
Priority 3: │██████████│          High
Priority 2: │          │███████│
Priority 1: │                  │████│  Low
```

**Pros**: Important tasks get CPU first
**Cons**: Can cause starvation

```c
tcb_t *schedule_priority(void) {
    tcb_t *highest = NULL;
    tcb_t *task = task_list;
    
    while (task != NULL) {
        if (task->state == READY) {
            if (highest == NULL || 
                task->priority > highest->priority) {
                highest = task;
            }
        }
        task = task->next;
    }
    return highest ? highest : idle_task;
}
```

### 3. Priority with Aging

Gradually increase waiting task priorities to prevent starvation.

```c
void age_tasks(void) {
    tcb_t *task = task_list;
    while (task != NULL) {
        if (task->state == READY && task != current_task) {
            task->effective_priority++;
        }
        task = task->next;
    }
}
```

### 4. Rate Monotonic Scheduling (RMS)

For periodic tasks: shorter period = higher priority.

```
Task A: Period=10ms, Execution=3ms → Priority HIGH
Task B: Period=20ms, Execution=5ms → Priority MEDIUM
Task C: Period=50ms, Execution=8ms → Priority LOW
```

**Schedulability test**: Σ(Ci/Ti) ≤ n(2^(1/n) - 1)

### 5. Earliest Deadline First (EDF)

Dynamic priority based on closest deadline.

```
Time 0:  Task A (deadline 15) runs
Time 5:  Task B (deadline 12) preempts (closer deadline)
Time 10: Task B completes, Task A resumes
```

---

## 🔒 Synchronization Primitives

### Spinlock

Busy-wait lock for short critical sections.

```c
typedef struct {
    volatile uint32_t locked;
} spinlock_t;

void spinlock_acquire(spinlock_t *lock) {
    while (__sync_lock_test_and_set(&lock->locked, 1)) {
        // Spin
    }
}

void spinlock_release(spinlock_t *lock) {
    __sync_lock_release(&lock->locked);
}
```

### Mutex (Blocking Lock)

Puts task to sleep while waiting.

```c
typedef struct {
    tcb_t *owner;
    tcb_t *wait_queue;
    uint32_t locked;
} mutex_t;

void mutex_lock(mutex_t *m) {
    disable_interrupts();
    if (m->locked) {
        // Add current task to wait queue
        current_task->state = BLOCKED;
        add_to_queue(&m->wait_queue, current_task);
        enable_interrupts();
        schedule();
    } else {
        m->locked = 1;
        m->owner = current_task;
        enable_interrupts();
    }
}

void mutex_unlock(mutex_t *m) {
    disable_interrupts();
    if (m->wait_queue != NULL) {
        tcb_t *next = remove_from_queue(&m->wait_queue);
        next->state = READY;
        m->owner = next;
    } else {
        m->locked = 0;
        m->owner = NULL;
    }
    enable_interrupts();
}
```

### Semaphore

Counting synchronization primitive.

```c
typedef struct {
    int32_t count;
    tcb_t *wait_queue;
} semaphore_t;

void sem_wait(semaphore_t *s) {  // P operation
    disable_interrupts();
    s->count--;
    if (s->count < 0) {
        current_task->state = BLOCKED;
        add_to_queue(&s->wait_queue, current_task);
        enable_interrupts();
        schedule();
    } else {
        enable_interrupts();
    }
}

void sem_signal(semaphore_t *s) {  // V operation
    disable_interrupts();
    s->count++;
    if (s->count <= 0) {
        tcb_t *task = remove_from_queue(&s->wait_queue);
        task->state = READY;
    }
    enable_interrupts();
}
```

### Priority Inversion Problem

When a high-priority task waits for a lock held by a low-priority task, while a medium-priority task runs.

**Solution: Priority Inheritance**
```c
void mutex_lock_with_inheritance(mutex_t *m) {
    if (m->locked && m->owner->priority < current_task->priority) {
        // Temporarily boost owner's priority
        m->owner->priority = current_task->priority;
    }
    // ... rest of lock logic
}
```

---

## 📬 Inter-Process Communication (IPC)

### Message Queue

```c
typedef struct {
    void *buffer;
    uint32_t msg_size;
    uint32_t capacity;
    uint32_t count;
    uint32_t head;
    uint32_t tail;
    semaphore_t empty_slots;
    semaphore_t filled_slots;
    mutex_t lock;
} message_queue_t;

int mq_send(message_queue_t *mq, void *msg) {
    sem_wait(&mq->empty_slots);
    mutex_lock(&mq->lock);
    
    memcpy(mq->buffer + mq->tail * mq->msg_size, msg, mq->msg_size);
    mq->tail = (mq->tail + 1) % mq->capacity;
    mq->count++;
    
    mutex_unlock(&mq->lock);
    sem_signal(&mq->filled_slots);
    return 0;
}

int mq_receive(message_queue_t *mq, void *msg) {
    sem_wait(&mq->filled_slots);
    mutex_lock(&mq->lock);
    
    memcpy(msg, mq->buffer + mq->head * mq->msg_size, mq->msg_size);
    mq->head = (mq->head + 1) % mq->capacity;
    mq->count--;
    
    mutex_unlock(&mq->lock);
    sem_signal(&mq->empty_slots);
    return 0;
}
```

### Event Flags

```c
typedef struct {
    uint32_t flags;
    tcb_t *wait_queue;
} event_group_t;

// Wait for any of the specified flags
uint32_t event_wait(event_group_t *eg, uint32_t flags, int wait_all) {
    while (1) {
        disable_interrupts();
        if (wait_all) {
            if ((eg->flags & flags) == flags) {
                enable_interrupts();
                return eg->flags & flags;
            }
        } else {
            if (eg->flags & flags) {
                enable_interrupts();
                return eg->flags & flags;
            }
        }
        current_task->state = BLOCKED;
        add_to_queue(&eg->wait_queue, current_task);
        enable_interrupts();
        schedule();
    }
}

void event_set(event_group_t *eg, uint32_t flags) {
    disable_interrupts();
    eg->flags |= flags;
    // Wake up all waiting tasks to check flags
    while (eg->wait_queue != NULL) {
        tcb_t *task = remove_from_queue(&eg->wait_queue);
        task->state = READY;
    }
    enable_interrupts();
}
```

---

## ⏱️ Timer Management

### Software Timers

```c
typedef struct timer {
    uint32_t expiry_tick;
    uint32_t period;
    void (*callback)(void *arg);
    void *arg;
    int active;
    int periodic;
    struct timer *next;
} timer_t;

void timer_tick(void) {
    system_ticks++;
    timer_t *t = timer_list;
    
    while (t != NULL) {
        if (t->active && system_ticks >= t->expiry_tick) {
            t->callback(t->arg);
            
            if (t->periodic) {
                t->expiry_tick = system_ticks + t->period;
            } else {
                t->active = 0;
            }
        }
        t = t->next;
    }
}
```

---

## 💾 Memory Management

### Stack Allocation

Each task needs its own stack. Common approaches:

1. **Static allocation**: Fixed-size arrays at compile time
2. **Pool allocation**: Pre-allocated pool of fixed-size stacks
3. **Dynamic allocation**: Heap allocation (more flexible)

```c
#define STACK_SIZE 1024
static uint32_t task_stacks[MAX_TASKS][STACK_SIZE];

tcb_t *create_task(task_func_t func, int priority) {
    static int task_count = 0;
    
    tcb_t *tcb = allocate_tcb();
    tcb->stack_base = task_stacks[task_count];
    tcb->stack_size = STACK_SIZE * sizeof(uint32_t);
    tcb->stack_pointer = tcb->stack_base + STACK_SIZE - 1;
    
    // Initialize stack with initial context
    *(--tcb->stack_pointer) = (uint32_t)func;  // PC
    // ... push initial register values
    
    task_count++;
    return tcb;
}
```

### Stack Overflow Detection

```c
#define STACK_MAGIC 0xDEADBEEF

void init_stack(uint32_t *stack, uint32_t size) {
    // Fill with pattern
    for (int i = 0; i < size; i++) {
        stack[i] = STACK_MAGIC;
    }
}

int check_stack_overflow(tcb_t *task) {
    return task->stack_base[0] != STACK_MAGIC;
}
```

---

## 🔗 Resources

- [FreeRTOS Kernel Reference](https://www.freertos.org/RTOS.html)
- [Operating Systems: Three Easy Pieces](https://pages.cs.wisc.edu/~remzi/OSTEP/)
- [Real-Time Systems - Jane Liu](https://www.pearson.com/us/higher-education/program/Liu-Real-Time-Systems/PGM153424.html)
- [The Little Book of Semaphores](https://greenteapress.com/wp/semaphores/)
