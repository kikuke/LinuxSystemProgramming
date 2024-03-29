#include <stdlib.h>
#include <string.h>

#include "ssu_backup_filetree.h"

struct filetree* CreateFileTree(const char* fileName)
{
	struct filetree* newFileTree;

	newFileTree = (struct filetree*)malloc(sizeof(struct filetree));
	strcpy(newFileTree->file, fileName);
	
	newFileTree->parentNode = NULL;
	newFileTree->childNodeNum = 0;
	newFileTree->childNodes = NULL;
	return newFileTree;
}

void AddChildFileNode(struct filetree* parentNode, struct filetree* childNode)
{
	struct filetree** tempNodes;

	childNode->parentNode = parentNode;

	tempNodes = (struct filetree**)malloc(sizeof(struct filetree*) * (parentNode->childNodeNum + 1));
	if(parentNode->childNodeNum > 0){
		memcpy(tempNodes, parentNode->childNodes, sizeof(struct filetree*) * parentNode->childNodeNum);
		free(parentNode->childNodes);
	}

	tempNodes[parentNode->childNodeNum] = childNode;
	parentNode->childNodeNum++;

	parentNode->childNodes = tempNodes;
}

void RemoveFileNode(struct filetree* node)
{
	int idx;
	int cNodeNum;
	struct filetree* pNode;

	if((pNode = node->parentNode) != NULL){
		idx = 0;
		for(int i=0; i<pNode->childNodeNum; i++){
			if(pNode->childNodes[i] == node)
				continue;

			pNode->childNodes[idx] = pNode->childNodes[i];
			idx++;
		}
		pNode->childNodeNum--;
	}

	cNodeNum = node->childNodeNum;
	for(int i=0; i<cNodeNum; i++){
		RemoveFileNode(*(node->childNodes));
	}

	free(node);
}