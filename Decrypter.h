//
// Created by asrosent on 12/29/2020.
//

#ifndef LINUXTHREADS_DECRYPTER_H
#define LINUXTHREADS_DECRYPTER_H

#include <sys/types.h>
#include "ConcurrentQueue.h"

void Decrypter_Run(ConcurrentQueue *queue, char *encryptedString, int strLength, int id, pthread_rwlock_t *lock);

#endif //LINUXTHREADS_DECRYPTER_H
