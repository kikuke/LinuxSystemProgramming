#ifndef SSU_MONITOR_PATH_H
#define SSU_MONITOR_PATH_H

//Comment: dest에 target문자열을 붙입니다. 사이에 /가 추가됩니다.
//	dest의 시작 주소값을 리턴합니다.
char* ConcatPath(char* dest, const char* target);

#endif
