# ESP32-S3 FreeRTOS Study Session Timer

This project implements a 10-second study session timer using an ESP32-S3, an SSD1306 OLED display, and the onboard RGB LED, managed entirely by FreeRTOS tasks and timers.

## Hardware Wiring

* **Board:** ESP32-S3-DevKitC-1
* **Display:** SSD1306 OLED (I2C Address `0x3C`)
* **Pin Connections:**
  * **OLED SDA:** GPIO 13 
  * **OLED SCL:** GPIO 14
  * **OLED VCC:** 5V
  * **OLED GND:** GND
* **Onboard LED:** internal WS2812 RGB LED (accessed via `RGB_BUILTIN` or GPIO 38).

## Required Libraries

* **Adafruit GFX Library** (for graphics core)
* **Adafruit SSD1306** (for OLED hardware control)
* *Note: The onboard RGB LED is controlled using the native `neopixelWrite()` function built into the ESP32 Arduino Core (v3.0.0+), so no external NeoPixel library is required.*

## Board Settings (Arduino IDE)

* **Board:** ESP32S3 Dev Module (or your specific DevKit model)
* **USB CDC On Boot:** Enabled (Required to see Serial Monitor output via the native USB port)
* **Upload Speed:** 921600
* **Flash Mode:** QIO 80MHz

## Test Results

| Test | Expected result | Observed Result | Pass/Fail |
| :--- | :--- | :--- | :--- |
| Restart the board and wait over 10 seconds | Ready screen remains visible; LED stays off | Ready screen displayed indefinitely; LED is off. | **Pass** |
| Send `s` | Session starts, countdown appears and LED turns on | OLED updates to "Session running" & countdown; LED turns Green. | **Pass** |
| Send `s` repeatedly during the session | Original session continues without extending | Extra 's' inputs are ignored; countdown continues accurately. | **Pass** |
| Allow the session to finish | Completion screen appears and LED turns off after approximately 10 seconds | OLED displays "Session complete" right at 0s; LED turns off. | **Pass** |
| Wait after completion | Completion screen remains visible | "Session complete" remains on screen indefinitely. | **Pass** |
| Send `s` again | A new session starts with the full duration | Countdown resets to 10s, LED turns Green, timer works normally. | **Pass** |

## Design Questions & Answers

**1. Why does this system need both a one-shot and an auto-reload timer?**
The auto-reload timer (1000 ms) is required to trigger periodic events to update the countdown on the screen. The one-shot timer (10,000 ms) is used to guarantee the session ends at exactly the correct time, ensuring that small scheduling delays in the 1-second ticks do not accumulate and extend the session.

**2. Which task executes the timer callbacks?**
Timer callbacks are executed by the FreeRTOS Timer Service Task (often called the RTOS daemon task, `prvTimerTask`).

**3. Why do OLED updates belong in the application task?**
Timer callbacks must be kept as short as possible and must never block. Updating the OLED requires I2C communication, which is relatively slow and blocking. If done inside the callback, it would stall the Timer Service Task, delaying all other timers in the system.

**4. When are the timers dormant?**
Both timers are dormant upon system startup before the first `s` is sent. They also become dormant after a session finishes: the one-shot timer transitions to dormant
