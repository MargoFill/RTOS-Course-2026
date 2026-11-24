# Priority Inversion

## Part A

**1. With a binary semaphore, what happens after Task_H requests the lock? Explain how Task_M affects Task_L.**  

When `Task_H` requests the binary semaphore at 100 ms, it blocks because `Task_L` already holds it, while `Task_L` stays at priority 1. At 150 ms, `Task_M` (priority 2) wakes up and preempts `Task_L` (priority 1). Because `Task_M` runs for 1000 ms without blocking, it prevents `Task_L` from finishing its critical section and releasing the lock, causing unbounded priority inversion for `Task_H`.

**2. Timeline (Binary Semaphore – `USE_MUTEX 0`):**

0 ms        100 ms      150 ms                    1150 ms     1300 ms     1350 ms
|---Task_L---|--Task_L---|---------Task_M----------|---Task_L---|--Task_H---|
(takes lock,  (H requests (preempts L at prio 1;    (finishes    (gets lock,
 section 1)    lock &      runs 1000 ms without      critical     runs 50 ms
               blocks)     using the lock)           section)     & finishes)


**3. What changes when a mutex with priority inheritance is used? Describe and draw the new timeline.**

When Task_H (priority 3) blocks waiting for the mutex held by Task_L (priority 1), FreeRTOS temporarily raises Task_L's priority to 3. When Task_M (priority 2) wakes up at 150 ms, it cannot preempt Task_L. Task_L finishes its critical section at ~300 ms, releases the mutex, and Task_H runs immediately; Task_M runs only after Task_H finishes.


0 ms        100 ms      150 ms      300 ms      350 ms                    1350 ms
|---Task_L---|--------Task_L--------|---Task_H---|---------Task_M----------|
(takes mutex, (L inherits prio 3;    (gets mutex, (runs 1000 ms after H
 section 1)    M wakes at 150 ms      runs 50 ms   and L finish critical
               but cannot preempt)    & finishes)  work)


**4. Which task temporarily inherits a higher priority? What priority does it inherit?**

Task_L temporarily inherits priority 3 (the priority of Task_H).

## Part B

### Step 1 - Binary semaphore

### Step 2 - Mutex with priority inheritance

### Step 3 - Change the workload


## Part C
