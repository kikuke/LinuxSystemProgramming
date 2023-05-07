#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <signal.h>
#include <sys/types.h>
#include <errno.h>

#include "ssu_monitor_define.h"
#include "ssu_monitor_usage.h"
#include "ssu_monitor_path.h"
#include "ssu_monitor_monitlist_util.h"
#include "ssu_monitor_delete.h"

int ssu_monitor_delete(int argc, char *argv[])
{
    pid_t killPid;
    struct monitlist *m_list = NULL;
    struct monitlist *m_search = NULL;
    char settingPath[SSU_MONITOR_MAX_PATH] = {};

    if(argc != 2) {
        Usage(USAGEIDX_DELETE);
        exit(1);
    }
    if(((killPid = atoi(argv[1])) <= 0) && strcmp(argv[1], "0")) {
        fprintf(stderr, "Pid must be more than 0\n");
        exit(1);
    }
    
    if(getcwd(settingPath, SSU_MONITOR_MAX_PATH) == NULL) {
        fprintf(stderr, "getcwd error\n");
        exit(1);
    }
    ConcatPath(settingPath, SSU_MONITOR_SETTING_FILE);
    //설정 파일이 없을 경우
    if(!access(settingPath, F_OK) < 0) {
        fprintf(stderr, "No list of Monitoring Pid - %d\n", killPid);
        exit(1);
    }

    if((m_list = MakeMonitListByPath(settingPath)) == NULL) {
        perror("MakeMonitListByPath Error");
        exit(1);
    }
    //일치하는 모니터링 리스트가 있는지 확인
    if((m_search = MatchMonitListByPid(m_list, killPid)) == NULL) {
        fprintf(stderr, "No list of Monitoring Pid - %d\n", killPid);
        exit(1);
    }

    if(kill(killPid, SSU_MONITOR_KILL_SIGNAL) < 0) {
        perror("kill failed");
        //해당 pid가 존재하지만, 시그널을 보내는데 실패했을 경우
        if(errno != EINVAL || errno != ESRCH)
            exit(1);
    }

    printf("monitoring ended (%s)\n", m_search->path);

    //해당 리스트 제거 후 저장
    if(m_search == m_list) {
        m_list = m_search->aft;
    }
    RemoveMonitList(m_search);
    if(SaveMonitListByPath(m_list, settingPath) < 0) {
        perror("SaveMonitListByPath failed");
        exit(1);
    }

    exit(0);
}