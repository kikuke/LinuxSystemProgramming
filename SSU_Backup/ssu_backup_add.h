#ifndef SSU_BACKUP_ADD_H
#define SSU_BACKUP_ADD_H

int AddBackupByFileTree(const char* addPath, const struct filetree* backupTree, const struct filetree* addTree);

//Comment: AddBackupByFileTree의 backupTree에서 없거나 업데이트 해야할 파일을 생성할때 호출하는 함수.
//	실패시 -1 성공시 0리턴
int CreateBackupFileByFileTree(const char* addPath, const struct filetree* addTree);

#endif
