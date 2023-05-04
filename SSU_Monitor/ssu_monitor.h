#ifndef SSU_MONITOR_H
#define SSU_MONITOR_H

#define SSU_MONITOR_SHELL_BUF_SZ 16384
#define SSU_MONITOR_SHELL_NAME "20192388>"

//쉘을 출력
void ShowShell();

//cmd를 실행
//  cmd 버퍼에 변형이 일어남
//  실패시 -1, 정상 실행 시 0, 종료 신호 시 1 리턴
int execute_cmd(char *cmd);

#endif