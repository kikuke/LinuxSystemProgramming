#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>

void ssu_echo_exit(int status);

int main(void)
{
    pid_t pid;
    int status;

    if ((pid = fork()) < 0) {
        fprintf(stderr, "fork error\n");
        exit(1);
    }
    //정상종료 7값 리턴
    else if (pid == 0)
        exit(7);

    if (wait(&status) != pid) {
        fprintf(stderr, "wait error\n");
        exit(1);
    }
    
    //정상종료 7값이 출력됨
    ssu_echo_exit(status);

    if ((pid = fork()) < 0) {
        fprintf(stderr, "fork error\n");
        exit(1);
    }
    //SIGABRT 시그널 발생시키며 종료
    else if (pid == 0)
        abort();
    
    if (wait(&status) != pid) {
        fprintf(stderr, "wait error\n");
        exit(1);
    }

    //SIGABRT값을 리턴하며 시그널 종료를 알림
    ssu_echo_exit(status);

    if ((pid = fork()) < 0) {
        fprintf(stderr, "fork error\n");
        exit(1);
    }
    //divide 0로 시그널 발생하며 종료
    else if (pid == 0)
        status /= 0;
    
    if(wait(&status) != pid) {
        fprintf(stderr, "wait error\n");
        exit(1);
    }

    //divide 0 시그널 값 출력
    ssu_echo_exit(status);
    exit(0);
}

void ssu_echo_exit(int status) {
    //정상 종료시 종료값 출력
    if (WIFEXITED(status))
        printf("normal termination, exit status = %d\n",
            WEXITSTATUS(status));
    //시그널을 받아 종료했을 경우 시그널 값 출력
    else if (WIFSIGNALED(status))
        printf("abnormal termination, signal number = %d%s\n",
            WTERMSIG(status),
    //coredump 설정이 있을 경우, 생성됏을 경우 출력
#ifdef WCOREDUMP
            WCOREDUMP(status) ? " (core file generated)" : "");
#else
            "");
#endif
    //중단됐을 경우 중단시킨 시그널 번호를 리턴
    else if (WIFSTOPPED(status))
        printf("child stopped, signal number = %d\n", WSTOPSIG(status));
}