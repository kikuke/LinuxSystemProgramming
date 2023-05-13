#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <setjmp.h>
#include <signal.h>

//jmp_buf1로 점프하며 리턴3
static void ssu_signal_handler1(int signo);
//jmp_buf2로 점프하며 리턴2
static void ssu_signal_handler2(int signo);

sigjmp_buf jmp_buf1;
sigjmp_buf jmp_buf2;

int main(void)
{
    struct sigaction act_sig1;
    struct sigaction act_sig2;
    int i, ret;

    printf("My PID is %d\n", getpid());
    //점프 1
    ret  = sigsetjmp(jmp_buf1, 1);

    //최초 실행시
    if (ret == 0) {
        act_sig1.sa_handler = ssu_signal_handler1;
        sigaction(SIGINT, &act_sig1, NULL);
    }
    //점프로 왔을 경우
    else if (ret == 3)
        printf("---------------\n");
    
    printf("Starting\n");
    //점프 2
    sigsetjmp(jmp_buf2, 2);
    act_sig2.sa_handler = ssu_signal_handler2;
    sigaction(SIGUSR1, &act_sig2, NULL);

    //초단위 출력
    for (i = 0; i < 20; i++) {
        printf("i = %d\n", i);
        sleep(1);
    }

    exit(0);
}

static void ssu_signal_handler1(int signo) {
    fprintf(stderr, "\nInterrupted\n");
    siglongjmp(jmp_buf1, 3);
}

static void ssu_signal_handler2(int signo) {
    fprintf(stderr, "\nSIGUSR1\n");
    siglongjmp(jmp_buf2, 2);
}