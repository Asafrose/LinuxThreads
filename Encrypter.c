//
// Created by asrosent on 12/29/2020.
//

#include <curses.h>
#include <ctype.h>
#include <mta_rand.h>
#include <mta_crypt.h>
#include <pthread.h>
#include <memory.h>
#include <malloc.h>
#include "Encrypter.h"
#include "Common.h"

void RecycleData(char *encryptedData, int* encryptedDataLength, int strLength, char *clearString, char *key, time_t* lastRecycle);

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

_Noreturn void* Encrypter_Run(void* encrypterArgumentsVoid)
{
    printf("Encrypter started\n");
    EncrypterArguments* encrypterArguments = (EncrypterArguments*)encrypterArgumentsVoid;
    char* clearString = (char*)malloc(encrypterArguments->StrLength + 1);
    char* key = (char*)malloc(encrypterArguments->StrLength/8 + 1);
    time_t lastRecycle;

    pthread_mutex_lock(encrypterArguments->shouldStartLock);
    RecycleData(encrypterArguments->EncryptedData, encrypterArguments->EncryptedDataLength, encrypterArguments->StrLength, clearString, key, &lastRecycle);
    pthread_mutex_unlock(encrypterArguments->shouldStartLock);

    pthread_cond_broadcast(encrypterArguments->shouldStartCondition);
    while (TRUE)
    {
        if (!ConcurrentQueue_IsEmpty(encrypterArguments->Queue))
        {
            pthread_rwlock_wrlock(encrypterArguments->Lock);
            QueueData data = ConcurrentQueue_Dequeue(encrypterArguments->Queue);
            if (strcmp(clearString, data.Payload) == 0)
            {
                printf("String successfully decrypted by decrypter %d which won the race\n", data.DecrypterId);
                RecycleData(encrypterArguments->EncryptedData, encrypterArguments->EncryptedDataLength, encrypterArguments->StrLength, clearString, key, &lastRecycle);
            }
            else
            {
                printf("Received wrong string from decrypter %d [clearString=%s, payload=%s]\n", data.DecrypterId, clearString, data.Payload);
            }

            free(data.Payload);
            pthread_rwlock_unlock(encrypterArguments->Lock);
        }

        if (encrypterArguments->TimeoutSeconds > 0 && difftime(time(NULL), lastRecycle) > encrypterArguments->TimeoutSeconds)
        {
            pthread_rwlock_wrlock(encrypterArguments->Lock);
            RecycleData(encrypterArguments->EncryptedData, encrypterArguments->EncryptedDataLength, encrypterArguments->StrLength, clearString, key, &lastRecycle);
            pthread_rwlock_unlock(encrypterArguments->Lock);
        }
    }

    free(clearString);
    free(key);
}

void RecycleData(char *encryptedData, int* encryptedDataLength, int strLength, char *clearString, char *key, time_t* lastRecycle) {
    CreatePrintableString(clearString, strLength);
    MTA_get_rand_data(key,strLength/8);
    MTA_encrypt(key,strLength/8, clearString, strLength, encryptedData, encryptedDataLength);
    time(lastRecycle);
    printf("Recycled data [clearString=%s]\n", clearString);
}


