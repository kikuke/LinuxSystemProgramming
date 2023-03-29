#ifndef SSU_BACKUP_H
#define SSU_BACKUP_H

//Comment: 사용법을 출력한다
void backup_usage();

//Comment: 쉘을 출력한다.
void ShowShell();

//Comment: 입력한 해시값을 가져온다.
int GetSelectHash();

//Comment: 명령을 실행한다
int execute_cmd(char* cmd);

//Comment: 스트링을 parser로 파싱한 후 destArray에 동적할당 한 배열을 넘김.
//	리턴값은 배열의 크기.
//	배열 끝엔 NULL을 채워서 줌.
int StringToArray(char* srcStr, char*** destArr, const char* parser);

//Comment: 포크후 exec를 실행해 인자를 넘긴다.
void fork_exec_cmd(const char* p_name, char* const* cmd);

#endif
