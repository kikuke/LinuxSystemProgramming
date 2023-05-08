#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>

#define LINE_MAX 2048

static void ssu_alarm(int signo);

int main(void)
{
    char buf[LINE_MAX];
    int n;

    //알람 핸들러 등록
    if (signal(SIGALRM, ssu_alarm) == SIG_ERR) {
        fprintf(stderr, "SIGALRM error\n");
        exit(1);
    }
    //10초후 알람
    alarm(10);

    //입력 받기
    if ((n = read(STDIN_FILENO, buf, LINE_MAX)) < 0) {
        fprintf(stderr, "read() error\n");
        exit(1);
    }
    
    //0초후 알람으로 재설정. 시그널 전달 되기도 전에 write수행후 프로그램이 종료되어 핸들러가 실행되지 않음.
    alarm(0);
    write(STDOUT_FILENO, buf, n);
    exit(0);
}

static void ssu_alarm(int signo) {
    printf("ssu_alarm() called!\n");
}