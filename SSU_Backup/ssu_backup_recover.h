#ifndef SSU_BACKUP_RECOVER_H
#define SSU_BACKUP_RECOVER_H

#include "ssu_backup_filetree.h"

//Comment: backupPath에는 백업 파일 경로, recoverPath에는 리커버 되는 파일 경로, backupTree에는 백업트리 원본, recoverTree에는 리커버되는 파일에 대한 트리, hashMode는 해시모드 값을 넣습니다.
//  재귀적으로 인자들에대해 리커버 백업을 진행합니다.
//  성공시 0, 실패시 -1을 리턴합니다.
int RecoverBackupByFileTree(const char* backupPath, const char* recoverPath, struct filetree* backupTree, struct filetree* recoverTree, int hashMode);

//Comment: 동일한 이름의 디렉토리, 파일들을 보여줍니다. 디렉토리를 선택할 경우 디렉토리 복사 루틴으로, 파일을 선택할 경우 파일이 복사됩니다.
int RecoverFileSelector(const char* parentPath, const char* destPath, const struct filetree* backupTree, const struct filetree** matchedTrees, int listNum, int hashMode);

int RecoverFileByFileTree(const char* backupPath, const char* recoverPath, const struct filetree** recoverTrees, int listNum);

#endif