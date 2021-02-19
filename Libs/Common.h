//
// Created by asrosent on 12/29/2020.
//

#ifndef LINUXTHREADS_COMMON_H
#define LINUXTHREADS_COMMON_H

#define FALSE 0
#define TRUE 1

#define ServerQueueName "/ServerQueue"

typedef enum messageType{
    Connect,
    Disconnect,
    Guess
} MessageType;

typedef struct serverRequest{
    MessageType Type;
    int DecrypterId;
    char Payload[1024];
} ServerRequest;

typedef struct newPasswordMessage{
    int StringLength;
    int EncryptedDataLength;
    char EncryptedData[1024];
} NewPasswordMessage;


int IsPrintable(char* str, int size);

void InitArray(char* arr, int size);

#endif //LINUXTHREADS_COMMON_H
