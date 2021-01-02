//
// Created by asrosent on 12/29/2020.
//

#ifndef LINUXTHREADS_DECRYPTER_H
#define LINUXTHREADS_DECRYPTER_H

#include <sys/types.h>
#include "ConcurrentQueue.h"

typedef struct decrypterArguments {
    ConcurrentQueue *Queue;
    char *EncryptedData;
    int* EncryptedDataLength;
    int StrLength;
    int Id;
    pthread_rwlock_t *Lock;
    pthread_mutex_t *shouldStartLock;
    pthread_cond_t *shouldStartCondition;
} DecrypterArguments;

void* Decrypter_Run(void* decrypterArgumentsVoid);

#endif //LINUXTHREADS_DECRYPTER_H
