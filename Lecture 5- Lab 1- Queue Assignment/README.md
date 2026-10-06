# FreeRTOS Queue
## Purpose
This program demonstrates inter-task communication in FreeRTOS by safely passing simulated temperature data from a producer task (Sensor) to a consumer task (Display) using a queue.
## How it works
The Sensor Task generates temperature readings and uses the `xQueueSend` function to send a copy of the data to the Queue. The Queue acts as a buffer (storing up to 5 elements) and delivers these copies to the Display Task in a FIFO order. The Display Task waits for data to arrive `xQueueReceive()` and then prints it to the serial monitor.
## Serial Monitor

<img width="2038" height="1136" alt="temperature_screen" src="https://github.com/user-attachments/assets/937d8b32-0e34-45ee-80e8-fe5ae5b36a06" />


## Reflection
The queue is useful in this program because it provides an organized and safe way to pass data between tasks. It can safely store multiple values when sends succeed, acting as a buffer so that no temperature readings are lost if the Sensor and Display tasks operate at different speeds.
