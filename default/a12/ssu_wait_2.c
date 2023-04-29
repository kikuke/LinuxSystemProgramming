#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

#define EXIT_CODE 1

int main(void)
{
    pid_t pid;
    int ret_val, status;

    if ((pid = fork()) == 0) {
        //자신, 부모의pid 및 내보낼 exit값 출력
        printf("child: pid = %d ppid = %d exit_code = %d\n",
            getpid(), getppid(), EXIT_CODE);
        exit(EXIT_CODE);
    }

    //fork로 받은 자식의 pid값 출력
    printf("parent: waiting for child = %d\n", pid);
    //ret_val은 자식의 pid값, status에 0000 0001/0000 0000값이 저장되어있으므로 100출력 및 8비트 이동후 값 출력은 1
    ret_val = wait(&status);
    printf("parent: return value = %d, ", ret_val);
    printf(" child's status = %x", status);
    printf(" and shifted = %x\n", (status >> 8));
    exit(0);
}