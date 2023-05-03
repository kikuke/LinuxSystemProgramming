
#include "ssu_monitor_define.h"
#include "ssu_monitor_daemon.h"

int change_daemon()
{
    //umask를 미리 설정
    umask(SSU_MONITOR_UMASK);

    
}