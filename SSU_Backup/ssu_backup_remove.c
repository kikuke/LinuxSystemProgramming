#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>

#include "ssu_backup_define.h"
#include "ssu_backup_usage.h"
#include "ssu_backup_path.h"
#include "ssu_backup_hash.h"
#include "ssu_backup_util.h"
#include "ssu_backup_filetree_util.h"
#include "ssu_backup_remove.h"

int main(int argc, char* argv[])
{
	int hashMode = -1;
	int removeType = SSU_BACKUP_TYPE_REG;
	char destPath[SSU_BACKUP_MAX_PATH_SZ];
	char removePath[SSU_BACKUP_MAX_PATH_SZ + 1];
	char pathBuf[SSU_BACKUP_MAX_PATH_SZ];
	char opt;
	int checkType;
	struct filetree* backupTree;
	struct filetree* removeTree;

	if(argc < 2 || argc > 3){
		Usage(USAGEIDX_REMOVE);
		exit(1);
	}

	strcpy(pathBuf, argv[1]);
	while((opt = getopt(argc, argv, "ac")) != -1){
		switch(opt){
			case 'a':
				if(removeType != SSU_BACKUP_TYPE_REG){
					Usage(USAGEIDX_REMOVE);
					exit(1);
				}
				removeType = SSU_BACKUP_TYPE_DIR;
				break;

			case 'c':
				if(removeType != SSU_BACKUP_TYPE_REG){
					Usage(USAGEIDX_REMOVE);
					exit(1);
				}
				removeType = SSU_BACKUP_TYPE_CLEAR;
				break;
			
			default:
				Usage(USAGEIDX_REMOVE);
				exit(1);
				break;
		}
	}

	//Comment: 가상으로 상대경로를 절대경로로 바꿔줍니다.
	//	././../.., ~/././.. 와 같은 표현 모두 가능합니다.
	if(GetVirtualRealPath(pathBuf, removePath) == NULL){
		Usage(USAGEIDX_REMOVE);
		exit(1);
	}

	GetBackupPath(pathBuf);
	if(strncmp(pathBuf, removePath, strlen(pathBuf)) == 0){
		fprintf(stdout, "<%s> can't be backuped\n", removePath);
		exit(1);
	}

	if((hashMode = GetHashMode()) == -1){
		fputs("GetHashMode() Failed!\n", stderr);
		exit(1);
	}

	strcpy(destPath, removePath);
	SourcePathToBackupPath(destPath);

/*
	if((checkType = CheckFileTypeByPath(addPath)) == -1){
		perror("CheckFileTypeByPath()");
		exit(1);
	}
	if((checkType == SSU_BACKUP_TYPE_DIR) && checkType != addType){
		fprintf(stderr, "\"%s\" is a directory file\n", addPath);
		exit(1);
	}

	GetBackupPath(destPath);
	if((backupTree = PathToFileTree(destPath, hashMode)) == NULL){
		perror("PathToFileTree()");
		exit(1);
	}
	if((addTree = PathToFileTree(addPath, hashMode)) == NULL){
		perror("PathToFileTree()");
		exit(1);
	}
*/
	return 0;
}

int RemoveBackupByFileTree(const char* backupPath, const char* addPath, struct filetree* backupTree, struct filetree* addTree, int hashMode)
{
	struct filetree* matchedTree;
	char backupTreePath[SSU_BACKUP_MAX_PATH_SZ];
	char addTreePath[SSU_BACKUP_MAX_PATH_SZ];

	strcpy(backupTreePath, backupPath);
	ConcatPath(backupTreePath, addTree->file);
	ExtractHomePath(backupTreePath);
	strcpy(addTreePath, addPath);
	ConcatPath(addTreePath, addTree->file);
	matchedTree = FindFileTreeInPath(backupTreePath, backupTree, 1);
	//Comment: 일치하는 백업파일이 없는 경우 해당 하위 파일 모두 생성
	if(matchedTree == NULL)
	{
		if(MakeDirPath(backupPath) == -1)
			return -1;
		return CreateFileByFileTree(backupPath, addPath, addTree, 0);
	}

	//Comment: 해시값이 같은 파일이 있는지 검사후 없으면 생성
	if(addTree->childNodeNum == 0){
		struct filetree* pTree = matchedTree->parentNode;
		for(int i=0; i < pTree->childNodeNum; i++){
			if(CompareHash(addTree->hash, pTree->childNodes[i]->hash, hashMode)){

				fprintf(stdout, "\"%s\" is already backuped\n", addTreePath);
				return 0;
			}
		}

		return CreateFileByFileTree(backupPath, addPath, addTree, 0);
	}

	//Comment: 폴더의 경우 재귀 호출
	strcpy(backupTreePath, backupPath);
	ConcatPath(backupTreePath, addTree->file);
	for(int i=0; i < addTree->childNodeNum; i++)
		if(RemoveBackupByFileTree(backupTreePath, addTreePath, backupTree, addTree->childNodes[i], hashMode) == -1)
			return -1;
	return 0;
}
