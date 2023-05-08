#include <stdio.h>
#include <stdlib.h>
#include <signal.h>

int main(void)
{
    sigset_t set;

    //전부 0으로 초기화
    sigemptyset(&set);
    //SIGINT 비트를 1로 만듦
    sigaddset(&set, SIGINT);

    //SIGINT 비트값 체크
    switch (sigismember(&set, SIGINT))
    {
        case 1 :
            printf("SIGINT is included. \n");
            break;
        case 0 :
            printf("SIGINT is not included. \n");
            break;
        default :
            printf("failed to call sigismember() \n");
    }

    //SIGSYS 비트값 체크
    switch (sigismember(&set, SIGSYS))
    {
        case 1 :
            printf("SIGSYS is included. \n");
            break;
        case 0 :
            printf("SIGSYS in not included. \n");
            break;
        default :
            printf("failed to call sigismember() \n");
    }

    exit(0);
}