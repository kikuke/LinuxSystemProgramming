#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

#include "ssu_backup_define.h"
#include "ssu_backup_util.h"
#include "ssu_backup.h"

int main(int argc, char* argv[])
{
	char path_buf[SSU_BACKUP_MAX_PATH_SZ];
	int selectHash;
	char opt;


	if(argc != 2 || (selectHash = GetSelectHash(argv[1])) == -1){
		backup_usage();
		exit(1);
	}
	if(GetBackupPath(path_buf) == NULL){
		if(errno == ENOENT){
			SetBackupPath(path_buf);
		} else {
			perror("GetBackupPath");
			exit(1);
		}
	}

	return 0;
}

int GetSelectHash(char* selectHash)
{
	if(!strcmp(selectHash, "md5")){
		return SSU_BACKUP_HASH_MD5;
	} else if(!strcmp(selectHash, "sha1")){
		return SSU_BACKUP_HASH_SHA1;
	}

	return -1;
}

void backup_usage()
{
	puts("Usage: ssu_backup <md5 | sha1>");
}
