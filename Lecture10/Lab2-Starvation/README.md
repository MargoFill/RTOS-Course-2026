# Starvation
## Part A

 **Which tasks may experience starvation? Explain why.**

Both `Task_Display` (priority 2) and `Task_Log` (priority 1) will experience starvation. Because all tasks run on the same core and FreeRTOS uses preemptive priority-based scheduling, `Task_Control` (priority 3) stays continuously in the Ready/Running state and monopolizes the CPU, preventing lower-priority tasks from executing.

**Why is it dangerous for a high-priority task to run continuously without blocking?**

It starves all lower-priority tasks—freezing display updates, logging, and background system tasks—which makes the device unresponsive and can trigger the task watchdog timer (TWDT), causing a system reset.

**Give two ways to allow lower-priority tasks to get CPU time without changing task priorities.**

1. Add periodic blocking delays such as `vTaskDelay()` or `vTaskDelayUntil()` inside the high-priority task's loop.  
2. Make the high-priority task event-driven by blocking on a semaphore, queue, or task notification (e.g., `xSemaphoreTake()` or `ulTaskNotifyTake()`) until new sensor data is ready instead of continuously polling.

## Part B

### Step 1 – Observing starvation


**Serial Monitor Output**

```
=== Demo 2: Starvation | STARVATION_FIX 0 ===
t= 499 ms | Task_Hog: SLEEP | Task_Low progress: +500
t= 999 ms | Task_Hog: SLEEP | Task_Low progress: +500
t=1499 ms | Task_Hog: SLEEP | Task_Low progress: +500
t=1999 ms | Task_Hog: SLEEP | Task_Low progress: +500
t=14999 ms | Task_Hog: BUSY  | Task_Low progress: +2498
t=15913 ms | Task_Hog: BUSY  | Task_Low progress: +0
t=16413 ms | Task_Hog: BUSY  | Task_Low progress: +0
t=16913 ms | Task_Hog: BUSY  | Task_Low progress: +0
t=17413 ms | Task_Hog: BUSY  | Task_Low progress: +0
```

**What is the progress of Task_Low while Task_Hog is BUSY?**

+0 (zero progress).

**What is the progress of Task_Low while Task_Hog is SLEEP?**

Around +500 every 500 ms (approximately 1 increment per 1 ms RTOS tick).

**What do these results tell you about starvation?**

A higher-priority task that never blocks completely prevents lower-priority tasks on the same core from running, even when those tasks are ready to execute.

### Step 2 – Enabling the fix

**Serial Monitor Output**

```
=== Demo 2: Starvation | STARVATION_FIX 1 ===
t= 499 ms | Task_Hog: BUSY  | Task_Low progress: +50
t= 999 ms | Task_Hog: BUSY  | Task_Low progress: +50
t=1499 ms | Task_Hog: BUSY  | Task_Low progress: +50
t=1999 ms | Task_Hog: BUSY  | Task_Low progress: +50
t=4999 ms | Task_Hog: SLEEP | Task_Low progress: +2059
t=7145 ms | Task_Hog: BUSY  | Task_Low progress: +1105
t=7645 ms | Task_Hog: BUSY  | Task_Low progress: +50
t=8145 ms | Task_Hog: BUSY  | Task_Low progress: +50
t=8645 ms | Task_Hog: BUSY  | Task_Low progress: +50
t=9145 ms | Task_Hog: SLEEP | Task_Low progress: +191
t=9645 ms | Task_Hog: SLEEP | Task_Low progress: +500
```

**Comparison with Step 1:**

During the BUSY period, Task_Low now makes steady progress (around +45 to +50 per 500 ms) instead of being stuck at +0 as in Step 1.

**Why Task_Low can now make progress while Task_Hog is busy:**

Calling vTaskDelay(1) moves Task_Hog from the Running state into the Blocked state for 1 tick every 10 ms. While Task_Hog is blocked, the FreeRTOS scheduler assigns the CPU to the highest-priority ready task (Task_Low), allowing it to execute its work step.

### Step 3 – Changing the delay


**Prediction:**

Changing vTaskDelay(1) to vTaskDelay(10) will block Task_Hog for 10 ticks instead of 1 tick every time it yields, giving Task_Low significantly more CPU time and increasing its progress during the BUSY period (to roughly +250 per 500 ms).

**Serial Monitor Output**

```
=== Demo 2: Starvation | STARVATION_FIX 1 ===
t= 499 ms | Task_Hog: BUSY  | Task_Low progress: +266
t= 999 ms | Task_Hog: BUSY  | Task_Low progress: +264
t=1499 ms | Task_Hog: BUSY  | Task_Low progress: +260
t=1999 ms | Task_Hog: BUSY  | Task_Low progress: +265
```

**Observation and explanation:**

During the BUSY period, Task_Low progress increased substantially (from around +45–50 to roughly +250 per 500 ms). Because vTaskDelay(10) puts Task_Hog into the Blocked state for 10 RTOS ticks (10 ms) after every 10 ms of work, Task_Hog spends about half of the period blocked. This gives Task_Low much longer windows to run and increment its counter.


### What I Learned
Through this experiment, I learned how FreeRTOS priority scheduling can cause starvation when a higher-priority task runs a continuous loop without blocking. Observing Task_Low freeze at +0 and then resume once vTaskDelay() was added demonstrated that high-priority tasks must periodically block or yield so lower-priority tasks can share the CPU.

