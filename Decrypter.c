//
// Created by asrosent on 12/29/2020.
//

#include <unistd.h>
#include <malloc.h>
#include <memory.h>
#include <mta_rand.h>
#include <mta_crypt.h>
#include <limits.h>
#include <stdlib.h>
#include <mqueue.h>
#include "Libs/Common.h"
#include <fcntl.h>
#include <errno.h>
#include <sched.h>

char EncryptedData[1024];
int EncryptedDataLength;
int StrLength;
int Counter;
int Id;

void ParseArgs(int argc, char* argv[], int* id, int* rounds)
{
    int idProvided = FALSE;

    *rounds = INT_MAX;

    char c;
    while ((c = getopt(argc, argv, "i:n:")) != -1)
    {
        switch (c)
        {
            case 'i':
                if (optarg == NULL)
                {
                    printf("You must provide value for decrypter index\n");
                    exit(1);
                }
                *id = atoi(optarg);
                idProvided = TRUE;
                break;
            case 'n':
                if (optarg == NULL)
                {
                    printf("You must provide value for string length\n");
                    exit(1);
                }
                *rounds = atoi(optarg);
                break;
            default:
                printf("Unsupported Flag %c with value %s\n", c, optarg != NULL ? optarg : "(NULL)");
        }
    }

    if (!idProvided)
    {
        printf("You must provide decrypter Id\n");
        exit(1);
    }
}

void ReceiveFromEncryptor(mqd_t clientMessageQueue)
{
    NewPasswordMessage message;
    mq_receive(clientMessageQueue, (char*)&message, sizeof(NewPasswordMessage), 0);

    EncryptedDataLength = message.EncryptedDataLength;
    memcpy(EncryptedData, message.EncryptedData, EncryptedDataLength);
    StrLength = message.StringLength;
    Counter = 0;
    printf("Encrypted data was reset [Id=%d]\n", Id);
}

void TryReceiveFromEncryptor(mqd_t clientMessageQueue)
{
    struct mq_attr attr = {0};
    mq_getattr(clientMessageQueue, &attr);
    if (attr.mq_curmsgs > 0)
    {
        ReceiveFromEncryptor(clientMessageQueue);
    }
}

void main(int argc, char* argv[]) {
    struct sched_param param = {0};
    param.sched_priority = 90;
    sched_setscheduler(getpid(), SCHED_RR, &param);
    int rounds;

    ParseArgs(argc, argv, &Id, &rounds);

    mqd_t serverMessageQueue = mq_open(ServerQueueName, O_WRONLY);
    char clientMessageQueueName[1024];
    sprintf(clientMessageQueueName, "/clientQueue_%d", Id);
    mq_unlink(clientMessageQueueName);
    struct mq_attr queueAttribute = {0};
    queueAttribute.mq_maxmsg = 10;
    queueAttribute.mq_msgsize = sizeof(NewPasswordMessage);
    mqd_t clientMessageQueue = mq_open(clientMessageQueueName, O_EXCL | O_CREAT | O_RDONLY, S_IRWXU | S_IRWXG, &queueAttribute);

    if (clientMessageQueue == -1)
    {
        printf("Failed to open mq [Id=%d] with errno=%d", Id, errno);
        if ( errno == EEXIST)
        {
            printf("ClientId already exists [Id=%d]", Id);
        }
        exit(1);
    }

    printf("Decrypter started [Id=%d]\n", Id);

    ServerRequest connectionRequest;
    connectionRequest.Type = Connect;
    connectionRequest.DecrypterId = Id;
    strcpy(connectionRequest.Payload,clientMessageQueueName);

    printf("Decrypter %d - sending to server started\n", Id);
    int res = mq_send(serverMessageQueue,(char*)&connectionRequest, sizeof(ServerRequest), 0);
    printf("Decrypter %d - sending to server finished\n", Id);
    if( res == -1 )
    {
        printf("Decrypter %d - Couldn't send message to servers mq. Failed with errno=%d", Id, errno);
    }
    printf("Decrypter %d - waiting for server started\n", Id);
    ReceiveFromEncryptor(clientMessageQueue);
    printf("Decrypter %d - waiting for server finished\n", Id);

    int decryptedStringLength;
    char decryptedString[4096];
    InitArray(decryptedString, 4096);

    char* guessedKey = (char*)malloc(StrLength/8 + 1);

    while (rounds > 0)
    {
        TryReceiveFromEncryptor(clientMessageQueue);
        Counter++;
        MTA_get_rand_data(guessedKey,StrLength/8);
        MTA_decrypt(guessedKey,StrLength/8, EncryptedData, EncryptedDataLength, decryptedString, &decryptedStringLength);

        if (IsPrintable(decryptedString,decryptedStringLength))
        {
            printf("Decrypter %d - found printable string %s after %d retries [RoundsLeft=%d]\n", Id, decryptedString, Counter, rounds);
            ServerRequest guess;
            guess.Type = Guess;
            guess.DecrypterId = Id;
            strcpy(guess.Payload, decryptedString);

            mq_send(serverMessageQueue,(char*)&guess, sizeof(ServerRequest), 0);
            rounds--;
        }
    }

    printf("Decrypter %d - rounds finished, disconnecting.\n", Id);
    ServerRequest disconnectRequest;
    disconnectRequest.Type = Disconnect;
    disconnectRequest.DecrypterId = Id;
    strcpy(disconnectRequest.Payload, clientMessageQueueName);
    mq_send(serverMessageQueue,(char*)&disconnectRequest, sizeof(ServerRequest), 0);

    mq_close(serverMessageQueue);
    mq_close(clientMessageQueue);
    mq_unlink(clientMessageQueueName);
    free(guessedKey);
}


