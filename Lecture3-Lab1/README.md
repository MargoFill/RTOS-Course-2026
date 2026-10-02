# RTOS Scheduler and Preemption Investigation
## Assignment purpose
The assignment purpose is to observe:
* How the FreeRTOS scheduler selects tasks
* Running, Ready and Blocked states
* Task priorities
* Preemption
* Equal-priority behaviour

in Real Time Systems.
## Hardware and software
### Hardware
* ESP32-S3-DevKitC-1 v1.1
### Software
* Arduino IDE 2.x
* Serial Monitor
* 115200 baud
## Test scenarios
### Scenario A
* Task A = priority 1
* Task B = priority 2

### Scenario B
* Task A = priority 1
* Task B = priority 1
  
### Scenario C
* Task A = priority 1
* Task B = priority 3


## Prediction and observation table

| Scenario | Prediction | Actual Observation | Match? Why? |
| :--- | :--- | :--- | :--- |
| **A: Task B priority = 2** | **Task B preempts Task A.**<br>As soon as Task A sends the notification, it is suspended, and Task B runs. | `Task A is printing slowly...Task B is running! Task A continues and finishes`| **Yes.** Preemptive scheduling. Since priority B (2) is higher than A (1), the scheduler immediately yields the core to Task B upon calling `xTaskNotifyGive()`. |
| **B: Task B priority = 1** | **Task B does NOT preempt Task A immediately. Tasks will time-slice, or B will run only when A blocks. | `Task A is printing slowly... Task A continues and finishes! Task B is running!` | **Yes.** Priorities are equal. The notification makes Task B "Ready" but doesn't trigger immediate preemption. The scheduler switches to Task B at the next OS tick or when A calls `vTaskDelay()`. |
| **C: Task B priority = 3** | **Task B preempts Task A. Behavior is exactly the same as Scenario A. |`Task A is printing slowly...Task B is running! Task A continues and finishes` | **Yes.** Any priority strictly higher than the current one (3 > 1) forces an immediate context switch. The absolute difference in priority doesn't matter. |
## Explanation
The FreeRTOS scheduler manages tasks using three main states. The Running state means a task is actively executing on the CPU. The Ready state applies to a task that is prepared to run but is waiting for CPU time. The Blocked state means a task is paused, waiting for a specific event or time delay.

In our code, Task B is initially Blocked because it calls ulTaskNotifyTake(portMAX_DELAY). It waits infinitely for a notification, allowing Task A to enter the Running state and print.

When Task A calls xTaskNotifyGive(), it triggers the event Task B is waiting for. This immediately moves Task B from the Blocked state to the Ready state.

Task B can preempt Task A because it has a higher priority (2 vs 1). FreeRTOS uses a preemptive scheduler: when a higher-priority task becomes Ready, it instantly pauses the lower-priority Running task and takes over the CPU. This physically interrupts Task A mid-word.

If Task B has the same priority as Task A, immediate preemption fails. Task B becomes Ready but must wait for the next OS tick (time-slicing) or until Task A blocks itself (e.g., via vTaskDelay).

Finally, Task A can continue exactly where it stopped due to a context switch. When preempted, the OS saves Task A's exact memory state and program counter. Once Task B finishes and blocks, the OS restores Task A's context, letting it seamlessly finish printing.
