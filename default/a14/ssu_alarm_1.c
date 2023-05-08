#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>

void ssu_signal_handler(int signo);

int count = 0;

int main(void)
{
    //알람에 시그널 핸들러 등록
    signal(SIGALRM, ssu_signal_handler);
    //1초후 실행
    alarm(1);

    //무한루프
    while(1);

    exit(0);
}

//1초에 근사하게 증가된 시간이 찍힘
void ssu_signal_handler(int signo) {
    printf("alarm %d\n", count++);
    alarm(1);
}