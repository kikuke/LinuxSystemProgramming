#ifndef SSU_BACKUP_FILETREE_H
#define SSU_BACKUP_FILETREE_H

#include <stddef.h>

#include "ssu_backup_define.h"

typedef struct filetree
{
	char file[SSU_BACKUP_MAX_FILENAME];
	char hash[SSU_BACKUP_HASH_MAX_LEN];
	
	struct filetree* parentNode;
	//Comment: childNodeNum이 0인경우는 파일 밖에 없음
	int childNodeNum;
	struct filetree** childNodes;
} FILETREE;

struct filetree* CreateFileTree(const char* fileName, const char* hash, size_t hash_sz);

void AddChildFileNode(struct filetree* parentNode, struct filetree* childNode);
//Todo: 파일 경로 parentNode이용해 찾는 함수 만들기
#endif
