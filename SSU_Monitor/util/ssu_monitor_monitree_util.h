#ifndef SSU_MONITOR_MONITREE_UTIL_H
#define SSU_MONITOR_MONITREE_UTIL_H

#include <dirent.h>

#include "ssu_monitor_monitree.h"

//형제 노드중에서 해당 inode와 일치하는 monitree를 찾습니다.
//  없거나 실패시 NULL을, 성공시 일치하는 노드를 리턴합니다.
struct monitree *SearchSiblingMoniTreeByInode(struct monitree *target, ino_t ino);

//해당 값으로 초기화된 monitree를 동적할당해 리턴합니다. 이 외에 값들은 NULL 입니다.
struct monitree *InitMoniTree(ino_t ino, int filetype, const char *filename, time_t md_time);

//target을 source의 가장 뒤의 형제로 배치합니다.
//  성공시 0, 실패시 -1 리턴
int AddSiblingMoniTree(struct monitree *source, struct monitree *target);

//부모 자식관계를 설정합니다.
//  성공시 자식트리를 리턴, 실패시 NULL 리턴
struct monitree *SetParentMoniTree(struct monitree *pTree, struct monitree *cTree);

//해당 트리 아래와 형제로 연결된 모든 트리를 삭제합니다.
void EraseMoniTree(struct monitree *target);

//scandir을 사용시 .이나 ..을 걸러줍니다.
int ScanDirFilter(const struct dirent* target);

//첫번째 자식을 리턴
struct monitree *PathToMoniTree(const char *path, struct monitree *pTree);

#endif