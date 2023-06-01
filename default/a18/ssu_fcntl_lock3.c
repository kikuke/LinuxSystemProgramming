#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>

#define NAMESIZE 50

struct employee{
    char name[NAMESIZE];
    int salary;
    int pid;
};

int main(int argc, char *argv[])
{
    struct flock lock;
    struct employee record;
    int fd, recnum, pid;
    long position;

    //파일 오픈
    if((fd = open(argv[1], O_RDWR)) == -1) {
        perror(argv[1]);
        exit(1);
    }

    pid = getpid();
    for(;;){
        printf("\nEnter record number: ");
        scanf("%d", &recnum);
        //0보다 작으면 나가기
        if(recnum < 0)
            break;
        //랜덤 액세스준비
        position = recnum * sizeof(record);
        //읽으려는 부분을 레코드 락
        lock.l_type = F_WRLCK;
        lock.l_whence = 0;//SEEK_SET
        lock.l_start = position;//읽으려는 시작 위치
        lock.l_len = sizeof(record);//락 범위
        //읽으려는 부분만 락을 검. 기다림.
        if(fcntl(fd, F_SETLKW, &lock) == -1) {
            perror(argv[1]);
            exit(2);
        }
        //해당 위치로 lseek.
        lseek(fd, position, 0);
        //해당 위치에 데이터가 없다면 락해제
        //락은 파일 시작위치 이전 영역이 아니라면 가능하므로
        if(read(fd, (char*)&record, sizeof(record)) == 0){
            printf("record %d not found\n", recnum);
            lock.l_type = F_UNLCK;
            fcntl(fd, F_SETLK, &lock);
            continue;
        }
        printf("Employee: %s, salary: %d\n", record.name, record.salary);
        record.pid = pid;
        //레코드 덮어쓰기
        printf("Enter new salary: ");
        scanf("%d", &record.salary);
        //락 걸었던 시작부분으로 이동
        lseek(fd, position, 0);
        write(fd, (char*)&record, sizeof(record));

        //언락
        lock.l_type = F_UNLCK;
        fcntl(fd, F_SETLK, &lock);
    }
    close(fd);
}