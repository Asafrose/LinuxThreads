//
// Created by asrosent on 12/29/2020.
//

#include "ConcurrentQueue.h"

#ifndef LINUXTHREADS_ENCRYPTER_H
#define LINUXTHREADS_ENCRYPTER_H

void Encrypter_Run(ConcurrentQueue *queue, char *encryptedString, int strLength, pthread_rwlock_t *lock, unsigned int timeoutSeconds);

#endif //LINUXTHREADS_ENCRYPTER_H
