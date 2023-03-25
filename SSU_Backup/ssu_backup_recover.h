#ifndef SSU_BACKUP_RECOVER_H
#define SSU_BACKUP_RECOVER_H

int CheckRecoverPathCondition(const char* path);

int CheckFileTypeCondition(const char* originPath, int recoverType, int checkType);

int RecoverFileSelector(const char* parentPath, const char* originPath, const struct filetree** recoverTrees, int listNum);

#endif