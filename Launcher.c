#include <stdio.h>
#include <getopt.h>
#include <stdlib.h>
#include <pthread.h>
#include <limits.h>
#include <unistd.h>
#include <errno.h>
#include "Common.h"

void ParseArgs(int argc, char* argv[], int* numberOfDecryptors, int* strLength, int* timeout, int* rounds)
{
    int numberOfDecryptorsProvided = FALSE;
    int strLengthProvided = FALSE;

    *timeout = INT_MAX;
    *rounds = INT_MAX;

    char c;
    while ((c = getopt(argc, argv, "c:l:t:n:")) != -1)
    {
        switch (c)
        {
            case 'c':
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
            case 'n':
                if (optarg == NULL)
                {
                    printf("You must provide value for rounds\n");
                    exit(1);
                }
                *rounds = atoi(optarg);
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
    int numberOfDecryptors, strLength, timeout, rounds;

    ParseArgs(argc,argv,&numberOfDecryptors,&strLength,&timeout, &rounds);

    pthread_t pid;
    char l[1024],t[1024],n[1024];

    sprintf(l,"%d", strLength);
    sprintf(t,"%d", timeout);
    sprintf(n,"%d", rounds);

    pid = fork();
    if (pid == 0) // Encrypter
    {
        char encrypterProgram[] = "./Encrypter.out";
        char* argv[] = {encrypterProgram, "-l", l, "-t", t, 0};
        int res = execv(encrypterProgram,argv);
        if (res == -1)
        {
            printf("failed launching decrypter errno=%d\n", errno);
        }
    }

    for (int i = 0; i < numberOfDecryptors; ++i)
    {
        char decryptorId[1024];
        sprintf(decryptorId, "%d", i);

        pid = fork();
        if (pid == 0)
        {
            char decrypterProgram[] = "./Decrypter.out";
            char* argv[] = {decrypterProgram, "-i", decryptorId, "-n", n, 0};
            int res = execv(decrypterProgram,argv);
            if (res == -1)
            {
                printf("failed launching decrypter errno=%d\n", errno);
            }
        }
    }

    pause();
}
