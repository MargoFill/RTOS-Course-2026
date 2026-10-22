/**
 * FreeRTOS Mutex Challenge
 * 
 * Pass a parameter to a task using a mutex.
 * 
 * Date: January 20, 2021
 * Author: Shawn Hymel
 * License: 0BSD
 */

// You'll likely need this on vanilla FreeRTOS
//#include <semphr.h>

#define RGB_BUILTIN 38

// Use only core 1 for demo purposes
#if CONFIG_FREERTOS_UNICORE
  static const BaseType_t app_cpu = 0;
#else
  static const BaseType_t app_cpu = 1;
#endif

// Pins (change this if your Arduino board does not have LED_BUILTIN defined)
static const int led_pin = LED_BUILTIN;

SemaphoreHandle_t mutex;

//*****************************************************************************
// Tasks

// Blink LED based on rate passed by parameter
void blinkLED(void *parameters) {

  xSemaphoreTake(mutex, portMAX_DELAY);

  // Copy the parameter into a local variable
  int num = *(int *)parameters;

  xSemaphoreGive(mutex);

  // Print the parameter
  Serial.print("Received: ");
  Serial.println(num);

  // Configure the LED pin
  pinMode(led_pin, OUTPUT);

  // Blink forever and ever
  while (1) {
    neopixelWrite(RGB_BUILTIN, 0, 50, 50); 
    vTaskDelay(num / portTICK_PERIOD_MS);
    neopixelWrite(RGB_BUILTIN, 0, 0, 0);
    vTaskDelay(num / portTICK_PERIOD_MS);
  }
}

//*****************************************************************************
// Main (runs as its own task with priority 1 on core 1)

void setup() {

  long int delay_arg;

  // Configure Serial
  Serial.begin(115200);

  // Wait a moment to start (so we don't miss Serial output)
  vTaskDelay(1000 / portTICK_PERIOD_MS);
  Serial.println();
  Serial.println("---FreeRTOS Mutex Challenge---");
  Serial.println("Enter a number for delay (milliseconds)");

  mutex = xSemaphoreCreateMutex();

  // Wait for input from Serial
  while (Serial.available() <= 0);

  // Read integer value
  delay_arg = Serial.parseInt();
  Serial.print("Sending: ");
  Serial.println(delay_arg);

  xSemaphoreTake(mutex, portMAX_DELAY);

  // Start task 1
  xTaskCreatePinnedToCore(blinkLED,
                          "Blink LED",
                          2048,
                          (void *)&delay_arg,
                          1,
                          NULL,
                          app_cpu);

  xSemaphoreGive(mutex);

  vTaskDelay(10 / portTICK_PERIOD_MS);

  xSemaphoreTake(mutex, portMAX_DELAY);

  // Show that we accomplished our task of passing the stack-based argument
  Serial.println("Done!");

  xSemaphoreGive(mutex);
}

void loop() {
  
  // Do nothing but allow yielding to lower-priority tasks
  vTaskDelay(1000 / portTICK_PERIOD_MS);
}
