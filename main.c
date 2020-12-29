#include <stdio.h>
#include "ConcurrentQueue.h"

int main() {
    ConcurrentQueue queue;
    ConcurrentQueue_Init(&queue);
    ConcurrentQueue_Enqueue(&queue, (QueueData){"",1});
    ConcurrentQueue_Enqueue(&queue, (QueueData){"",2});
    ConcurrentQueue_Enqueue(&queue, (QueueData){"",3});
    ConcurrentQueue_Enqueue(&queue, (QueueData){"",1});
    ConcurrentQueue_Enqueue(&queue, (QueueData){"",3});

    while (!ConcurrentQueue_IsEmpty(&queue))
    {
        printf("Item is %d", ConcurrentQueue_Dequeue(&queue).DecrypterId);
    }
}
