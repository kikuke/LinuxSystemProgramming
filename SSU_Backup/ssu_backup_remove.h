#ifndef SSU_BACKUP_REMOVE_H
#define SSU_BACKUP_REMOVE_H

//Comment: 지울 경로가 문제가 없는지 체크합니다.
//  정상시 0 비정상시 -1을 리턴합니다.
int CheckRemovePathCondition(const char* path);

//Comment: 파일 타입을 검사합니다.
//  정상시 0 비정상시 -1을 리턴합니다.
int CheckFileTypeCondition(const char* originPath, int removeType, int checkType);

//Comment: 파일 선택기를 통해 파일을 제거합니다.
//	성공시 0 실패시 -1을 리턴합니다.
//  Remove Folder if empty
int RemoveFileSelector(const char* parentPath, const char* originPath, const struct filetree** removeTrees, int listNum);

//Comment: removeTrees에 담긴 위치의 파일들을 모두 지웁니다.
//  성공시 0 실패시 -1을 리턴합니다.
//  Remove Folder if empty
int RemoveFileByFileTreeList(const char* parentPath, const struct filetree** removeTrees, int listNum);

//Comment: 해당경로를 removeTree를 이용해 삭제합니다.
//  Cnt에는 삭제된 폴더, 파일정보가 담깁니다.
//  isSilent가 0일 때는 삭제된 정보들이 출력되며 0이 아닐때는 출력되지 않습니다.
//  성공 시 0, 실패시 -1을 리턴합니다.
int RemoveBackupFolderByFileTree(const char* removePath, struct filetree* removeTree, int* foldCnt, int* fileCnt, int isSilent);

//Comment: RemoveFileSelector, RemoveBackupFolderByfileTree의 기능을 합친 함수입니다.
//  성공시 0, 실패시 -1을 리턴합니다.
int RemoveFileByFileTree(const char* destPath, const char* originPath, struct filetree** removeTrees, int listNum, int removeType);

//Comment: 백업폴더를 제외한 모든 파일을 지웁니다.
//  성공시 0, 실패시 -1을 리턴합니다.
int ClearBackupFolder(int hashMode);

#endif