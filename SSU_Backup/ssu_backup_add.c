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
	char addPath[SSU_BACKUP_MAX_PATH_SZ];
	char opt;
	int checkType;

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
		fputs("Use -d option", stderr);
		exit(1);
	}

	if((hashMode = GetHashMode()) == -1){
		fputs("GetHashMode() Failed!", stderr);
		exit(1);
	}
}
