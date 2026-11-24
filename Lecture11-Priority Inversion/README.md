# Priority Inversion

## Part A

**1. With a binary semaphore, what happens after Task_H requests the lock? Explain how Task_M affects Task_L.**  

When `Task_H` requests the binary semaphore at 100 ms, it blocks because `Task_L` already holds it, while `Task_L` stays at priority 1. At 150 ms, `Task_M` (priority 2) wakes up and preempts `Task_L` (priority 1). Because `Task_M` runs for 1000 ms without blocking, it prevents `Task_L` from finishing its critical section and releasing the lock, causing unbounded priority inversion for `Task_H`.

**2. Timeline (Binary Semaphore – `USE_MUTEX 0`):**

| Time (approx.) | Running Task | Event / State |
| :--- | :--- | :--- |
| **0 – 100 ms** | `Task_L` (prio 1) | Takes the binary semaphore and runs critical section 1/3 |
| **100 – 150 ms** | `Task_L` (prio 1) | `Task_H` (prio 3) requests the lock and blocks; `Task_L` continues at prio 1 |
| **150 – 1150 ms** | `Task_M` (prio 2) | Wakes up, **preempts** `Task_L`, and runs 1000 ms of work without using the lock |
| **1150 – 1300 ms** | `Task_L` (prio 1) | Resumes, finishes critical sections 2/3 & 3/3, and releases the semaphore |
| **1300 – 1350 ms** | `Task_H` (prio 3) | Acquires the lock after ~1200 ms wait, runs 50 ms, and finishes |

**3. What changes when a mutex with priority inheritance is used? Describe and draw the new timeline.**  
When `Task_H` (priority 3) blocks waiting for the mutex held by `Task_L` (priority 1), FreeRTOS temporarily raises `Task_L`'s priority to 3. When `Task_M` (priority 2) wakes up at 150 ms, it cannot preempt `Task_L`. `Task_L` finishes its critical section at ~300 ms, releases the mutex, and `Task_H` runs immediately; `Task_M` runs only after `Task_H` finishes.

| Time (approx.) | Running Task | Event / State |
| :--- | :--- | :--- |
| **0 – 100 ms** | `Task_L` (prio 1) | Takes the mutex and runs critical section 1/3 |
| **100 – 150 ms** | `Task_L` (**prio 3**) | `Task_H` (prio 3) requests the mutex and blocks; `Task_L` inherits priority 3 |
| **150 – 300 ms** | `Task_L` (**prio 3**) | `Task_M` (prio 2) wakes up at 150 ms, **cannot preempt** `Task_L`; `Task_L` finishes and releases the mutex |
| **300 – 350 ms** | `Task_H` (prio 3) | Acquires the mutex after ~200 ms wait, runs 50 ms, and finishes (`Task_L` returns to prio 1) |
| **350 – 1350 ms** | `Task_M` (prio 2) | Runs its 1000 ms workload after `Task_H` finishes |

**4. Which task temporarily inherits a higher priority? What priority does it inherit?**  
`Task_L` temporarily inherits priority **3** (the priority of `Task_H`).

## Part B

### Step 1 - Binary semaphore

### Step 2 - Mutex with priority inheritance




### Step 3 - Change the workload

## Part C

**1. Why does Task_H wait longer when a binary semaphore is used?**

When a binary semaphore is used, Task_L keeps its low priority of 1 while holding the lock. This allows the medium-priority Task_M to preempt Task_L and run its entire workload before Task_L can release the semaphore, forcing high-priority Task_H to wait for both tasks.

**2. Explain how Task_L inherits the priority of Task_H when a mutex is used.**

When Task_H attempts to take a mutex currently owned by Task_L, the FreeRTOS kernel detects that a higher-priority task is blocked on that resource. As a result, Task_L temporarily inherits the priority of Task_H (priority 3) until it releases the mutex, at which point its original priority of 1 is restored.

**3. Why can Task_M no longer preempt Task_L while Task_L has inherited priority 3?**

The FreeRTOS scheduler always runs the highest-priority Ready task on the core. Because Task_L temporarily inherits priority 3, its effective priority is higher than Task_M's priority of 2, so Task_M cannot preempt Task_L.

**4. What is the main difference between a binary semaphore and a mutex in this demonstration?**

A mutex enforces ownership and includes priority inheritance, so a low-priority holder inherits a blocked task's high priority and cannot be delayed by medium-priority tasks that would otherwise preempt it. A binary semaphore has no ownership or priority inheritance mechanism, leaving the system vulnerable to priority inversion.

## Part C
№№
