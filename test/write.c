#include <unistd.h>
#include <stdlib.h>
#include <fcntl.h>
#include <stdio.h>

int main(void)
{
    char buffer[1024];
    int fd;
    int len;
    fd = open("test.txt", O_WRONLY | O_CREAT | O_TRUNC, 0644);
    if(fd == -1){
        perror("Wrtie Failed");
        exit(1);
    }

    while(1){
        if((len = write(fd, "test", sizeof("test"))) < 0){
            puts("write failed");
        }
        puts("write");
        sleep(1);
    }
    exit(0);
}