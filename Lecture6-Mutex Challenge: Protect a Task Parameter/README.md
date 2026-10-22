# Lecture 6 -Assignment - Mutex Challenge: Protect a Task Parameter

## Overview
This project demonstrates how to safely pass a local variable from the `setup()` function to a newly created FreeRTOS task using a Mutex. 

The code was specifically adapted for an **ESP32-S3** board with a built-in WS2812 (NeoPixel) RGB LED on pin 38. We also increased the task stack size to `2048` bytes to prevent a `Stack canary watchpoint triggered` (Stack Overflow) error caused by `Serial.print()` memory usage on ESP32 chips.

## Expected Result
When a delay value (in milliseconds) is entered into the Serial Monitor, the `setup()` function safely passes this value to the `blinkLED` task. The `setup()` function successfully waits for the parameter to be copied before finishing, and the RGB LED blinks at the specified rate.

---

## Questions

### 1. Why can passing the address of a local variable to a task be unsafe?
Local variables (like `delay_arg` in `setup()`) are stored on the stack frame of the function where they are declared. If the `setup()` function finishes and exits before the newly created task has a chance to read the variable, that stack memory is freed and can be overwritten. When the task eventually tries to read the pointer, it will read garbage/corrupted data, leading to undefined behavior.

### 2. When does blinkLED copy the parameter?
The `blinkLED` task copies the parameter immediately after it successfully acquires (takes) the mutex using `xSemaphoreTake(mutex, portMAX_DELAY);`. Once the parameter is safely copied into its own local variable (`num`), it releases (gives) the mutex back.

### 3. How does the mutex make setup() wait?
The `setup()` function forces synchronization by doing the following:
1. It takes the mutex *before* creating the task.
2. It gives the mutex and adds a tiny delay, yielding the CPU so `blinkLED` can start and take the mutex.
3. `setup()` immediately tries to take the mutex again using `xSemaphoreTake(mutex, portMAX_DELAY);`. 
Because `blinkLED` currently holds the mutex while copying the variable, `setup()` is forced into a **Blocked state** and waits. It only unblocks and continues once `blinkLED` calls `xSemaphoreGive()`.

### 4. Why is this described as a demonstration hack? What would be a better design in a real application?
This is a "hack" because **Mutexes** (Mutual Exclusion) are designed to protect shared resources (like a shared variable, I/O port, or memory pool) from being accessed by multiple tasks simultaneously. They are *not* designed for signaling or synchronizing the execution flow between tasks.

**Better designs for a real application:**
- **Task Notifications (or Direct to Task Notifications):** The most lightweight and fastest way in FreeRTOS to unblock a task or send a simple state/value.
- **Binary Semaphores:** Specifically designed for signaling and synchronization between tasks (or between an ISR and a task).
- **Queues:** If multiple values need to be sent safely, a FreeRTOS Queue is the standard approach to pass data between tasks by copy, completely avoiding the local variable pointer issue.
