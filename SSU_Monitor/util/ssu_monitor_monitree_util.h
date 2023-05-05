#ifndef SSU_MONITOR_MONITREE_UTIL_H
#define SSU_MONITOR_MONITREE_UTIL_H

#include "ssu_monitor_monitree.h"

struct monitree *InitMonitree(const char *filename, ino_t ino, time_t md_time, struct monitree *parent, struct monitree *child, struct monitree *bef, struct monitree *aft);


#endif