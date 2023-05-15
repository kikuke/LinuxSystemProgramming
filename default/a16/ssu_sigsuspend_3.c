#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <time.h>

//단순 프린트
void ssu_signal_handler(int singno);
//시간 프린트찍음
void ssu_timestamp(char *str);

int main(void)
{
    struct sigaction sig_act;
    sigset_t blk_set;

    //전부 1로 세팅 후 SIGALRM만 제외
    sigfillset(&blk_set);
    sigdelset(&blk_set, SIGALRM);
    //SIGALRM의 핸들러 세팅
    sigemptyset(&sig_act.sa_mask);
    sig_act.sa_flags = 0;
    sig_act.sa_handler = ssu_signal_handler;
    sigaction(SIGALRM, &sig_act, NULL);
    //타이머전 시작 시간 찍기
    ssu_timestamp("before sigsuspend()");
    alarm(5);

    //알람 울리기 전까지 kill, stop 제외 전부 무시
    sigsuspend(&blk_set);
    ssu_timestamp("after sigsuspend()");
    exit(0);
}

void ssu_signal_handler(int signo) {
    printf("in ssu_signal_handler() function\n");
}

void ssu_timestamp(char *str) {
    time_t time_val;

    time(&time_val);
    printf("%s the time is %s\n", str, ctime(&time_val));
}