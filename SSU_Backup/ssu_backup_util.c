#include <stdlib.h>
#include <sys/stat.h>

#include "ssu_backup_define.h"
#include "ssu_backup_util.h"

char* GetBackupPath(char* buf)
{
	return realpath(SSU_BACKUP_ROOT_DIR, buf);
}

char* SetBackupPath(char* buf)
{
	mkdir(SSU_BACKUP_ROOT_DIR, 0700);
	return GetBackupPath(buf);
}
