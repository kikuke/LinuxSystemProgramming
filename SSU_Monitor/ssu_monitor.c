#include <stdio.h>

#include "ssu_monitor_system.h"
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
    int argc = 0;
    char **argv = NULL;

    if(cmd == NULL){
        fprintf(stderr, "cmd is NULL\n");
        return -1;
    }

    if(isBlank(cmd))
        return 0;

    if((argc = StringToArgv(cmd, &argv, " ")) < 1){
        fprintf(stderr, "StringToArgv Error\n");
        return -1;
    }

    //개별 포인터는 cmd의 주소이기 때문에 투포인터만 동작할당 해제
    free(argv);
}

void ShowShell()
{
	printf("%s ", SSU_MONITOR_SHELL_NAME);
}