#ifndef SSU_MONITOR_MONITLIST_UTIL_H
#define SSU_MONITOR_MONITLIST_UTIL_H

#include "ssu_monitor_monitlist.h"

//monitlist를 만들어 다음의 인자들로 초기화 한 뒤 리턴한다.
struct monitlist *InitMonitList(const char *path, pid_t pid, struct monitlist *bef, struct monitlist *aft);

//source의 가장 뒤에 target을 추가한다.
//  성공 시 0, 실패 시 -1 리턴
int AddMonitList(struct monitlist *source, struct monitlist *target);

//해당 monitlist를 리스트에서 지우고, 연결관계를 재설정 후, 제거한 대상 뒤에 오는 monitlist를 리턴한다.
struct monitlist *RemoveMonitList(struct monitlist *target);

//path를 기준으로 일치하는 monitlist를 찾는다.
//  없다면 NULL 리턴
struct monitlist *SerachMonitListByPath(struct monitlist *source, const char *path);

//pid를 기준으로 일치하는 monitlist를 찾는다.
//  없다면 NULL 리턴
struct monitlist *SerachMonitListByPid(struct monitlist *source, pid_t pid);

//해당 경로에 있는 파일을 읽어들여 monitlist로 변환한 뒤 리턴한다.
//  실패시 NULL 리턴
struct monitlist *MakeMonitListByPath(const char *path);

//해당 monitlist를 파일로 변환해 path에 저장한다.
//  성공시 0, 실패시 -1 리턴
int SaveMonitListByPath(struct monitlist *source, const char *path);

#endif