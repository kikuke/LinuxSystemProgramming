#include <stdio.h>
#include <stdlib.h>

int main(void)
{
    char *fname = "ssu_test.txt";
    FILE *fp;
    long position;
    int character;

    //ssu_test 파일을 열기
    if ((fp = fopen(fname, "r")) == NULL) {
        fprintf(stderr, "fopen error for %s\n", fname);
        exit(1);
    }

    printf("Input number >>");
    //fseek할 위치를 받아옴
    scanf("%ld", &position);
    //파일 포인터를 처음위치로 부터 입력받은 위치 -1의 위치로 이동
    fseek(fp, position - 1, SEEK_SET);
    //이동한 위치에서 한글자 읽어온 뒤 출력
    character = getc(fp);
    printf("%ld character => %c\n", position, character);
    exit(0);
}