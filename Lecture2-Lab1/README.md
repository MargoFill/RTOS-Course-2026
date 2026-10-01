# RTOS Mini Assignment: ESP32-S3 Task Scheduling & States

## 1. Hardware and software
* **Board:** ESP32-S3-DevKitC-1 v1.1
* **Onboard output:** Addressable RGB LED (GPIO 38)
* **Software:** Arduino IDE with "esp32 by Espressif Systems"
* **Serial Monitor:** 115200 baud

## 2. Baseline implementation
Both tasks were successfully created and pinned to Core 1. Task A prints a periodic heartbeat to the Serial Monitor every 1000 ms, while Task B controls the onboard RGB LED, keeping it ON for 500 ms and OFF for 500 ms.

## 3. Prediction and observation table

| Scenario | Prediction before test | Actual observation | Did it match? Why? |
| :--- | :--- | :--- | :--- |
| **A.** Both tasks at priority 1 | Both tasks run concurrently. | Both the Serial Monitor output and the LED blinking operated smoothly in parallel. | Yes. Tasks with identical priorities share processor time and operate concurrently.|
| **B.** Task A priority 2; Task B priority 1 | Task A will execute first. | Task A prints first right after creation, but then both run periodically without visible interruptions. | Yes. Because Task A is priority 2, it instantly preempts setup() and runs first. However, it doesn't dominate permanently because vTaskDelay() blocks it, giving the CPU to Task B. |
| **C.** Task A priority 1; Task B priority 2 | Task B will execute first. | Task B starts its LED logic instantly upon creation, but overall both tasks continue working smoothly. | Yes. Task B preempts the creation process because of priority 2. But again, vTaskDelay() puts it in the Blocked state, allowing Task A to take the CPU.|

## 4. Priority experiments
During scenarios A, B, and C, the priority values inside `xTaskCreatePinnedToCore()` were adjusted accordingly (e.g., setting Task A or Task B to priority `2` while keeping the other at `1`). The tests demonstrated that priority alone does not dictate periodic frequency; because periodic tasks regularly block, lower-priority tasks still receive sufficient execution time.

## 5. Starvation experiment
* **Configuration:** Task A set to priority `2`, Task B set to priority `1`, both pinned to Core 1. The `vTaskDelay(pdMS_TO_TICKS(1000))` inside Task A was temporarily commented out.
* **Result:** The Serial Monitor rapidly flooded with continuous "Task A alive" messages.
* **Explanation:** Without `vTaskDelay()`, Task A never enters the Blocked state. Since it maintains a higher priority, the FreeRTOS scheduler continuously allocates the single core to Task A, starving Task B of CPU time and keeping it perpetually in the Ready state.

## 6. Ready, Running and Blocked explanation
When a task is actively executing instructions on the CPU, it is in the **Running** state. To prevent a task from occupying the processor continuously, calling `vTaskDelay()` moves the task into the **Blocked** state for the specified duration, freeing the CPU for other operations. Once the delay time expires, the task transitions from Blocked to the **Ready** state, indicating it is prepared to execute but waiting for the scheduler. The FreeRTOS scheduler constantly selects the highest-priority task that is in the Ready state to run next. 

During our normal periodic tests (Scenarios A, B, and C), priority changes did not visibly break execution because tasks frequently entered the Blocked state. Even when a task had a higher priority, it relinquished the CPU during its delays, permitting lower-priority tasks to transition from Ready to Running. However, during the temporary starvation experiment, removing `vTaskDelay()` from the higher-priority Task A kept it permanently out of the Blocked state. Because it remained Ready or Running indefinitely, the scheduler never assigned the CPU to the lower-priority Task B, causing complete starvation.

## 7. Final restored configuration
* **Status:** Confirmed. The temporary starvation experiment has been reverted. 
* **Delays:** All `vTaskDelay()` calls are fully active in `RTOS_Mini_Assignment.ino`.
* **Priorities:** Both tasks are restored to priority `1` (or matching the final requirement) and pinned to Core 1.
