#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <openssl/md5.h>
#include <openssl/sha.h>

#include "ssu_backup_define.h"
#include "ssu_backup_path.h"
#include "ssu_backup_hash.h"
#include "ssu_backup_util.h"
#include "ssu_backup_filetree_util.h"

char* GetRealNameByFileTree(char* buf, const struct filetree* ftree)
{
	size_t fileLen;

	if(ftree->childNodeNum > 0){
		strcpy(buf, ftree->file);
		return buf;
	}

	fileLen = strlen(ftree->file) - SSU_BACKUP_FILE_META_LEN;
	strncpy(buf, ftree->file, fileLen);

	return buf;
}

char* GetPathByFileTree(char* buf, struct filetree* ftree, int isBackup)
{
	struct filetree* ptree = ftree;
	size_t parentLen;
	char pathBuf[SSU_BACKUP_MAX_PATH_SZ];

	if(isBackup){
		GetRealNameByFileTree(buf, ptree);
	} else {
		strcpy(buf, ptree->file);
	}
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

struct filetree* FileToFileTree(const char* path, int hashMode)
{
	char* fileName = NULL;
	char pathBuf[SSU_BACKUP_MAX_PATH_SZ];
	char hashBuf[SSU_BACKUP_HASH_MAX_LEN];
	int hashReturnVal;
	size_t hash_sz = 0;

	strcpy(pathBuf, path);
	if((fileName = GetFileNameByPath(pathBuf)) == NULL){
		return NULL;
	}

	switch(hashMode){
		case SSU_BACKUP_HASH_MD5:
			hashReturnVal = GetMd5HashByPath(path, hashBuf);
			hash_sz = MD5_DIGEST_LENGTH;
			break;
		case SSU_BACKUP_HASH_SHA1:
			hashReturnVal = GetSha1HashByPath(path, hashBuf);
			hash_sz = SHA_DIGEST_LENGTH;
			break;
		default:
			hashReturnVal = -1;
			break;
	}
	if(hashReturnVal == -1){
		perror("GetHash()");
		return NULL;
	}

	return CreateFileTree(fileName, hashBuf, hash_sz);
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
			ftree = FileToFileTree(path, hashMode);
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
	ptree = FileToFileTree(path, hashMode);
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

int CreateFileByFileTree(const char* destPath, const char* addPath, const struct filetree* addTree, int isRecover)
{
	char addFilePath[SSU_BACKUP_MAX_PATH_SZ];
	char destFilePath[SSU_BACKUP_MAX_PATH_SZ];

	strcpy(destFilePath, destPath);
	ConcatPath(destFilePath, addTree->file);
	strcpy(addFilePath, addPath);
	ConcatPath(addFilePath, addTree->file);
	if(addTree->childNodeNum == 0){
		if(isRecover){
			destFilePath[strlen(destFilePath) - SSU_BACKUP_FILE_META_LEN] = '\0';
			if(CopyFile(destFilePath, addFilePath) == -1)
				return -1;

			fprintf(stdout, "\"%s\" backup recover to \"%s\"\n", addFilePath, destFilePath);
			return 0;
		}

		if(GetNowTime(destFilePath + strlen(destFilePath)) == -1)
			return -1;
		if(CopyFile(destFilePath, addFilePath) == -1)
			return -1;

		fprintf(stdout, "\"%s\" backuped\n", destFilePath);
		return 0;
	}

	//Comment: 폴더 만들고 재귀 호출
	if(MakeDirPath(destFilePath) == -1)
		return -1;
	for(int i=0; i < addTree->childNodeNum; i++){
		if(CreateFileByFileTree(destFilePath, addFilePath, addTree->childNodes[i], isRecover) == -1)
			return -1;
	}

	return 0;
}