#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>

int main(void)
{
    sigset_t pendingset;
    sigset_t sig_set;
    int count = 0;

    //전부 1로 채움
    sigfillset(&sig_set);
    //전부 시그널 마스킹 시킴
    sigprocmask(SIG_SETMASK, &sig_set, NULL);

    while(1) {
        printf("count: %d\n", count++);
        sleep(1);

        //펜딩된 시그널들을 가져옴
        if (sigpending(&pendingset) == 0) {
            //해당 시그널이 펜딩됐는지 체크
            if (sigismember(&pendingset, SIGINT)) {
                printf("SIGINT가 블록되어 대기 중. 무한 루프를 종료\n");
                break;
            }
        }
    }

    exit(0);
}