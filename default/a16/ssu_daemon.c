#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <signal.h>
#include <syslog.h>
#include <sys/stat.h>
#include <sys/types.h>

int ssu_daemon_init(void);

int main(void)
{
    pid_t pid;

    pid = getpid();
    printf("parent process : %d\n", pid);
    printf("daemon process initialization\n");

    if (ssu_daemon_init() < 0) {
        fprintf(stderr, "ssu_daemon_init failed\n");
        exit(1);
    }
    exit(0);
}

int ssu_daemon_init(void) {
    pid_t pid;
    int fd, maxfd;

    //부모 프로세스 죽이고 자신이 활동함
    //  백그라운드 실행, 부모가 /etc/init 프로세스가 됨
    if ((pid = fork()) < 0) {
        fprintf(stderr, "fork error\n");
        exit(1);
    }
    else if (pid != 0)
        exit(0);

    pid = getpid();
    printf("process %d running as daemon\n", pid);
    //자신이 세션의 리더가 됨.
    //더이상 상속된 프로세스 그룹 등의 영향을 받지 않음
    setsid();
    //표준 입출력과 정지 시그널을 무시
    //작업 제어에 관련된 시그널들을 무시한다
    signal(SIGTTIN, SIG_IGN);
    signal(SIGTTOU, SIG_IGN);
    signal(SIGTSTP, SIG_IGN);
    maxfd = getdtablesize();

    //열려있는 모든 fd를 닫음
    //상속받은 디스크립터를 버리고, 불필요한 파일 입출력을 하지 않게 함
    for (fd = 0; fd < maxfd; fd++)
        close(fd);

    //상속받은 파일 마스크 무시
    //자신이 파일 입출력을 할때 나도 모르는 마스크로인한 불상사 방지
    umask(0);
    //현재폴더 언마운트 할수 있게끔
    chdir("/");
    //표준입출력과 표준 에러의 터미널을 없앤다.
    //라이브러리 루틴 무효화
    fd = open("/dev/null", O_RDWR);
    dup(0);
    dup(0);
    return 0;
}