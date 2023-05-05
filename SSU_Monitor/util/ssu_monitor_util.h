#ifndef SSU_MONITOR_UTIL_H
#define SSU_MONITOR_UTIL_H

#include <sys/stat.h>

#define SSU_MONITOR_TYPE_ERROR -1
#define SSU_MONITOR_TYPE_OTHER 0
#define SSU_MONITOR_TYPE_REG 1
#define SSU_MONITOR_TYPE_DIR 2

//해당 문자열이 공백인지확인.
//  맞다면 1, 아니라면 0, 에러시 -1 리턴
int isBlank(char *str);

//인자로 들어온 stat의 타입을 리턴함.
int CheckFileType(const struct stat* p_stat);

//해당 경로의 파일의 타입을 리턴함.
//	에러시 -1 리턴
int CheckFileTypeByPath(const char* path);

#endif