#ifndef SSU_BACKUP_UTIL_H
#define SSU_BACKUP_UTIL_H

#include <dirent.h>
#include <sys/stat.h>

#include "ssu_backup_define.h"

//Comment: 인자로 백업 경로를 저장할 버퍼를 받습니다.
//	성공시 0 실패시 -1을 리턴하고 mkdir의 에러값을 errno에 남깁니다.
int SetBackupPath(char* buf);

//Comment: 인자로 해쉬 매크로의 값을 리턴합니다.
//	실패시 -1을 리턴합니다.
int GetHashMode();

//Comment: 인자로 해쉬 매크로의 값을 받습니다.
// 성공시 0 실패시 -1을 리턴합니다.
// 백업경로의 설정 파일을 읽어들입니다.
int SetHashMode(int mode);

//Comment: realpath를 사용하는 함수들에서 내부적으로 오류를 핸들할때 사용하는 함수
char* RealpathAndHandle(const char* path, char* resolved_path, SSU_BACKUP_IDX thisUsage);

//Comment: 인자로 들어온 파일의 타입을 리턴합니다.
//  -1인 경우 에러, 0인경우 일반 파일 1인 경우 디렉토리 2인경우 그 이외를 의미합니다.
//  -1인 경우 errno를 설정합니다.
int CheckFileType(const struct stat* p_stat);
int CheckFileTypeByPath(const char* path);

//Comment: 경로 조건을 검사합니다.
//  -1인 경우 잘못된 경로, 0인경우 올바른 조건입니다.
int CheckPathCondition(const char* path);

//Comment: 파일 옵션을 검사합니다.
//  -1인 경우 잘못된 옵션, 0인경우 올바른 조건입니다.
int CheckFileTypeCondition(const char* originPath, int selectType, int checkType);

//Comment: 해당 경로가 없다면 path로의 디렉토리를 재귀적으로 만들어줍니다.
//	성공시 0 실패시 -1을 리턴합니다.
int MakeDirPath(const char* path);

//Comment: dirent를 생성할 때 제외할 필터입니다.
int filterParentInScanDir(const struct dirent* target);

//Comment: 해당 루트 하위의 빈 디렉토리를 찾아 지웁니다.
//  성공시 0, 실패시 -1을 리턴합니다.
int ClearEmptyDirectory(const char* path);

//Comment: destPath의 이름으로 sourcePath의 파일을 복사합니다.
//	성공시 0 실패시 -1을 리턴합니다.
int CopyFile(const char* destPath, const char* sourcePath);

//Comment: 현재시간을 buf에 _YYMMDDHHMMSS 형식으로 넣어줍니다.
//	성공시 0 실패시 -1을 리턴합니다.
int GetNowTime(char* buf);

//Comment: int를 1000단위 컴마가 찍힌 스트링으로 변환해줍니다.
char* IntToCommaString(int integer, char* buf);

#endif
