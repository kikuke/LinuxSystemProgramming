#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

#include "ssu_backup_define.h"
#include "ssu_backup_util.h"
#include "ssu_backup.h"

int main(int argc, char* argv[])
{
	int selectHash;
	char path_buf[SSU_BACKUP_MAX_PATH_SZ];
	char shell_buf[SSU_BACKUP_SHELL_MAX_BUF_SZ];

	//Comment: 인자가 2개가 아니거나 인자값이 잘못된 경우 Usage 출력
	if(argc != 2 || (selectHash = GetSelectHash(argv[1])) == -1){
		backup_usage();
		exit(1);
	}

	//Comment: 백업 경로를 구하고, 그 경로의 디렉토리가 없을경우 생성
	GetBackupPath(path_buf);
	if(access(path_buf, 0) == -1){
		if(SetBackupPath(path_buf) == -1){
			perror("SetBackupPath()");
			exit(1);
		}
	}
	
	//Comment: 선택한 해쉬 방식을 백업경로의 설정파일에 저장
	if(SetHashMode(selectHash) == -1){
		perror("SetHashMode()");
		exit(1);
	}

	while(1)
	{
		ShowShell();
		scanf("%s", shell_buf);
		execute_cmd(shell_buf);
	}

	return 0;
}

int execute_cmd(char* cmd)
{
	int cmd_fd;
	char* shell_tok;
	
	shell_tok = strtok(cmd, " ");
	if(!strcmp(shell_tok, "add")){
		strtok(NULL, " ");
		return 0;
	} else {
	}

	return -1;
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

void ShowShell()
{
	printf("%s⟩ ", SSU_BACKUP_SHELL_NAME);
}

void backup_usage()
{
	puts("Usage: ssu_backup ⟨md5 | sha1⟩");
}
