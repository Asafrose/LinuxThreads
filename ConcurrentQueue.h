//
// Created by asrosent on 12/29/2020.
//

#ifndef LINUXTHREADS_CONCURRENTQUEUE_H
#define LINUXTHREADS_CONCURRENTQUEUE_H

#include <sys/types.h>

typedef struct queueData {
    char* Payload;
    int DecrypterId;
} QueueData;

typedef struct queueNode {
    QueueData Data;
    struct queueNode* Next;
} QueueNode;

typedef struct concurrentQueue
{
    QueueNode* Head;
    QueueNode* Tail;
    pthread_mutex_t Mutex;
} ConcurrentQueue;

void ConcurrentQueue_Init(ConcurrentQueue* queue);
int ConcurrentQueue_IsEmpty(ConcurrentQueue* queue);
void ConcurrentQueue_Enqueue(ConcurrentQueue* queue, QueueData data);
QueueData ConcurrentQueue_Dequeue(ConcurrentQueue* queue);

#endif //LINUXTHREADS_CONCURRENTQUEUE_H
