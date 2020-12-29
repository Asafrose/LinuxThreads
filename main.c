#include <stdio.h>
#include <mta_rand.h>
#include <memory.h>
#include <ctype.h>
#include <curses.h>
#include "ConcurrentQueue.h"



int main() {
    char str[9];
    do {
        MTA_get_rand_data(str,8);
    }

    printf("%s\n",str);
}
