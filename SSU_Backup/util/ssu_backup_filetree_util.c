#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <openssl/md5.h>
#include <openssl/sha.h>
#include <errno.h>

#include "ssu_backup_define.h"
#include "ssu_backup_path.h"
#include "ssu_backup_hash.h"
#include "ssu_backup_util.h"
#include "ssu_backup_filetree_util.h"

char* GetRealNameByFileTree(char* buf, const struct filetree* ftree)
{
	size_t fileLen;

	fileLen = strlen(ftree->file) - SSU_BACKUP_FILE_META_LEN;
	if((ftree->childNodeNum > 0) || fileLen < 1){
		strcpy(buf, ftree->file);
		return buf;
	}

	strncpy(buf, ftree->file, fileLen);
	buf[fileLen] = '\0';

	return buf;
}

char* GetCreateTimeByFileTree(char* buf, const struct filetree* ftree)
{
	size_t fileLen;

	if(ftree->childNodeNum != 0){
		return NULL;
	}

	fileLen = strlen(ftree->file) - SSU_BACKUP_FILE_META_LEN;
	strcpy(buf, (ftree->file) + fileLen + 1);

	return buf;
}

char* GetPathByFileTree(char* buf, struct filetree* ftree)
{
	struct filetree* ptree = ftree;
	size_t parentLen;
	char pathBuf[SSU_BACKUP_MAX_PATH_SZ];

	strcpy(buf, ptree->file);
	while(ptree->parentNode != NULL){
		parentLen = strlen(ptree->parentNode->file);
		strcpy(pathBuf, buf);
		strcpy(buf, ptree->parentNode->file);
		buf[parentLen] = '/';
		strcpy(buf + parentLen + 1, pathBuf);
		ptree = ptree->parentNode;
	}

	return buf;
}

struct filetree* FindFileTreeInPath(const char* path, struct filetree* ftree, int isBackup)
{
	size_t nextStartPathLen;
	size_t ftreeNameLen;
	size_t pathFileLen;
	struct filetree* retTree = NULL;

	nextStartPathLen = 0;
	if(path[0] == '/'){
		nextStartPathLen++;
	}

	ftreeNameLen = strlen(ftree->file);
	if(isBackup && ftree->childNodeNum == 0){
		ftreeNameLen -= SSU_BACKUP_FILE_META_LEN;
	}
	pathFileLen = 0;
	while((path[nextStartPathLen] != '/') && (path[nextStartPathLen] != '\0')){
		if(ftree->file[pathFileLen] != path[nextStartPathLen]){
			return NULL;
		}
		nextStartPathLen++;
		pathFileLen++;
	}
	if(pathFileLen != ftreeNameLen){
		return NULL;
	}
	if(ftree->childNodeNum == 0){
		if(path[nextStartPathLen] != '\0')
			return NULL;

		return ftree;
	}

	//Comment: 폴더가 마지막 경로일 경우 리턴
	if(path[nextStartPathLen] == '\0')
		return ftree;
	for(int i=0; i<ftree->childNodeNum; i++){
		retTree = FindFileTreeInPath(&path[nextStartPathLen], ftree->childNodes[i], isBackup);
		if(retTree != NULL)
			return retTree;
	}
	return NULL;
}

int FindAllFileTreeInPath(const char* path, struct filetree* ftree, struct filetree*** matchedTrees, int isBackup)
{
	int matchCnt;
	int idx;
	struct filetree* pTree;
	struct filetree* cTree;
	struct filetree* mTree;
	char fileName[SSU_BACKUP_MAX_FILENAME];
	char cmpFileName[SSU_BACKUP_MAX_FILENAME];

	if((mTree = FindFileTreeInPath(path, ftree, isBackup)) == NULL){
		matchedTrees = NULL;
		return 0;
	}

	matchCnt = 0;
	//Comment: 부모노드가 없는 경우 그대로 리턴
	if((pTree = mTree->parentNode) == NULL){
		matchCnt++;
		(*matchedTrees) = (struct filetree**)malloc(sizeof(struct filetree*) * matchCnt);
		(*matchedTrees)[matchCnt - 1] = mTree;

		return matchCnt;
	}

	if(isBackup){
		GetRealNameByFileTree(fileName, mTree);
	} else {
		strcpy(fileName, mTree->file);
	}
	for(int i=0; i < pTree->childNodeNum; i++){
		cTree = pTree->childNodes[i];

		if(isBackup){
			GetRealNameByFileTree(cmpFileName, cTree);
		} else {
			strcpy(cmpFileName, cTree->file);
		}

		if(strcmp(fileName, cmpFileName) == 0){
			matchCnt++;
		}
	}

	idx = 0;
	(*matchedTrees) = (struct filetree**)malloc(sizeof(struct filetree*) * matchCnt);
	for(int i=0; i < pTree->childNodeNum; i++){
		cTree = pTree->childNodes[i];
		if(cTree->childNodeNum != 0)
			continue;

		GetRealNameByFileTree(cmpFileName, cTree);
		if(strcmp(fileName, cmpFileName) == 0){
			(*matchedTrees)[idx] = cTree;
			idx++;
		}
	}

	return matchCnt;
}

