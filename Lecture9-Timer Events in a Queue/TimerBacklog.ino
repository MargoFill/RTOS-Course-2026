#include <Arduino.h>

// Run A: 20 ms delay  (consumer faster than 100 ms interrupt period)
// Run B: 500 ms delay (consumer slower than 100 ms interrupt period)
#define CONSUMER_DELAY_MS 500

static const uint32_t TIMER_FREQ_HZ   = 1000000; // 1 MHz counter (1 tick = 1 us)
static const uint64_t ALARM_TICKS     = 100000;  // 100,000 ticks = 100 ms
static const UBaseType_t QUEUE_LENGTH = 4;       // 4 slots for uint32_t values

static QueueHandle_t timerQueue = nullptr;
static hw_timer_t *timer        = nullptr;

void ARDUINO_ISR_ATTR onTimer() {
  // Sequence counter owned exclusively by the ISR; starts at 1 on first firing
  static uint32_t seq = 0;
  seq++;

  uint32_t item = seq;
  BaseType_t xHigherPriorityTaskWoken = pdFALSE;

  // Send a copy of seq; if the 4-slot queue is full, xQueueSendFromISR drops item
  xQueueSendFromISR(timerQueue, &item, &xHigherPriorityTaskWoken);

  if (xHigherPriorityTaskWoken == pdTRUE) {
    portYIELD_FROM_ISR();
  }
}

void consumerTask(void *pvParameters) {
  (void)pvParameters;
  uint32_t receivedSeq = 0;

  for (;;) {
    if (xQueueReceive(timerQueue, &receivedSeq, portMAX_DELAY) == pdTRUE) {
      Serial.println(receivedSeq);
      vTaskDelay(pdMS_TO_TICKS(CONSUMER_DELAY_MS));
    }
  }
}

void setup() {
  Serial.begin(115200);
  delay(1000); // Give ESP32-S3 USB CDC Serial time to reconnect after reset

  // 1. Create the 4-slot queue before creating the task or enabling the alarm
  timerQueue = xQueueCreate(QUEUE_LENGTH, sizeof(uint32_t));
  if (timerQueue == nullptr) {
    Serial.println("Failed to create queue");
    while (true) {
      delay(1000);
    }
  }

  // 2. Create the consumer task before enabling the alarm
  BaseType_t taskCreated = xTaskCreate(
      consumerTask,
      "ConsumerTask",
      2048,
      nullptr,
      2,
      nullptr);

  if (taskCreated != pdPASS) {
    Serial.println("Failed to create consumer task");
    while (true) {
      delay(1000);
    }
  }

  // 3. Configure 1 MHz timer and start auto-reload alarm every 100,000 ticks (100 ms)
  timer = timerBegin(TIMER_FREQ_HZ);
  timerAttachInterrupt(timer, &onTimer);
  timerAlarm(timer, ALARM_TICKS, true, 0);
}

void loop() {
  vTaskDelay(portMAX_DELAY);
}
