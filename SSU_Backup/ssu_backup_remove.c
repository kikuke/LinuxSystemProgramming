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
	struct filetree* backupTree;
	struct filetree** removeTrees;
	int matchNum;

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

	//Todo: C옵션일 경우 처리
	//	없을경우 예외처리도 해주기

	//Comment: 가상으로 상대경로를 절대경로로 바꿔줍니다.
	//	././../.., ~/././.. 와 같은 표현 모두 가능합니다.
	if(GetVirtualRealPath(pathBuf, removePath) == NULL){
		Usage(USAGEIDX_REMOVE);
		exit(1);
	}

	if(CheckRemovePathCondition(removePath) == -1){
		exit(1);
	}

	if((hashMode = GetHashMode()) == -1){
		fputs("GetHashMode() Failed!\n", stderr);
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
	if(CheckFileTypeCondition(pathBuf, removePath, removeType) == -1){
		exit(1);
	}

	//Todo: 나중에 케이스로 해서 함수화 하기
	if(removeType == SSU_BACKUP_TYPE_REG){
		GetParentPath(destPath, pathBuf);
		if(RemoveFileSelector(pathBuf, removePath, (const struct filetree**)removeTrees, matchNum) == - 1){
			perror("RemoveFileSelector()");
			exit(1);
		}
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

int CheckFileTypeCondition(const char* destPath, const char* originPath, int removeType)
{
	int checkType;

	if((checkType = CheckFileTypeByPath(destPath)) == SSU_BACKUP_TYPE_ERROR){
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
