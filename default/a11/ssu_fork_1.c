#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

char glob_str[] = "write to standard output\n";
int glob_val = 10;

int main(void)
{
    pid_t pid;
    int loc_val;

    loc_val = 100;
    //write는 버퍼링을 하지 않기에 즉시 출력된다.
    if(write(STDOUT_FILENO,
        glob_str, sizeof(glob_str)-1) != sizeof(glob_str) - 1) {
            fprintf(stderr, "write error\n");
            exit(1);
        }

    //입출력 재지정을 통해 stdout이 바뀌게 되면 라인버퍼상태가 풀버퍼가 되어 이 문자열이 버퍼에 남아있게 된다.
    //  그럴경우 해당 버퍼상태 또한 자식프로세스에 복사되어 이후 두번 출력되게 된다.
    printf("before fork\n");

    //프로세스 공간 복제
    if((pid = fork()) < 0) {
        fprintf(stderr, "fork error\n");
        exit(1);
    }
    else if (pid == 0) {
        //부모프로세스에는 메모리가 다르므로 적용되지 않음.
        glob_val++;
        loc_val++;
    }
    else
        sleep(3);

    //부모와 자식의 프린트가 각각 출력됨.
    //자식은 부모와 달리 각 변수들이 1씩 증가한 상태로 출력이 됨. 서로 메모리가 다르므로.
    printf("pid = %d, glob-val = %d, loc_val = %d\n", getpid(), glob_val, loc_val);
    exit(0);
}