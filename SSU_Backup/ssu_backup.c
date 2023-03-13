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
	int endFlag;
	char path_buf[SSU_BACKUP_MAX_PATH_SZ];
	char shell_buf[SSU_BACKUP_SHELL_MAX_BUF_SZ];

	//Comment: 인자가 2개가 아니거나 인자값이 잘못된 경우 Usage 출력
	if(argc != 2 || (selectHash = GetSelectHash(argv[1])) == -1){
		backup_usage();
		exit(1);
	}

	//Comment: 백업 경로를 구하고, 그 경로의 디렉토리가 없을경우 생성
	GetBackupPath(path_buf);
	if(access(path_buf, F_OK) == -1){
		if(SetBackupPath(path_buf) == -1){
			perror("SetBackupPath()");
			exit(1);
		}
	}
	if(access(path_buf, R_OK | W_OK) == -1){
		perror("access()");
		exit(1);
	}
	
	//Comment: 선택한 해쉬 방식을 백업경로의 설정파일에 저장
	if(SetHashMode(selectHash) == -1){
		perror("SetHashMode()");
		exit(1);
	}
	
	endFlag = 0;
	while(!endFlag)
	{
		ShowShell();

		fgets(shell_buf, SSU_BACKUP_SHELL_MAX_BUF_SZ, stdin);
		shell_buf[strlen(shell_buf) - 1] = '\0';

		endFlag = execute_cmd(shell_buf);
	}

	return 0;
}

int execute_cmd(char* cmd)
{
	int cmd_fd;
	char* shell_tok;

	if(!strcmp(cmd, "")){
		return 0;
	}

	shell_tok = strtok(cmd, " ");
	if(!strcmp(shell_tok, SSU_BACKUP_ADD)){
		strtok(NULL, " ");
		return 0;
	} else if(!strcmp(shell_tok, SSU_BACKUP_EXIT)){
		return 1;
	}
	
	//Todo: help넣기
	puts("헬프 넣기");
	return 0;
}

void fork_exec_cmd(const char* p_name, const char* cmd)
{
	pid_t pid;

	pid = fork();
	switch(pid){
		//Comment: 프로세스 복제에 실패했을 경우
		case -1:
			perror("fork()");
			exit(1);
			break;
		//Comment: fork()로 복제된 프로세스일 경우
		case 0:
			execl(p_name, cmd);
			break;
		//Comment: fork()를 호출한 프로세스일경우
		default:
			//Comment: 상태정보에 따른 처리는 하지 않아 wsatus = NULL
			if(waitpid(pid, NULL, 0) == -1){
				perror("waitpid()");
				exit(1);
			}
			break;
	}
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
