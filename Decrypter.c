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

void WaitForEncryptor(const DecrypterArguments *decrypterArguments);

_Noreturn void* Decrypter_Run(void* decrypterArgumentsVoid) {
    DecrypterArguments* decrypterArguments = (DecrypterArguments*)decrypterArgumentsVoid;
    printf("Dcrypter started [id=%d]\n", decrypterArguments->Id);


    WaitForEncryptor(decrypterArguments);

    int decryptedStringLength;
    char decryptedString[4096];
    char lastKnownEncryptedData[4096];
    InitArray(decryptedString, 4096);
    InitArray(lastKnownEncryptedData, 4096);

    int lastKnownEncryptedDataLength = *decrypterArguments->EncryptedDataLength;
    char* guessedKey = (char*)malloc(decrypterArguments->StrLength/8 + 1);

    memcpy(lastKnownEncryptedData, decrypterArguments->EncryptedData, *decrypterArguments->EncryptedDataLength);
    int counter = 0;
    while (TRUE)
    {
        pthread_rwlock_rdlock(decrypterArguments->Lock);
        if (*decrypterArguments->EncryptedDataLength != lastKnownEncryptedDataLength || memcmp(lastKnownEncryptedData, decrypterArguments->EncryptedData, *decrypterArguments->EncryptedDataLength) != 0)
        {
            counter = 0;
            memcpy(lastKnownEncryptedData, decrypterArguments->EncryptedData, *decrypterArguments->EncryptedDataLength);
            lastKnownEncryptedDataLength = *decrypterArguments->EncryptedDataLength;

            printf("Encrypted data was reset [id=%d]\n", decrypterArguments->Id);
        }
        pthread_rwlock_unlock(decrypterArguments->Lock);

        counter++;
        MTA_get_rand_data(guessedKey,decrypterArguments->StrLength/8);
        MTA_decrypt(guessedKey,decrypterArguments->StrLength/8, lastKnownEncryptedData, lastKnownEncryptedDataLength, decryptedString, &decryptedStringLength);

        if (IsPrintable(decryptedString,decryptedStringLength))
        {
            printf("Decrypter %d found printable string %s after %d retries\n", decrypterArguments->Id, decryptedString, counter);
            QueueData data;
            data.DecrypterId = decrypterArguments->Id;
            data.Payload = (char*)malloc(decryptedStringLength);
            strcpy(data.Payload,decryptedString);
            ConcurrentQueue_Enqueue(decrypterArguments->Queue,data);
        }
    }

    free(guessedKey);
}

void WaitForEncryptor(const DecrypterArguments *decrypterArguments) {

    pthread_mutex_lock(decrypterArguments->shouldStartLock);
    if (decrypterArguments->EncryptedDataLength == 0)
    {
        printf("Waiting for start signal [id=%d]\n", decrypterArguments->Id);
        pthread_cond_wait(decrypterArguments->shouldStartCondition, decrypterArguments->shouldStartLock);
        printf("Start signal recieved [id=%d]\n", decrypterArguments->Id);
    }
    pthread_mutex_unlock(decrypterArguments->shouldStartLock);
}
