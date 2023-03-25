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

	//Todo: 이거 쓰기
	int RecoverBackupByFileTree(const char* backupPath, const char* recoverPath, struct filetree* backupTree, struct filetree* recoverTree, int hashMode)

	exit(0);
}

int RecoverBackupByFileTree(const char* backupPath, const char* recoverPath, struct filetree* backupTree, struct filetree* recoverTree, int hashMode)
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
				if((retVal = CompareHashByPath(backupTreePath, addPath, hashMode)) == -1)
					return -1;

				if(retVal == 1){
					fprintf(stdout, "\"%s\" is already backuped\n", backupTreePath);
					return 0;
				}
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

int RecoverFileSelector(const char* parentPath, const char* destPath, const struct filetree* backupTree, const struct filetree** matchedTrees, int listNum, int hashMode)
{
	int sellect;
	char c;
	struct filetree* pTree;
	char recoverPath[SSU_BACKUP_MAX_PATH_SZ];

	sellect = -1;
	while(sellect < 0 || sellect > listNum){
		printf("backup file list of \"%s\"\n", destPath);
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
	return RecoverBackupByFileTree(recoverPath, destPath, backupTree, NULL, hashMode);
}

int RecoverFileByFileTree(const char* backupPath, const char* recoverPath, const struct filetree** recoverTrees, int listNum)
{
	char backupFilePath[SSU_BACKUP_MAX_PATH_SZ];
	char recoverFilePath[SSU_BACKUP_MAX_PATH_SZ];

	//Todo: 실제 생성 루틴
	//Comment: 1개 이상의 동일한 이름의 파일, 폴더가 있을 경우
	if(listNum > 1){
		GetParentPath(backupPath, backupFilePath);
		return RecoverFileSelector(backupFilePath, recoverPath, recoverTrees, listNum);
	}

	//Comment: 디렉토리인 경우와 1개의 파일인 경우

	strcpy(backupFilePath, backupPath);
	strcpy(recoverFilePath, recoverPath);
	if(addTree->childNodeNum == 0){
		if(isRecover){
			destFilePath[strlen(destFilePath) - SSU_BACKUP_FILE_META_LEN] = '\0';
			if(CopyFile(destFilePath, addFilePath) == -1){
				fprintf(stderr, "\"%s\" to \"%s\" CopyFile Failed! - %s\n", addFilePath, destFilePath, strerror(errno));
				return -1;
			}

			fprintf(stdout, "\"%s\" backup recover to \"%s\"\n", addFilePath, destFilePath);
			return 0;
		}

		if(GetNowTime(destFilePath + strlen(destFilePath)) == -1){
			perror("GetNowTime()");
			return -1;
		}
		if(CopyFile(destFilePath, addFilePath) == -1){
			fprintf(stderr, "\"%s\" to \"%s\" CopyFile Failed! - %s\n", addFilePath, destFilePath, strerror(errno));
			return -1;
		}

		fprintf(stdout, "\"%s\" backuped\n", destFilePath);
		return 0;
	}

	//Comment: 폴더 만들고 재귀 호출
	if(MakeDirPath(destFilePath) == -1){
		perror("MakeDirPath()");
		return -1;
	}
	for(int i=0; i < addTree->childNodeNum; i++){
		strcpy(destFilePath, destPath);
		ConcatPath(destFilePath, addTree->childNodes[i]->file);
		strcpy(addFilePath, addPath);
		ConcatPath(addFilePath, addTree->childNodes[i]->file);
		if(CreateFileByFileTree(destFilePath, addFilePath, addTree->childNodes[i], isRecover) == -1)
			return -1;
	}

	return 0;
}