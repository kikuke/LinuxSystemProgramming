#ifndef SSU_MONITOR_H
#define SSU_MONITOR_H

#define SSU_MONITOR_SHELL_BUF_SZ 16384
#define SSU_MONITOR_SHELL_NAME "20192388>"

#define SSU_MONITOR_ADD "add"
#define SSU_MONITOR_DELETE "delete"
#define SSU_MONITOR_TREE "tree"
#define SSU_MONITOR_EXIT "exit"

//쉘을 출력
void ShowShell();

//cmd를 실행
//  cmd 버퍼에 변형이 일어남
//  실패시 -1, 정상 실행 시 0, 종료 신호 시 1 리턴
int execute_cmd(char *cmd);

//인자로 들어온 함수를 main함수 취급하여 exec를 실행한 것과 같은 효과를 낸다.
//  해당 함수의 실행이 끝날 때 까지 대기한다
//  실패시 -1, 성공시 0을 리턴한다.
int virtual_exec(int (*exec_proc)(int argc, char *argv[]), int argc, char *argv[]);

//정상 종료이고, exit인자가 0을 넘겨줬을 경우 0, 그이외에 1 리턴
//  오류사항 표준에러로 출력
int check_statloc(int stat_loc);

#endif