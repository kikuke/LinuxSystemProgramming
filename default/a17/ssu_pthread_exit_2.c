#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <pthread.h>

//5초후 출력
void *ssu_thread(void *arg);

int main(void)
{
    pthread_t tid;

    if (pthread_create(&tid, NULL, ssu_thread, NULL) != 0) {
        fprintf(stderr, "pthread_create error\n");
        exit(1);
    }

    //메인이 먼저 끝나서 스레드의 뒤에 문장 출력 x
    sleep(1);
    printf("쓰레드가 완료되기전 main 함수가 먼저 종료되면 실행중 쓰레드 소멸\n");
    printf("메인 종료\n");
    exit(1);
}

void *ssu_thread(void *arg) {
    printf("쓰레드 시작\n");
    //순서 보장 못함
    sleep(5);
    printf("쓰레드 수행 완료\n");
    pthread_exit(NULL);
    return NULL;
}