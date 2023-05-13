#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <setjmp.h>

//인자로 들어온 시그널에 대해 SIG_IGN처리후
//문자열을 하나 읽어 y가 아닐경우 SIGINT에 대해 해당 함수를 다시 시그널 핸들러로 설정후 롱점프함
void ssu_signal_handler(int signo);

jmp_buf jump_buffer;

int main(void)
{
    signal(SIGINT, ssu_signal_handler);

    while (1) {
        //longjmp가 실행되면 루프
        if (setjmp(jump_buffer) == 0) {
            printf("Hit Ctrl-c at anytime ... \n");
            pause();
        }
    }

    exit(0);
}

void ssu_signal_handler(int signo) {
    char character;

    signal(signo, SIG_IGN);
    printf("Did you hit Ctrl-c?\n" "Do you really want to quit? [y/n] ");

    character = getchar();

    if (character == 'y' || character == 'Y')
        exit(0);
    else {
        signal(SIGINT, ssu_signal_handler);
        //longjmp는 리턴됐지만 핸들러는 리턴이 끝나지 않았음. 따라서 해당 시그널이 block상태에 놓임
        longjmp(jump_buffer, 1);
    }
}
