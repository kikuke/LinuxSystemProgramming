#ifndef SSU_MONITOR_TREE_H
#define SSU_MONITOR_TREE_H

#include <sys/types.h>
#include <time.h>

#include "ssu_monitor_define.h"

enum MtreeNext {
    MTREE_PARENT,
    MTREE_CHILD,
    MTREE_BEF,
    MTREE_AFT
};

typedef struct monitree {
    //파일 이름
    char filename[SSU_MONITOR_MAX_FILENAME+1];
    //Todo: 파일이름과 ino, mdtime을 모두 비교해 같은 파일인지 체크.
    //i-node 번호
    ino_t ino;
    //수정 시각
    time_t mdtime;

    //이동할 수 있는 포인터
    struct monitree *move[4];
} MONITREE;

#endif