#include <string.h>

#include "ssu_monitor_path.h"

char* ConcatPath(char* dest, const char* target)
{
	strcat(dest, "/");
	strcat(dest, target);
	return dest;
}