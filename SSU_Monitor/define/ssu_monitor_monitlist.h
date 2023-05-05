#ifndef SSU_MONITOR_MONITLIST_H
#define SSU_MONITOR_MONITLIST_H

#include <sys/types.h>

#include "ssu_monitor_define.h"

typedef struct monitlist {
    //모니터링 경로
    char path[SSU_MONITOR_MAX_PATH];
    pid_t pid;

    struct monitlist *bef;
    struct monitlist *aft;
} MONITLIST;

#endif