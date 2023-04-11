#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>

int main(void)
{
    char *fname = "ssu_test.txt";
    int fd;

    //Comment: 표준 출력으로 글자 출력
    printf("First printf : Hello, OSLAB!!\n");

    //Comment: 파일을 열어보고 있는지 체크
    if ((fd = open(fname, O_RDONLY)) < 0) {
        fprintf(stderr, "open error for %s\n", fname);
        exit(1);
    }

    //Comment: 확인한 파일을 표준출력 파일 포인터로 재지정해서 열음.
    //  따라서 파일에 아래 글자가 출력됨.
    if (freopen(fname, "w", stdout) != NULL)
        printf("Second printf : Hello, OSLAB!!\n");

    exit(0);
}