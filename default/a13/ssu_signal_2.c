#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>

//해당 소스에서만 활용하는 함수
//  메시지 출력 후 프로세스를 종료한다
static void ssu_signal_handler(int signo);

int main(void)
{
    //시그널 핸들러 함수 지정에 오류 발생시
    if (signal(SIGINT, ssu_signal_handler) == SIG_ERR) {
        fprintf(stderr, "cannot handle SIGINT\n");
        exit(EXIT_FAILURE);
    }

    if (signal(SIGTERM, ssu_signal_handler) == SIG_ERR) {
        fprintf(stderr, "cannot handle SIGTERM\n");
        exit(EXIT_FAILURE);
    }

    //시그널 디폴트 함수 지정에 에러 발생시
    if (signal(SIGPROF, SIG_DFL) == SIG_ERR) {
        fprintf(stderr, "cannot handle SIGPROF\n");
        exit(EXIT_FAILURE);
    }

    //시그널 무시
    if (signal(SIGHUP, SIG_IGN) == SIG_ERR) {
        fprintf(stderr, "cannot handle SIGHUP\n");
        exit(EXIT_FAILURE);
    }

    while (1)
        pause();
    
    exit(0);
}

static void ssu_signal_handler(int signo) {
    if (signo == SIGINT)
        printf("caught SIGINT\n");
    else if (signo == SIGTERM)
        printf("caught SIGTERM\n");
    else {
        fprintf(stderr, "unexpected signal\n");
        exit(EXIT_FAILURE);
    }

    exit(EXIT_SUCCESS);
}