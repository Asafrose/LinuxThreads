//
// Created by asrosent on 12/29/2020.
//

#include <ctype.h>
#include <errno.h>
#include <mta_rand.h>
#include <mta_crypt.h>
#include <pthread.h>
#include <memory.h>
#include <malloc.h>
#include <time.h>
#include <mqueue.h>
#include <getopt.h>
#include <limits.h>
#include <stdlib.h>
#include <unistd.h>
#include "Libs/Common.h"
#include "Libs/LinkedList.h"

char* Key;
char* ClearString;
int StrLength;
char EncryptedData[1024];
int EncryptedDataLength;
List Queues;

void RecycleData();
void SendCurrentData(mqd_t queue);

void ParseArgs(int argc, char* argv[], int* strLength, int* timeout)
{
    int strLengthProvided = FALSE;

    *timeout = INT_MAX;

    char c;
    while ((c = getopt(argc, argv, "l:t:")) != -1)
    {
        switch (c)
        {
            case 'l':
                if (optarg == NULL)
                {
                    printf("You must provide value for string length\n");
                    exit(1);
                }
                *strLength = atoi(optarg);
                strLengthProvided = TRUE;
                break;
            case 't':
                if (optarg == NULL)
                {
                    printf("You must provide value for timeout\n");
                    exit(1);
                }
                *timeout = atoi(optarg);
                break;
            default:
                printf("Unsupported Flag %c with value %s\n", c, optarg != NULL ? optarg : "(NULL)");
        }
    }

    if (!strLengthProvided)
    {
        printf("You must provide value for string length\n");
        exit(1);
    }
}

void CreatePrintableString(char* str, int length)
{
    for (int i = 0; i < length; ++i)
    {
        char c;
        do {
           c = MTA_get_rand_char();
        } while (!isprint(c));

        str[i] = c;
    }

    str[length] = '\0';
}

void HandleConnect(ServerRequest* request)
{
    printf("Received Connect request from id %d\n", request->DecrypterId);
    mqd_t queue = mq_open(request->Payload, O_WRONLY);

    List_Add(&Queues, queue, request->Payload, request->DecrypterId);

    SendCurrentData(queue);
}

void HandleGuess(ServerRequest* request)
{
    if (strcmp(ClearString, request->Payload) == 0)
    {
        printf("String successfully decrypted by decrypter %d which won the race\n", request->DecrypterId);
        RecycleData();
    }
    else
    {
        printf("Received wrong string from decrypter %d [clearString=%s, payload=%s]\n", request->DecrypterId, ClearString, request->Payload);
    }
}

void HandleDisconnect(ServerRequest* request)
{
    printf("Received disconnect request from id %d\n", request->DecrypterId);
    List_Remove(&Queues, request->DecrypterId);
}

void HandleMessage(ServerRequest* request)
{
    switch (request->Type)
    {
        case Connect:
            HandleConnect(request);
            break;
        case Guess:
            HandleGuess(request);
            break;
        case Disconnect:
            HandleDisconnect(request);
            break;
        default:
            printf("SQL Injection detected! OMFG!! panic!!!!");
            exit(1);
    }
}


void main(int argc, char* argv[])
{
    struct sched_param param = {0};
    param.sched_priority = 99;
    sched_setscheduler(getpid(), SCHED_FIFO, &param);
    int timeout;
    ParseArgs(argc,argv,&StrLength,&timeout);
    List_Init(&Queues);

    mq_unlink(ServerQueueName);
    struct mq_attr queueAttribute = {0};
    queueAttribute.mq_maxmsg = 10;
    queueAttribute.mq_msgsize = sizeof(ServerRequest);
    mqd_t serverQueue = mq_open(ServerQueueName,O_RDONLY | O_CREAT, S_IRWXU | S_IRWXG, &queueAttribute);

    if (serverQueue == -1)
    {
        printf("failed creating messageQueue errno=%d\n", errno);
        exit(errno);
    }

    printf("Encrypter started\n");
    ClearString = (char*)malloc(StrLength + 1);
    Key = (char*)malloc(StrLength/8 + 1);

    RecycleData();

    while (TRUE)
    {
        ServerRequest request;
        struct timespec tm;
        clock_gettime(CLOCK_REALTIME, &tm);
        tm.tv_sec += timeout;
        ssize_t receiveResult = mq_timedreceive(serverQueue, &request, sizeof(ServerRequest), 0, &tm);

        if (receiveResult == -1)
        {
            printf("failed to send message errno=%d\n", errno);
            if (errno == ETIMEDOUT)
            {
                RecycleData();
            }
        }
        else
        {
            HandleMessage(&request);
        }
    }

    List_Free(&Queues);
    free(ClearString);
    free(Key);
}

void SendCurrentData(mqd_t queue)
{
    NewPasswordMessage msg;

    msg.EncryptedDataLength = EncryptedDataLength;
    msg.StringLength = StrLength;
    memcpy(msg.EncryptedData, EncryptedData, EncryptedDataLength);

    mq_send(queue, &msg, sizeof(NewPasswordMessage), 0);
}

void RecycleData() {
    CreatePrintableString(ClearString, StrLength);
    MTA_get_rand_data(Key,StrLength/8);
    MTA_encrypt(Key,StrLength/8, ClearString, StrLength, EncryptedData, &EncryptedDataLength);

    printf("Recycled data [clearString=%s]\n", ClearString);

    for (ListNode* current = Queues.Head->Next; current != Queues.Tail; current = current->Next)
    {
        SendCurrentData(current->Data->Queue);
    }
}


