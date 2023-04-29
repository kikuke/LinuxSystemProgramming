#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <sys/resource.h>
#include <sys/wait.h>

double ssu_maketime(struct timeval *time);

void term_stat(int stat);

void ssu_print_child_info(int stat, struct rusage *rusage);

int main(void)
{
    struct rusage rusage;
    pid_t pid;
    int status;

    //find함수 자식에서 실행
    if ((pid = fork()) == 0) {
        char *args[] = {"find", "/", "-maxdepth", "4", "-name", "stdio.h", NULL};

        if (execv("/usr/bin/find", args) < 0) {
            fprintf(stderr, "execv error\n");
            exit(1);
        }
    }

    //자식에 대한 종료상태 및 정보들 출력
    if (wait3(&status, 0, &rusage) == pid)
        ssu_print_child_info(status, &rusage);
    else {
        fprintf(stderr, "wait3 error\n");
        exit(1);
    }

    exit(0);
}

double ssu_maketime(struct timeval *time) {
    //timeval값을 double로 바꿔 리턴
    return ((double)time -> tv_sec + (double)time -> tv_usec/1000000.0);
}

void term_stat(int stat) {
    //정상 종료시 종료값 출력
    if (WIFEXITED(stat))
        printf("normal terminated. exit status = %d\n",
            WEXITSTATUS(stat));
    //시그널을 받아 종료했을 경우 시그널 값 출력
    else if (WIFSIGNALED(stat))
        printf("abnormal termination by signal %d. %s\n", WTERMSIG(stat),
    //coredump 설정이 있을 경우, 생성됏을 경우 출력
#ifdef WCOREDUMP
            WCOREDUMP(stat) ? "core dumped" : "no core");
#else
            NULL);
#endif
    //중단됐을 경우 중단시킨 시그널 번호를 리턴
    else if (WIFSTOPPED(stat))
        printf("stopped by signal %d\n", WSTOPSIG(stat));
}

void ssu_print_child_info(int stat, struct rusage *rusage) {
    printf("Termination info follows\n");
    //스탯 정보 출력
    term_stat(stat);
    //io시간 포함 사용시간 출력
    printf("user CPU time : %.2f(sec)\n",
    ssu_maketime(&rusage->ru_utime));
    //cpu 점유 시간 출력
    printf("system CPU time : %.2f(sec)\n",
    ssu_maketime(&rusage->ru_stime));
}