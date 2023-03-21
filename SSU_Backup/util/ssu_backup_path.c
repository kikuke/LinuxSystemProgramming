#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "ssu_backup_define.h"
#include "ssu_backup_path.h"

char* ConcatPath(char* dest, const char* target)
{
	strcat(dest, "/");
	strcat(dest, target);
	return dest;
}

char* GetParentPath(const char* path, char* buf)
{
	size_t endLen;

	strcpy(buf, path);
	endLen = strlen(path);
	if(path[endLen] == '/'){
		endLen--;
	}

	while((endLen > 0) && path[endLen] != '/')
		endLen--;
	buf[endLen] = '\0';

	return buf;
}

char* GetFileNameByPath(char* path)
{
	char* fileName = NULL;
	char* fptr = strtok(path, "/");
	
	while(fptr != NULL)
	{
		fileName = fptr;
		fptr = strtok(NULL, "/");
	}

	return fileName;
}

char* GetBackupPath(char* buf)
{
	char* homeDir = getenv("HOME");
	strcpy(buf, homeDir);
	ConcatPath(buf, SSU_BACKUP_ROOT_DIR_NAME);

	return buf;
}

char* ExtractHomePath(char* path)
{
	char* homeDir = getenv("HOME");
	char* homePtr = strstr(path, homeDir);
	size_t homeLen;
	char pathBuf[SSU_BACKUP_MAX_PATH_SZ];

	if(homePtr == NULL || homePtr != path){
		return path;
	}
	homeLen = strlen(homeDir);
	if(*(path+homeLen) == '\0'){
		path[0] = '\0';
		return path;
	}

	strcpy(pathBuf, path + homeLen + 1);
	strcpy(path, pathBuf);

	return path;
}

char* SourcePathToBackupPath(char* path)
{
	char* homeDir = getenv("HOME");
	char* matchPtr;
	char rootPathName[SSU_BACKUP_MAX_FILENAME+1];
	char tempPath[SSU_BACKUP_MAX_PATH_SZ];

	if((matchPtr = strstr(path, homeDir)) == NULL)
		return path;

	strcpy(rootPathName, "/");
	strcat(rootPathName, SSU_BACKUP_ROOT_DIR_NAME);

	strcpy(tempPath, matchPtr + strlen(homeDir));
	strcpy(path + strlen(homeDir), rootPathName);
	strcat(path, tempPath);
	return path;
}

char* BackupPathToSourcePath(char* path)
{
	char* matchPtr;
	char rootPathName[SSU_BACKUP_MAX_FILENAME + 2];
	char tempPath[SSU_BACKUP_MAX_PATH_SZ];

	strcpy(rootPathName, "/");
	strcat(rootPathName, SSU_BACKUP_ROOT_DIR_NAME);
	strcat(rootPathName, "/");
	if((matchPtr = strstr(path, rootPathName)) == NULL)
		return path;

	strcpy(tempPath, matchPtr + strlen(rootPathName) - 1);
	strcpy(matchPtr, tempPath);
	return path;
}

char* GetRealBackupPath(const char* path, char* resolved_path)
{
	//Todo: ..이랑 .할때 문제있음. 해결하기
	char* homeDir = getenv("HOME");
	char* tokPtr;
	char temp_path[SSU_BACKUP_MAX_PATH_SZ];
	char post_path[SSU_BACKUP_MAX_PATH_SZ];
	size_t pathLen;
	
	//Comment: ~ 홈경로로 바꿔주기
	if(path[0] == '~'){
		strcpy(resolved_path, homeDir);
		strcat(resolved_path, path+1);
	} else {
		strcpy(resolved_path, path);
	}
	pathLen = strlen(resolved_path);
	//Comment: 경로 끝이 /일 경우 제거
	if(pathLen > 1 && resolved_path[pathLen-1] == '/'){
		resolved_path[pathLen-1] = '\0';
	}
	//Comment: 절대경로가 아닐 경우
	if(resolved_path[0] != '/'){
		if((tokPtr = strstr(resolved_path, "/")) != NULL){
			//Comment: 바꿀수 있는 상위 경로가 있을 경우
			strcpy(post_path, tokPtr+1);
			*tokPtr = '\0';
			if(realpath(resolved_path, temp_path) == NULL){
				perror("GetCurRealPathFiled");
				return NULL;
			}
			strcpy(resolved_path, temp_path);
		} else {
			//Comment: 폴더조차 사라져있을 가능성이 있으므로.
			strcpy(post_path, resolved_path);
			if(realpath(".", resolved_path) == NULL){
				perror("GetCurRealPathFiled");
				return NULL;
			}
		}
		ConcatPath(resolved_path, post_path);
	}
	//Comment: 백업패스를 포함하는지 검사
	GetBackupPath(temp_path);
    if(strncmp(temp_path, resolved_path, strlen(temp_path)) == 0)
		return NULL;

	SourcePathToBackupPath(resolved_path);
	return resolved_path;
}
