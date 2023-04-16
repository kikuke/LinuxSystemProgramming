#include <stdio.h>
#include <stdlib.h>

#define BUFFER_MAX 256

int main(void)
{
    char command[BUFFER_MAX];
    char *prompt = "Prompt>>";

    while (1) {
        //프롬프트를 표준 출력으로 출력
        fputs(prompt, stdout);

        //실행할 명령어를 표준 출력으로부터 입력받는다. 만약 파일 끝을 읽었다면 종료
        if (fgets(command, sizeof(command), stdin) == NULL)
            break;
        
        //입력받은 명령어 실행
        system(command);
    }

    //남아있는 표준 출력버퍼를 모두 비움
    fprintf(stdout, "Good bye...\n");
    fflush(stdout);
    exit(0);
}