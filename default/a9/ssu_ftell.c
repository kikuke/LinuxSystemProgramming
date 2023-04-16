#include <stdio.h>
#include <stdlib.h>

int main(void)
{
    char *fname = "ssu_test.txt";
    FILE *fp;
    long fsize;

    //ssu_test파일을 열기
    if ((fp = fopen(fname, "r")) == NULL) {
        fprintf(stderr, "fopen error for %s\n", fname);
        exit(1);
    }

    //파일의 맨 끝 위치로 이동
    fseek(fp, 0, SEEK_END);
    //맨 끝 위치를 얻었으므로 파일의 크기가 됨
    fsize = ftell(fp);
    printf("The size of <%s> is %ld bytes\n", fname, fsize);
    exit(0);
}