#ifndef SSU_BACKUP_FILETREE_UTIL_H
#define SSU_BACKUP_FILETREE_UTIL_H

#include <dirent.h>

#include "ssu_backup_filetree.h"

//Comment: 해당 파일트리의 시간을 제외한 실제이름을 리턴합니다.
//	반드시 백업경로의 트리어야 합니다.
char* GetRealNameByFileTree(char* buf, const struct filetree* ftree);

//Comment: 해당 트리가 백업 파일일 경우 생성 시간을 리턴합니다.
//	폴더일 경우 NULL을 리턴합니다.
char* GetCreateTimeByFileTree(char* buf, const struct filetree* ftree);

//Comment: 해당 버퍼 뒤로 파일트리의 중간을 부모노드로 거슬로 올라가며 파일의 경로를 찾아줍니다.
//	이때 넣는 버퍼는 파일트리 이전의 경로값을 넣어진 상태여야 정상 작동합니다.
//	isBackup은 0이 기본값인데, 0이 아닌 값으로 설정 시 백업폴더의 파일들로 간주하여 파일의 저장 시간 정보를 지운 패스가 반영됩니다. 
char* GetPathByFileTree(char* buf, struct filetree* ftree);

//Comment: 알파벳 순으로 정렬된 파일트리에서 해당 경로의 이름으로 가장 처음으로 경로가 일치한 파일트리를 리턴합니다.
//	발견하지 못했다면 NULL을 리턴합니다.
// isBackup이 0이 아니면 ftree를 백업파일 트리로 간주하고 검색합니다.
struct filetree* FindFileTreeInPath(const char* path, struct filetree* ftree, int isBackup);

//Comment: ftree에서 path와 일치하는 경로를 가진 트리를 모두 찾아 matchedTrees에 저장합니다.
//	matchedTrees에 각 요소들은 ftree의 자식포인터들이지만, 배열자체는 동적할당을 합니다.
//	matchedTrees는 일치 요소가 없을 경우 NULL이 됩니다.
//	이름이 일치한 파일의 개수를 리턴합니다.
int FindAllFileTreeInPath(const char* path, struct filetree* ftree, struct filetree*** matchedTrees, int isBackup);

//Comment: 파일트리 리스트를 출력합니다.
//	정상 종료시 0 비정상 종료시 -1을 리턴합니다.
int PrintFileTreeList(const char* parentFilePath, const struct filetree** fileTreeList, int listNum);

//Comment: 해당 경로의 파일을 파일트리로 바꿔줍니다.
// 실패시 NULL을 리턴합니다.
struct filetree* FileToFileTree(const char* path);

//Comment: 해당 경로와 하위의 모든 파일들을 filetree화 합니다.
//	실패했거나 디렉토리일 경우, 하위 파일이 없을 때 NULL을 리턴.
//	트리들은 알파벳 오름차순으로 정렬됩니다.
struct filetree* PathToFileTree(const char* path, int hashMode);
//Comment: 디렉토리일때 PathToFileTree에서 내부적으로 사용하는 함수.
//	실패했거나 하위 파일이 없으면 NULL을 리턴합니다.
struct filetree* _PathToFileTreeDir(const char* path, int hashMode);

//Comment: 해당경로를 removeTree 이하 모든 경로를 삭제합니다.
//  Cnt에는 삭제된 폴더, 파일정보가 담깁니다.
//  isSilent가 0일 때는 삭제된 정보들이 출력되며 0이 아닐때는 출력되지 않습니다.
//  성공 시 0, 실패시 -1을 리턴합니다.
int RemoveBackupByFileTree(const char* removePath, struct filetree* removeTree, int* foldCnt, int* fileCnt, int isSilent);

#endif
