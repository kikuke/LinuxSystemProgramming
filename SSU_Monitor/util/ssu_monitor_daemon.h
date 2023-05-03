#ifndef SSU_MONITOR_DAEMON_H
#define SSU_MONITOR_DAEMON_H

//해당 프로세스가 종료되며 자식프로세스가 daemon 프로세스가 되어 대신 실행함
//  성공시 0, 실패시 -1이 됨.
int change_daemon();

#endif