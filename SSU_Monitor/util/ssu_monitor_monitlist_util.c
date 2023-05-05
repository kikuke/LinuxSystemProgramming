#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "ssu_monitor_monitlist_util.h"

struct monitlist *InitMonitList(const char *path, pid_t pid, struct monitlist *bef, struct monitlist *aft)
{
    struct monitlist *monit = NULL;

    if((monit = (struct monitlist *)malloc(sizeof(struct monitlist))) == NULL)
        return NULL;

    strcpy(monit->path, path);
    monit->pid = pid;

    monit->bef = bef;
    monit->aft = aft;

    return monit;
}

int AddMonitList(struct monitlist *source, struct monitlist *target)
{
    if(source == NULL || target == NULL)
        return -1;
    if(target->bef != NULL)
        return -1;

    while(source->aft != NULL)
        source = source->aft;
    source->aft = target;
    target->bef = source;

    return 0;
}

struct monitlist *RemoveMonitList(struct monitlist *target)
{
    struct monitlist *ret = NULL;

    if(target == NULL)
        return NULL;
    
    ret = target->aft;
    if(target->bef != NULL) {
        target->bef->aft = ret;
    }

    free(target);
    return ret;
}

struct monitlist *SerachMonitListByPath(struct monitlist *source, const char *path)
{
    if(source == NULL || path == NULL)
        return NULL;
    
    while(source != NULL) {
        if(!strcmp(source->path, path))
            break;
        source = source->aft;
    }

    return source;
}

struct monitlist *SerachMonitListByPid(struct monitlist *source, pid_t pid)
{
    if(source == NULL)
        return NULL;
    
    while(source != NULL) {
        if(source->pid == pid)
            break;
        source = source->aft;
    }

    return source;
}

struct monitlist *MakeMonitListByPath(const char *path)
{
    struct monitlist *m_list = NULL;
    struct monitlist *new = NULL;

    FILE *fp = NULL;

    if(path == NULL) {
        fprintf(stderr, "path is NULL\n");
        return NULL;
    }
    
    if((fp = fopen(path, "r")) == NULL) {
        perror("fopen()");
        return NULL;
    }

    while(!feof(fp)) {
        new = InitMonitList("", -1, NULL, NULL);

        if(m_list == NULL) {
            m_list = new;
        }

        fscanf(fp, SSU_MONITOR_MONITLIST_FILE_FORAMT, new->path, &(new->pid));

        if(ferror(fp)) {
            perror("ferror()");

            while(m_list != NULL){
                m_list = RemoveMonitList(m_list);
            }
            return NULL;
        }

        if(m_list == new)
            continue;

        AddMonitList(m_list, new);
    }

    if(fclose(fp) == EOF) {
        perror("fclose()");

        while(m_list != NULL){
            m_list = RemoveMonitList(m_list);
        }
        return NULL;
    }
    return m_list;
}

int SaveMonitListByPath(struct monitlist *source, const char *path)
{
    FILE *fp = NULL;
    
    if(source == NULL || path == NULL) {
        fprintf(stderr, "parameter is NULL\n");
        return -1;
    }
    
    if((fp = fopen(path, "w")) == NULL) {
        perror("fopen()");
        return -1;
    }

    while(source != NULL){
        fprintf(fp, SSU_MONITOR_MONITLIST_FILE_FORAMT, source->path, source->pid);

        if(ferror(fp)) {
            perror("ferror()");
            return -1;
        }

        source = source->aft;
    }

    if(fclose(fp) == EOF) {
        perror("fclose()");
        return -1;
    }
    return 0;
}