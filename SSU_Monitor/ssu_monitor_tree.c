#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>

#include "ssu_monitor_define.h"
#include "ssu_monitor_usage.h"
#include "ssu_monitor_path.h"
#include "ssu_monitor_monitlist_util.h"
#include "ssu_monitor_monitree_util.h"
#include "ssu_monitor_tree.h"

int ssu_monitor_tree(int argc, char *argv[])
{
    struct monitlist *m_list = NULL;
    struct monitlist *m_match = NULL;
    char tmpBuf[SSU_MONITOR_MAX_PATH] = {};
    char treePath[SSU_MONITOR_MAX_PATH] = {};
    char settingPath[SSU_MONITOR_MAX_PATH] = {};

    if(argc != 2) {
        Usage(USAGEIDX_TREE);
        exit(1);
    }
    strcpy(tmpBuf, argv[1]);

    if(GetVirtualRealPath(tmpBuf, treePath) == NULL) {
        Usage(USAGEIDX_TREE);
        exit(1);
    }
    strcpy(tmpBuf, treePath);
    if(realpath(tmpBuf, treePath) == NULL) {
        if(errno == ENOENT || errno == ENOTDIR) {
            Usage(USAGEIDX_TREE);
        } else {
            perror("realpath()");
        }
        exit(1);
    }

    if(getcwd(settingPath, SSU_MONITOR_MAX_PATH) == NULL) {
        fprintf(stderr, "getcwd error\n");
        exit(1);
    }
    ConcatPath(settingPath, SSU_MONITOR_SETTING_FILE);
    //설정 파일이 없을 경우
    if(access(settingPath, F_OK) < 0) {
        fprintf(stderr, "No file in \"%s\"\n", settingPath);
        exit(1);
    }
    if((m_list = MakeMonitListByPath(settingPath)) == NULL) {
        fprintf(stderr, "MakeMonitListByPath Error\n");
        exit(1);
    }

    //일치하는 경로가 있는지 탐색
    if((m_match = MatchMonitListByPath(m_list, treePath)) == NULL) {
        fprintf(stderr, "No matched monitoring path \"%s\"\n", treePath);
        exit(1);
    }

    if(PrintMoniTreeByPath(treePath) < 0) {
        fprintf(stderr, "PrintMoniTreeByPath failed in \"%s\"\n", treePath);
        exit(1);
    }

    exit(0);
}

void PrintMoniTree(struct monitree *tree)
{
    //Todo: 임시 테스트용

    while(tree != NULL) {
        printf("%s\n", tree->filename);

        PrintMoniTree(tree->move[MTREE_CHILD]);
        tree = tree->move[MTREE_AFT];
    }
}

int PrintMoniTreeByPath(const char* path)
{
    struct monitree *tree = InitMoniTree(0, 0, NULL, 0);

    PathToMoniTree(path, tree);
    //Todo: 가장 위에 tree도 출력하기

    PrintMoniTree(tree->move[MTREE_CHILD]);

    EraseMoniTree(tree);
    return 0;
}