#ifndef SSU_MONITOR_MONITREE_UTIL_H
#define SSU_MONITOR_MONITREE_UTIL_H

#include <dirent.h>

#include "ssu_monitor_monitree.h"

//해당 값으로 초기화된 monitree를 동적할당해 리턴합니다. 이 외에 값들은 NULL 입니다.
struct monitree *InitMoniTree(ino_t ino, int filetype, const char *filename, time_t md_time);

//target을 source의 가장 뒤의 형제로 배치합니다.
//  성공시 0, 실패시 -1 리턴
int AddSiblingMoniTree(struct monitree *source, struct monitree *target);

//부모 자식관계를 설정합니다.
//  성공시 자식트리를 리턴, 실패시 NULL 리턴
struct monitree *SetParentMoniTree(struct monitree *pTree, struct monitree *cTree);

//Todo: 전체 제거 함수 만들기

//scandir을 사용시 .이나 ..을 걸러줍니다.
int ScanDirFilter(const struct dirent* target);

//첫번째 자식을 리턴
struct monitree *PathToMoniTree(const char *path, struct monitree *pTree);

#endif