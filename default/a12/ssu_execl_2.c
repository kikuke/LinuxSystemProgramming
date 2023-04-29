#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int main(void)
{
    if (fork() == 0) {
        execl("/bin/echo", "echo", "this is", "message one", (char *)0);
        //echo 프로그램들로 전환. 전환된 곳 밑으로는 출력되지 않는다.
        fprintf(stderr, "exec error\n");
        exit(1);
    }

    if (fork() == 0) {
        execl("/bin/echo", "echo", "this is", "message two", (char *)0);
        fprintf(stderr, "exec error\n");
        exit(1);
    }

    if (fork() == 0) {
        execl("/bin/echo", "echo", "this is", "message three", (char *)0);
        fprintf(stderr, "exec error\n");
        exit(1);
    }

    //이 글자가 먼저 출력됨.
    printf("Parent program ending\n");
    exit(0);
}