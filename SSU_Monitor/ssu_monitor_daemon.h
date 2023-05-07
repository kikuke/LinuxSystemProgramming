#ifndef SSU_MONITOR_DAEMON_H
#define SSU_MONITOR_DAEMON_H

#include <stdio.h>

#include "ssu_monitor_monitree.h"

#define SSU_MONITOR_LOG_FILE_FORAMT "[%04d-%02d-%02d %02d:%02d:%02d][%s][%s]\n"

#define SSU_MONITOR_LOG_STATUS_CREATE "create"
#define SSU_MONITOR_LOG_STATUS_MODIFY "modify"
#define SSU_MONITOR_LOG_STATUS_REMOVE "remove"

//데몬이 실행하는 루틴.
//  해당 경로를 모니터링 한다.
//  성공시 0, 실패시 -1 리턴
int monitor_routine(const char *m_path);

//시그널 핸들러 이거 받으면 죽음
void SSUMonitorKillSignalHandler(int signo);

//두 트리를 비교한 뒤 그 결과를 path에 작성한다.
//  성공시 0, 실패시 -1 리턴
int CompareMoniTreeAndWriteInFp(struct monitree *oldTree, struct monitree *newTree, const char *path, FILE *fp);

//monitree를 이용해 삭제됐거나 변경된 파일을 찾고, 해당 파일을 fd에 쓰기한다.
//  성공시 0, 실패시 -1 리턴
int FindRemoveOrModifyFileByMoniTreeAndWriteInFp(struct monitree *oldTree, struct monitree *newTree, const char *path, FILE *fp);

//monitree를 이용해 생성된 파일을 찾고, 해당 파일을 fd에 쓰기한다.
//  성공시 0, 실패시 -1 리턴
int FindCreateFileByMoniTreeAndWriteInFp(struct monitree *oldTree, struct monitree *newTree, const char *path, FILE *fp);

//해당 인자로 로그파일을 쓴다.
//  성공시 0 실패시 -1 리턴
int WriteLogFile(FILE *fp, time_t time, const char *status, const char *path);

#endif