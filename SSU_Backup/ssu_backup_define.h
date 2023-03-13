#ifndef SSU_BACKUP_DEFINE_H
#define SSU_BACKUP_DEFINE_H

#define SSU_BACKUP_SHELL_NAME "20192388"
#define SSU_BACKUP_SHELL_MAX_BUF_SZ 16384

#define SSU_BACKUP_ROOT_DIR_NAME "backup"
#define SSU_BACKUP_MAX_PATH_SZ 4096
#define SSU_BACKUP_MAX_FILENAME 255
#define SSU_BACKUP_MKDIR_AUTH 0700

#define SSU_BACKUP_HASH_SET_FILE ".hash"
#define SSU_BACKUP_HASH_MD5 1
#define SSU_BACKUP_HASH_SHA1 2

//##### Define Command #####
#define SSU_BACKUP_ADD "add"
#define SSU_BACKUP_ADD_PATH ""

#define SSU_BACKUP_LS "ls"
#define SSU_BACKUP_LS_PATH "ls"

#define SSU_BACKUP_VI "vi"
#define SSU_BACKUP_VI_PATH "vi"

#define SSU_BACKUP_VIM "vim"
#define SSU_BACKUP_VIM_PATH "vim"

#define SSU_BACKUP_EXIT "exit"

typedef enum SSU_Backup_Idx {
	USAGEIDX_MAX
} SSU_BACKUP_IDX;

#endif
