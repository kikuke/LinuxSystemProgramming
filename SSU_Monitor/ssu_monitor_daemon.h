#ifndef SSU_MONITOR_DAEMON_H
#define SSU_MONITOR_DAEMON_H

#include "ssu_monitor_monitree.h"

//데몬이 실행하는 루틴.
//  해당 경로를 모니터링 한다.
//  성공시 0, 실패시 -1 리턴
int monitor_routine(const char *m_path);

//두 트리를 비교한 뒤 그 결과를 path에 작성한다.
//  성공시 0, 실패시 -1 리턴
int CompareMoniTreeAndWriteInPath(struct monitree *oldTree, struct monitree *newTree, const char *path);

#endif