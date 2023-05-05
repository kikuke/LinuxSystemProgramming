#ifndef SSU_MONITOR_SYSTEM_H
#define SSU_MONITOR_SYSTEM_H

//  sighandler_t는 _GNU_SOURCE가 정의 됐을 때 사용가능하므로
#ifndef _GNU_SOURCE
typedef void (*sighandler_t)(int);
#endif

//해당 문자열이 공백인지확인.
//  맞다면 1, 아니라면 0, 에러시 -1 리턴
int isBlank(char *str);

//입력받은 srcStr을 parser를 기준으로 토큰화 후, argv에 동적할당 된 배열을 넘김
//  리턴은 argc. 오류시 0 리턴. 실제 배열 크기는 argc+1이다.
//  배열 끝엔 NULL을 채워서 줌
int StringToArgv(char *srcStr, char ***argv, const char *parser);

//인자로 들어온 함수를 main함수 취급하여 system을 실행한 것과 같은 효과를 낸다.
//  해당 함수의 실행이 끝날 때 까지 대기한다
//  실행이 끝난 후 check_statloc의 결과값을 리턴한다.
//  실패시 -1, 성공시 0을 리턴한다.
int virtual_system(int (*exec_proc)(int argc, char *argv[]), int argc, char *argv[]);

//정상 종료이고, exit인자가 0을 넘겨줬을 경우 0, 그이외에 1 리턴
//  오류사항 표준에러로 출력
int check_statloc(int stat_loc);

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
//  hupAction은 SIGHUP 시그널이 왔을 경우 호출할 핸들러를 넣는다. 딱히 없다면 SIG_IGN 넣기
//  성공시 0, 실패시 -1이 됨.
int change_daemon(const char *ident, sighandler_t hupAction);

#endif