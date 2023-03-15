#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <errno.h>

#include "ssu_backup_define.h"
#include "ssu_backup_usage.h"
#include "ssu_backup_util.h"

char* ConcatPath(char* dest, const char* target)
{
	strcat(dest, "/");
	strcat(dest, target);
	return dest;
}

char* GetRealpathAndHandle(const char* path, char* resolved_path, SSU_BACKUP_IDX thisUsage)
{
	char* homeDir = getenv("HOME");
	char backup_path[SSU_BACKUP_MAX_PATH_SZ];

	if((realpath(path, resolved_path) == NULL)){
		switch(errno){
			case EACCES:
				fputs("권한이 없습니다.", stderr);
				break;

			case EINVAL:
				fputs("경로의 값이 NULL입니다.", stderr);
				break;
			
			case ENAMETOOLONG:
				fputs("경로의 길이가 큽니다.", stderr);
				break;

			case ENOENT:
				Usage(thisUsage);
				break;

			case ENOTDIR:
				Usage(thisUsage);
				break;

			default:
				fputs("Unhandled RealpathError!", stderr);
				break;
		}
		return NULL;
	}
	//test
			puts(resolved_path);

	if(strncmp(homeDir, resolved_path, strlen(homeDir)) != 0){
		fprintf(stdout, "<%s> can't be backuped\n", resolved_path);
		return NULL;
	}

	if(strncmp(resolved_path, homeDir, strlen(resolved_path)) == 0){
		fprintf(stdout, "<%s> can't be backuped\n", resolved_path);
		return NULL;
	}

	GetBackupPath(backup_path);
	if(strncmp(backup_path, resolved_path, strlen(backup_path)) == 0){
		fprintf(stdout, "<%s> can't be backuped\n", resolved_path);
		return NULL;
	}

	if(CheckFileTypeByPath(resolved_path) == SSU_BACKUP_TYPE_OTHER){
		fputs("일반 파일이나 디렉토리가 아닙니다.", stderr);
		return NULL;
	}
	
	return resolved_path;
}

int CheckFileTypeByPath(const char* path)
{
	struct stat f_stat;
	if(stat(path, &f_stat) == -1){
		return SSU_BACKUP_TYPE_ERROR;
	}

	return CheckFileType(&f_stat);
}

int CheckFileType(const struct stat* p_stat)
{
	if(S_ISREG((*p_stat).st_mode)){
		return SSU_BACKUP_TYPE_REG;
	} else if(S_ISDIR((*p_stat).st_mode)){
		return SSU_BACKUP_TYPE_DIR;
	}

	return SSU_BACKUP_TYPE_OTHER;
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
