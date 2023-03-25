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
#include "ssu_backup_recover.h"

int main(int argc, char* argv[])
{
	int hashMode = -1;
	int recoverType = SSU_BACKUP_TYPE_REG;
	int checkType;
	char opt;
	struct filetree* backupTree;
	struct filetree* recoverTree;
	struct filetree* matchTree;
	char backupPath[SSU_BACKUP_MAX_PATH_SZ + 1];
	char recoverPath[SSU_BACKUP_MAX_PATH_SZ + 1];
	char pathBuf[SSU_BACKUP_MAX_PATH_SZ];

	if(argc < 2 || argc > 5){
		Usage(USAGEIDX_RECOVER);
		exit(1);
	}

	strcpy(backupPath, argv[1]);
    strcpy(recoverPath, ".");
	while((opt = getopt(argc, argv, "dn:")) != -1){
		switch(opt){
			case 'd':
				recoverType = SSU_BACKUP_TYPE_DIR;
				break;

			case 'n':
				if(optarg == NULL){
					Usage(USAGEIDX_RECOVER);
					exit(1);
				}
                strcpy(recoverPath, optarg);
				break;
			
			default:
				Usage(USAGEIDX_RECOVER);
				exit(1);
				break;
		}
	}

	//Comment: 가상으로 상대경로를 절대경로로 바꿔줍니다.
	//	././../.., ~/././.. 와 같은 표현 모두 가능합니다.
    strcpy(pathBuf, backupPath);
	if(GetVirtualRealPath(pathBuf, backupPath) == NULL){
		Usage(USAGEIDX_RECOVER);
		exit(1);
	}
	if(CheckRecoverPathCondition(backupPath) == -1){
		exit(1);
	}
    SourcePathToBackupPath(backupPath);

    strcpy(pathBuf, recoverPath);
	if(GetVirtualRealPath(pathBuf, recoverPath) == NULL){
		Usage(USAGEIDX_RECOVER);
		exit(1);
	}
	if(CheckRecoverPathCondition(recoverPath) == -1){
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

	strcpy(pathBuf, backupPath);
	ExtractHomePath(pathBuf);
	if((matchTree = FindFileTreeInPath(pathBuf, backupTree, 1)) == NULL){
		Usage(USAGEIDX_RECOVER);
		exit(1);
	}

	GetParentPath(backupPath, pathBuf);
	ConcatPath(pathBuf, matchTree->file);
	checkType = CheckFileTypeByPath(pathBuf);
	if(CheckFileTypeCondition(pathBuf, recoverType, checkType) == -1){
		exit(1);
	}

    //Todo: 리코버 패스가 실제 있는 경로인지. 없다면 생성하기 두 파일의 타입이 다른 경우 에러 발생 메시지 출력하기
	//	add꺼 배껴서 넣기
	//	이건 그냥 재귀 호출하면서 반복하기
	//	리커버 트리 가져오기, 이전에 만들어둔 함수 사용하기
	//	폴더 삭제기능 필요하면 딜리트꺼 유틸로 옮기고 사용하기
	//	NULL이던 말던 그대로 집어넣으면 됨. 함수에서 처리함.
	//	해당 경로 파일 트리로 만드는 함수 쓰기
	//	add 성공하면 remove기능 이용하여 지우기

	//Comment: 복원 지점 경로의 파일을 일단 가져옴. 없어도 NULL로 가져옴.
	recoverTree = PathToFileTree(recoverPath, hashMode);



	exit(0);
}

int CheckRecoverPathCondition(const char* path)
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

int CheckFileTypeCondition(const char* originPath, int recoverType, int checkType)
{
	if(checkType == SSU_BACKUP_TYPE_ERROR){
		perror("CheckFileTypeByPath()");
		return -1;
	}
	if(checkType == SSU_BACKUP_TYPE_OTHER){
		fputs("일반 파일이나 디렉토리가 아닙니다.", stderr);
		return -1;
    }
	if((checkType == SSU_BACKUP_TYPE_DIR) && checkType != recoverType){
		fprintf(stderr, "\"%s\" is a directory file\n", originPath);
		return -1;
	}

	return 0;
}

int RecoverFileSelector(const char* parentPath, const char* originPath, const struct filetree** recoverTrees, int listNum)
{
	int sellect;
	char c;
	struct filetree* pTree;
	char pathBuf[SSU_BACKUP_MAX_PATH_SZ];

	sellect = -1;
	while(sellect < 0 || sellect > listNum){
		printf("backup file list of \"%s\"\n", originPath);
		puts("0. exit");
		PrintFileTreeList(parentPath, recoverTrees, listNum);
		puts("Choose file to recover");
		printf("%s ", SSU_BACKUP_SHELL_SELLECTOR);
		if(!scanf("%d", &sellect)){
			puts("Wrong Input!\n");
			while((c = getchar()) != '\n' && c != EOF);
		}
	}
	if(sellect == 0)
		return 0;

	//Todo: 수정하기
	strcpy(pathBuf, parentPath);
	ConcatPath(pathBuf, recoverTrees[sellect-1]->file);
	if(unlink(pathBuf) == -1)
		return -1;
	printf("\"%s\" backup file removed\n", pathBuf);

	//Todo: 수정하기
	//Comment: Remove Folder if empty
	if((pTree = recoverTrees[0]->parentNode) != NULL){
		if(pTree->childNodeNum == listNum){
			if(rmdir(parentPath) == -1)
				return -1;
		}
	}

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
			if(RecoverFileSelector(pathBuf, originPath, (const struct filetree**)removeTrees, listNum) == - 1){
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