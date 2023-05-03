#include <signal.h>
#include <sys/types.h>
#include <sys/resource.h>

#include "ssu_monitor_define.h"
#include "ssu_monitor_daemon.h"

int change_daemon()
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

    if(getrlimit(RLIMIT_NOFILE, &rl) < 0) {
        perror("getrlimit()");
    }

    
}