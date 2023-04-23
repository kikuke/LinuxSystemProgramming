#include <stdio.h>
#include <unistd.h>

int main(void)
{
    printf("Good afternoon?");
    //버퍼 비우는 루틴이 실행되지 않아 출력 안됨.
    _exit(0);
}