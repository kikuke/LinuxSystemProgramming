#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "ssu_monitor_define.h"
#include "ssu_monitor_util.h"
#include "ssu_monitor_add.h"

static int check_option(int argc, char *argv[]);

static int tOption = 0;
static unsigned int rTime = 1;

int add_daemon(int argc, char *argv[])
{
    char addPath[SSU_MONITOR_MAX_PATH];

    if(argc < 2) {
        Usage(USAGEIDX_ADD);
        exit(1);
    }
    strcpy(addPath, argv[1]);

    if(check_option(argc, argv) < 0) {
        Usage(USAGEIDX_ADD);
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