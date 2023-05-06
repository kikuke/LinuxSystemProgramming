#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>
#include <string.h>
#include <sys/stat.h>
#include <errno.h>

#include "ssu_monitor_define.h"
#include "ssu_monitor_path.h"
#include "ssu_monitor_util.h"
#include "ssu_monitor_monitree_util.h"

struct monitree *InitMoniTree(ino_t ino, int filetype, const char *filename, time_t md_time)
{
    struct monitree *tree = NULL;

    if((tree = (struct monitree *)malloc(sizeof(struct monitree))) == NULL)
        return NULL;
    
    tree->ino = ino;
    tree->filetype = filetype;
    strcpy(tree->filename, filename);
    tree->md_time = md_time;

    for(int i=0; i<MTREE_MAX; i++) {
        tree->move[i] = NULL;
    }

    return tree;
}

int ScanDirFilter(const struct dirent* target)
{
	if(!strcmp(target->d_name, ".") || !strcmp(target->d_name, "..")){
		return 0;
	}
	return 1;
}

struct monitree *PathToMoniTree(const char *path, struct monitree *pTree)
{
    struct filetree *cTree = NULL;
    int childCount = 0;
    struct dirent **childList = NULL;
    struct stat f_stat;
    char nextPath[SSU_MONITOR_MAX_PATH] = {0};

    //자식이 이미 있는 경우 바로 리턴
    if(pTree->move[MTREE_CHILD] != NULL){
        fprintf(stderr, "child is not NULL\n");
        return NULL;
    }
    
    //하위 파일들이 없는 경우 바로 리턴
    if((childCount = scandir(path, &childList, ScanDirFilter, alphasort)) < 1)
        return NULL;
    
    strcpy(nextPath, path);
    for(int i = 0; i < childCount; i++) {
        nextPath[strlen(path)] = '\0';
        ConcatPath(nextPath, childList[i]->d_name);
        if(lstat(nextPath, &f_stat) < 0) {
            fprintf(stderr, "failed lstat in \"%s\": %s\n", nextPath, strerror(errno));
            free(childList[i]);
            continue;
        }

        cTree = InitMoniTree(f_stat.st_ino, CheckFileType(&f_stat), childList[i]->d_name, f_stat.st_mtime);

        //첫번째 자식인 경우 pTree아래에 저장
        if(i == 0) {

        } else {

        }
        free(childList[i]);
    }
    free(childList);

    return pTree->move[MTREE_CHILD];
}