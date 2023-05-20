#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

#define THREAD_NUM 5

//들어온 인자값을 출력후 pthread_exit에 NULL을 전달하며 종료
void *ssu_printhello(void *arg);

int main(void)
{
    pthread_t tid[THREAD_NUM];
    int i;

    for (i=0; i < THREAD_NUM; i++) {
        printf("In main: creating thread %d\n", i);

        //스레드에 i 값 전달. 이때 i의 주소가 넘어가서 해당 값의 주소가 넘어가므로 출력이 의도치 않음
        if (pthread_create(&tid[i], NULL, ssu_printhello, (void *)&i) != 0) {
            fprintf(stderr, "pthread_create error\n");
            exit(1);
        }
    }

    pthread_exit(NULL);
    exit(0);
}

void *ssu_printhello(void *arg) {
    int thread_index;

    //스레드의 인자를 int로 판단
    thread_index = *((int *)arg);
    //인자 출력
    printf("Hello World! It's me, thread #%d!\n", thread_index);
    //join 하는 곳에 NULL 값 전달하며 스레드 종료
    pthread_exit(NULL);
    //이거 실행될 일 없음
    return NULL;
}