#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>

int main(void)
{
    sigset_t sig_set;
    int count;

    //전부 0으로 만듦
    sigemptyset(&sig_set);
    //SIGINT만 값 설정
    sigaddset(&sig_set, SIGINT);
    //NULL과 sig_set의 합집합을 마스크에 씌웠으므로 SIGINT신호를 무시하게 됨,
    sigprocmask(SIG_BLOCK, &sig_set, NULL);

    //3초간 카운트
    for (count = 3 ; 0 < count ; count--) {
        printf("count %d\n", count);
        sleep(1);
    }

    printf("Ctrl-C에 대한 블록을 해제\n");
    //NULL 과의 교집합 이므로 모든 비트가 해제됨.
    //따라서 큐에 있던 SIGINT신호를 처리해서 디폴드 핸들러를 실행해 즉시 종료됨.
    sigprocmask(SIG_UNBLOCK, &sig_set, NULL);
    printf("count중 Ctrl-C입력하면 이 문장은 출력 되지 않음.\n");

    while(1);

    exit(0);
}