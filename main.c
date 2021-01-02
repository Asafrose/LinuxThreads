#include <stdio.h>
#include <getopt.h>
#include <stdlib.h>
#include <curses.h>
#include <pthread.h>
#include "ConcurrentQueue.h"
#include "Encrypter.h"
#include "Decrypter.h"
#include "Common.h"

void ParseArgs(int argc, char* argv[], int* numberOfDecryptors, int* strLength, int* timeout)
{
    int numberOfDecryptorsProvided = FALSE;
    int strLengthProvided = FALSE;

    *timeout = INT32_MAX;

    char c;
    while ((c = getopt(argc, argv, "n:l:t:")) != -1)
    {
        switch (c)
        {
            case 'n':
                if (optarg == NULL)
                {
                    printf("You must provide value for number of decrypters\n");
                    exit(1);
                }
                *numberOfDecryptors = atoi(optarg);
                numberOfDecryptorsProvided = TRUE;
                break;
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

    if (!numberOfDecryptorsProvided)
    {
        printf("You must provide value for number of decrypters\n");
        exit(1);
    }

    if (!strLengthProvided)
    {
        printf("You must provide value for string length\n");
        exit(1);
    }
}

int main(int argc, char* argv[]) {
    int numberOfDecryptors, strLength, timeout;

    ParseArgs(argc,argv,&numberOfDecryptors,&strLength,&timeout);

    printf("started [numberOfDecryptors=%d strLength=%d timeout=%d]\n",numberOfDecryptors,strLength,timeout);
    pthread_rwlock_t lock;
    pthread_rwlock_init(&lock, NULL);
    pthread_mutex_t shouldStartLock;
    pthread_mutex_init(&shouldStartLock, NULL);
    pthread_cond_t shouldStartCondition;
    pthread_cond_init(&shouldStartCondition, NULL);
    ConcurrentQueue queue;
    ConcurrentQueue_Init(&queue);

    char encryptedData[4096];
    InitArray(encryptedData,4096);
    int encryptedDataLength = 0;

    pthread_t encrypterThread;
    EncrypterArguments encrypterArguments;
    encrypterArguments.EncryptedDataLength = &encryptedDataLength;
    encrypterArguments.EncryptedData = encryptedData;
    encrypterArguments.StrLength = strLength;
    encrypterArguments.Lock = &lock;
    encrypterArguments.Queue = &queue;
    encrypterArguments.TimeoutSeconds = timeout;
    encrypterArguments.shouldStartLock = &shouldStartLock;
    encrypterArguments.shouldStartCondition = &shouldStartCondition;

    pthread_create(&encrypterThread, NULL, Encrypter_Run, &encrypterArguments);

    pthread_t* decryptorThreads = (pthread_t*)malloc(sizeof(pthread_t) * numberOfDecryptors);
    DecrypterArguments* decrypterArguments = (DecrypterArguments*)malloc(sizeof(DecrypterArguments) * numberOfDecryptors);

    for (int i = 0; i < numberOfDecryptors; ++i) {
        decrypterArguments[i].Queue = &queue;
        decrypterArguments[i].Lock = &lock;
        decrypterArguments[i].StrLength = strLength;
        decrypterArguments[i].EncryptedDataLength = &encryptedDataLength;
        decrypterArguments[i].EncryptedData = encryptedData;
        decrypterArguments[i].shouldStartLock = &shouldStartLock;
        decrypterArguments[i].shouldStartCondition = &shouldStartCondition;
        decrypterArguments[i].Id = i;

        pthread_create(&decryptorThreads[i], NULL, Decrypter_Run, &decrypterArguments[i]);
    }

    pthread_join(encrypterThread, NULL);

    for (int i = 0; i < numberOfDecryptors; ++i) {
        pthread_join(decryptorThreads[i], NULL);
    }

    printf("hello!");
}
