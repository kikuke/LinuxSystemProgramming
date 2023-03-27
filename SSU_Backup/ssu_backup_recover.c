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
	if((matchNum = FindAllFileTreeInPath(pathBuf, backupTree, &matchedTrees, 1)) < 1){
		Usage(USAGEIDX_RECOVER);
		exit(1);
	}

	//Comment: 같은 이름의 디렉토리와, 파일이 공존할수 있으니 검사
	for(int i=0; i < matchNum; i++){
		GetParentPath(backupPath, pathBuf);
		ConcatPath(pathBuf, matchedTrees[i]->file);
		checkType = CheckFileTypeByPath(pathBuf);

		strcpy(pathBuf, backupPath);
		BackupPathToSourcePath(pathBuf);
		if(CheckFileTypeCondition(pathBuf, recoverType, checkType) == -1){
			exit(1);
		}
	}

	if(RecoverEntry(backupPath, recoverPath, backupTree, matchedTrees, matchNum, hashMode) == -1){
		exit(1);
	}

	exit(0);
}

int RecoverEntry(const char* backupPath, const char* recoverPath, struct filetree* backupTree, struct filetree** matchedTrees, int matchNum, int hashMode)
{
	char pathBuf[SSU_BACKUP_MAX_PATH_SZ];

	//Comment: 검색된 일치 목록이 하나 이상일 경우. 동일한 이름의 디렉토리와 파일이 섞여있을 경우 포함.
	if(matchNum > 1){
		GetParentPath(backupPath, pathBuf);
		if(RecoverFileSelector(pathBuf, recoverPath, backupTree, matchedTrees, matchNum, hashMode) == -1){
			fprintf(stderr, "\"%s\" FindAllFileTreeInPath() Failed! - %s\n", backupPath, strerror(errno));
			return -1;
		}

		return 0;
	}
	
	//Comment: 검색된 일치 목록이 하나일 경우
	if(RecoverFileByFileTree(backupPath, recoverPath, backupTree, *matchedTrees, hashMode) == -1){
		perror("RecoverFileByFileTree()");
		return -1;
	}

	return 0;
}

int RecoverFileSelector(const char* parentPath, const char* destPath, struct filetree* backupTree, struct filetree** matchedTrees, int listNum, int hashMode)
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
		PrintFileTreeList(parentPath, (const struct filetree**)matchedTrees, listNum);
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

int RecoverFileByFileTree(const char* backupPath, const char* recoverPath, struct filetree* backupTree, struct filetree* recoverTree, int hashMode)
{
	int foldCnt, fileCnt;
	int retVal;

	//Comment: backupPath가 파일인 경우
	if(recoverTree->childNodeNum == 0){
		//Comment: 파일이 recoverPath에 존재할 경우 두 파일의 해시값 비교.
		if(CheckFileTypeByPath(recoverPath) != SSU_BACKUP_TYPE_ERROR){
			if((retVal = CompareHashByPath(backupPath, recoverPath, hashMode)) == -1)
				return -1;

			if(retVal == 1){
				fprintf(stdout, "\"%s\" is already backuped\n", recoverPath);
				return 0;
			}
		}

		if(CopyFile(recoverPath, backupPath) == -1){
			fprintf(stderr, "\"%s\" to \"%s\" CopyFile Failed! - %s\n", backupPath, recoverPath, strerror(errno));
			return -1;
		}

		//Comment: 복원된 백업 경로의 파일을 삭제
		if(RemoveBackupByFileTree(backupPath, recoverTree, &foldCnt, &fileCnt, 1) == -1){
			return -1;
		}

		fprintf(stdout, "\"%s\" backup recover to \"%s\"\n", backupPath, recoverPath);
		return 0;
	}

	//Comment: recoverTree가 디렉토리인 경우
	//	디렉토리 만들고 RecoverBackupByFileTree로 인자들을 넘긴다.
	if(MakeDirPath(recoverPath) == -1){
		perror("MakeDirPath()");
		return -1;
	}

	return RecoverBackupByFileTree(backupPath, recoverPath, backupTree, recoverTree, hashMode);
}

int RecoverBackupByFileTree(const char* pBackupPath, const char* pRecoverPath, struct filetree* backupTree, struct filetree* pRecoverTree, int hashMode)
{
	int nodeNum;
	int matchNum;
	struct filetree* nRecoverTree;
	struct filetree** recoverTrees;
	char nextBackupPath[SSU_BACKUP_MAX_PATH_SZ];
	char nextRecoverPath[SSU_BACKUP_MAX_PATH_SZ];
	char pathBuf[SSU_BACKUP_MAX_PATH_SZ];
	
	nodeNum = pRecoverTree->childNodeNum;
	//Comment: 하위 파일들 순회
	for(int i=0; i < nodeNum; i+=matchNum){
		nRecoverTree = pRecoverTree->childNodes[i];

		//Comment: 부모 경로로 변환
		strcpy(nextBackupPath, pBackupPath);
		strcpy(nextRecoverPath, pRecoverPath);

		//Comment: 실제 경로로 변환
		//	복원이 되면 recover트리가 하나씩 지워지므로 복원 수 만큼 idx를 감소시켜야한다.
		GetRealNameByFileTree(pathBuf, nRecoverTree);
		ConcatPath(nextBackupPath, pathBuf);
		ConcatPath(nextRecoverPath, pathBuf);

		//Comment: 검색을 위한 추출
		strcpy(pathBuf, nextBackupPath);
		ExtractHomePath(pathBuf);
		//Comment: 해당 경로에 대해 모든 일치하는 백업트리를 찾음.
		if((matchNum = FindAllFileTreeInPath(pathBuf, backupTree, &recoverTrees, 1)) < 1){
			fprintf(stderr, "\"%s\" FindAllFileTreeInPath() Failed! - %s\n", nextBackupPath, strerror(errno));
			return -1;
		}

		GetParentPath(nextBackupPath, pathBuf);
		strcpy(nextBackupPath, pathBuf);
		if(matchNum == 1){
			ConcatPath(nextBackupPath, (*recoverTrees)->file);
			if(RecoverFileByFileTree(nextBackupPath, nextRecoverPath, backupTree, nRecoverTree, hashMode) == -1){
				return -1;
			}
		} else {
			if(RecoverFileSelector(nextBackupPath, nextRecoverPath, backupTree, recoverTrees, matchNum, hashMode) == -1){
				perror("RecoverFileSelector()");
				return -1;
			}
		}
	}
	
	//Comment: 하위에서 지워지지 않은 경우 폴더 지우기
	if(pRecoverTree->childNodeNum == 0){
		if(access(pBackupPath, F_OK) == 0){
			if(rmdir(pBackupPath) == -1){
				fprintf(stderr, "\"%s\" rmdir Failed! - %s\n", pBackupPath, strerror(errno));
				return -1;
			}
		}
	}
	
	return 0;
}