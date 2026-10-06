#include <Arduino.h>

QueueHandle_t temperatureQueue;


// --------------------------------------------------
// Sensor Task
// --------------------------------------------------

void sensorTask(void *parameter)
{
  int temperature = 20;

  while (1)
  {
    // Copy the number. Do not wait for free space.
    if (xQueueSend(temperatureQueue, &temperature, 0) != pdPASS)
    {
      Serial.println("Queue full: value not sent");
    }
    // Increase the number even if sending failed.
    temperature++;

    vTaskDelay(pdMS_TO_TICKS(1000));
  }
}


// --------------------------------------------------
// Display Task
// --------------------------------------------------

void displayTask(void *parameter)
{
  int receivedTemperature;

  while (1)
  {
     // Receive one number. Do not wait for data.
    if (xQueueReceive(temperatureQueue, &receivedTemperature, 0) == pdPASS)
    {
      Serial.printf("Temperature: %d\n", receivedTemperature, "C");
    }

    vTaskDelay(pdMS_TO_TICKS(100));
  }
}


// --------------------------------------------------
// Setup
// --------------------------------------------------

void setup()
{
  Serial.begin(115200);

  temperatureQueue = xQueueCreate(5, sizeof(int));
  if (temperatureQueue == NULL)
  {
    Serial.println("Queue creation failed");
    return;
  }

  // Both tasks run on core 1 with priority 1.
  BaseType_t sensorResult = xTaskCreatePinnedToCore(sensorTask, "Sensor", 4096, NULL, 1, NULL, 1);
  BaseType_t displayResult = xTaskCreatePinnedToCore(displayTask, "Display", 4096, NULL, 1, NULL, 1);

  if (sensorResult != pdPASS || displayResult != pdPASS)
  {
    Serial.println("Task creation failed");
  }
}


void loop()
{
   delay(1000);
}