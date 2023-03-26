#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

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
	int checkType;
	char opt;
	struct filetree* backupTree;
	struct filetree** removeTrees;
	int matchNum;
	char destPath[SSU_BACKUP_MAX_PATH_SZ];
	char removePath[SSU_BACKUP_MAX_PATH_SZ + 1];
	char pathBuf[SSU_BACKUP_MAX_PATH_SZ];

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
				if(argc > 2 || removeType != SSU_BACKUP_TYPE_REG){
					Usage(USAGEIDX_REMOVE);
					exit(1);
				}
				GetHomePath(pathBuf);
				removeType = SSU_BACKUP_TYPE_DIR;
				break;
			
			default:
				Usage(USAGEIDX_REMOVE);
				exit(1);
				break;
		}
	}

	if((hashMode = GetHashMode()) == -1){
		fputs("GetHashMode() Failed!\n", stderr);
		exit(1);
	}

	//Comment: 가상으로 상대경로를 절대경로로 바꿔줍니다.
	//	././../.., ~/././.. 와 같은 표현 모두 가능합니다.
	if(GetVirtualRealPath(pathBuf, removePath) == NULL){
		Usage(USAGEIDX_REMOVE);
		exit(1);
	}

	if(CheckPathCondition(removePath) == -1){
		exit(1);
	}

	//Comment: 홈이거나 C옵션일 경우
	strcpy(destPath, removePath);
	SourcePathToBackupPath(destPath);
	GetBackupPath(pathBuf);
	if((removeType == SSU_BACKUP_TYPE_DIR) && (strcmp(destPath, pathBuf) == 0)){
		if(ClearBackup(hashMode) == -1){
			perror("ClearBackup()");
			exit(1);
		}
		exit(0);
	}

	if((backupTree = PathToFileTree(pathBuf, hashMode)) == NULL){
		perror("PathToFileTree()");
		exit(1);
	}

	strcpy(pathBuf, destPath);
	ExtractHomePath(pathBuf);
	if((matchNum = FindAllFileTreeInPath(pathBuf, backupTree, &removeTrees, 1)) < 1){
		Usage(USAGEIDX_REMOVE);
		exit(1);
	}

	GetParentPath(destPath, pathBuf);
	strcpy(destPath, pathBuf);
	//Comment: 같은 이름의 파일과 폴더가 공존할수 있으니, 폴더인 경우까지 검사하기 위해
	for(int i=0; i < matchNum; i++){
		strcpy(pathBuf, destPath);
		ConcatPath(pathBuf, removeTrees[i]->file);
		checkType = CheckFileTypeByPath(pathBuf);
		if(CheckFileTypeCondition(removePath, removeType, checkType) == -1){
			exit(1);
		}
	}

	if(RemoveFileByFileTree(destPath, removePath, removeTrees, matchNum, removeType) == -1){
		perror("RemoveFileByFileTree");
		exit(1);
	}

	exit(0);
}

int RemoveFileByFileTree(const char* parentPath, const char* originPath, struct filetree** removeTrees, int listNum, int removeType)
{
	int foldCnt, fileCnt;

	//Comment: -a 옵션을 사용한 경우 or 파일이 하나일 경우
	if(removeType == SSU_BACKUP_TYPE_DIR || listNum == 1){
		return RemoveFileByFileTreeList(parentPath, removeTrees, listNum);
	}

	return RemoveFileSelector(parentPath, originPath, removeTrees, listNum);
}

int RemoveFileByFileTreeList(const char* parentPath, struct filetree** removeTrees, int listNum)
{
	int foldCnt=0, fileCnt=0;
	char removePath[SSU_BACKUP_MAX_PATH_SZ];

	for(int i=0; i < listNum; i++){
		strcpy(removePath, parentPath);
		ConcatPath(removePath, removeTrees[i]->file);
		if(RemoveBackupByFileTree(removePath, removeTrees[i], &foldCnt, &fileCnt, 0) == -1)
			return -1;
	}

	return 0;
}

int RemoveFileSelector(const char* parentPath, const char* originPath, struct filetree** removeTrees, int listNum)
{
	int sellect;
	int foldCnt=0, fileCnt=0;
	char c;
	char removePath[SSU_BACKUP_MAX_PATH_SZ];

	sellect = -1;
	while(sellect < 0 || sellect > listNum){
		printf("backup file list of \"%s\"\n", originPath);
		puts("0. exit");
		PrintFileTreeList(parentPath, (const struct filetree**)removeTrees, listNum);
		puts("Choose file to remove");
		printf("%s ", SSU_BACKUP_SHELL_SELLECTOR);
		if(!scanf("%d", &sellect)){
			puts("Wrong Input!\n");
			while((c = getchar()) != '\n' && c != EOF);
		}
	}
	if(sellect == 0)
		return 0;

	strcpy(removePath, parentPath);
	ConcatPath(removePath, removeTrees[sellect-1]->file);
	return RemoveBackupByFileTree(removePath, removeTrees[sellect-1], &foldCnt, &fileCnt, 0);
}

int ClearBackup(int hashMode)
{
	struct filetree* backupTree;
	struct filetree* cTree;
	int foldCnt, fileCnt;
	char destPath[SSU_BACKUP_MAX_PATH_SZ];

	GetBackupPath(destPath);

	if((backupTree = PathToFileTree(destPath, hashMode)) == NULL){
		perror("PathToFileTree()");
		return -1;
	}

	foldCnt = 0, fileCnt = 0;
	for(int i=0; i<backupTree->childNodeNum; i++){
		cTree = backupTree->childNodes[i];
		if(strcmp(cTree->file, SSU_BACKUP_HASH_SET_FILE) == 0)
			continue;

		GetBackupPath(destPath);
		ConcatPath(destPath, cTree->file);
		if(RemoveBackupByFileTree(destPath, cTree, &foldCnt, &fileCnt, 1) == -1){
			perror("RemoveBackupByFileTree()");
			return -1;
		}
	}
	if(foldCnt > 0 || fileCnt > 0){
		printf("backup directoy cleared(%d regular files and %d subdirectories totally).\n", fileCnt, foldCnt);
	} else {
		puts("no file(s) in the backup");
	}

	return 0;
}