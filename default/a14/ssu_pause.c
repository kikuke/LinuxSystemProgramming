#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>

void ssu_alarm(int signo);

int main(void)
{
    printf("Alarm Setting\n");
    //알람 핸들러 설정
    signal(SIGALRM, ssu_alarm);
    //2초후 알람
    alarm(2);

    while (1) {
        printf("done\n");
        //아래 두개로 인해 sleep과 비슷한 효과를 가짐.
        pause();
        alarm(2);
    }

    exit(0);
}

void ssu_alarm(int signo) {
    printf("alarm..!!!\n");
}