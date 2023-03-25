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
#include "ssu_backup_add.h"

int main(int argc, char* argv[])
{
	int hashMode = -1;
	int addType = SSU_BACKUP_TYPE_REG;
	char destPath[SSU_BACKUP_MAX_PATH_SZ];
	char addPath[SSU_BACKUP_MAX_PATH_SZ];
	char pathBuf[SSU_BACKUP_MAX_PATH_SZ];
	char opt;
	struct filetree* backupTree;
	struct filetree* addTree;

	if(argc < 2 || argc > 3){
		Usage(USAGEIDX_ADD);
		exit(1);
	}

	strcpy(pathBuf, argv[1]);
	while((opt = getopt(argc, argv, "d")) != -1){
		switch(opt){
			case 'd':
				addType = SSU_BACKUP_TYPE_DIR;
				break;
			
			default:
				Usage(USAGEIDX_ADD);
				exit(1);
				break;
		}
	}

	//Comment: add 경로 값에 따른 에러 핸들링을 합니다.
	if(RealpathAndHandle(pathBuf, addPath, USAGEIDX_ADD) == NULL){
		exit(1);
	}

	if(CheckBackupCondition(addPath, addType) == -1){
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
	if((addTree = PathToFileTree(addPath, hashMode)) == NULL){
		perror("PathToFileTree()");
		exit(1);
	}

	strcpy(destPath, addPath);
	SourcePathToBackupPath(destPath);
	//Comment: destPath와 addPath는 부모디렉토리가 아닌 백업하려는 파일의 경로가 들어가게됨.
	//	백업 경로가 홈디렉토리도 허용됨.
	if(AddBackupByFileTree(destPath, addPath, backupTree, addTree, hashMode) == -1){
		perror("AddBackupByFileTree()");
		exit(1);
	}

	exit(0);
}

int CheckBackupCondition(const char* path, int addType)
{
	char* homeDir = getenv("HOME");
	int checkType;
	char temp_path[SSU_BACKUP_MAX_PATH_SZ];

	if(strncmp(homeDir, path, strlen(homeDir)) != 0){
		fprintf(stdout, "<%s> can't be backuped\n", path);
		return -1;
	}

	GetBackupPath(temp_path);
	if(strncmp(temp_path, path, strlen(temp_path)) == 0){
		fprintf(stdout, "<%s> can't be backuped\n", path);
		return -1;
	}

	if((checkType = CheckFileTypeByPath(path)) == SSU_BACKUP_TYPE_ERROR){
		perror("CheckFileTypeByPath()");
		return -1;
	}
	if(checkType == SSU_BACKUP_TYPE_OTHER){
		fputs("일반 파일이나 디렉토리가 아닙니다.", stderr);
		return -1;
	}
	if((checkType == SSU_BACKUP_TYPE_DIR) && checkType != addType){
		fprintf(stderr, "\"%s\" is a directory file\n", path);
		return -1;
	}

	return 0;
}

int AddBackupByFileTree(const char* backupPath, const char* addPath, struct filetree* backupTree, struct filetree* addTree, int hashMode)
{
	int retVal;
	struct filetree* matchedTree;
	char checkBackupPath[SSU_BACKUP_MAX_PATH_SZ];
	char backupTreePath[SSU_BACKUP_MAX_PATH_SZ];
	char addTreePath[SSU_BACKUP_MAX_PATH_SZ];

	GetBackupPath(checkBackupPath);
	//Comment: 백업 패스인 경우 제외
	if(strncmp(checkBackupPath, addPath, strlen(checkBackupPath)) == 0)
		return 0;

	strcpy(backupTreePath, backupPath);
	ExtractHomePath(backupTreePath);
	//Comment: 일치하는 백업파일이 없는 경우 해당 하위 파일 모두 생성
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
		if(AddBackupByFileTree(backupTreePath, addTreePath, backupTree, addTree->childNodes[i], hashMode) == -1)
			return -1;
	}
	return 0;
}
