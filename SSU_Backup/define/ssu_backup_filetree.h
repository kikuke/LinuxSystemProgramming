#ifndef SSU_BACKUP_FILETREE_H
#define SSU_BACKUP_FILETREE_H

#include <stddef.h>

#include "ssu_backup_define.h"

typedef struct filetree
{
	char file[SSU_BACKUP_MAX_FILENAME];
	
	struct filetree* parentNode;
	//Comment: childNodeNum이 0인경우는 파일 밖에 없음
	int childNodeNum;
	struct filetree** childNodes;
} FILETREE;

struct filetree* CreateFileTree(const char* fileName);

void AddChildFileNode(struct filetree* parentNode, struct filetree* childNode);

//Comment: 하위 노드와 자신을 지우고, 부모 노드에 반영합니다.
void RemoveFileNode(struct filetree* node);

#endif