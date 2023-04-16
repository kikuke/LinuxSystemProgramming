#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#define BUFFER_SIZE 1024

int main(void)
{
    char buf[BUFFER_SIZE];

    //라인 단위로 stdin버퍼를 읽어들인다. 파일 끝이 올때 까지 계속 읽어들인다.
    while (fgets(buf, BUFFER_SIZE, stdin) != NULL)
        //출력에서 에러가 발생했을 경우 에러 핸들링
        if (fputs(buf, stdout) == EOF) {
            fprintf(stderr, "standard output error\n");
            exit(1);
        }
    
    //표준 입력에서 에러가 발생했는지 체크
    if (ferror(stdin)) {
        fprintf(stderr, "standard input error\n");
        exit(1);
    }

    exit(0);
}