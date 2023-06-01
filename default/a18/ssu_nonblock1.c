#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <fcntl.h>

//fd의 플래그에 flags를 추가함
void set_flags(int fd, int flags);
//fd의 플래그에 flags 비트들을 끔
void clr_flags(int fd, int flags);

char buf[500000];

int main(void)
{
    int ntowrite, nwrite;
    char *ptr;

    //출력할 문자열을 읽어들이고 바이트수를 저장
    ntowrite = read(STDIN_FILENO, buf, sizeof(buf));
    fprintf(stderr, "reading %d bytes\n", ntowrite);

    //표준출력을 논블록으로
    set_flags(STDOUT_FILENO, O_NONBLOCK);

    //포인터를 버퍼의 시작지점으로
    ptr = buf;
    //아직 다쓰지 않았다면
    while (ntowrite > 0) {
        //다 못쓰면 에러 세팅되므로
        errno = 0;
        //몇줄 썼는지만 리턴
        nwrite = write(STDOUT_FILENO, ptr, ntowrite);
        fprintf(stderr, "nwrite = %d, errno = %d\n", nwrite, errno);

        //만약 쓴것이 있다면 그만큼 쓰기위치 조정하고 써야할 바이트 줄이기
        if (nwrite > 0) {
            ptr += nwrite;
            ntowrite -= nwrite;
        }
    }

    exit(0);
}

void set_flags(int fd, int flags)
{
    int val;

    //fd의 플래그를 가져옴
    if ((val = fcntl(fd, F_GETFL)) < 0) {
        fprintf(stderr, "fcntl F_GETFL failed");
        exit(1);
    }

    //가져온 플래그에 인자로 들어온 플래그를 추가
    val |= flags;

    //적용
    if (fcntl(fd, F_SETFL, val) < 0) {
        fprintf(stderr, "fcntl F_SETFL failed");
        exit(1);
    }
}

void clr_flags(int fd, int flags)
{
    int val;

    if ((val = fcntl(fd, F_GETFL, 0)) < 0) {
        fprintf(stderr, "fcntl F_GETFL failed");
        exit(1);
    }

    //설정한 비트를 꺼버림
    val &= ~flags;

    if (fcntl(fd, F_SETFL, val) < 0) {
        fprintf(stderr, "fcntl F_SETFL failed");
        exit(1);
    }
}