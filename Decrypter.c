//
// Created by asrosent on 12/29/2020.
//

#include <unistd.h>
#include <z3.h>
#include <malloc.h>
#include <memory.h>
#include <curses.h>
#include <pthread.h>
#include <mta_rand.h>
#include <mta_crypt.h>
#include "Decrypter.h"
#include "Common.h"

_Noreturn void Decrypter_Run(ConcurrentQueue *queue, char *encryptedString, int strLength, int id, pthread_rwlock_t *lock) {
    while (encryptedString[0] == '\0')
    {
        sleep(0.1);
        printf("Alon efes %d", id);
    }

    char* lastKnownString = (char*)malloc(strLength + 1);
    char* decryptedString = (char*)malloc(strLength + 1);
    char* guessedKey = (char*)malloc(strLength/8 + 1);

    memcpy(lastKnownString, encryptedString, strLength);
    int counter = 0;
    while (TRUE)
    {
        pthread_rwlock_rdlock(lock);
        if (memcmp(lastKnownString,encryptedString, strLength) != 0)
        {
            counter = 0;
            memcpy(lastKnownString, encryptedString, strLength);
        }
        pthread_rwlock_unlock(lock);

        counter++;
        MTA_get_rand_data(guessedKey,strLength/8);
        MTA_decrypt(guessedKey,strLength/8,lastKnownString, strLength, decryptedString, strLength);

        if (IsPrintable(decryptedString,strLength))
        {
            printf("Decrypter %d found printable string %s after %d retries", id, decryptedString, counter);
            QueueData data;
            data.DecrypterId = id;
            data.Payload = (char*)malloc(strLength+1);
            strcpy(data.Payload,decryptedString);
            ConcurrentQueue_Enqueue(queue,data);
        }
    }

    free(lastKnownString);
    free(decryptedString);
    free(guessedKey);
}
