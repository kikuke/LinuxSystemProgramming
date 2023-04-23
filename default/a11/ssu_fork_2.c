#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int main(int argc, char *argv[])
{
    pid_t pid;
    char character, first, last;
    long i;

    //부모인 경우 대문자
    if ((pid = fork()) > 0) {
        first = 'A';
        last = 'Z';
    }
    //자식인 경우 소문자
    else if (pid == 0) {
        first = 'a';
        last = 'z';
    }
    else {
        fprintf(stderr, "%s\n", argv[0]);
        exit(1);
    }

    //알파벳 전부 출력
    for (character = first; character <=last; character++) {
        //지연시키기 위한 연산
        for (i = 0; i <= 100000; i++)
            ;

        //부모 자식 동시에 출력됨
        write(1, &character, 1);
    }

    printf("\n");
    exit(0);
}