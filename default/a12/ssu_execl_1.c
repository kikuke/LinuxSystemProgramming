#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int main(void)
{
    printf("this is the original program\n");
    //테스트 프로그램을 실행
    execl("./ssu_execl_test_1", "ssu_execl_test_1", "param1", "param2", "param3", (char *)0);
    //execl로 인해 아래 문장은 출력 안됨.
    printf("%s\n", "this line should never get printed\n");
    exit(0);
}