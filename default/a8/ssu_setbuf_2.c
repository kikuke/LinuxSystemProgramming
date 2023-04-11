#include <stdio.h>
#include <stdlib.h>

#define BUFFER_SIZE 1024

int main(void)
{
    char buf[BUFFER_SIZE];
    int a, b;
    int i;

    //Comment: 표준 입력의 버퍼를 buf로 지정함. 크기는 지정과 관계없이 BUFSIZ로 간주함
    //  풀버퍼가 됨. 따라서 개행도 받아짐
    setbuf(stdin, buf);
    scanf("%d %d", &a, &b);

    //Comment: 개행 만나면 출력 종료
    for(i = 0; buf[i] != '\n'; i++)
        putchar(buf[i]);
    
    putchar('\n');
    exit(0);
}