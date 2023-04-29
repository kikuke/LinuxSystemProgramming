#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int main(void)
{
    //실행 파일에 들어갈 인자들
    char *argv[] = {
        "ssu_execl_test_1", "param1", "param2", (char *)0
    };
    //실행 파일에 들어갈 환경 변수들
    char *env[] = {
        "NAME = value",
        "nextname=nextvalue",
        "HOME=/home/kikuke",
        (char *)0
    };

    printf("this is the original program\n");
    //실행
    execve("./ssu_execl_test_1", argv, env);
    //이밑으로는 실행되지 않음
    printf("%s\n", "This line should never get printed\n");
    exit(0);
}