#ifndef SSU_BACKUP_UTIL_H
#define SSU_BACKUP_UTIL_H

#include <sys/stat.h>
#include <dirent.h>

#include "ssu_backup_filetree.h"

//Comment: 부모 경로를 리턴합니다.
char* GetParentPath(const char* path, char* buf);

//Comment: 현재시간을 buf에 _YYMMDDHHMMSS 형식으로 넣어줍니다.
//	성공시 0 실패시 -1을 리턴합니다.
int GetNowTime(char* buf);

//Comment: destPath의 이름으로 sourcePath의 파일을 복사합니다.
//	성공시 0 실패시 -1을 리턴합니다.
int CopyFile(const char* destPath, const char* sourcePath);

//Comment: addPath의 경로 아래에 addTree의 하위 파일들을 모두 생성합니다.
//	isRecover가 0일 경우 백업 모드로 동작하며 디렉토리가 아닌 파일 뒤에 백업시간이 추가됩니다.
//	isRecover가 0이 아닐 경우 Recover모드로 동작합니다.
int CreateFileByFileTree(const char* addPath, const struct filetree* addTree, int isRecover);

//Comment: 알파벳 순으로 정렬된 파일트리에서 해당 경로의 이름으로 가장 처음으로 경로가 일치한 파일트리를 리턴합니다.
//	발견하지 못했다면 NULL을 리턴합니다.
// isBackup이 0이 아니면 ftree를 백업파일 트리로 간주하고 검색합니다.
struct filetree* FindFileTreeInPath(const char* path, struct filetree* ftree, int isBackup);

//Comment: 해당 파일트리의 시간을 제외한 실제이름을 리턴합니다.
//	반드시 백업경로의 트리어야 합니다.
char* GetRealNameByFileTree(char* buf, const struct filetree* ftree);

//Comment: 해당 버퍼 뒤로 파일트리의 중간을 부모노드로 거슬로 올라가며 파일의 경로를 찾아줍니다.
//	이때 넣는 버퍼는 파일트리 이전의 경로값을 넣어진 상태여야 정상 작동합니다.
//	isBackup은 0이 기본값인데, 0이 아닌 값으로 설정 시 백업폴더의 파일들로 간주하여 파일의 저장 시간 정보를 지운 패스가 반영됩니다. 
char* GetPathByFileTree(char* buf, struct filetree* ftree, int isBackup);

//Comment: 해당 경로와 하위의 모든 파일들을 filetree화 합니다.
//	실패했거나 디렉토리일 경우, 하위 파일이 없을 때 NULL을 리턴.
//	트리들은 알파벳 오름차순으로 정렬됩니다.
struct filetree* PathToFileTree(const char* path, int hashMode);

//Comment: 디렉토리일때 PathToFileTree에서 내부적으로 사용하는 함수.
//	실패했거나 하위 파일이 없으면 NULL을 리턴합니다.
struct filetree* _PathToFileTreeDir(const char* path, int hashMode);

int filterParentInScanDir(const struct dirent* target);

//Comment: 해당 경로의 파일을 파일트리로 바꿔줍니다.
// 실패시 NULL을 리턴합니다.
struct filetree* FileToFileTree(const char* path, int hashMode);

//Comment: 경로의 마지막 파일 이름을 반환합니다.
//	실패시 NULL을 리턴합니다.
//	path의 문자열의 변경이 있습니다.
char* GetFileNameByPath(char* path);

//Comment: 해쉬모드를 기준으로 두 해시값을 비교합니다.
//	일치하지 않으면 0 일치하면 1을 리턴합니다.
int CompareHash(const char* hash1, const char* hash2, int hashMode);

//성공시 0, 실패시 -1리턴
//	hashBuf에 해시값을 채워서 줍니다.
int GetMd5HashByPath(const char* path, char* hashBuf);

//성공시 0, 실패시 -1리턴
//	hashBuf에 해시값을 채워서 줍니다.
int GetSha1HashByPath(const char* path, char* hashBuf);

//Comment: dest에 target문자열을 붙입니다. 사이에 /가 추가됩니다.
//	dest의 시작 주소값을 리턴합니다.
char* ConcatPath(char* dest, const char* target);

//Comment: realpath를 내부적으로 호출 후, 공통된 예외처리를 합니다.
//예외 발생시 NULL을 리턴합니다.
char* GetRealpathAndHandle(const char* path, char* resolved_path, SSU_BACKUP_IDX thisUsage);

//Comment: 인자로 들어온 파일의 타입을 리턴합니다.
//-1인 경우 에러, 0인경우 일반 파일 1인 경우 디렉토리 2인경우 그 이외를 의미합니다.
//-1인 경우 errno를 설정합니다.
int CheckFileTypeByPath(const char* path);
int CheckFileType(const struct stat* p_stat);

//Comment: 인자로 백업 경로를 저장할 버퍼를 받습니다.
// 버퍼의 시작 주소값을 리턴합니다.
char* GetBackupPath(char* buf);

//Comment: 인자로 백업 경로를 저장할 버퍼를 받습니다.
//	성공시 0 실패시 -1을 리턴하고 mkdir의 에러값을 errno에 남깁니다.
//	만들어진 디렉토리는 사용자만 접근 가능합니다.
int SetBackupPath(char* buf);

//Comment: 인자로 해쉬 매크로의 값을 받습니다.
// 성공시 0 실패시 -1을 리턴합니다.
// 백업경로의 설정 파일을 읽어들입니다.
int SetHashMode(int mode);

//Comment: 인자로 해쉬 매크로의 값을 리턴합니다.
//	실패시 -1을 리턴합니다.
int GetHashMode();

#endif
