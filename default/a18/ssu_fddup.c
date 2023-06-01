#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <stdio.h>

int main(void)
{
    int testfd;
    int fd;

    //fd 3
    fd = open("test.txt", O_CREAT);

    //fd 5
    testfd = fcntl(fd, F_DUPFD, 5);
    printf("testfd :%d\n", testfd);
    //fd6
    testfd = fcntl(fd, F_DUPFD, 5);
    printf("testfd :%d\n", testfd);

    getchar();
}