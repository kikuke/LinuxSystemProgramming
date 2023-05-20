#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <string.h>

//pid와 tid를 출력하는 함수
void *ssu_thread(void *arg);

int main(void)
{
    pthread_t tid;
    pid_t pid;

    //스레드 id를 tid에 받고 attr은 유저모드로, ssu_thread 함수를 스레드로 실행, 함수의 인자는 없음
    if (pthread_create(&tid, NULL, ssu_thread, NULL) != 0) {
        fprintf(stderr, "pthread_create error\n");
        exit(1);
    }

    //메인스레드의 pid와 tid를 출력
    pid = getpid();
    tid = pthread_self();
    printf("Main Thread: pid %u tid %u \n",
        (unsigned int)pid, (unsigned int)tid);
    sleep(1);
    exit(0);
}

void *ssu_thread(void *arg) {
    pthread_t tid;
    pid_t pid;

    pid = getpid();
    tid = pthread_self();
    printf("New Thread: pid %d tid %u \n", (int)pid, (unsigned int)tid);
    return NULL;
}