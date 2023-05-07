#include <stdio.h>
#include <stdlib.h>
#include <signal.h>

int main(int argc, char *argv[])
{
    if (argc != 2) {
        //argv[0]은 해당 프로세스의 이름이 됨.
        fprintf(stderr, "usage: %s [Process ID]\n", argv[0]);
        exit(1);
    }
    else
        //인자로 받는 프로세스를 죽임
        kill(atoi(argv[1]), SIGKILL);
    
    exit(0);
}