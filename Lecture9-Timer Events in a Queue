# ESP32-S3 Hardware Timer ISR to FreeRTOS Queue

## Board & Hardware Setup
- **Microcontroller:** ESP32-S3 DevKit
- **Connections / Wiring:** No external wiring or components required. The board is connected to the host PC via USB for flashing and serial monitoring.
- **Serial Configuration:** `115200` baud
- **Timer & Queue Parameters:**
  - Hardware timer counter frequency: `1 MHz` (`1 tick = 1 µs`)
  - Auto-reload alarm period: `100,000 ticks` (`100 ms`, or `10` interrupts per second)
  - FreeRTOS queue capacity: `4` slots of `uint32_t`
  - Consumer task delay: `20 ms` (Run A) and `500 ms` (Run B)

---

## Pre-Run B Prediction
Before executing Run B (`500 ms` consumer delay), I predict that the 4-slot queue will fill completely and drop events.

- **Producer rate:** `1` event every `100 ms` (`10 events/s`).
- **Consumer rate:** `1` event every `500 ms` (`2 events/s`).
- Because events arrive 5 times faster than the task removes them, the 4 buffer slots will fill up during the consumer task's very first `500 ms` delay period. Every subsequent interrupt that fires while all 4 slots remain occupied will fail `xQueueSendFromISR()` and be discarded, creating gaps in the printed sequence.

---

## Serial Output Excerpts (`115200` baud)

### Run A: `20 ms` Task Delay (12+ consecutive values)
The consumer processes each item well before the next `100 ms` timer interrupt arrives. No queue buildup or packet loss occurs:

```text
1
2
3
4
5
6
7
8
9
10
11
12
```

### Run B: `500 ms` Task Delay (12+ consecutive values)
The queue saturates during the first sleep cycle. The **first observed gap occurs immediately after `5`** (jumping from `5` to `7`, indicating that sequence number `6` was dropped):

```text
1
2
3
4
5   <-- Last pre-overflow item; first observed gap occurs after this line (6 was dropped)
7   <-- First item received after queue overflow (steady-state +5 step begins)
12
17
22
27
32
37
42
47
52
57
```

---

## Comparison & Explanation

### 1. Run A vs. Run B Comparison

| Parameter | Run A (`20 ms` delay) | Run B (`500 ms` delay) |
| :--- | :--- | :--- |
| **Timer Interrupt Period** | `100 ms` | `100 ms` |
| **Consumer Loop Period** | `~20 ms` | `~500 ms` |
| **Max Queue Occupancy** | `1 / 4` slots | `4 / 4` slots (saturated) |
| **First Observed Gap** | None (`1` through `12` intact) | After `5` (`5 -> 7`, item `6` lost) |
| **Steady-State Increment** | `+1` per line (`0%` loss) | `+5` per line (`80%` loss) |

### 2. Why Older Numbers Already Queued Appear Before a Gap
1. **Initial Buffering (`t = 100 ms` to `500 ms`):** When interrupt `1` fires at `100 ms`, the waiting consumer task immediately wakes up, dequeues `1`, prints it, and enters `vTaskDelay(500 ms)` until `t ≈ 600 ms`. While the task is sleeping, interrupts `2` (`200 ms`), `3` (`300 ms`), `4` (`400 ms`), and `5` (`500 ms`) arrive and fill all `4` available slots in the queue without any loss.
2. **FIFO Ordering & Drop-Newest Overflow Policy:** A FreeRTOS queue operates as a strict First-In, First-Out (FIFO) buffer, and `xQueueSendFromISR()` drops newly arriving items when the queue is full rather than overwriting unread slots. At `t = 600 ms`, when interrupt `6` fires just before the task wakes up, all `4` slots are still occupied by `[2, 3, 4, 5]`, so `6` is discarded while the ISR still increments its internal sequence counter.
3. **Draining the Backlog:** Each time the consumer task wakes up every `500 ms`, `xQueueReceive()` removes the **oldest** item from the front of the queue (`2`, then `3`, then `4`, then `5`), freeing a single slot for the next arriving interrupt (`7`, `12`, `17`, ...). Because the task must drain all four pre-overflow items (`2, 3, 4, 5`) before reaching the slot written after the drop (`7`), the uninterrupted sequence `1..5` appears in the output before the first gap is observed.
