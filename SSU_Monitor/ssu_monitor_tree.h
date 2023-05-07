#ifndef SSU_MONITOR_TREE_H
#define SSU_MONITOR_TREE_H

#include "ssu_monitor_monitree.h"

int ssu_monitor_tree(int argc, char *argv[]);

void PrintMoniTree(struct monitree *tree, int hierarchy);

//해당 경로를 tree로 출력.
//성공시 0 실패시 -1 리턴
int PrintMoniTreeByPath(const char *argv1, const char* path);

#endif