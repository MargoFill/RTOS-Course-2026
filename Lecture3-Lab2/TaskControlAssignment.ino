#define RUNNING_CORE 1
#define RGB_BUILTIN 38 
#define RGB_BRIGHTNESS 50 

volatile uint32_t blinkInterval = 500;

TaskHandle_t TaskAHandle=NULL;
TaskHandle_t taskBHandle = NULL;

void taskA(void *parameter) {
  String inputString;
  for (;;) {
    if (Serial.available() > 0) {
      inputString = Serial.readStringUntil('\n');
      inputString.trim(); 

      // 2. Suspend command
      if (inputString == "suspend") {
        if (taskBHandle != NULL) {
          vTaskSuspend(taskBHandle);
          Serial.println("Task Suspended: LED stopped blinking.");
        }
      }

      // 3. Resume command
      else if (inputString == "resume") {
        if (taskBHandle != NULL) {
          vTaskResume(taskBHandle);
          Serial.println("Task Resumed: LED started blinking again.");
        }
      }

      else if (inputString == "250" || inputString == "500" || inputString == "1000") {
        blinkInterval = inputString.toInt();
        Serial.print("Success! New interval: ");
        Serial.println(blinkInterval);
      } else if (inputString.length() > 0) {
        Serial.println("Error: Invalid input. Use 250, 500, or 1000.");
      }
    }
    vTaskDelay(pdMS_TO_TICKS(50));
  }
}

void taskB(void *parameter) {
  neopixelWrite(RGB_BUILTIN, 0, 0, 0);
  for (;;) {
    neopixelWrite(RGB_BUILTIN, 0, 0, RGB_BRIGHTNESS);
    vTaskDelay(pdMS_TO_TICKS(blinkInterval));
    neopixelWrite(RGB_BUILTIN, 0, 0, 0);
    vTaskDelay(pdMS_TO_TICKS(blinkInterval)); 
  }
}

void setup() {
  Serial.begin(115200);
  delay(1000);
  Serial.println("System ready. Enter 250, 500, or 1000:");
  xTaskCreatePinnedToCore(taskB, "Task B", 2048, NULL, 2, &taskBHandle, RUNNING_CORE);
  xTaskCreatePinnedToCore(taskA, "Task A", 2048, NULL, 1, NULL, RUNNING_CORE);
}

void loop() {
}