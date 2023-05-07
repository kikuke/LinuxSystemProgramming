#ifndef SSU_MONITOR_MONITREE_H
#define SSU_MONITOR_MONITREE_H

#include <sys/types.h>
#include <time.h>

#include "ssu_monitor_define.h"

enum MtreeNext {
    MTREE_PARENT,
    MTREE_CHILD,
    MTREE_BEF,
    MTREE_AFT,
    MTREE_MAX
};

typedef struct monitree {
    //i-node 번호
    ino_t ino;
    //파일 타입
    int filetype;
    //파일 이름
    char filename[SSU_MONITOR_MAX_FILENAME+1];
    //수정 시각
    time_t md_time;

    //이동할 수 있는 포인터
    struct monitree *move[4];
} MONITREE;

#endif