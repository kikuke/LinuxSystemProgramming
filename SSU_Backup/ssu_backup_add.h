#ifndef SSU_BACKUP_ADD_H
#define SSU_BACKUP_ADD_H

#include "ssu_backup_filetree.h"

//Comment: addPath에는 addTree의 부모 디렉토리를 넣어줍니다.
//	성공시 0, 실패시 -1 리턴
int AddBackupByFileTree(const char* backupPath, const char* addPath, struct filetree* backupTree, struct filetree* addTree, int hashMode);

#endif
