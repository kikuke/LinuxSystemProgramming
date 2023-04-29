#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int main(void)
{
    //아래의 프로그램을 실행
    char *argv[] = {
        "ssu_execl_test_1", "param1", "param2", (char *)0
    };

    printf("this is the original program\n");
    execv("./ssu_execl_test_1", argv);
    //이 쪽 줄은 위의 execv로 인해 출력되지 않음
    printf("%s\n", "This line should never get printed\n");
    exit(0);
}