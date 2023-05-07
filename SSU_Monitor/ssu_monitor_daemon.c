#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <syslog.h>
#include <errno.h>

#include "ssu_monitor_define.h"
#include "ssu_monitor_util.h"
#include "ssu_monitor_path.h"
#include "ssu_monitor_monitree_util.h"
#include "ssu_monitor_daemon.h"

static char logPath[SSU_MONITOR_MAX_PATH];

int monitor_routine(const char *m_path)
{
    static struct monitree *oldTree = NULL;
    static struct monitree *newTree = NULL;
    FILE *fp = NULL;

    //최초 실행 시 초기화
    if(oldTree == NULL) {
        oldTree = InitMoniTree(0, 0, "", 0);
        PathToMoniTree(m_path, oldTree);

        return 0;
    }
    //새롭게 폴더 탐색
    newTree = InitMoniTree(0, 0, "", 0);
    PathToMoniTree(m_path, newTree);

    strcpy(logPath, m_path);
    ConcatPath(logPath, SSU_MONITOR_LOG_FILE);
    if((fp = fopen(logPath, "a")) == NULL) {
        syslog(LOG_ERR, "fopen failed in \"%s\": %s\n", logPath, strerror(errno));
        return -1;
    }
    if(CompareMoniTreeAndWriteInFp(oldTree->move[MTREE_CHILD], newTree->move[MTREE_CHILD], m_path, fp) < 0) {
        syslog(LOG_ERR, "CompareMoniTreeAndWriteInPath failed in \"%s\": %s\n", m_path, strerror(errno));
        return -1;
    }
    if(fclose(fp) == EOF) {
        syslog(LOG_ERR, "fclose failed: %s\n", strerror(errno));
        return -1;
    }

    //이전의 트리 삭제 후 새 트리로 교체 후 종료
    EraseMoniTree(oldTree);
    oldTree = newTree;
    newTree = NULL;

    return 0;
}

void SSUMonitorKillSignalHandler(int signo)
{
    exit(0);
}

int CompareMoniTreeAndWriteInFp(struct monitree *oldTree, struct monitree *newTree, const char *path, FILE *fp)
{
    if(FindCreateFileByMoniTreeAndWriteInFp(oldTree, newTree, path, fp) < 0) {
        syslog(LOG_ERR, "FindCreateFileByMoniTreeAndWriteInFp failed in \"%s\"\n", path);
        return -1;
    }

    if(FindRemoveOrModifyFileByMoniTreeAndWriteInFp(oldTree, newTree, path, fp) < 0) {
        syslog(LOG_ERR, "FindRemoveOrModifyFileByMoniTreeAndWriteInFp failed in \"%s\"\n", path);
        return -1;
    }

    return 0;
}

int FindRemoveOrModifyFileByMoniTreeAndWriteInFp(struct monitree *oldTree, struct monitree *newTree, const char *path, FILE *fp)
{
    struct monitree *searchTree = NULL;
    struct monitree *cSearchTree = NULL;
    time_t nowTime = 0;
    char nextPath[SSU_MONITOR_MAX_PATH] = {0};

    nowTime = time(NULL);
    strcpy(nextPath, path);
    //없다면 파일이 삭제된 것.
    //  변경 시간 파악
    for(; oldTree != NULL; oldTree = oldTree->move[MTREE_AFT]) {
        nextPath[strlen(path)] = '\0';
        ConcatPath(nextPath, oldTree->filename);

        //디렉토리인 경우 재귀호출
        if(oldTree->filetype == SSU_MONITOR_TYPE_DIR) {
            cSearchTree = NULL;
            if((searchTree = SearchSiblingMoniTreeByInode(newTree, oldTree->ino)) != NULL) {
                cSearchTree = searchTree->move[MTREE_CHILD];
            }
            if(FindRemoveOrModifyFileByMoniTreeAndWriteInFp(oldTree->move[MTREE_CHILD], cSearchTree, nextPath, fp) < 0)
                return -1;

            continue;
        }
        //레귤러인 경우 비교
        if(oldTree->filetype == SSU_MONITOR_TYPE_REG) {
            //로그파일인 경우 건너뛰기
            if(!strcmp(nextPath, logPath))
                continue;
            //파일이 삭제된 경우
            if((searchTree = SearchSiblingMoniTreeByInode(newTree, oldTree->ino)) == NULL) {
                if(WriteLogFile(fp, nowTime, SSU_MONITOR_LOG_STATUS_REMOVE, nextPath) < 0) {
                    syslog(LOG_ERR, "WriteLogFile failed in \"%s\"\n", path);
                    return -1;
                }
                continue;
            }
            //파일이 변경된 경우
            if(oldTree->md_time != searchTree->md_time) {
                if(WriteLogFile(fp, searchTree->md_time, SSU_MONITOR_LOG_STATUS_MODIFY, nextPath) < 0) {
                    syslog(LOG_ERR, "WriteLogFile failed in \"%s\"\n", path);
                    return -1;
                }
                continue;
            }
        }
    }

    return 0;
}

int FindCreateFileByMoniTreeAndWriteInFp(struct monitree *oldTree, struct monitree *newTree, const char *path, FILE *fp)
{
    struct monitree *searchTree = NULL;
    struct monitree *cSearchTree = NULL;
    char nextPath[SSU_MONITOR_MAX_PATH] = {0};

    strcpy(nextPath, path);
    //없다면 파일이 삭제된 것.
    //  변경 시간 파악
    for(; newTree != NULL; newTree = newTree->move[MTREE_AFT]) {
        nextPath[strlen(path)] = '\0';
        ConcatPath(nextPath, newTree->filename);

        //디렉토리인 경우 재귀호출
        if(newTree->filetype == SSU_MONITOR_TYPE_DIR) {
            cSearchTree = NULL;
            if((searchTree = SearchSiblingMoniTreeByInode(oldTree, newTree->ino)) != NULL) {
                cSearchTree = searchTree->move[MTREE_CHILD];
            }
            if(FindCreateFileByMoniTreeAndWriteInFp(cSearchTree, newTree->move[MTREE_CHILD], nextPath, fp) < 0)
                return -1;

            continue;
        }
        //레귤러인 경우 비교
        if(newTree->filetype == SSU_MONITOR_TYPE_REG) {
            //로그파일인 경우 건너뛰기
            if(!strcmp(nextPath, logPath))
                continue;
            //파일이 생성된 경우
            if((searchTree = SearchSiblingMoniTreeByInode(oldTree, newTree->ino)) == NULL) {
                if(WriteLogFile(fp, newTree->md_time, SSU_MONITOR_LOG_STATUS_CREATE, nextPath) < 0) {
                    syslog(LOG_ERR, "WriteLogFile failed in \"%s\"\n", path);
                    return -1;
                }
                continue;
            }
        }
    }

    return 0;
}

int WriteLogFile(FILE *fp, time_t time, const char *status, const char *path)
{
    struct tm *tm = localtime(&time);

    if(fp == NULL || status == NULL || path == NULL)
        return -1;
        
    fprintf(fp, SSU_MONITOR_LOG_FILE_FORAMT, 
        tm->tm_year+1900, tm->tm_mon+1, tm->tm_mday,
        tm->tm_hour, tm->tm_min, tm->tm_sec,
        status, path);

    return ferror(fp);
}