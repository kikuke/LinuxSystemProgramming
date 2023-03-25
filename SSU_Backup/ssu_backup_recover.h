#ifndef SSU_BACKUP_RECOVER_H
#define SSU_BACKUP_RECOVER_H

int RecoverFileSelector(const char* parentPath, const char* originPath, const struct filetree** recoverTrees, int listNum);

#endif