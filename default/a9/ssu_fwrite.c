#include <stdio.h>
#include <stdlib.h>

struct ssu_id {
    char name[64];
    int id;
};

int main(void)
{
    struct ssu_id test1, test2;
    char *fname = "ssu_exam.dat";
    FILE *fp;

    //위에서 지정한 파일을 쓰기모드로 열음. 새로 만들게 됨.
    if((fp = fopen(fname, "w")) == NULL) {
        fprintf(stderr, "fopen error for %s\n", fname);
        exit(1);
    }

    //id와 이름을 스캔으로 받아서 각 구조체에 저장함.
    printf("Input ID>> ");
    scanf("%d", &test1.id);
    printf("Input name>> ");
    scanf("%s", test1.name);

    //ssu_id 구조체 단위로 한개를 test1버퍼에서 받아서 파일포인터가 가리키는 곳에 쓰기함.
    if (fwrite(&test1, sizeof(struct ssu_id), 1, fp) != 1) {
        fprintf(stderr, "fwrite error\n");
        exit(1);
    }

    fclose(fp);

    //다시 썼던 파일을 읽기 파일로 열음
    if ((fp = fopen(fname, "r")) == NULL) {
        fprintf(stderr, "fopen error for %s\n", fname);
        exit(1);
    }
    //파일포인터가 가리키는 곳에서 ssu_id 구조체 단위로 한개크기 만큼 test2버퍼에 쓰기함
    if (fread(&test2, sizeof(struct ssu_id), 1, fp) != 1) {
        fprintf(stderr, "fread error\n");
        exit(1);
    }

    printf("\nID    name\n");
    printf(" ============\n");
    printf("%d      %s\n", test2.id, test2.name);
    fclose(fp);
    exit(0);
}
