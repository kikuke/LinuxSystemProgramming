#include <stdlib.h>
#include <unistd.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>

#include "ssu_backup_define.h"
#include "ssu_backup_util.h"

char* ConcatPath(char* dest, const char* target)
{
	strcat(dest, "/");
	strcat(dest, target);
	return dest;
}

char* GetBackupPath(char* buf)
{
	char* homeDir = getenv("HOME");
	strcpy(buf, homeDir);
	ConcatPath(buf, SSU_BACKUP_ROOT_DIR_NAME);

	return buf;
}

int SetBackupPath(char* buf)
{
	GetBackupPath(buf);
	return mkdir(buf, SSU_BACKUP_MKDIR_AUTH);
}

int SetHashMode(int mode)
{
	int fd;
	char path_buf[SSU_BACKUP_MAX_PATH_SZ];

	GetBackupPath(path_buf);
	ConcatPath(path_buf, SSU_BACKUP_HASH_SET_FILE);
	if((fd = open(path_buf, O_WRONLY | O_CREAT | O_TRUNC, 0644)) == -1){
		return -1;
	}
	
	write(fd, (void*)&mode, sizeof(mode));
	close(fd);
	return 0;
}

int GetHashMode()
{
	int fd;
	int mode;
	char path_buf[SSU_BACKUP_MAX_PATH_SZ];

	GetBackupPath(path_buf);
	ConcatPath(path_buf, SSU_BACKUP_HASH_SET_FILE);
	if((fd = open(path_buf, O_RDONLY)) == -1){
		return -1;
	}
	read(fd, &mode, sizeof(mode));

	return mode;
}
