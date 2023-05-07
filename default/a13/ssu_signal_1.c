#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>

//핸들러
void ssu_signal_handler(int signo);

//함수 포인터
void (*ssu_func)(int);

int main(void)
{
    //이전에 있던 핸들러가 리턴됨. 디폴드 핸들러를 의미함.
    ssu_func = signal(SIGINT, ssu_signal_handler);

    while (1) {
        printf("process running...\n");
        sleep(1);
    }

    exit(0);
}

void ssu_signal_handler(int signo) {
    printf("SIGINT 시그널 발생.\n");
    printf("SIGINT를 SIG_DFL로 재설정 함.\n");
    //디폴트 핸들러가 설정됨.
    //한번더 SIGINT를 받으면 프로그램 종료됨.
    signal(SIGINT, ssu_func);
}