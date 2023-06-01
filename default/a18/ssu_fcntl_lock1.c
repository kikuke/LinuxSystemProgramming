#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <fcntl.h>

#define NAMESIZE 50
#define MAXTRIES 5

struct employee{
    char name[NAMESIZE];
    int salary;
    int pid;
};

int main(int argc, char *argv[])
{
    struct flock lock;
    struct employee record;
    int fd, sum=0, try=0;

    sleep(10);

    //입력 받은 파일을 읽기전용으로 오픈
    if((fd = open(argv[1], O_RDONLY)) == -1) {
        perror(argv[1]);
        exit(1);
    }

    //readlock으로 파일 전체 락
    lock.l_type = F_RDLCK;
    lock.l_whence = 0;//SEEK_SET
    lock.l_start = 0L;
    lock.l_len = 0L;

    //setlk에 대한 busy wait
    while(fcntl(fd, F_SETLK, &lock) == -1) {
        //다른 프로세스가 WRLCK로 점유중일 경우
        if(errno == EACCES) {
            if(try++ < MAXTRIES) {
                sleep(1);
                continue;
            }
            printf("%s busy -- try later\n", argv[1]);
            exit(2);
        }
        perror(argv[1]);
        exit(3);
    }
    sum = 0;
    //해당 바이트 단위로 읽어들임
    while(read(fd, (char *)&record, sizeof(record)) > 0) {
        printf("Employee: %s, Salary: %d\n", record.name, record.salary);
        sum += record.salary;
    }
    printf("\nTotal salary: %d\n", sum);

    //언락
    lock.l_type = F_UNLCK;
    fcntl(fd, F_SETLK, &lock);
    close(fd);
}