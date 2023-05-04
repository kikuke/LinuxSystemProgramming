#ifndef SSU_MONITOR_DAEMON_H
#define SSU_MONITOR_DAEMON_H

/* 데몬 프로세스의 조건
1. init 프로세스의 자식프로세스 -> PPID가 1번(INIT)이다 -> 어떤 프로세스의 영향도 받지 않는다.
2. 제어 터미널과 연결 해제 -> 터미널을 가지지 않는다. -> 인터페이스가 없다(표준입출력)

일반적인 조건 - 출처: https://en.wikipedia.org/wiki/Daemon_(computing)
1. 선택적 불필요한 변수 제거
2. 포크 및 종료로 백그라운드 작업 실행. 고아가 되어 PPID가 INIT프로세스가 됨.
3. 단일 작업 수행 호출 세션에서 분리 - setsid() -> 제어 tty에서 분리, 새 세션의 리더, 새로운 로그인상태?
    https://www.joinc.co.kr/w/man/2/setsid
4. 루트 디렉토리를 현재 작업디렉토리로 설정해 사용 중인 디렉토리를 유지하지 않도록 함. 언마운트 등을 위해
5. umask를 0으로 변경해 자체 권한 마스크를 제공하고, umask에 의존하지 않도록함.
6. 죽지 않는 표준스트림 3개에 대해 fd를 /dev/null 또는 logfile로 리다이렉션 후 상위 프로세스에서 상속된 모든 fd를 닫는다.
*/

//해당 프로세스가 종료되며 자식프로세스가 daemon 프로세스가 되어 대신 실행함
//  인자로 /var/log/syslog에 로그가 생성될 때 붙는 prefix를 받는다.
//  성공시 0, 실패시 -1이 됨.
int change_daemon(const char *ident);

#endif