#define RUNNING_CORE 1

TaskHandle_t taskBHandle = NULL;


void taskA(void *parameter) { 
  const char* msg = "Task A is printing slowly...";
  int msgLen = strlen(msg); 
  for (;;) { 
    for (int i = 0; i < msgLen; i++) {
      Serial.print(msg[i]);
      vTaskDelay(pdMS_TO_TICKS(100));
    }
    xTaskNotifyGive(taskBHandle);
    Serial.println("Task A continues and finishes.");
    vTaskDelay(pdMS_TO_TICKS(2000));
  } 
}
 
void taskB(void *parameter) { 
  for (;;) { 
    ulTaskNotifyTake(pdTRUE, portMAX_DELAY);
    Serial.println("Task B is running!"); 
  } 
} 
 
void setup() { 
  Serial.begin(115200); 
  delay(500); 

  xTaskCreatePinnedToCore(taskB, "Task B", 2048, NULL, 2, &taskBHandle, RUNNING_CORE); 
  xTaskCreatePinnedToCore(taskA, "Task A", 2048, NULL, 1, NULL, RUNNING_CORE); 
} 
 
void loop() { 
}