#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

//문자열을 버퍼링 없이 바로 천천히 출력하는 함수.
//  정적 함수라서 다른 소스에서 똑같은 함수 선언이 가능하다.
static void ssu_charatatime(char *str);

int main(void)
{
    pid_t pid;

    if ((pid = fork()) < 0) {
        fprintf(stderr, "fork error\n");
        exit(1);
    }
    //글자당 출력이 10ms씩 지연되기에 아래의 글자들이 섞여서 출력된다.
    else if (pid == 0)
        ssu_charatatime("output from child\n");
    else
        ssu_charatatime("output from parent\n");
    
    exit(0);
}

static void ssu_charatatime(char *str) {
    char *ptr;
    int print_char;

    //표준출력이 버퍼링을 하지 않게됨.
    setbuf(stdout, NULL);

    //문자열 끝까지 천천히 출력함
    for (ptr = str; (print_char = *ptr++) != 0; ) {
        putc(print_char, stdout);
        usleep(10);
    }
}