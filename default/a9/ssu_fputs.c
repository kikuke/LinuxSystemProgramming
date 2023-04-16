#include <stdio.h>
#include <stdlib.h>

#define BUFFER_SIZE 1024

int main (int argc, char *argv[])
{
    char buf[BUFFER_SIZE];
    FILE *fp;

    //인자 하나를 받는다
    if (argc != 2) {
        fprintf(stderr, "usage: %s <file>\n", argv[0]);
        exit(1);
    }

    //첫번째 인자로 받은 파일을 새로 읽기/쓰기로 만든다.
    if ((fp = fopen(argv[1], "w+")) == NULL) {
        fprintf(stderr, "fopen error for %s\n", argv[1]);
        exit(1);
    }

    //개행 자동 추가 없이 표준출력으로 출력
    fputs("Input String >> ", stdout);
    //표준입력에서 개행을 빼고 버퍼로 입력된 문자열을 받는다.
    gets(buf);
    //개행 자동추가 없이 열었던 파일에 쓰기한다
    fputs(buf, fp);
    //파일 포인터를 처음으로 돌린다.
    rewind(fp);
    //해당 파일을 개행까지 모두 버퍼로 읽어들인다.
    fgets(buf, sizeof(buf), fp);
    //버퍼로 이동된 파일은 개행이 없겠지만 puts를 사용햇기에 뒤에 자동으로 개행이 붙는다.
    puts(buf);
    fclose(fp);
}