#include <stdio.h>
#include <stdlib.h>

int main(int argc, char *argv[])
{
    FILE *fp;
    int character;

    //인자를 하나 받음
    if (argc != 2) {
        fprintf(stderr, "usage: %s <filename>\n", argv[0]);
        exit(1);
    }
    //첫번째 인자로 들어온 파일을 읽기모드로 오픈
    if ((fp = fopen(argv[1], "r")) == NULL) {
        fprintf(stderr, "fopen error for %s\n", argv[1]);
        exit(1);
    }

    //첫 한글자를 읽어들임
    character = fgetc(fp);

    //파일이 끝났을 때 까지. 오직 eof인 경우만 에러해당 x
    while (!feof(fp)) {
        //표준출력으로 출력
        fputc(character, stdout);

        //에러발생했는지 검사
        if(ferror(fp)) {
            fprintf(stderr, "Error detected!!\n");
            //에러 플래그 및 eof플래그 지우기
            clearerr(fp);
        }

        //파일포인터에서 한문자 읽어들임
        character = fgetc(fp);
    }

    fclose(fp);
    exit(0);
}