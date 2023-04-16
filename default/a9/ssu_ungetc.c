#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>

int main(void)
{
    char operator;
    FILE *fp;
    int character;
    int number = 0;

    //ssu_expr.txt파일을 오픈
    if ((fp = fopen("ssu_expr.txt", "r")) == NULL) {
        fprintf(stderr, "fopen error for ssu_expr.txt\n");
        exit(1);
    }

    //파일을 다 읽을 때 까지
    while (!feof(fp)) {
        //문자열 끝이 아니고 숫자일 때 까지, 문자를 숫자 변환읽어들인 숫자를 왼쪽으로 옮기며 십진수 자릿수를 유지함
        while((character = fgetc(fp)) != EOF && isdigit(character))
            number = 10 * number + character - 48;

        //읽어들인 숫자 출력
        fprintf(stdout, " %d\n", number);
        number = 0;

        //문자열 끝이 아닌 경우 위에서 숫자가 아닌것을 읽은 것으로 이를 연산자로 간주하고 스트림에 다시 넣고 다시 읽어들임. 이를 오퍼레이터라고 출력
        if(character != EOF) {
            ungetc(character, fp);
            operator = fgetc(fp);
            printf("Operator => %c\n", operator);
        }
    }

    fclose(fp);
    exit(0);
}