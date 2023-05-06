#include <string.h>
#include <syslog.h>

#include "ssu_monitor_define.h"
#include "ssu_monitor_monitree_util.h"
#include "ssu_monitor_daemon.h"

int monitor_routine(const char *m_path)
{
    static struct monitree *oldTree = NULL;
    static struct monitree *newTree = NULL;

    //최초 실행 시 초기화
    if(oldTree == NULL) {
        oldTree = InitMoniTree(0, 0, "", 0);
        PathToMoniTree(m_path, oldTree);

        return 0;
    }
    //새롭게 폴더 탐색
    newTree = InitMoniTree(0, 0, "", 0);
    PathToMoniTree(m_path, newTree);

    if(CompareMoniTreeAndWriteInPath(oldTree, newTree, m_path) < 0) {
        syslog(LOG_ERR, "CompareMoniTreeAndWriteInPath failed in \"%s\"\n", m_path);
        return -1;
    }

    //이전의 트리 삭제 후 새 트리로 교체 후 종료
    EraseMoniTree(oldTree);
    oldTree = newTree;
    newTree = NULL;

    return 0;
}

int CompareMoniTreeAndWriteInPath(struct monitree *oldTree, struct monitree *newTree, const char *path)
{
    
}