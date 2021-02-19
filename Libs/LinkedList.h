//
// Created by asrosent on 1/30/2021.
//
#ifndef LINUXTHREADS_LINKEDLIST_H
#define LINUXTHREADS_LINKEDLIST_H

#include <mqueue.h>

typedef struct listData{
    mqd_t Queue;
    char QueueName[1024];
    int DecrypterId;
} ListData;

typedef struct listNode {
    ListData* Data;
    struct listNode* Next;
} ListNode;

typedef struct list {
    ListNode* Head;
    ListNode* Tail;
} List;

void List_Init(List* list);
void List_Add(List* list, mqd_t queue, const char* queueName, int decrypterId);
void List_Remove(List* list, int decrypterId);
void List_Free(List* list);

#endif //LINUXTHREADS_LINKEDLIST_H
