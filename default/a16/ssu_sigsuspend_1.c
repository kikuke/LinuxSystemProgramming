#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>

int main(void)
{
    sigset_t old_set;
    sigset_t sig_set;

    //0으로 채움
    sigemptyset(&sig_set);
    //SIGINT 시그널 비트만 세팅함
    sigaddset(&sig_set, SIGINT);
    //세팅 비트 반영하며 old_set에 이전 마스크 가져옴
    sigprocmask(SIG_BLOCK, &sig_set, &old_set);
    //이전 마스크에는 SIGINT 마스크가 안되어있으므로 시그널 처리됨.
    sigsuspend(&old_set);
    exit(0);
}