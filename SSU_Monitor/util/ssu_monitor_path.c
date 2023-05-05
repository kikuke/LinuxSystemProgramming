#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "ssu_monitor_define.h"
#include "ssu_monitor_path.h"

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

int CompareIncludePath(const char* path1, const char* path2)
{
	char cmp[SSU_MONITOR_MAX_PATH];

	if(path1 == NULL || path2 == NULL)
		return 0;
	//긴 것이 왼쪽에 오게
	if (strlen(path1) < strlen(path2))
		return CompareIncludePath(path2, path1);
	
	strcpy(cmp, path1);
	while(strlen(cmp) > strlen(path2))
		GetParentPath(cmp, cmp);
	
	return !strcmp(cmp, path2);
}

char* GetVirtualRealPath(const char* path, char* resolved_path)
{
	char* homeDir = getenv("HOME");
	char* sPtr;
	char* ePtr;
	char temp_path[SSU_MONITOR_MAX_PATH + 1];
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