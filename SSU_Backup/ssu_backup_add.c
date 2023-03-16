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
		fprintf(stderr, "<%s> is a directory file\n", addPath);
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

	return 0;
}

int AddBackupByFileTree(const char* addPath, const struct filetree* backupTree, const struct filetree* addTree, int hashMode)
{
	struct filetree* matchedTree;
	char pathBuf[SSU_BACKUP_MAX_PATH_SZ];

	strcpy(pathBuf, addPath);
	ConcatPath(pathBuf, addTree->file);
	matchedTree = FindFileTreeInPath(pathBuf, backupTree, 1);
	//Comment: 일치하는 백업파일이 없는 경우
	if(matchedTree == NULL){
		//Todo: 해당 하위 경로 파일 모두 생성하는 함수
	} else {
		if(addTree->childNodeNum == 0){
		//Todo: 부모로 올라가 해당 트리 자식 검색하며 해시값 같은지 비교
		//Todo: 파일 이름 비교함수, 파일경로 찾기 함수
		//	재귀 할땐 addTree의 경우 찾은값으로 업데이트해서 넣어주기
		} else {
			//Todo: 재귀 호출
		}
	}

	return 0;
}
