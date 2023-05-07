#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>

#include "ssu_monitor_usage.h"

int ssu_monitor_delete(int argc, char *argv[])
{
    pid_t killPid;

    if(argc != 2) {
        Usage(USAGEIDX_DELETE);
        exit(1);
    }
    if(((killPid = atoi(argv[1])) <= 0) && strcmp(argv[1], "0")) {
        fprintf(stderr, "Pid must be more than 0\n");
        exit(1);
    }

    exit(0);
}