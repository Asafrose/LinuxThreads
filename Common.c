//
// Created by asrosent on 12/29/2020.
//

#include <ctype.h>
#include <curses.h>
#include "Common.h"
int IsPrintable(char *str, int size) {
    for (int i = 0; i < size; ++i)
    {
        if (!isprint(str[i]) || str[i] == '\0')
        {
            return FALSE;
        }
    }

    return TRUE;
}

void InitArray(char *arr, int size) {
    for (int i = 0; i < size; ++i) {
        arr[i] = 0;
    }
}
