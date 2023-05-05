#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

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
        //Todo: 함수 인자 넣기
        virtual_exec()
    } else if(!strcmp(*argv, SSU_MONITOR_DELETE)){
        //Todo: 함수 인자 넣기
    } else if(!strcmp(*argv, SSU_MONITOR_TREE)){
        //Todo: 함수 인자 넣기
    } else if(!strcmp(*argv, SSU_MONITOR_EXIT)){
        return 1;
    } else {
        //Todo: 도움말 출력
    }

    //Todo: wait로 함수 실행 대기하기
    
    //개별 포인터는 cmd의 주소이기 때문에 투포인터만 동작할당 해제
    free(argv);
    return 0;
}

int virtual_exec(int (*exec_proc)(int argc, char *argv[]), int argc, char *argv[])
{
    pid_t pid = 0;
    int stat_loc = 0;
    int ret = 0;

    //자식 프로세스인 경우
    if((pid = fork()) == 0) {
        ret = exec_proc(argc, argv);
        //보통 여기까지 실행될 일 없음. exec_proc에서 exit로 종료하므로.
        //메인에서 exit 대신 return을 사용했을 경우 이걸로 exit됨.
        exit(ret);
    } else if(pid < 0) {
        perror("fork()");
        return -1;
    }

    //부모 프로세스인 경우
    if(waitpid(pid, &stat_loc, 0) < 0){
        perror("waitpid()");
        return -1;
    }

    //stat_loc 값 체크
    return check_statloc(stat_loc);
}

int check_statloc(int stat_loc)
{
    int ret = 0;

    if(WIFEXITED(stat_loc)) {
        if((ret = WEXITSTATUS(stat_loc)) == 0)
            return 0;
        
        fprintf(stderr, "return status value is %d\n", ret);
        return -1;
    } else if(WIFSIGNALED(stat_loc)) {
        fprintf(stderr, "get signal is %d\n", WTERMSIG(stat_loc));
        return -1;
    } else if(WIFSTOPPED(stat_loc)) {
        fprintf(stderr, "stop signal is %d\n", WSTOPSIG(stat_loc));
        return -1;
    }

    perror("check_statloc()");
    return -1;
}

void ShowShell()
{
	printf("%s ", SSU_MONITOR_SHELL_NAME);
}