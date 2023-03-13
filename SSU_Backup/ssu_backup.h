#ifndef SSU_BACKUP_H
#define SSU_BACKUP_H

void backup_usage();
void ShowShell();
int GetSelectHash();
int execute_cmd(char* cmd);
//Comment: 스트링을 parser로 파싱한 후 destArray에 동적할당 한 배열을 넘김.
//	리턴값은 배열의 크기.
//	배열 끝엔 NULL을 채워서 줌.
int StringToArray(char* srcStr, char*** destArr, const char* parser);
void fork_exec_cmd(const char* p_name, char* const* cmd);

#endif
