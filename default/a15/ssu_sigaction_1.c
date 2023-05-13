#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>

void ssu_signal_handler(int signo) {
    printf("ssu_signal_handler control\n");
}

int main(void) {
    struct sigaction sig_act;
    sigset_t sig_set;

    //sigaction의 마스크를 0으로 만듦
    sigemptyset(&sig_act.sa_mask);
    //시그널 옵션은 디폴트로 0으로 씀
    sig_act.sa_flags = 0;
    //적용될 핸들러
    sig_act.sa_handler  = ssu_signal_handler;
    //위의 정보를 SIGUSR1에 등록
    sigaction(SIGUSR1, &sig_act, NULL);
    printf("before first kill()\n");
    //SIGUSR1시그널을 자신에게 전송. 핸들러가 실행
    kill(getpid(), SIGUSR1);
    //sig_set을 0으로 채움
    sigemptyset(&sig_set);
    //SIGUSR1만 비트를 세팅
    sigaddset(&sig_set, SIGUSR1);
    //해당 비트 마스킹
    sigprocmask(SIG_SETMASK, &sig_set, NULL);
    printf("before second kill()\n");
    //시그널이 차단됨
    kill(getpid(), SIGUSR1);
    printf("after second kill()\n");
    exit(0);
}