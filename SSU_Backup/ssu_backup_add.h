#ifndef SSU_BACKUP_ADD_H
#define SSU_BACKUP_ADD_H

#include "ssu_backup_filetree.h"

//Comment: Backup조건에 맞는 경로인지 평가합니다.
//	조건을 만족하면 0, 만족하지 않으면 -1을 리턴합니다.
int CheckBackupCondition(const char* path, int addType);

//Comment: addPath에생성할 파일 경로, addTree에 생성할 파일이 있는 트리를 넣어줍니다.
//  add의 범위는 홈디렉토리부터 허용됩니다.
//	성공시 0, 실패시 -1 리턴
int AddBackupByFileTree(const char* backupPath, const char* addPath, struct filetree* backupTree, struct filetree* addTree, int hashMode);

#endif
