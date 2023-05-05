#include <string.h>
#include <dirent.h>
#include <syslog.h>

#include "ssu_monitor_define.h"
#include "ssu_monitor_daemon.h"

int monitor_routine(const char *m_path)
{
    static int isInit = 0;
    DIR *dp;
    struct dirent *dirp;

    //Todo: 함수로 따로 빼기
    if((dp = opendir(m_path)) == NULL) {
        syslog(LOG_ERR, "opendir error\n");
    }

    return 0;
}