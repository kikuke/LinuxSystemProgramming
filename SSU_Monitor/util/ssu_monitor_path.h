#ifndef SSU_MONITOR_PATH_H
#define SSU_MONITOR_PATH_H

//Comment: dest에 target문자열을 붙입니다. 사이에 /가 추가됩니다.
//	dest의 시작 주소값을 리턴합니다.
char* ConcatPath(char* dest, const char* target);

//Comment: 부모 경로를 리턴합니다.
char* GetParentPath(const char* path, char* buf);

//Comment: path를 realpath함수가 아닌 가상으로  변환해줍니다.
//	상대경로, 절대경로 모두 변환 허용합니다.
//	변환된 주소를 리턴합니다.
//	resolved_path는 최대 패스 길이 보다 1 커야합니다.
//	실패하거나 잘못된 입력일 경우 NULL을 리턴합니다.
char* GetVirtualRealPath(const char* path, char* resolved_path);

#endif
