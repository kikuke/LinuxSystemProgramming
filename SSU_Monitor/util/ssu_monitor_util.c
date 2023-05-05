#include <stdio.h>

#include "ssu_monitor_util.h"

int isBlank(char *str)
{
    if(str == NULL)
        return -1;

    while(*str != '\0') {
        if(*str != ' ')
            break;
        str++;
    }
    if(*str == '\0')
        return 1;

    return 0;
}

int CheckFileType(const struct stat* p_stat)
{
	if(S_ISREG((*p_stat).st_mode)){
		return SSU_MONITOR_TYPE_REG;
	} else if(S_ISDIR((*p_stat).st_mode)){
		return SSU_MONITOR_TYPE_DIR;
	}

	return SSU_MONITOR_TYPE_OTHER;
}

int CheckFileTypeByPath(const char* path)
{
	struct stat f_stat;

	if(lstat(path, &f_stat) == -1)
		return SSU_MONITOR_TYPE_ERROR;
	
	return CheckFileType(&f_stat);
}