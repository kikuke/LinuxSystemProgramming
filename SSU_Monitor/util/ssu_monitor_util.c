#include <stdio.h>

#include "ssu_monitor_util.h"

int isBlank(char *str)
{
    if(str == NULL)
        return -1;

    while(*str != '\0') {
        if(*str != ' ')
            break;
        str++;
    }
    if(*str == '\0')
        return 1;

    return 0;
}