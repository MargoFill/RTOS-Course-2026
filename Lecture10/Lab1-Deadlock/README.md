# Deadlock

## Part A

**1. Which two tasks can cause a deadlock? Explain how they can end up waiting for each other.**  
`Task_Sensor` and `Task_Logger` can cause a deadlock. `Task_Sensor` acquires `i2cMutex` first and then waits for `uartMutex`, while `Task_Logger` acquires `uartMutex` first and then waits for `i2cMutex`. If both tasks lock their first mutex at the same time, neither can continue because each holds the mutex the other needs.

**2. Can Task_Display create a circular wait by itself? Explain why or why not.**  
No. A circular wait requires at least two tasks forming a closed chain where each task holds one resource and waits for another. `Task_Display` only acquires a single mutex (`i2cMutex`), so it never holds one mutex while waiting for a second one.

**3. Which Coffman condition is removed when all tasks acquire mutexes in the same order?**  
Acquiring mutexes in the same global order removes the **circular wait** condition.


## Part B
### Step 1 – Observing the deadlock

#### Serial Monitor Output

```
=== Demo 1: Deadlock | DEMO_MODE 0 ===
[Task_1] taking A ...
[Task_1] has A, now needs B
[Task_2] taking B ...
[Task_2] has B, now needs A
[HEARTBEAT] t=2 s | rounds: Task_1=0 Task_2=0
[HEARTBEAT] t=3 s | rounds: Task_1=0 Task_2=0
[HEARTBEAT] t=4 s | rounds: Task_1=0 Task_2=0
[HEARTBEAT] t=20 s | rounds: Task_1=0 Task_2=0
[HEARTBEAT] t=21 s | rounds: Task_1=0 Task_2=0
[HEARTBEAT] t=22 s | rounds: Task_1=0 Task_2=0
[HEARTBEAT] t=23 s | rounds: Task_1=0 Task_2=0
[HEARTBEAT] t=24 s | rounds: Task_1=0 Task_2=0
[HEARTBEAT] t=25 s | rounds: Task_1=0 Task_2=0
```
**What happens to the worker tasks?**

Both Task_1 and Task_2 acquire their first mutex (A and B, respectively) and then block forever while trying to take the second mutex. Neither task can complete a round.

**Which numbers in the `HEARTBEAT` line stop changing?**

The rounds counters (Task_1=0 and Task_2=0) stop increasing, while the uptime counter t continues to increment every second.


### Step 2 – Fixing the lock order

#### Serial Monitor Output

```
[Task_1] taking A ...
[Task_1] has A, now needs B
[Task_2] taking A ...
[Task_1] has BOTH locks -> work done (round 1)
[Task_2] has A, now needs B
[Task_1] taking A ...
[Task_2] has BOTH locks -> work done (round 1)
[Task_1] has A, now needs B
[Task_1] has A, now needs B
[Task_1] has BOTH locks -> work done (round 35)
[HEARTBEAT] t=123 s | rounds: Task_1=35 Task_2=31
[Task_2] has A, now needs B
```

**Why the tasks can now continue working:**

Both Task_1 and Task_2 now acquire mutexA first and mutexB second. Whichever task locks mutexA first is guaranteed to acquire mutexB next, finish its work, and release both mutexes so the other task can proceed without circular waiting.


### Step 3 – Observing timeout and backoff

#### Serial Monitor Output

```
=== Demo 1: Deadlock | DEMO_MODE 1 ===
[Task_1] taking A ...
[Task_1] has A, now needs B
[Task_2] taking A ...
[Task_1] has BOTH locks -> work done (round 1)
[Task_2] has A, now needs B
[Task_1] taking A ...
[Task_2] has BOTH locks -> work done (round 1)
[Task_1] has A, now needs B
[Task_1] has BOTH locks -> work done (round 2)
```

When a task cannot acquire the second mutex within the 500 ms timeout, xSemaphoreTake fails and the task aborts its current attempt instead of blocking forever. It must release its first mutex so the other task is not blocked and can acquire both locks to finish its work. Finally, the random backoff delay prevents both tasks from retrying at the exact same moment and repeatedly colliding in a livelock.

### What I learned

Through this experiment, I learned how easily a deadlock occurs in a multitasking FreeRTOS system when two tasks lock shared mutexes in opposite orders. Observing the frozen task counters on the ESP32 showed the practical impact of circular wait, and modifying the lock sequence demonstrated that enforcing a consistent lock order is a simple and reliable way to prevent deadlocks.

