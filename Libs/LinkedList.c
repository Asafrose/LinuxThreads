//
// Created by asrosent on 1/30/2021.
//

#include <malloc.h>
#include <string.h>
#include "LinkedList.h"
#include "Common.h"

void List_Init(List *list) {
    list->Head = malloc(sizeof(ListNode));
    list->Tail = malloc(sizeof(ListNode));

    list->Head->Next = list->Tail;
    list->Tail->Next = list->Head;
}

void List_Add(List *list, mqd_t queue, const char *queueName, int decrypterId) {
    ListData* data = malloc(sizeof(ListData));
    ListNode* node = malloc(sizeof(ListNode));

    data->DecrypterId = decrypterId;
    data->Queue = queue;
    strcpy(data->QueueName,queueName);
    node->Data = data;

    list->Tail->Next->Next = node;
    list->Tail->Next = node;
    node->Next = list->Tail;
}

void Node_FreeNode(ListNode* node, int isDummy)
{
    if (!isDummy)
    {
        mq_close(node->Data->Queue);
        free(node->Data);
    }
    free(node);
}

void List_Remove(List *list, int decrypterId)
{
    for (ListNode* current = list->Head; current != list->Tail->Next; current = current->Next)
    {
        if (current->Next->Data->DecrypterId == decrypterId)
        {
            ListNode* nodeToDelete = current->Next;
            current->Next = nodeToDelete->Next;
            Node_FreeNode(nodeToDelete, FALSE);
            return;
        }
    }
}

void List_Free(List *list) {
    ListNode* next = NULL;
    ListNode* current = list->Head;

    while (current != list->Tail)
    {
        next = current->Next;
        Node_FreeNode(current, current == list->Head);
        current = next;
    }

    Node_FreeNode(list->Tail, TRUE);
}
