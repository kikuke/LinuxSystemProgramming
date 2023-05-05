#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>

#include "ssu_monitor_add.h"
#include "ssu_monitor_usage.h"
#include "ssu_monitor_system.h"
#include "ssu_monitor_util.h"
#include "ssu_monitor.h"

int main(void)
{
    int endFlag = 0;
    char shell_buf[SSU_MONITOR_SHELL_BUF_SZ];

    while(!endFlag) {
        ShowShell();

        fgets(shell_buf, SSU_MONITOR_SHELL_BUF_SZ, stdin);
        shell_buf[strlen(shell_buf) - 1] = '\0';

        if((endFlag = execute_cmd(shell_buf)) < 0) {
            perror("execute_cmd()");
            exit(1);
        }
    }

    exit(0);
}

int execute_cmd(char *cmd)
{
    int (*exec_proc)(int argc, char *argv[]) = NULL;
    int argc = 0;
    char **argv = NULL;

    if(cmd == NULL){
        fprintf(stderr, "cmd is NULL\n");
        return -1;
    }

    //입력이 공백인 경우 다시 프롬프트 출력
    if(isBlank(cmd))
        return 0;

    //cmd 토큰화
    if((argc = StringToArgv(cmd, &argv, " ")) < 1){
        fprintf(stderr, "StringToArgv Error\n");
        return -1;
    }

    if(!strcmp(*argv, SSU_MONITOR_ADD)){
        virtual_system(add_daemon, argc, argv);
    } else if(!strcmp(*argv, SSU_MONITOR_DELETE)){
        //Todo: 함수 인자 넣기
        //Comment: 임시
        puts("delete");
    } else if(!strcmp(*argv, SSU_MONITOR_TREE)){
        //Todo: 함수 인자 넣기
        //Comment: 임시
        puts("tree");
    } else if(!strcmp(*argv, SSU_MONITOR_EXIT)){
        return 1;
    } else {
        Usage(USAGEIDX_MAX);
    }
    
    //개별 포인터는 cmd의 주소이기 때문에 투포인터만 동작할당 해제
    free(argv);
    return 0;
}

void ShowShell()
{
	printf("%s ", SSU_MONITOR_SHELL_NAME);
}