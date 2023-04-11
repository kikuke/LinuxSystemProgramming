#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#define BUFFER_SIZE 1024

int main(void)
{
    char buf[BUFFER_SIZE];

    //Comment: 표준 출력의 버퍼를 buf로 지정함. 크기는 지정과 관계없이 BUFSIZ로 간주함
    //  풀버퍼가 됨. 따라서 기다렸다 한번에 쭉 출력됨. 아래의 버퍼가 NULL로 되기 전까지.
    setbuf(stdout, buf);
    printf("Hello, ");
    sleep(1);
    printf("OSLAB!!");
    sleep(1);
    printf("\n");
    sleep(1);

    //Comment: 버퍼가 0이 됨 위에서 풀버퍼로 인해 쌓였던 글자가 출력됨.
    //  아래 글자들은 버퍼를 사용하지 않기에 바로 출력됨.
    setbuf(stdout, NULL);
    printf("How");
    sleep(1);
    printf(" are");
    sleep(1);
    printf(" you?");
    sleep(1);
    printf("\n");
    exit(0);
}