#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>

//해당 signo에 대해 펜딩된 여부를 체크하는 함수
void ssu_check_pending(int signo, char *signame);
//핸들 용 프린트 함수
void ssu_signal_handler(int signo);

int main(void)
{
    struct sigaction sig_act;
    sigset_t sig_set;

    //sigaction 값 주입
    sigemptyset(&sig_act.sa_mask);
    sig_act.sa_flags = 0;
    sig_act.sa_handler = ssu_signal_handler;

    //SIGUSR1시그널에 대해 sigaction 전달
    if (sigaction(SIGUSR1, &sig_act, NULL) != 0) {
        fprintf(stderr, "sigaction() error\n");
        exit(1);
    }
    else {
        //SIGUSR1에만 비트가 설정된 sig_set
        sigemptyset(&sig_set);
        sigaddset(&sig_set, SIGUSR1);

        //SIGUSR1 비트 마스크
        if (sigprocmask(SIG_SETMASK, &sig_set, NULL) != 0) {
            fprintf(stderr, "sigprocmask() error\n");
            exit(1);
        }
        else {
            printf("SIGUSR1 signals are now blocked\n");
            //SIGUSR1 자신에게 전송
            kill(getpid(), SIGUSR1);
            printf("after kill()\n");
            //체크했을 때 처리되지 않아 펜딩됐으며 블록된 상태임
            ssu_check_pending(SIGUSR1, "SIGUSR1");
            //모든 비트 0 세팅
            sigemptyset(&sig_set);
            sigprocmask(SIG_SETMASK, &sig_set, NULL);
            printf("SIGUSR1 signals are no longer blocked\n");
            //모든 마스크를 해제했으므로 펜딩된 시그널이 처리되어 펜딩상태가 해제됨
            ssu_check_pending(SIGUSR1, "SIGUSR1");
        }
    }

    exit(0);
}

void ssu_check_pending(int signo, char * signame) {
    sigset_t sig_set;

    if (sigpending(&sig_set) != 0)
        printf("sigpending() error\n");
    else if (sigismember(&sig_set, signo))
        printf("a %s signal is pending\n", signame);
    else
        printf("%s signals are not pending\n", signame);
}

void ssu_signal_handler(int signo) {
    printf("in ssu_signal_handler function\n");
}