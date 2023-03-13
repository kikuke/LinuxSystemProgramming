#ifndef SSU_BACKUP_H
#define SSU_BACKUP_H

void backup_usage();
void ShowShell();
int GetSelectHash();
int execute_cmd(char* cmd);
void fork_exec_cmd(const char* p_name, const char* cmd);

#endif
