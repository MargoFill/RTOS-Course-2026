# Task Control and Independent Activities
## Hardware and Software
* Board: ESP32-S3-DevKitC-1 v1.1
* RGB LED: GPIO 38
* IDE: Arduino IDE 2.x
* Serial Monitor: 115200 baud
## Task Design
The application is designed around two independent activities managed by the FreeRTOS scheduler:
* Serial Task (Task A): This task continuously checks for user input via the Serial Monitor. It reads commands, validates them, and updates the shared blinkInterval variable (if the input is 250, 500, or 1000). It also listens for suspend and resume commands to control Task B.
* RGB LED Task (Task B): This task runs in an infinite loop, turning the onboard RGB LED on and off. It uses the blinkInterval shared variable in its vTaskDelay() function to determine how long to wait between state changes.
* Core Assignment: Both tasks are pinned to Core 1 (RUNNING_CORE = 1) to demonstrate how the FreeRTOS scheduler allocates time on a single processor core.
* Priorities: The LED Task is set to Priority 2, while the Serial Task is set to Priority 1.
## Task Handles
`TaskHandle_t TaskAHandle=NULL;` -> Serial Task Handler
`TaskHandle_t taskBHandle = NULL;` -> LED Task Handler

When creating the tasks, we pass the memory address of the handle to xTaskCreatePinnedToCore(). The OS populates this variable with the reference. We then use this handle in the Serial Task to control the LED task (e.g., vTaskSuspend(taskBHandle);).
## Suspend / Resume Test
* suspend: When the user types suspend, the Task A calls vTaskSuspend(taskBHandle). The LED task instantly freezes in its current state (either ON or OFF). Task A continues to run and accept inputs normally because it is completely independent.
* resume: When the user types resume, the Task A calls vTaskResume(taskBHandle). The LED task immediately picks up exactly where it left off, finishing its delay cycle and continuing to blink using the most recently set blinkInterval
## Prediction
| Situation | Prediction | Actual Observation | Match? |
|---|---|---|---|
| LED task running normally | The LED will blink at the default 500ms. Entering 250, 500, or 1000 will change the blink speed. | The LED blinks normally. Entering valid numbers updates the speed instantly. Invalid inputs throw an error. | Yes. |
| LED task suspended | The LED will freeze in its current state (on or off). Serial will keep working. | The LED stopped blinking entirely. We could still send new intervals via the Serial Monitor. | Yes. |
| LED task resumed | The LED will start blinking again using the most recently assigned interval. | The LED immediately resumed blinking with the latest speed settings. | Yes. |
## State Analysis
### Situation 1 – Normal operation
* Task A: Blocked / Running (It spends most of its time in the Blocked state waiting for vTaskDelay to finish, briefly entering the Running state to check for Serial input).
* Task B: Blocked / Running (It stays in the Blocked state during the LED on/off intervals, briefly switching to Running to change the LED state).

### Situation 2 – B task is suspended
* Task A: Blocked / Running (Unaffected, continues checking for input normally).
* Task B: Suspended (It was explicitly moved to this state by vTaskSuspend(). The scheduler ignores it entirely).

### Situation 3 – B task is resumed
* Task A: Blocked / Running (Unaffected).
* Task B: Ready -> Blocked/Running (Calling vTaskResume() moves the task into the Ready state. The scheduler then allows it to return to its normal Running/Blocked cycle).
## Reflection
We use a task handle as a unique reference to a specific task instance, allowing us to interact with and control it from entirely different parts of the application. When vTaskSuspend(taskBHandle); is called, the LED task transitions immediately into the Suspended state. In this state, it is completely ignored by the FreeRTOS scheduler and will not consume any CPU time, pausing its execution exactly where it was. Conversely, when vTaskResume(taskBHandle); is called, the task is moved from the Suspended state into the Ready state, making it eligible for the scheduler to allocate CPU time to it again.

The Serial Task (Task A) can continue working perfectly when the LED Task is suspended because FreeRTOS manages them as completely independent activities. Since they run concurrently under the scheduler's control, suspending one simply removes it from the scheduling queue, leaving the other to transition normally between its Blocked (waiting for delays) and Running states. This demonstrates the core advantage of an RTOS: blocking or suspending one task does not halt the microprocessor's ability to execute other tasks.
