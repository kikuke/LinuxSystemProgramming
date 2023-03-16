#ifndef SSU_BACKUP_ADD_H
#define SSU_BACKUP_ADD_H

//Comment: addPath에는 addTree의 부모 디렉토리를 넣어줍니다.
int AddBackupByFileTree(const char* addPath, const struct filetree* backupTree, const struct filetree* addTree, int hashMode);

//Comment: AddBackupByFileTree의 backupTree에서 없거나 업데이트 해야할 파일을 생성할때 호출하는 함수.
//	실패시 -1 성공시 0리턴
//	함수 유틸 기능으로 옮기기.
int CreateBackupFileByFileTree(const char* addPath, const struct filetree* addTree);

#endif
