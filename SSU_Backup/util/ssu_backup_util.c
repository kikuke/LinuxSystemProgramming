#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <errno.h>
#include <time.h>
#include <sys/time.h>

#include "ssu_backup_usage.h"
#include "ssu_backup_path.h"
#include "ssu_backup_util.h"

int SetBackupPath(char* buf)
{
	GetBackupPath(buf);
	return mkdir(buf, SSU_BACKUP_MKDIR_AUTH);
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

char* RealpathAndHandle(const char* path, char* resolved_path, SSU_BACKUP_IDX thisUsage)
{
	char* homeDir = getenv("HOME");
	char temp_path[SSU_BACKUP_MAX_PATH_SZ];

	if(path[0] == '~' && (path[1] == '\0' || path[1] == '/')){
		strcpy(temp_path, homeDir);
		strcat(temp_path, path+1);
	} else {
		strcpy(temp_path, path);
	}
	if((realpath(temp_path, resolved_path) == NULL)){
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

	return resolved_path;
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

int CheckFileTypeByPath(const char* path)
{
	struct stat f_stat;
	if(stat(path, &f_stat) == -1){
		return SSU_BACKUP_TYPE_ERROR;
	}

	return CheckFileType(&f_stat);
}

int CheckPathCondition(const char* path)
{
	char* homeDir = getenv("HOME");
	char temp_path[SSU_BACKUP_MAX_PATH_SZ];

	if(strlen(path) > SSU_BACKUP_MAX_PATH_SZ){
		fputs("경로의 길이가 큽니다.\n", stderr);
		return -1;
	}

	if(strncmp(homeDir, path, strlen(homeDir)) != 0){
        fprintf(stdout, "<%s> can't be backuped\n", path);
		return -1;
    }

	GetBackupPath(temp_path);
	if(strncmp(temp_path, path, strlen(temp_path)) == 0){
		fprintf(stdout, "<%s> can't be backuped\n", path);
		return -1;
	}

	return 0;
}

int CheckFileTypeCondition(const char* originPath, int selectType, int checkType)
{
	if(checkType == SSU_BACKUP_TYPE_ERROR){
		perror("CheckFileTypeByPath()");
		return -1;
	}
	if(checkType == SSU_BACKUP_TYPE_OTHER){
		fputs("일반 파일이나 디렉토리가 아닙니다.", stderr);
		return -1;
    }
	if((checkType == SSU_BACKUP_TYPE_DIR) && checkType != selectType){
		fprintf(stderr, "\"%s\" is a directory file\n", originPath);
		return -1;
	}

	return 0;
}

int MakeDirPath(const char* path)
{
	int fileType;
	char* dirPtr;
	char pathBuf[SSU_BACKUP_MAX_PATH_SZ];

	strcpy(pathBuf, path);
	pathBuf[SSU_BACKUP_MAX_PATH_SZ-1] = '\0';
	dirPtr = pathBuf; 

	if(*dirPtr == '/')
		dirPtr++;
	while(*dirPtr != '\0'){
		if(*dirPtr == '/'){
			*dirPtr = '\0';
			if((fileType = CheckFileTypeByPath(pathBuf)) != SSU_BACKUP_TYPE_DIR){
				if(mkdir(pathBuf, SSU_BACKUP_MKDIR_AUTH) == -1)
					return -1;
			}
			*dirPtr = '/';
		}
		dirPtr++;
	}
	if((fileType = CheckFileTypeByPath(pathBuf)) != SSU_BACKUP_TYPE_DIR){
		if(mkdir(pathBuf, SSU_BACKUP_MKDIR_AUTH) == -1)
			return -1;
	}

	return 0;
}

int CopyFile(const char* destPath, const char* sourcePath)
{
	int srcFd, destFd;
	size_t readLen;
	unsigned char* fileBuf[SSU_BACKUP_FILE_BUF_SZ];

	if((srcFd = open(sourcePath, O_RDONLY)) == -1)
		return -1;
	if((destFd = open(destPath, O_WRONLY|O_CREAT|O_TRUNC, SSU_BACKUP_MKFILE_AUTH)) == -1)
		return -1;

	while((readLen = read(srcFd, fileBuf, SSU_BACKUP_FILE_BUF_SZ)) > 0){
		if(write(destFd, fileBuf, readLen) == -1)
			return -1;
	}
	if(readLen == -1)
		return -1;

	close(srcFd);
	close(destFd);
	return 0;
}

int GetNowTime(char* buf)
{
	struct timeval now;
	struct tm* tm_now;

	if(gettimeofday(&now, NULL) == -1)
		return -1;

	tm_now = localtime(&now.tv_sec);
	sprintf(buf, "_%02d%02d%02d%02d%02d%02d",
			(tm_now->tm_year+1900)%100, tm_now->tm_mon+1, tm_now->tm_mday,
			tm_now->tm_hour, tm_now->tm_min, tm_now->tm_sec);
	
	return 0;
}

char* IntToCommaString(int integer, char* buf)
{
	size_t intLen;
	size_t idx;
	char tempBuf[SSU_BACKUP_MAX_PRINT_INT_BUF];

	sprintf(tempBuf, "%d", integer);
	intLen = strlen(tempBuf);
	idx = 0;
	for(size_t i=0; i < intLen; i++){
		if(((intLen - i) % 3) == 0 && i != 0){
			buf[idx] = ',';
			idx++;
		}

		buf[idx] = tempBuf[i];
		idx++;
	}
	buf[idx] = '\0';

	return buf;
}
