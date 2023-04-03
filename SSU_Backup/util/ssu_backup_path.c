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
	endLen = strlen(path) - 1;
	if(path[endLen] == '/'){
		if(endLen == 0){
			buf[endLen] = '\0';
			return buf;
		}
		endLen--;
	}

	while((endLen > 0) && path[endLen] != '/'){
		if(endLen == 1 && path[endLen-1] == '/')
			break;
		endLen--;
	}
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

char* GetHomePath(char* buf)
{
	char* homeDir = getenv("HOME");
	strcpy(buf, homeDir);

	return buf;
}

char* GetBackupPath(char* buf)
{
	GetHomePath(buf);
	ConcatPath(buf, SSU_BACKUP_ROOT_DIR_NAME);

	return buf;
}

char* ExtractHomePath(char* path)
{
	char* homeDir = getenv("HOME");
	char* homePtr = strstr(path, homeDir);
	size_t homeLen;
	char pathBuf[SSU_BACKUP_MAX_PATH_BUF_SZ];

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
	char rootPathName[SSU_BACKUP_MAX_FILENAME_BUF_SZ + 1];
	char tempPath[SSU_BACKUP_MAX_PATH_BUF_SZ];

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
	char rootPathName[SSU_BACKUP_MAX_FILENAME_BUF_SZ + 2];
	char tempPath[SSU_BACKUP_MAX_PATH_BUF_SZ];

	strcpy(rootPathName, "/");
	strcat(rootPathName, SSU_BACKUP_ROOT_DIR_NAME);
	strcat(rootPathName, "/");
	if((matchPtr = strstr(path, rootPathName)) == NULL)
		return path;

	strcpy(tempPath, matchPtr + strlen(rootPathName) - 1);
	strcpy(matchPtr, tempPath);
	return path;
}

char* GetVirtualRealPath(const char* path, char* resolved_path)
{
	char* homeDir = getenv("HOME");
	char* sPtr;
	char* ePtr;
	char temp_path[SSU_BACKUP_MAX_PATH_BUF_SZ + 1];
	size_t pathLen;
	
	//Comment: 경로 끝이 /일 경우 제거
	pathLen = strlen(resolved_path);
	if(pathLen > 1 && resolved_path[pathLen-1] == '/'){
		resolved_path[pathLen-1] = '\0';
	}

	//Comment: 주소가 ~인 경우 절대 경로 변환
	if(path[0] == '~' && (path[1] == '\0' || path[1] == '/')){
		strcpy(resolved_path, homeDir);
		strcat(resolved_path, path+1);
	} else {
		strcpy(resolved_path, path);
	}
	//Comment: 상대경로인 경우 절대 경로 변환
	//	../../..와 DV/.., ./.., ./., /home/.. 과 같은 경로도 파싱할 수 있어야함.
	if(resolved_path[0] != '/'){
		strcpy(temp_path, resolved_path);
		if(realpath(".", resolved_path) == NULL){
			perror("GetRealPathFiled");
			return NULL;
		}
		ConcatPath(resolved_path, temp_path);
	}
	//Comment: 각 경로의 변환
	sPtr = resolved_path + 1;
	ePtr = sPtr - 1;
	//Comment: 연산의 안전을 위해 끝에 EOF를 하나 더둠.
	//	이로인해 resolved_path의 크기는 패스 최대 크기의 +1 이어야함.
	pathLen = strlen(resolved_path);
	resolved_path[pathLen+1] = '\0';
	while(*sPtr != '\0'){
		ePtr++;
		if(*ePtr == '/' || *ePtr == '\0'){
			*ePtr = '\0';
			if(!strcmp(sPtr, ".") || !strcmp(sPtr, "..")){
				sPtr--;
				*sPtr = '\0';
				if(strcmp(sPtr + 1, "..") == 0){
					if(*GetParentPath(resolved_path, temp_path) == '\0')
						return NULL;
					strcpy(resolved_path, temp_path);
					//Comment: sPtr은 '\0'을 가리키고 있음.
					sPtr = resolved_path + strlen(resolved_path);
				}
				ePtr++;
				if(*ePtr != '\0'){
					//Comment: sPtr이 '/'다음을 가리키게됨.
					strcpy(temp_path, ePtr);
					ConcatPath(resolved_path, temp_path);
					sPtr++;
				}
				ePtr = sPtr-1;
				pathLen = strlen(resolved_path);
				resolved_path[pathLen + 1] = '\0';
			} else {
				*ePtr = '/';
				sPtr = ePtr + 1;
			}
		}
	}
	pathLen = strlen(resolved_path);
	if(pathLen > 1 && resolved_path[pathLen-1] == '/'){
		resolved_path[pathLen-1] = '\0';
	}
	return resolved_path;
}
