#ifndef SSU_BACKUP_REMOVE_H
#define SSU_BACKUP_REMOVE_H

//Comment: 지울 경로가 문제가 없는지 체크합니다.
//  정상시 0 비정상시 -1을 리턴합니다.
int CheckRemovePathCondition(const char* path);

//Comment: 파일 타입을 검사합니다.
//  정상시 0 비정상시 -1을 리턴합니다.
int CheckFileTypeCondition(const char* destPath, const char* originPath, int removeType);

//Comment: 파일 선택기를 통해 파일을 제거합니다.
//	성공시 0 실패시 -1을 리턴합니다.
int RemoveFileSelector(const char* parentPath, const char* originPath, const struct filetree** removeTrees, int listNum);

#endif
