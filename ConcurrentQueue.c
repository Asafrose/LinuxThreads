//
// Created by asrosent on 12/29/2020.
//

#include <stddef.h>
#include <stdlib.h>
#include <pthread.h>
#include "ConcurrentQueue.h"

void ConcurrentQueue_Init(ConcurrentQueue *queue) {
    queue->Head = NULL;
    queue->Tail = NULL;

    pthread_mutex_init(&queue->Mutex, NULL);
}

int ConcurrentQueue_IsEmpty(ConcurrentQueue *queue) {
    pthread_mutex_lock(&queue->Mutex);
    int result = queue->Head == NULL;
    pthread_mutex_unlock(&queue->Mutex);
    return result;
}

void ConcurrentQueue_Enqueue(ConcurrentQueue *queue, QueueData data) {
    QueueNode* newNode = (QueueNode*)malloc(sizeof(QueueNode));
    newNode->Data = data;
    newNode->Next = NULL;

    pthread_mutex_lock(&queue->Mutex);
    if (queue->Tail != NULL)
    {
        queue->Tail->Next = newNode;
        queue->Tail = newNode;
    }
    else
    {
        queue->Head = newNode;
        queue->Tail = newNode;
    }
    pthread_mutex_unlock(&queue->Mutex);
}

QueueData ConcurrentQueue_Dequeue(ConcurrentQueue *queue) {
    pthread_mutex_lock(&queue->Mutex);
    QueueNode* dequeued = queue->Head;
    queue->Head = queue->Head->Next;
    if (queue->Tail == dequeued)
    {
        queue->Tail = NULL;
    }
    pthread_mutex_unlock(&queue->Mutex);
    QueueData data = dequeued->Data;
    free(dequeued);
    return data;
}


