#ifndef SSU_BACKUP_FILETREE_UTIL_H
#define SSU_BACKUP_FILETREE_UTIL_H

#include <dirent.h>

#include "ssu_backup_filetree.h"

//Comment: 해당 파일트리의 시간을 제외한 실제이름을 리턴합니다.
//	반드시 백업경로의 트리어야 합니다.
char* GetRealNameByFileTree(char* buf, const struct filetree* ftree);

//Comment: 해당 버퍼 뒤로 파일트리의 중간을 부모노드로 거슬로 올라가며 파일의 경로를 찾아줍니다.
//	이때 넣는 버퍼는 파일트리 이전의 경로값을 넣어진 상태여야 정상 작동합니다.
//	isBackup은 0이 기본값인데, 0이 아닌 값으로 설정 시 백업폴더의 파일들로 간주하여 파일의 저장 시간 정보를 지운 패스가 반영됩니다. 
char* GetPathByFileTree(char* buf, struct filetree* ftree, int isBackup);

//Comment: 알파벳 순으로 정렬된 파일트리에서 해당 경로의 이름으로 가장 처음으로 경로가 일치한 파일트리를 리턴합니다.
//	발견하지 못했다면 NULL을 리턴합니다.
// isBackup이 0이 아니면 ftree를 백업파일 트리로 간주하고 검색합니다.
struct filetree* FindFileTreeInPath(const char* path, struct filetree* ftree, int isBackup);

//Comment: 해당 경로의 파일을 파일트리로 바꿔줍니다.
// 실패시 NULL을 리턴합니다.
struct filetree* FileToFileTree(const char* path, int hashMode);

//Comment: 해당 경로와 하위의 모든 파일들을 filetree화 합니다.
//	실패했거나 디렉토리일 경우, 하위 파일이 없을 때 NULL을 리턴.
//	트리들은 알파벳 오름차순으로 정렬됩니다.
struct filetree* PathToFileTree(const char* path, int hashMode);
//Comment: 디렉토리일때 PathToFileTree에서 내부적으로 사용하는 함수.
//	실패했거나 하위 파일이 없으면 NULL을 리턴합니다.
struct filetree* _PathToFileTreeDir(const char* path, int hashMode);
//Comment: 파일트리를 만들때 정렬 기준
//  파일 이름의 오름차순입니다.
int filterParentInScanDir(const struct dirent* target);

//Comment: destPath에 addPath에 있는 파일들을 해당 패스의 파일에 해당하는 addTree를 이용해 생성합니다.
//	isRecover가 0일 경우 백업 모드로 동작하며 디렉토리가 아닌 파일 뒤에 백업시간이 추가됩니다.
//	isRecover가 0이 아닐 경우 Recover모드로 동작합니다.
int CreateFileByFileTree(const char* destPath, const char* addPath, const struct filetree* addTree, int isRecover);

#endif