#include <unistd.h>
#include <fcntl.h>
#include <stdlib.h>
#include <stdio.h>

int main(void)
{
    char buffer[1024];
    int fd;
    int len;
    fd = open("test.txt", O_RDONLY, 0644);
    if(fd == -1){
        puts("read failed");
        exit(1);
    }

    while(1){
        if((len = read(fd, buffer, 1024)) > 0){
            buffer[len] = 0;
            puts(buffer);
        }
    }
    exit(0);
}