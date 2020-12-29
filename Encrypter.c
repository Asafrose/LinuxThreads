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

void RecycleData(const char *encryptedString, int strLength, const char *clearString, const char *key, time_t* lastRecycle);

void CreatePrintableString(char* str, int length)
{
    do
    {
        MTA_get_rand_data(str,length);
    } while (!IsPrintable(str, length));
}
_Noreturn void Encrypter_Run(ConcurrentQueue *queue, char *encryptedString, int strLength, pthread_rwlock_t *lock, unsigned int timeoutSeconds)
{
    char* clearString = (char*)malloc(strLength + 1);
    char* key = (char*)malloc(strLength/8 + 1);
    time_t lastRecycle;

    RecycleData(encryptedString, strLength, clearString, key);

    while (TRUE)
    {
        if (!ConcurrentQueue_IsEmpty(queue))
        {
            pthread_rwlock_wrlock(lock);
            int isDecrypted = FALSE;
            while (!ConcurrentQueue_IsEmpty(queue))
            {
                QueueData data = ConcurrentQueue_Dequeue(queue);
                if (strcmp(clearString, data.Payload) == 0)
                {
                    if (isDecrypted)
                    {
                        printf("String successfully decrypted by decrypter %d buy its too late", data.DecrypterId);
                    }
                    else
                    {
                        printf("String successfully decrypted by decrypter %d which won the race", data.DecrypterId);
                    }

                    isDecrypted = TRUE;
                }
                else
                {
                    printf("Recevied wrong string from decrypter %d [clearString=%s, payload=%s]", data.DecrypterId, clearString, data.Payload);
                }

                free(data.Payload);
            }
            if (isDecrypted)
            {
                RecycleData(encryptedString,strLength,clearString,key);
            }

            pthread_rwlock_unlock(lock);
        }

        if (timeoutSeconds > 0 && difftime(time(NULL), lastRecycle) > timeoutSeconds)
        {
            RecycleData(encryptedString,strLength,clearString,key,lastRecycle);
        }
    }

    free(clearString);
    free(key);
}

void RecycleData(const char *encryptedString, int strLength, const char *clearString, const char *key, time_t* lastRecycle) {
    CreatePrintableString(clearString, strLength);
    MTA_get_rand_data(key,strLength/8);
    MTA_encrypt(key,strLength/8, clearString, strLength, encryptedString, strLength);
    time(lastRecycle);
    printf("Recycled data [clearString=%s]", clearString);
}


