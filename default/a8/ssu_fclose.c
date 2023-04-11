#include <stdio.h>
#include <stdlib.h>

int main(void)
{
    char *fname = "ssu_test.txt";
    FILE *fp;

    //Comment: 파일을 읽기모드로 열음
    //  없는 경우 에러
    if ((fp = fopen(fname, "r")) == NULL){
        fprintf(stderr, "fopen error for %s\n", fname);
        exit(1);
    }
    else {
        printf("Success!\n");
        printf("Opening \"%s\" in \"r\" mode!\n", fname);
    }

    //Comment: 파일포인터로 파일을 닫음.
    fclose(fp);
    exit(0);
}