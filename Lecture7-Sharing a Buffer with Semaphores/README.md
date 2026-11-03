# Sharing a Buffer with Semaphores
## What does emptySlots count?
`emptySlots` count free places in buffer.
## What does filledSlots count?
`fillesSlots` count how many slots are filled in buffer.
## What does the mutex protect?
Mutex protects shared access.
## What did you change in the second test, and what happened?
I changed the CONSUMER_DELAY from 400ms to 2000ms. All 15 values were received successufully as with the initial delay. Even though Сonsumer became much slower than Producer, no data was lost or overwritten. This is because the emptySlots counting semaphore forced the Producer to block and wait for a new empty slot to appear, rather than overwriting existing unread data. 
# AI assistance
AI was utilised for proofreading.
