#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <signal.h>
#include <syslog.h>

#include "ssu_monitor_define.h"
#include "ssu_monitor_system.h"
#include "ssu_monitor_usage.h"
#include "ssu_monitor_path.h"
#include "ssu_monitor_util.h"
#include "ssu_monitor_monitlist_util.h"
#include "ssu_monitor_add.h"

static int check_option(int argc, char *argv[]);

static int tOption = 0;
static unsigned int rTime = 1;

int add_daemon(int argc, char *argv[])
{
    struct monitlist *m_list;
    struct monitlist *m_new;
    char tmpBuf[SSU_MONITOR_MAX_PATH] = {};
    char addPath[SSU_MONITOR_MAX_PATH] = {};
    char settingPath[SSU_MONITOR_MAX_PATH] = {};

    if(argc < 2) {
        Usage(USAGEIDX_ADD);
        exit(1);
    }
    strcpy(tmpBuf, argv[1]);

    if(check_option(argc, argv) < 0) {
        Usage(USAGEIDX_ADD);
        exit(1);
    }

    if(GetVirtualRealPath(tmpBuf, addPath) == NULL) {
        Usage(USAGEIDX_ADD);
        exit(1);
    }
    strcpy(tmpBuf, addPath);
    if(realpath(tmpBuf, addPath) == NULL) {
        if(errno == ENOENT || errno == ENOTDIR) {
            Usage(USAGEIDX_ADD);
        } else {
            perror("realpath()");
        }
        exit(1);
    }

    if(CheckFileTypeByPath(addPath) != SSU_MONITOR_TYPE_DIR) {
        Usage(USAGEIDX_ADD);
        exit(1);
    }

    if(getcwd(settingPath, SSU_MONITOR_MAX_PATH) == NULL) {
        fprintf(stderr, "getcwd error\n");
        exit(1);
    }
    ConcatPath(settingPath, SSU_MONITOR_SETTING_FILE);

    printf("monitoring started (%s)\n", addPath);

    //따로 환경설정이 없는 데몬이므로 SIG_IGN
    //부모 프로세스는 종료되며 자식프로세스가 데몬 프로세스가 됨.
    if(change_daemon(SSU_MONITOR_DAEMON_NAME, SSU_MONITOR_LOG_IDENT, SIG_IGN) < 0) {
        fprintf(stderr, "change_daemon error\n");
        exit(1);
    }

    m_new = InitMonitList(addPath, getpid(), NULL, NULL);

    //설정 파일이 없을 경우
    if(access(settingPath, F_OK) < 0) {
        m_list = m_new;
    } else {
        //설정파일이 있을 경우
        if((m_list = MakeMonitListByPath(settingPath)) == NULL) {
            fprintf(stderr, "MakeMonitListByPath Error\n");
            exit(1);
        }
        if(AddMonitList(m_list, m_new) < 0) {
            fprintf(stderr, "AddMonitList Error\n");
            exit(1);
        }
    }

    if(SaveMonitListByPath(m_list, settingPath) < 0) {
        fprintf(stderr, "MakeMonitListByPath Error\n");
        exit(1);
    }
    
    exit(0);
}

//오류시 -1, 정상 종료시 0 리턴
static int check_option(int argc, char *argv[])
{
    int c = 0;

    while((c = getopt(argc, argv, "t:")) != -1) {
        switch (c) {
        case 't':
            if((rTime = atoi(optarg)) == 0) {
                fprintf(stderr, "Wrong time value\n");
                return -1;
            }
            break;

        case '?':
            fprintf(stderr, "Unknown option %c\n", optopt);
            return -1;
            break;
        
        default:
            fprintf(stderr, "Switch default\n");
            return -1;
            break;
        }
    }

    return 0;
}