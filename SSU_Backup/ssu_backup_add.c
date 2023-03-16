#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>

#include "ssu_backup_define.h"
#include "ssu_backup_util.h"
#include "ssu_backup_usage.h"
#include "ssu_backup_add.h"

int main(int argc, char* argv[])
{
	int hashMode = -1;
	int addType = SSU_BACKUP_TYPE_REG;
	char pathBuf[SSU_BACKUP_MAX_PATH_SZ];
	char addPath[SSU_BACKUP_MAX_PATH_SZ];
	char opt;
	int checkType;
	struct filetree* backupTree;
	struct filetree* addTree;

	if(argc < 2 || argc > 3){
		Usage(USAGEIDX_ADD);
		exit(1);
	}

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
	if(GetRealpathAndHandle(argv[1], addPath, USAGEIDX_ADD) == NULL){
		exit(1);
	}

	if((checkType = CheckFileTypeByPath(addPath)) == -1){
		perror("CheckFileTypeByPath()");
		exit(1);
	}
	if((checkType == SSU_BACKUP_TYPE_DIR) && checkType != addType){
		fprintf(stderr, "\"%s\" is a directory file\n", addPath);
		exit(1);
	}

	if((hashMode = GetHashMode()) == -1){
		fputs("GetHashMode() Failed!\n", stderr);
		exit(1);
	}

	if((backupTree = PathToFileTree(GetBackupPath(pathBuf), hashMode)) == NULL){
		perror("PathToFileTree()");
		exit(1);
	}
	if((addTree = PathToFileTree(addPath, hashMode)) == NULL){
		perror("PathToFileTree()");
		exit(1);
	}

	if(AddBackupByFileTree(GetParentPath(addPath, pathBuf), backupTree, addTree, hashMode) == -1){
		perror("AddBackupByFileTree()");
		exit(1);
	}

	return 0;
}

int AddBackupByFileTree(const char* addPath, const struct filetree* backupTree, const struct filetree* addTree, int hashMode)
{
	struct filetree* matchedTree;
	char addTreePath[SSU_BACKUP_MAX_PATH_SZ];

	strcpy(addTreePath, addPath);
	ConcatPath(addTreePath, addTree->file);
	matchedTree = FindFileTreeInPath(addTreePath, backupTree, 1);
	//Comment: 일치하는 백업파일이 없는 경우 해당 하위 파일 모두 생성
	if(matchedTree == NULL)
		return CreateFileByFileTree(addPath, addTree, 0);

	//Comment: 해시값이 같은 파일이 있는지 검사후 없으면 생성
	if(addTree->childNodeNum == 0){
		struct filetree* pTree = matchedTree->parentNode;
		for(int i=0; i < pTree->childNodeNum; i++){
			if(CompareHash(addTree->hash, pTree->childNodes[i]->hash, hashMode)){
				fprintf(stdout, "\"%s\" is already backuped\n", addTreePath);
				return 0;
			}
		}

		return CreateFileByFileTree(addPath, addTree, 0);
	}

	//Comment: 폴더의 경우 재귀 호출
	for(int i=0; i < addTree->childNodeNum; i++)
		if(AddBackupByFileTree(addTreePath, backupTree, addTree->childNodes[i], hashMode) == -1)
			return -1;
	return 0;
}
