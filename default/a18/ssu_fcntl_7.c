#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>

int main(void)
{
    char *filename = "ssu_test.txt";
    int fd1, fd2;
    int flag;

    //ssu_test.txt. 파일을 오픈한다.
    if ((fd1 = open(filename, O_RDWR | O_APPEND, 0644)) < 0) {
        fprintf(stderr, "open error for %s\n", filename);
        exit(1);
    }

    //exec할 때 상속되지 않게 한다.
    if (fcntl(fd1, F_SETFD, FD_CLOEXEC) == -1) {
        fprintf(stderr, "fcntl F_SETFD error\n");
        exit(1);
    }

    //속성및 플래그를 가져온다.
    if ((flag = fcntl(fd1, F_GETFL, 0)) == -1) {
        fprintf(stderr, "fcntl F_GETFL error\n");
        exit(1);
    }

    //O_APPEND가 있는지 체크
    if (flag & O_APPEND)
        printf("fd1 : O_APPEND flag is set.\n");
    else
        printf("fd1 : O_APPEND flag is NOT set.\n");

    //속성및 플래그를 가져온다.
    if ((flag = fcntl(fd1, F_GETFD, 0)) == -1) {
        fprintf(stderr, "fcntl F_GETFD error\n");
        exit(1);
    }

    //FD_CLOEXEC가 있는지 체크
    //있음
    if (flag & FD_CLOEXEC)
        printf("fd1 : FD_CLOEXEC flag is set.\n");
    else
        printf("fd1 : FD_CLOEXEC flag is NOT set.\n");
    
    //0이상 할당받을 수 있는 수들 중 가장 큰 수인 4 할당
    if ((fd2 = fcntl(fd1, F_DUPFD, 0)) == -1) {
        fprintf(stderr, "fcntl F_DUPFD error\n");
        exit(1);
    }

    if ((flag = fcntl(fd2, F_GETFL, 0)) == -1) {
        fprintf(stderr, "fcntl F_GETFL error\n");
        exit(1);
    }

    //O_APPEND가 있는지 체크
    if (flag & O_APPEND)
        printf("fd2 : O_APPEND flag is set.\n");
    else
        printf("fd2 : O_APPEND flag is NOT set.\n");

    //속성및 플래그를 가져온다.
    if ((flag = fcntl(fd2, F_GETFD, 0)) == -1) {
        fprintf(stderr, "fcntl F_GETFD error\n");
        exit(1);
    }

    //FD_CLOEXEC가 있는지 체크
    //없음
    if (flag & FD_CLOEXEC)
        printf("fd2 : FD_CLOEXEC flag is set.\n");
    else
        printf("fd2 : FD_CLOEXEC flag is NOT set.\n");

    exit(0);
}