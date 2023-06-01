#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <errno.h>
#include <fcntl.h>

#define MAX_CALL 100
#define MAX_BUF 20000

//표준출력 에러로 출력후 종료
void serror(char *str)
{
    fprintf(stderr, "%s\n", str);
    exit(1);
}

int main(void)
{
    int nread, nwrite, val, i=0;
    char *ptr;
    char buf[MAX_BUF];

    int call[MAX_CALL];

    //표준입력에서 읽어들이고 읽은 바이트 리턴
    nread = read(STDIN_FILENO, buf, sizeof(buf));
    fprintf(stderr, "read %d bytes\n", nread);

    //표준출력의 플래그를 가져오고, 논블록 비트를 세팅후 적용
    if ((val = fcntl(STDOUT_FILENO, F_GETFL, 0)) < 0)
        serror("fcntl F_GETFL error");
    val |= O_NONBLOCK;
    if (fcntl(STDOUT_FILENO, F_SETFL, val) < 0)
        serror("fcntl F_SETFL error");

    //입력받은 길이만큼 표준출력에 쓰기
    for (ptr = buf; nread > 0; i++) {
        errno = 0;
        nwrite = write(STDOUT_FILENO, ptr, nread);

        //읽어들인 길이 저장. 논블록으로 결과 받기도 전에 리턴되면 -1
        if (i < MAX_CALL)
            call[i] = nwrite;

        //쓴값 만큼 포인터 옮기고 읽을 길이 조정
        fprintf(stderr, "nwrite = %d, errno = %d\n", nwrite, errno);
        if (nwrite > 0) {
            ptr += nwrite;
            nread -= nwrite;
        }
    }

    //논블록 비트 취소
    if ((val = fcntl(STDOUT_FILENO, F_GETFL, 0)) < 0)
        serror("fcntl F_GETFL error");
    val &= ~O_NONBLOCK;
    if (fcntl(STDOUT_FILENO, F_SETFL, val) < 0)
        serror("fcntl F_SETFL error");

    //읽어들였던 기록들 전부 출력
    for (i=0; i < MAX_CALL; i++)
        fprintf(stdout, "call[%d] = %d\n", i, call[i]);
    
    exit(0);
}