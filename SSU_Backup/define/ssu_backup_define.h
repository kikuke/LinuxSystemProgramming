#ifndef SSU_BACKUP_DEFINE_H
#define SSU_BACKUP_DEFINE_H

#include <sys/stat.h>

#define SSU_BACKUP_SHELL_NAME "20192388>" 
#define SSU_BACKUP_SHELL_MAX_BUF_SZ 16384

#define SSU_BACKUP_ROOT_DIR_NAME "backup"
#define SSU_BACKUP_MAX_PATH_SZ 4096
#define SSU_BACKUP_MAX_FILENAME 255
#define SSU_BACKUP_FILE_META_LEN 13
#define SSU_BACKUP_MKDIR_AUTH (S_IRWXU | S_IRGRP | S_IXGRP | S_IROTH | S_IXOTH)
#define SSU_BACKUP_MKFILE_AUTH (S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH)
#define SSU_BACKUP_FILE_BUF_SZ 1024

#define SSU_BACKUP_HASH_SET_FILE ".hash"
#define SSU_BACKUP_HASH_MD5 1
#define SSU_BACKUP_HASH_SHA1 2
#define SSU_BACKUP_HASH_MAX_LEN 20
#define SSU_BACKUP_HASH_BUF_SZ 1024*16

#define SSU_BACKUP_TYPE_ERROR -1
#define SSU_BACKUP_TYPE_REG 0
#define SSU_BACKUP_TYPE_DIR 1
#define SSU_BACKUP_TYPE_OTHER 2
#define SSU_BACKUP_TYPE_CLEAR 3

//##### Define Command #####
#define SSU_BACKUP_ADD "add"
#define SSU_BACKUP_ADD_PATH "ssu_backup_add"

#define SSU_BACKUP_LS "ls"
#define SSU_BACKUP_LS_PATH "ls"

#define SSU_BACKUP_VI "vi"
#define SSU_BACKUP_VI_PATH "vi"

#define SSU_BACKUP_VIM "vim"
#define SSU_BACKUP_VIM_PATH "vim"

#define SSU_BACKUP_HELP "help"
#define SSU_BACKUP_HELP_PATH "ssu_backup_help"

#define SSU_BACKUP_EXIT "exit"

typedef enum SSU_Backup_Idx {
	USAGEIDX_ADD,
	USAGEIDX_REMOVE,
	USAGEIDX_RECOVER,
	USAGEIDX_LS,
	USAGEIDX_VI,
	USAGEIDX_VIM,
	USAGEIDX_HELP,
	USAGEIDX_EXIT,
	USAGEIDX_MAX
} SSU_BACKUP_IDX;

#endif