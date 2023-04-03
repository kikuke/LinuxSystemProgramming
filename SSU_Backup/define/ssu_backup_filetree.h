#ifndef SSU_BACKUP_FILETREE_H
#define SSU_BACKUP_FILETREE_H

#include <stddef.h>

#include "ssu_backup_define.h"

typedef struct filetree
{
	char file[SSU_BACKUP_MAX_FILENAME_BUF_SZ];
	
	struct filetree* parentNode;
	//Comment: childNodeNum이 0인경우는 파일 밖에 없음
	int childNodeNum;
	struct filetree** childNodes;
} FILETREE;

//Comment: 파일트리를 생성하고, 값들을 초기화한다.
struct filetree* CreateFileTree(const char* fileName);

//Comment: 부모 자식 노드간의 링크를 연결해주고, 부모의 childNodeNum값을 증가시킨다.
void AddChildFileNode(struct filetree* parentNode, struct filetree* childNode);

//Comment: 하위 노드와 자신을 지우고, 부모 노드에 반영합니다.
void RemoveFileNode(struct filetree* node);

#endif