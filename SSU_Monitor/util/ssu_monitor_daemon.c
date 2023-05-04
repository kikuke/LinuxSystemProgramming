#include <signal.h>
#include <fcntl.h>
#include <syslog.h>
#include <sys/types.h>
#include <sys/resource.h>

#include "ssu_monitor_define.h"
#include "ssu_monitor_daemon.h"

int change_daemon(const char *ident)
{
    pid_t pid;
    struct rlimit rl;

    if((pid = fork()) < 0) {
        perror("fork()");
        return -1;
    } else if (pid != 0) {
        exit(0);
    }

    if(setsid() < 0) {
        perror("setsid()");
        return -1;
    }

    //umask를 미리 설정
    umask(SSU_MONITOR_UMASK);

    //최대로 열수있는 파일 디스크립터의 수 가져옴
    if(getrlimit(RLIMIT_NOFILE, &rl) < 0) {
        perror("getrlimit()");
        return -1;
    }

    if(signal(SIGHUP, SIG_IGN) == SIG_ERR) {
        perror("signal()");
        return -1;
    }

    if(chdir("/") < 0) {
        perror("chdir()");
        return -1;
    }

    //-1(무한대) 일경우
    if(rl.rlim_max == RLIM_INFINITY) {
        rl.rlim_max = 1024;
    }

    //로그를 연다. 표준입출력과 터미널이 닫힌상태이므로 syslog 이용.
    //ident는 /var/log/syslog에 생성될 때 붙는 prefix
    //LOG_CONS: 시스템 로거로 보내는 동안 오류가 있으면 시스템 콘솔에 출력
    //LOG_DAEMON: 별도 기능 값이 없는 데몬 유형
    openlog(ident, LOG_CONS, LOG_DAEMON);

    //열려있는 모든 fd를 닫음
    for(int i=0; i < rl.rlim_max; i++) {
        if(close(i) < 0) {
            syslog(LOG_ERR, "close failed fd: %d\n", i);
            return -1;
        }
    }
    //0,1,2 표준 입출력 fd를 /dev/null 터미널로 리다이렉션
    if(open("/dev/null", O_RDWR) < 0) {
        syslog(LOG_ERR, "open fd0 /dev/null failed\n");
        return -1;
    }
    if(dup(0) < 0){
        syslog(LOG_ERR, "dup fd1 /dev/null failed\n");
        return -1;
    }
    if(dup(0) < 0){
        syslog(LOG_ERR, "dup fd2 /dev/null failed\n");
        return -1;
    }

    //로그 닫기
    closelog();
    return 0;
}