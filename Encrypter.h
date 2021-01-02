//
// Created by asrosent on 12/29/2020.
//

#include "ConcurrentQueue.h"

#ifndef LINUXTHREADS_ENCRYPTER_H
#define LINUXTHREADS_ENCRYPTER_H

typedef struct encrypterArguments {
    ConcurrentQueue *Queue;
    char *EncryptedData;
    int* EncryptedDataLength;
    int StrLength;
    pthread_rwlock_t *Lock;
    unsigned int TimeoutSeconds;
    pthread_mutex_t *shouldStartLock;
    pthread_cond_t *shouldStartCondition;
} EncrypterArguments;

void* Encrypter_Run(void* encrypterArgumentsVoid);

#endif //LINUXTHREADS_ENCRYPTER_H
