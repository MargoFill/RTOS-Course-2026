# Dual-Core Data Acquisition and Processing

## 1. Application Architecture

The application implements a dual-core data processing pipeline on the ESP32-S3 using FreeRTOS primitives:

```text
[SensorProducer Task (Core 0)]
              │
              ▼  FreeRTOS Queue (SensorReading)
[DataConsumer Task (Core 1)] ──► updates ──► [SharedStatistics]
                                                   ▲
                                                   │ protected by statisticsMutex
[Monitor in loop() (Core 1)] ──── reads ───────────┘
```

All FreeRTOS resources (queue, mutex, and both pinned tasks) are validated at startup in setup(), halting safely via fatalError() if allocation fails.

## 2. Responsibility of the Producer Task

The SensorProducer task runs on Core 0 every 500 ms using vTaskDelayUntil(). It generates a simulated temperature reading (22.00 to 24.80), attaches an incrementing sequence number and creation timestamp (producedAt), and pushes the SensorReading struct into readingQueue. It never accesses SharedStatistics directly.

## 3. Responsibility of the Consumer Task

The DataConsumer task runs on Core 1 and blocks on xQueueReceive() until a new reading arrives. When data is received, it calculates the queue transfer latency, checks whether the value exceeds the alarm threshold (24.20), acquires statisticsMutex, updates SharedStatistics, copies a local snapshot, and immediately releases the mutex before printing to the Serial Monitor.

## 4. Purpose of the Monitor

The Arduino loop() function acts as a periodic system monitor running every 3 seconds. It acquires statisticsMutex, copies the entire SharedStatistics structure into a local snapshot, releases the mutex right away, and prints a summary report (count, avg, min, max, alarms, and maxLat) to the Serial Monitor.

## 5. Why Producer and Consumer Use Different Cores

Pinning SensorProducer to Core 0 and DataConsumer to Core 1 separates data acquisition from data processing across the ESP32-S3's two Xtensa LX7 cores. In a real embedded system, this prevents heavy downstream calculations, logging, or alarm handling on Core 1 from delaying time-critical sensor sampling on Core 0.

## 6. Why a Queue Is Used

A FreeRTOS queue (readingQueue) transfers complete SensorReading structs safely between Core 0 and Core 1 without shared-memory race conditions. It also decouples the two tasks: the queue buffers up to 8 readings if the consumer is momentarily busy, while xQueueReceive(..., portMAX_DELAY) lets the consumer sleep without wasting CPU cycles when no new samples are available.

## 7. Why Shared Statistics Require a Mutex

SharedStatistics contains multiple related fields (sampleCount, sum, runningAverage, minimum, maximum, alarmCount, maxLatencyMs) written by the Consumer and read by the Monitor. Without statisticsMutex, the Monitor could read a partially updated struct mid-calculation (for example, seeing an incremented sampleCount paired with an outdated runningAverage). Keeping the critical section limited to a fast struct copy ensures Serial.printf() never blocks the other task.

## 8. Extensions Beyond the Lecture Demonstration

Compared to the lecture starter code, two extensions were added:

Threshold Alarm (HIGH_TEMP_THRESHOLD = 24.20): Whenever a reading exceeds 24.20 (24.40 and 24.80 in the simulated waveform), the Consumer increments alarmCount inside SharedStatistics and prints an [ALERT] message after releasing the mutex.

Queue Latency Tracking: Using the producedAt tick stamp in SensorReading, the Consumer measures the end-to-end transfer delay (lat) in milliseconds for every sample and records maxLatencyMs in SharedStatistics.

## 9. How to Compile, Upload, and Test

Open Dual_Core_Data_Pipeline.ino in Arduino IDE 2.x.

Select Tools → Board → ESP32S3 Dev Module (and enable Tools → USB CDC On Boot → Enabled if using the native USB port).

Click Upload to flash the ESP32-S3 DevKitC-1.

Open Tools → Serial Monitor and set the baud rate to 115200.

Let the board run for at least 10–12 seconds to observe more than 20 readings, periodic Monitor summaries, and threshold alerts.

## 10. Serial Monitor Output

The screenshot confirms the dual-core execution and synchronization behavior:

Every [Producer | Core 0] line runs on Core 0, incrementing seq every 500 ms and keeping queued=1 (or 0 when immediately consumed).

Every [Consumer | Core 1] line runs on Core 1, updating avg, min, max, count, alarms, and transfer latency (lat).

Whenever value reaches 24.40 or 24.80, [ALERT | Core 1] triggers and increments the alarm counter.

Every 3 seconds, [Monitor | Core 1] prints a consistent snapshot of the shared statistics matching the Consumer's state.
