/*
  Lecture 12: Multicore Systems
  ESP32-S3 Dual-Core Producer-Consumer-Monitor Demonstration (Extended)

  Board: ESP32-S3 DevKitC-1
  Serial Monitor: 115200 baud

  Extensions implemented:
  1. Threshold Alarm: flags readings above HIGH_TEMP_THRESHOLD (24.20)
     and tracks total alarm count in SharedStatistics.
  2. Queue Latency Tracking: measures transfer delay (ms) between
     Producer (Core 0) and Consumer (Core 1) using producedAt ticks.
*/

#include <Arduino.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "freertos/semphr.h"

// ------------------------------------------------------------
// Data transferred through the queue
// ------------------------------------------------------------
struct SensorReading {
  uint32_t sequence;
  float value;
  TickType_t producedAt;
};

// ------------------------------------------------------------
// Shared data protected by statisticsMutex
// ------------------------------------------------------------
struct SharedStatistics {
  uint32_t sampleCount;
  uint32_t alarmCount;
  uint32_t maxLatencyMs;
  float sum;
  float runningAverage;
  float minimum;
  float maximum;
};

// ------------------------------------------------------------
// Configuration
// ------------------------------------------------------------
constexpr UBaseType_t QUEUE_LENGTH = 8;
constexpr uint32_t PRODUCER_PERIOD_MS = 500;
constexpr uint32_t QUEUE_WAIT_MS = 100;
constexpr uint32_t MUTEX_WAIT_MS = 50;
constexpr uint32_t MONITOR_PERIOD_MS = 3000;
constexpr uint32_t PRODUCER_STACK = 4096;
constexpr uint32_t CONSUMER_STACK = 4096;
constexpr float HIGH_TEMP_THRESHOLD = 24.20f;

// ------------------------------------------------------------
// FreeRTOS objects
// ------------------------------------------------------------
QueueHandle_t readingQueue = nullptr;
SemaphoreHandle_t statisticsMutex = nullptr;

// ------------------------------------------------------------
// Shared statistics instance
// ------------------------------------------------------------
SharedStatistics statistics = {
  0,      // sampleCount
  0,      // alarmCount
  0,      // maxLatencyMs
  0.0f,   // sum
  0.0f,   // runningAverage
  0.0f,   // minimum
  0.0f    // maximum
};

// ------------------------------------------------------------
// Fatal error handler
// ------------------------------------------------------------
void fatalError(const char *message) {
  Serial.printf("FATAL: %s\n", message);
  Serial.flush();
  while (true) {
    delay(1000);
  }
}

// ------------------------------------------------------------
// Simulated sensor (22.00 to 24.80 in 0.40 steps)
// ------------------------------------------------------------
float makeSimulatedReading(uint32_t sequence) {
  return 22.0f + static_cast<float>(sequence % 8U) * 0.4f;
}

// ============================================================
// TASK 1: SENSOR PRODUCER (Pinned to Core 0)
// ============================================================
void sensorProducerTask(void *parameter) {
  (void)parameter;

  uint32_t sequence = 0;
  TickType_t lastWake = xTaskGetTickCount();

  for (;;) {
    SensorReading reading;
    reading.sequence = sequence++;
    reading.value = makeSimulatedReading(reading.sequence);
    reading.producedAt = xTaskGetTickCount();

    if (xQueueSend(readingQueue, &reading, pdMS_TO_TICKS(QUEUE_WAIT_MS)) == pdPASS) {
      Serial.printf(
          "[Producer | Core %d] seq=%lu value=%.2f queued=%u\n",
          xPortGetCoreID(),
          static_cast<unsigned long>(reading.sequence),
          reading.value,
          static_cast<unsigned>(uxQueueMessagesWaiting(readingQueue)));
    } else {
      Serial.printf(
          "[Producer | Core %d] queue full, dropped seq=%lu\n",
          xPortGetCoreID(),
          static_cast<unsigned long>(reading.sequence));
    }

    vTaskDelayUntil(&lastWake, pdMS_TO_TICKS(PRODUCER_PERIOD_MS));
  }
}

