#ifndef SSU_BACKUP_UTIL_H
#define SSU_BACKUP_UTIL_H

#include <sys/stat.h>

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

//Todo: 파일리스트 가져오는 함수 만들기

#endif
