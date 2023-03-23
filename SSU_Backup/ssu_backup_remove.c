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
				removeType = SSU_BACKUP_TYPE_CLEAR;
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

	if(removeType == SSU_BACKUP_TYPE_CLEAR){
		if(ClearBackupFolder(hashMode) == -1){
			perror("ClearBackupFolder()");
			exit(1);
		}
		exit(0);
	}

	//Comment: 가상으로 상대경로를 절대경로로 바꿔줍니다.
	//	././../.., ~/././.. 와 같은 표현 모두 가능합니다.
	if(GetVirtualRealPath(pathBuf, removePath) == NULL){
		Usage(USAGEIDX_REMOVE);
		exit(1);
	}

	if(CheckRemovePathCondition(removePath) == -1){
		exit(1);
	}

	GetBackupPath(pathBuf);
	if((backupTree = PathToFileTree(pathBuf, hashMode)) == NULL){
		perror("PathToFileTree()");
		exit(1);
	}

	strcpy(destPath, removePath);
	SourcePathToBackupPath(destPath);
	strcpy(pathBuf, destPath);
	ExtractHomePath(pathBuf);
	if((matchNum = FindAllFileTreeInPath(pathBuf, backupTree, &removeTrees, 1)) < 1){
		Usage(USAGEIDX_REMOVE);
		exit(1);
	}

	GetParentPath(destPath, pathBuf);
	ConcatPath(pathBuf, removeTrees[0]->file);
	strcpy(destPath, pathBuf);
	checkType = CheckFileTypeByPath(destPath);
	if(CheckFileTypeCondition(removePath, removeType, checkType) == -1){
		exit(1);
	}

	//Comment: 파일에 -a 옵션을 사용한 경우
	if(removeType == SSU_BACKUP_TYPE_DIR && checkType == SSU_BACKUP_TYPE_REG){
		GetParentPath(destPath, pathBuf);
		if(RemoveFileByFileTreeList(pathBuf, (const struct filetree**)removeTrees, matchNum) == -1){
			perror("RemoveFileByFileTreeList()");
			exit(1);
		}
		exit(0);
	}

	if(RemoveFileByFileTree(destPath, removePath, removeTrees, matchNum, removeType) == -1){
		exit(1);
	}

	exit(0);
}

int CheckRemovePathCondition(const char* path)
{
	char* homeDir = getenv("HOME");
	char temp_path[SSU_BACKUP_MAX_PATH_SZ];

	if(strlen(path) > SSU_BACKUP_MAX_PATH_SZ){
		fputs("경로의 길이가 큽니다.\n", stderr);
		return -1;
	}

	if(strncmp(homeDir, path, strlen(homeDir)) != 0){
        fprintf(stdout, "<%s> can't be backuped\n", path);
		return -1;
    }

	GetBackupPath(temp_path);
	if(strncmp(temp_path, path, strlen(temp_path)) == 0){
		fprintf(stdout, "<%s> can't be backuped\n", path);
		return -1;
	}

	return 0;
}

int CheckFileTypeCondition(const char* originPath, int removeType, int checkType)
{
	if(checkType == SSU_BACKUP_TYPE_ERROR){
		perror("CheckFileTypeByPath()");
		return -1;
	}
	if(checkType == SSU_BACKUP_TYPE_OTHER){
		fputs("일반 파일이나 디렉토리가 아닙니다.", stderr);
		return -1;
    }
	if((checkType == SSU_BACKUP_TYPE_DIR) && checkType != removeType){
		fprintf(stderr, "\"%s\" is a directory file\n", originPath);
		return -1;
	}

	return 0;
}

int RemoveFileSelector(const char* parentPath, const char* originPath, const struct filetree** removeTrees, int listNum)
{
	int sellect;
	char c;
	char pathBuf[SSU_BACKUP_MAX_PATH_SZ];

	sellect = -1;
	while(sellect < 0 || sellect > listNum){
		printf("backup file list of \"%s\"\n", originPath);
		puts("0. exit");
		PrintFileTreeList(parentPath, removeTrees, listNum);
		puts("Choose file to remove");
		printf("%s ", SSU_BACKUP_SHELL_SELLECTOR);
		if(!scanf("%d", &sellect)){
			puts("Wrong Input!\n");
			while((c = getchar()) != '\n' && c != EOF);
		}
	}
	if(sellect == 0)
		return 0;

	strcpy(pathBuf, parentPath);
	ConcatPath(pathBuf, removeTrees[sellect-1]->file);
	if(unlink(pathBuf) == -1)
		return -1;

	printf("\"%s\" backup file removed\n", pathBuf);
	return 0;
}

int RemoveFileByFileTreeList(const char* parentPath, const struct filetree** removeTrees, int listNum)
{
	struct filetree* pTree;
	
	char pathBuf[SSU_BACKUP_MAX_PATH_SZ];

	for(int i=0; i < listNum; i++){
		strcpy(pathBuf, parentPath);
		ConcatPath(pathBuf, removeTrees[i]->file);
		if(unlink(pathBuf) == -1)
			return -1;

		printf("\"%s\" backup file removed\n", pathBuf);
	}

	//Comment: Remove Folder if empty
	if((pTree = removeTrees[0]->parentNode) != NULL){
		if(pTree->childNodeNum == listNum){
			if(rmdir(parentPath) == -1)
				return -1;
		}
	}

	return 0;
}

int RemoveBackupFolderByFileTree(const char* removePath, struct filetree* removeTree, int* foldCnt, int* fileCnt, int isSilent)
{
	char nextRemovePath[SSU_BACKUP_MAX_PATH_SZ];

	//Comment: 해시값이 같은 파일이 있는지 검사후 없으면 생성
	if(removeTree->childNodeNum == 0){
		if(unlink(removePath) == -1)
			return -1;
		(*fileCnt)++;

		if(!isSilent){
			printf("\"%s\" backup file removed\n", removePath);
		}
		return 0;
	}

	//Comment: 폴더의 경우 재귀 호출
	for(int i=0; i < removeTree->childNodeNum; i++){
		strcpy(nextRemovePath, removePath);
		ConcatPath(nextRemovePath, removeTree->childNodes[i]->file);
		if(RemoveBackupFolderByFileTree(nextRemovePath, removeTree->childNodes[i], foldCnt, fileCnt, isSilent) == -1)
			return -1;
	}
	if(rmdir(removePath) == -1)
		return -1;
	(*foldCnt)++;

	return 0;
}

int RemoveFileByFileTree(const char* destPath, const char* originPath, struct filetree** removeTrees, int listNum, int removeType)
{
	int foldCnt, fileCnt;
	char pathBuf[SSU_BACKUP_MAX_PATH_SZ];

	switch (removeType)
	{
		case SSU_BACKUP_TYPE_REG:
			GetParentPath(destPath, pathBuf);
			if(RemoveFileSelector(pathBuf, originPath, (const struct filetree**)removeTrees, listNum) == - 1){
				perror("RemoveFileSelector()");
				return -1;
			}
			break;
		case SSU_BACKUP_TYPE_DIR:
			if(RemoveBackupFolderByFileTree(destPath, removeTrees[0], &foldCnt, &fileCnt, 0) == -1){
				perror("RemoveBackupFolderByFileTree()");
				return -1;
			}
			break;
		default:
			fputs("Undefined removeType\n", stderr);
			return -1;
			break;
	}

	return 0;
}

int ClearBackupFolder(int hashMode)
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
		if(RemoveBackupFolderByFileTree(destPath, cTree, &foldCnt, &fileCnt, 1) == -1){
			perror("RemoveBackupFolderByFileTree()");
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