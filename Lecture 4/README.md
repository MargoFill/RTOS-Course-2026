# Memory Detective
## Stack Size Experiment
### Stack Size = 4096, Serial Output

<img width="1917" height="1120" alt="Снимок экрана 2026-10-05 180648" src="https://github.com/user-attachments/assets/093e2d48-d17d-4447-a49a-c54cd1ec1ba2" />

### Stack Size = 8192, Serial Output

<img width="1917" height="1027" alt="8192" src="https://github.com/user-attachments/assets/7fef8dd5-a1a4-4439-9543-8190cc87835c" />

| Task Stack Size | Free Heap Before Tasks | Free Heap After Task A | Free Heap After Task B |
| :--- | :--- | :--- | :--- |
| **4096 bytes** | 349356 bytes | 344628 bytes | 339900 bytes |
| **8192 bytes** | 349356 bytes | 340276 bytes | 331452 bytes |

## Questions
a. Did the free heap change when you increased the task stack size?

Yes.

b. What happened to the free heap?

Free heap slightly decreased with 8192 stack size after task A was created and after task B was created. After task A creation: 344628 (4096) -> 340276(8192). After task B creation: 339900(4096) -> 331452(4096).

c. Why does a task need stack memory?

Task needs stack memory because local variables, calls and temporary data for task are stored in stack memory. Stack is a working memory for tasks. 

d. Why does creating a FreeRTOS task use RAM?

FreeRTOS task creation uses RAM because the operating system needs to allocate memory for two main components: the Task Stack (for the task's local variables and execution history) and the Task Control Block (TCB). The TCB is an internal data structure used by FreeRTOS to manage the task, keeping track of its priority, current state, and stack pointer.