int PrintFileTreeList(const char* parentFilePath, const struct filetree** fileTreeList, int listNum)
{
	struct stat f_stat;
	char intBuf[SSU_BACKUP_MAX_PRINT_INT_BUF];
	char fileName[SSU_BACKUP_MAX_FILENAME];
	char filePath[SSU_BACKUP_MAX_PATH_SZ];

	for(int i=0; i<listNum; i++){
		strcpy(filePath, parentFilePath);
		ConcatPath(filePath, fileTreeList[i]->file);
		if(stat(filePath, &f_stat) == -1)
			return -1;

		GetCreateTimeByFileTree(fileName, fileTreeList[i]);
		IntToCommaString(f_stat.st_size, intBuf);
		printf("%d. %s    %sbytes\n", i + 1, fileName, intBuf);
	}

	return 0;
}

struct filetree* FileToFileTree(const char* path)
{
	char* fileName = NULL;
	char pathBuf[SSU_BACKUP_MAX_PATH_SZ];

	strcpy(pathBuf, path);
	if((fileName = GetFileNameByPath(pathBuf)) == NULL){
		return NULL;
	}

	return CreateFileTree(fileName);
}

struct filetree* PathToFileTree(const char* path, int hashMode)
{
	struct filetree* ftree = NULL;
	int fileType;

	fileType = CheckFileTypeByPath(path);
	switch(fileType){
		case SSU_BACKUP_TYPE_ERROR:
			ftree = NULL;
			break;
		case SSU_BACKUP_TYPE_REG:
			ftree = FileToFileTree(path);
			break;
		case SSU_BACKUP_TYPE_DIR:
			ftree = _PathToFileTreeDir(path, hashMode);
			break;
		default:
			ftree = NULL;
			break;
	}

	return ftree;
}

struct filetree* _PathToFileTreeDir(const char* path, int hashMode)
{
	struct filetree* ptree = NULL;
	struct filetree* ctree = NULL;
	struct dirent** childList;
	int childCount;
	int realChildCnt;
	char pathBuf[SSU_BACKUP_MAX_PATH_SZ];

	if((childCount = scandir(path, &childList, filterParentInScanDir, alphasort)) < 1){
		return NULL;
	}
	ptree = FileToFileTree(path);
	realChildCnt = 0;
	for(int i=0; i<childCount; i++){
		strcpy(pathBuf, path);
		ConcatPath(pathBuf, childList[i]->d_name);
		ctree = PathToFileTree(pathBuf, hashMode);
		free(childList[i]);
		if(ctree == NULL) continue;

		AddChildFileNode(ptree, ctree);	
		realChildCnt++;
	}
	free(childList);

	if(realChildCnt < 1)
		return NULL;
	return ptree;
}

int filterParentInScanDir(const struct dirent* target)
{
	if(!strcmp(target->d_name, ".") || !strcmp(target->d_name, "..")){
		return 0;
	}
	return 1;
}

int RemoveBackupByFileTree(const char* removePath, struct filetree* removeTree, int* foldCnt, int* fileCnt, int isSilent)
{
	struct filetree* pNode;
	char nextRemovePath[SSU_BACKUP_MAX_PATH_SZ];

	if(removeTree->childNodeNum == 0){
		if(unlink(removePath) == -1){
			fprintf(stderr, "\"%s\" unlink Failed! - %s\n", removePath, strerror(errno));
			return -1;
		}
		(*fileCnt)++;

		if(!isSilent){
			printf("\"%s\" backup file removed\n", removePath);
		}

		//Comment: 빈 디렉토리가 된 것은은 삭제.
		pNode = removeTree->parentNode;
		if((pNode != NULL) && pNode->childNodeNum == 1){
			GetParentPath(removePath, nextRemovePath);
			if(rmdir(nextRemovePath) == -1){
				fprintf(stderr, "\"%s\" rmdir Failed! - %s\n", nextRemovePath, strerror(errno));
				return -1;
			}
			(*foldCnt)++;
		}
		return 0;
	}

	//Comment: 폴더의 경우 재귀 호출
	while(removeTree->childNodeNum != 0){
		strcpy(nextRemovePath, removePath);
		ConcatPath(nextRemovePath, (*(removeTree->childNodes))->file);
		if(RemoveBackupByFileTree(nextRemovePath, *(removeTree->childNodes), foldCnt, fileCnt, isSilent) == -1)
			return -1;
		RemoveFileNode(*(removeTree->childNodes));
	}
	//Comment: 하위에서 지워지지 않은 경우 폴더 지우기
	if(CheckFileTypeByPath(removePath) == SSU_BACKUP_TYPE_DIR){
		if(rmdir(removePath) == -1){
			fprintf(stderr, "\"%s\" rmdir Failed! - %s\n", removePath, strerror(errno));
			return -1;
		}
		(*foldCnt)++;
	}

	return 0;
}