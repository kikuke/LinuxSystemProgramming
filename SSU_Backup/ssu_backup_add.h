#ifndef SSU_BACKUP_ADD_H
#define SSU_BACKUP_ADD_H

//Comment: addPath에는 addTree의 부모 디렉토리를 넣어줍니다.
//	성공시 0, 실패시 -1 리턴
int AddBackupByFileTree(const char* addPath, const struct filetree* backupTree, const struct filetree* addTree, int hashMode);

#endif
