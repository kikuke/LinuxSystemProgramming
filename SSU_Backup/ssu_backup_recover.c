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
#include "ssu_backup_recover.h"

int main(int argc, char* argv[])
{
	int hashMode = -1;
	int recoverType = SSU_BACKUP_TYPE_REG;
	int checkType;
	char opt;
	struct filetree* backupTree;
	struct filetree** matchedTrees;
	int matchNum;
	char backupPath[SSU_BACKUP_MAX_PATH_SZ + 1];
	char recoverPath[SSU_BACKUP_MAX_PATH_SZ + 1];
	char originPath[SSU_BACKUP_MAX_PATH_SZ];
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
	if(CheckPathCondition(backupPath) == -1){
		exit(1);
	}
    SourcePathToBackupPath(backupPath);

    strcpy(pathBuf, recoverPath);
	if(GetVirtualRealPath(pathBuf, recoverPath) == NULL){
		Usage(USAGEIDX_RECOVER);
		exit(1);
	}
	if(CheckPathCondition(recoverPath) == -1){
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
	if((matchNum = FindAllFileTreeInPath(pathBuf, backupTree, matchedTrees, 1)) < 1){
		Usage(USAGEIDX_RECOVER);
		exit(1);
	}

	strcpy(originPath, backupPath);
	BackupPathToSourcePath(originPath);
	//Comment: 같은 이름의 디렉토리와, 파일이 공존할수 있으니 검사
	for(int i=0; i < matchNum; i++){
		GetParentPath(backupPath, pathBuf);
		ConcatPath(pathBuf, matchedTrees[i]->file);
		checkType = CheckFileTypeByPath(pathBuf);
		if(CheckFileTypeCondition(originPath, recoverType, checkType) == -1){
			exit(1);
		}
	}

	//Comment: 검색된 일치 목록이 하나 이상일 경우. 동일한 이름의 디렉토리와 파일이 섞여있을 경우 포함.
	if(matchNum > 1){
		GetParentPath(backupPath, pathBuf);
		if(RecoverFileSelector(pathBuf, recoverPath, backupTree, matchedTrees, matchNum, hashMode) == -1){
			fprintf(stderr, "\"%s\" FindAllFileTreeInPath() Failed! - %s\n", backupPath, strerror(errno));
			exit(1);
		}

		exit(0);
	}
	
	//Comment: 검색된 일치 목록이 하나일 경우
	if(RecoverFileByFileTree(backupPath, recoverPath, backupTree, *matchedTrees, hashMode) == -1){
		perror("RecoverFileByFileTree");
		exit(1);
	}

	exit(0);
}

int RecoverFileSelector(const char* parentPath, const char* destPath, const struct filetree* backupTree, const struct filetree** matchedTrees, int listNum, int hashMode)
{
	int sellect;
	char c;
	struct filetree* pTree;
	char recoverPath[SSU_BACKUP_MAX_PATH_SZ];
	char pathBuf[SSU_BACKUP_MAX_PATH_SZ];

	strcpy(recoverPath, parentPath);
	GetRealNameByFileTree(pathBuf, *matchedTrees);
	ConcatPath(recoverPath, pathBuf);
	BackupPathToSourcePath(recoverPath);
	sellect = -1;
	while(sellect < 0 || sellect > listNum){
		printf("backup file list of \"%s\"\n", recoverPath);
		puts("0. exit");
		PrintFileTreeList(parentPath, matchedTrees, listNum);
		puts("Choose file to recover");
		printf("%s ", SSU_BACKUP_SHELL_SELLECTOR);
		if(!scanf("%d", &sellect)){
			puts("Wrong Input!\n");
			while((c = getchar()) != '\n' && c != EOF);
		}
	}
	if(sellect == 0)
		return 0;

	strcpy(recoverPath, parentPath);
	ConcatPath(recoverPath, matchedTrees[sellect-1]->file);
	return RecoverFileByFileTree(recoverPath, destPath, backupTree, matchedTrees[sellect-1], hashMode);
}

int RecoverFileByFileTree(const char* backupPath, const char* recoverPath, const struct filetree* backupTree, const struct filetree* recoverTree, int hashMode)
{
	int foldCnt, fileCnt;
	int retVal;

	//Comment: 파일이 recoverPath에 존재할 경우 두 파일의 해시값 비교. 디렉토리도 해시값이 동일하면 건너뜀.
	if(CheckFileTypeByPath(recoverPath) != SSU_BACKUP_TYPE_ERROR){
		if((retVal = CompareHashByPath(backupPath, recoverPath, hashMode)) == -1)
			return -1;

		if(retVal == 1){
			fprintf(stdout, "\"%s\" is already backuped\n", recoverPath);
			return 0;
		}
	}

	//Comment: backupPath가 파일인 경우
	if(recoverTree->childNodeNum == 0){
		if(CopyFile(backupPath, recoverPath) == -1){
			fprintf(stderr, "\"%s\" to \"%s\" CopyFile Failed! - %s\n", backupPath, recoverPath, strerror(errno));
			return -1;
		}

		//Comment: 복원된 백업 경로의 파일을 삭제
		if(RemoveBackupByFileTree(backupPath, recoverTree, &foldCnt, &fileCnt, 1) == -1){
			perror("RemoveBackupByFileTree()");
			return -1;
		}

		fprintf(stdout, "\"%s\" backup recover to \"%s\"\n", backupPath, recoverPath);
		return 0;
	}

	//Comment: recoverTree가 디렉토리인 경우
	//	디렉토리 만들고 RecoverBackupByFileTree로 인자들을 넘긴다.
	if(MakeDirPath(backupPath) == -1){
		perror("MakeDirPath()");
		return -1;
	}

	return RecoverBackupByFileTree(backupPath, recoverPath, backupTree, recoverTree, hashMode)
}

int RecoverBackupByFileTree(const char* pBackupPath, const char* pRecoverPath, struct filetree* backupTree, struct filetree* pRecoverTree, int hashMode)
{
	int retVal;
	int backupCheckType;
	int recoverCheckType;
	int matchNum;
	struct filetree** recoverTrees;
	char checkBackupPath[SSU_BACKUP_MAX_PATH_SZ];
	char backupTreePath[SSU_BACKUP_MAX_PATH_SZ];
	char recoverTreePath[SSU_BACKUP_MAX_PATH_SZ];

	GetBackupPath(checkBackupPath);
	//Comment: 백업 패스인 경우 제외
	if(strncmp(checkBackupPath, recoverPath, strlen(checkBackupPath)) == 0)
		return 0;

	strcpy(backupTreePath, backupPath);
	ExtractHomePath(backupTreePath);
	//Comment: 해당 경로에 대해 모든 일치하는 백업트리를 찾음.
	if((matchNum = FindAllFileTreeInPath(backupTreePath, backupTree, &recoverTrees, 1)) < 1){
		fprintf(stderr, "\"%s\" FindAllFileTreeInPath() Failed! - %s\n", backupPath, strerror(errno));
		return -1;
	}

	//Todo: CreateFileByFileTree 보면서 추가하고 그함수도 리커버 인수 지우는걸로 수정하기
	//	수정 다 끝나면 저 함수 파일트리 유틸에서 애드함수로 옮겨주기.
	//Comment: 복원지점에 폴더나 파일이 없는 경우 또는 덮어쓰는 경우.
	//Todo: 이거 아님. 따로 만들어서 직접 검사하기
	if(recoverTree == NULL){
		//Todo: 평가없이 생성하는 함수 만들기
		//	파일 체크없이 쭉 생성하는 함수. 위에 함수 참고해서 구현하기.
		//	아래에서는 파일타입검사후, 생성여부를 결정함. 아래에서도 위의 함수를 쓰긴함.
		return
	}

	//Todo: 많은 경우 셀렉터 호출

	if((matchedTree = FindFileTreeInPath(backupTreePath, backupTree, 1)) == NULL)
	{
		if(MakeDirPath(backupPath) == -1)
			return -1;
		return CreateFileByFileTree(backupPath, addPath, addTree, 0);
	}

	//Comment: 해시값이 같은 파일이 있는지 검사후 없으면 생성
	if(addTree->childNodeNum == 0){
		struct filetree* pTree = matchedTree->parentNode;
		if(pTree != NULL){
			for(int i=0; i < pTree->childNodeNum; i++){
				GetParentPath(backupPath, backupTreePath);
				ConcatPath(backupTreePath, pTree->childNodes[i]->file);
			}
		}

		return CreateFileByFileTree(backupPath, addPath, addTree, 0);
	}

	//Comment: 폴더의 경우 재귀 호출
	for(int i=0; i < addTree->childNodeNum; i++){
		strcpy(backupTreePath, backupPath);
		ConcatPath(backupTreePath, addTree->childNodes[i]->file);
		strcpy(addTreePath, addPath);
		ConcatPath(addTreePath, addTree->childNodes[i]->file);
		if(RecoverBackupByFileTree(backupTreePath, addTreePath, backupTree, addTree->childNodes[i], hashMode) == -1)
			return -1;
	}
	return 0;
}