// ============================================================
// TASK 2: DATA CONSUMER (Pinned to Core 1)
// ============================================================
void dataConsumerTask(void *parameter) {
  (void)parameter;

  SensorReading reading;

  for (;;) {
    if (xQueueReceive(readingQueue, &reading, portMAX_DELAY) == pdPASS) {
      SharedStatistics snapshot;
      bool updated = false;

      // Calculate queue transfer latency before taking the mutex
      uint32_t latencyMs = static_cast<uint32_t>(
          (xTaskGetTickCount() - reading.producedAt) * portTICK_PERIOD_MS);
      bool thresholdExceeded = (reading.value > HIGH_TEMP_THRESHOLD);

      // Critical section: update shared statistics and take a snapshot
      if (xSemaphoreTake(statisticsMutex, pdMS_TO_TICKS(MUTEX_WAIT_MS)) == pdTRUE) {
        if (statistics.sampleCount == 0) {
          statistics.minimum = reading.value;
          statistics.maximum = reading.value;
          statistics.maxLatencyMs = latencyMs;
        } else {
          if (reading.value < statistics.minimum) {
            statistics.minimum = reading.value;
          }
          if (reading.value > statistics.maximum) {
            statistics.maximum = reading.value;
          }
          if (latencyMs > statistics.maxLatencyMs) {
            statistics.maxLatencyMs = latencyMs;
          }
        }

        if (thresholdExceeded) {
          statistics.alarmCount++;
        }

        statistics.sampleCount++;
        statistics.sum += reading.value;
        statistics.runningAverage = statistics.sum / statistics.sampleCount;

        snapshot = statistics;
        updated = true;

        // Release mutex immediately before any Serial printing
        xSemaphoreGive(statisticsMutex);
      }

      // Print results outside the mutex-protected section
      if (updated) {
        Serial.printf(
            "[Consumer | Core %d] seq=%lu avg=%.2f min=%.2f max=%.2f count=%lu alarms=%lu lat=%lums\n",
            xPortGetCoreID(),
            static_cast<unsigned long>(reading.sequence),
            snapshot.runningAverage,
            snapshot.minimum,
            snapshot.maximum,
            static_cast<unsigned long>(snapshot.sampleCount),
            static_cast<unsigned long>(snapshot.alarmCount),
            static_cast<unsigned long>(latencyMs));

        if (thresholdExceeded) {
          Serial.printf(
              "[ALERT    | Core %d] Threshold exceeded! seq=%lu value=%.2f > %.2f (total alarms=%lu)\n",
              xPortGetCoreID(),
              static_cast<unsigned long>(reading.sequence),
              reading.value,
              HIGH_TEMP_THRESHOLD,
              static_cast<unsigned long>(snapshot.alarmCount));
        }
      } else {
        Serial.printf(
            "[Consumer | Core %d] mutex timeout for seq=%lu\n",
            xPortGetCoreID(),
            static_cast<unsigned long>(reading.sequence));
      }
    }
  }
}

// ============================================================
// SETUP
// ============================================================
void setup() {
  Serial.begin(115200);
  delay(1000);

  Serial.println();
  Serial.println("Lecture 12 - ESP32-S3 Dual-Core Queue + Mutex Demo");
  Serial.printf("setup() is running on core %d\n", xPortGetCoreID());

  readingQueue = xQueueCreate(QUEUE_LENGTH, sizeof(SensorReading));
  if (readingQueue == nullptr) {
    fatalError("Could not create reading queue");
  }

  statisticsMutex = xSemaphoreCreateMutex();
  if (statisticsMutex == nullptr) {
    vQueueDelete(readingQueue);
    readingQueue = nullptr;
    fatalError("Could not create statistics mutex");
  }

  BaseType_t producerCreated = xTaskCreatePinnedToCore(
      sensorProducerTask,
      "SensorProducer",
      PRODUCER_STACK,
      nullptr,
      2,
      nullptr,
      0);  // Core 0

  if (producerCreated != pdPASS) {
    fatalError("Could not create producer task on Core 0");
  }

  BaseType_t consumerCreated = xTaskCreatePinnedToCore(
      dataConsumerTask,
      "DataConsumer",
      CONSUMER_STACK,
      nullptr,
      2,
      nullptr,
      1);  // Core 1

  if (consumerCreated != pdPASS) {
    fatalError("Could not create consumer task on Core 1");
  }

  Serial.println("Tasks created. Watch the core IDs below.");
}

// ============================================================
// MONITOR (Arduino loop())
// ============================================================
void loop() {
  static uint32_t lastMonitorTime = 0;

  if (millis() - lastMonitorTime >= MONITOR_PERIOD_MS) {
    lastMonitorTime = millis();

    SharedStatistics snapshot;
    bool copied = false;

    // Lock mutex only long enough to copy the shared struct
    if (xSemaphoreTake(statisticsMutex, pdMS_TO_TICKS(MUTEX_WAIT_MS)) == pdTRUE) {
      snapshot = statistics;
      copied = true;
      xSemaphoreGive(statisticsMutex);
    }

    // Print after releasing the mutex
    if (copied) {
      Serial.printf(
          "[Monitor  | Core %d] count=%lu avg=%.2f min=%.2f max=%.2f alarms=%lu maxLat=%lums\n",
          xPortGetCoreID(),
          static_cast<unsigned long>(snapshot.sampleCount),
          snapshot.runningAverage,
          snapshot.minimum,
          snapshot.maximum,
          static_cast<unsigned long>(snapshot.alarmCount),
          static_cast<unsigned long>(snapshot.maxLatencyMs));
    } else {
      Serial.printf(
          "[Monitor  | Core %d] mutex timeout\n",
          xPortGetCoreID());
    }
  }

  delay(10);
}