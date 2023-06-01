#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <fcntl.h>

int main(int argc, char *argv[])
{
    struct flock lock;
    int fd;
    char command[100];

    if((fd = open(argv[1], O_RDWR)) == -1) {
        perror(argv[1]);
        exit(1);
    }
    //쓰기락 파일 전체
    lock.l_type = F_WRLCK;
    lock.l_whence = 0; //SEEK_SET
    lock.l_start = 0l;
    lock.l_len = 0l;
    //다른 프로세스가 파일 점유중이라면 종료
    if(fcntl(fd, F_SETFL, &lock) == -1) {
        if (errno == EACCES) {
            printf("%s busy -- try later\n", argv[1]);
            exit(2);
        }
        perror(argv[1]);
        exit(3);
    }
    //입력받은 인자를 vim 으로 열기
    //fork exec이므로 락은 상속되지 않음
    sprintf(command, "vim %s\n", argv[1]);
    system(command);
    lock.l_type = F_UNLCK;
    fcntl(fd, F_SETLK, &lock);
    close(fd);

    return 0;
}