#include <stdio.h>
#include <stdlib.h>

int main(void)
{
    int character;

    //stdin에서 입력받은 unsigned char을 int형태로 받아오고, 에러가 발생했는지 체크한다.
    //  stdin. 터미널은 라인단위 버퍼이므로 개행이와야 stdin 버퍼로 데이터가 이동한다. 엔터가 입력된 이후부터 읽어들이기 시작한다.
    //  개행까지 입력에 포함된다.
    //  EOF를 입력받을 때 까지 계속 입력받는다
    //  stdin버퍼에는 라인단위로 입력했다고 해서 자동으로 '\0'이 오지 않는다
    while ((character = getc(stdin)) != EOF)
        if (putc(character, stdout) == EOF) {
            fprintf(stderr, "standard output error\n");
            exit(1);
        }
    
    //정상 종료 됐다면 0인 값을, 아니라면 0이 아닌 값을 리턴한다.
    if (ferror(stdin)) {
        fprintf(stderr, "standard input error\n");
        exit(1);
    }

    exit(0);
}