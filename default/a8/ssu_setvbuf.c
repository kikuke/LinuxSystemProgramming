#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#define BUFFER_SIZE 1024

void ssu_setbuf(FILE *fp, char *buf);

int main(void)
{
    char buf[BUFFER_SIZE];
    //Comment: 터미널 장치 파일
    char *fname = "/dev/pts/0";
    FILE *fp;

    //Comment: 터미널 장치 파일을 열었음
    if((fp = fopen(fname, "w")) == NULL) {
        fprintf(stderr, "fopen error for %s", fname);
        exit(1);
    }

	//Comment: 라인단위 출력이 됨.
    ssu_setbuf(fp, buf);
    fprintf(fp, "Hello, ");
    sleep(1);
    fprintf(fp, "UNIX!!");
    sleep(1);
    fprintf(fp, "\n");
    sleep(1);
    //Comment: 논버퍼가 되서 바로 바로 출력됨
    ssu_setbuf(fp, NULL);
    fprintf(fp, "HOW");
    sleep(1);
    fprintf(fp, " ARE");
    sleep(1);
    fprintf(fp, " YOU?");
    sleep(1);
    fprintf(fp, "\n");
    sleep(1);
    exit(0);
}

void ssu_setbuf(FILE *fp, char *buf) {
    size_t size;
    int fd;
    int mode;

    //Comment: 파일 디스크립터로 변환
    fd = fileno(fp);

    //Comment: 터미널 장치인지 체크 하고 터미널인 경우 라인버퍼, 아닌경우 논버퍼로 설정
    if (isatty(fd))
        mode = _IOLBF;
    else
        mode = _IONBF;
    
    //Comment: 버퍼가 없을 경우 모드를 논버퍼로 설정
    //  있을 경우 사이즈를 BUFFER_SIZE로 설정
    if (buf == NULL) {
        mode = _IONBF;
        size = 0;
    }
    else
        size = BUFFER_SIZE;

    //Comment: 위 설정에 따라 버퍼 설정
    setvbuf(fp, buf, mode , size);
}
