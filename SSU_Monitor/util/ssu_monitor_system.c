#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <fcntl.h>
#include <syslog.h>
#include <string.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <sys/resource.h>

#include "ssu_monitor_define.h"
#include "ssu_monitor_system.h"

int isBlank(char *str)
{
    if(str == NULL)
        return -1;

    while(*str != '\0') {
        if(*str != ' ')
            break;
        str++;
    }
    if(*str == '\0')
        return 1;

    return 0;
}

int StringToArgv(char *srcStr, char ***argv, const char *parser)
{
	int arrSz=1;
	char *tok_ptr = NULL;

    if(srcStr == NULL) {
        return 0;
    }

	tok_ptr = strtok(srcStr, parser);
	while(tok_ptr != NULL) {
		arrSz++;
		tok_ptr = strtok(NULL, parser);
	}

	(*argv) = (char**)malloc(sizeof(char*) * arrSz);
	tok_ptr = srcStr;
	for(int i=0; i < (arrSz - 1); i++){
		(*argv)[i] = tok_ptr;
		while((*tok_ptr) != '\0'){
			tok_ptr++;
		}
		tok_ptr++;
	}
	(*argv)[arrSz-1] = NULL;

	return arrSz-1;
}

int check_statloc(int stat_loc)
{
    int ret = 0;

    if(WIFEXITED(stat_loc)) {
        if((ret = WEXITSTATUS(stat_loc)) == 0)
            return 0;
        
        fprintf(stderr, "get status value is %d\n", ret);
        return -1;
    } else if(WIFSIGNALED(stat_loc)) {
        fprintf(stderr, "get signal is %d\n", WTERMSIG(stat_loc));
        return -1;
    } else if(WIFSTOPPED(stat_loc)) {
        fprintf(stderr, "stop signal is %d\n", WSTOPSIG(stat_loc));
        return -1;
    }

    perror("check_statloc()");
    return -1;
}

int virtual_system(int (*exec_proc)(int argc, char *argv[]), int argc, char *argv[])
{
    pid_t pid = 0;
    int stat_loc = 0;
    int ret = 0;

    //자식 프로세스인 경우
    if((pid = fork()) == 0) {
        ret = exec_proc(argc, argv);
        //보통 여기까지 실행될 일 없음. exec_proc에서 exit로 종료하므로.
        //메인에서 exit 대신 return을 사용했을 경우 이걸로 exit됨.
        exit(ret);
    } else if(pid < 0) {
        perror("fork()");
        return -1;
    }

    //부모 프로세스인 경우
    if(waitpid(pid, &stat_loc, 0) < 0){
        perror("waitpid()");
        return -1;
    }

    //stat_loc 값 체크
    return check_statloc(stat_loc);
}

int change_daemon(const char *ident, sighandler_t hupAction)
{
    pid_t pid;
    struct rlimit rl;

    //부모 프로세스(자신)을 종료시켜 자신을 백그라운드로 실행시킨다.
    //  이때 PPID는 1(Init 프로세스)이 된다.
    if((pid = fork()) < 0) {
        perror("fork()");
        return -1;
    } else if (pid != 0) {
        exit(0);
    }

    //새로운 세션을 만들고, 해당 세션의 리더가 된다.
    //  터미널 그룹에서 벗어나게 된다.
    if(setsid() < 0) {
        perror("setsid()");
        return -1;
    }

    //HANGUP(터미널과 연결이 끊겼을 때 하위 프로세스들에 전달; 데몬 프로세스에서는 재시작(환경설정) 할 때 사용)
    //  SIGHUP 시그널이 왔을 경우 해당 핸들러를 실행한다.
    if(signal(SIGHUP, hupAction) == SIG_ERR) {
        perror("signal()");
        return -1;
    }

    //현재 작업 디렉토리를 루트디렉토리로 옮겨 이전의 작업디렉토리가 unmount 가능하게 함
    if(chdir("/") < 0) {
        perror("chdir()");
        return -1;
    }

    //이전의 umask에 의존하지 않게 한다.
    umask(SSU_MONITOR_UMASK);

    //최대로 열수있는 파일 디스크립터의 수 가져옴
    if(getrlimit(RLIMIT_NOFILE, &rl) < 0) {
        perror("getrlimit()");
        return -1;
    }

    //-1(무한대) 일경우 임의 수 지정
    if(rl.rlim_max == RLIM_INFINITY) {
        rl.rlim_max = 1024;
    }

    //로그를 연다. 표준입출력과 터미널이 닫힌상태이므로 syslog 이용.
    //ident는 /var/log/syslog에 생성될 때 붙는 prefix
    //LOG_CONS: 시스템 로거로 보내는 동안 오류가 있으면 시스템 콘솔에 출력
    //LOG_DAEMON: 별도 기능 값이 없는 데몬 유형
    openlog(ident, LOG_CONS, LOG_DAEMON);

    //열려있는 모든 fd를 닫음
    for(int i=0; i < rl.rlim_max; i++) {
        if(close(i) < 0) {
            syslog(LOG_ERR, "close failed fd: %d\n", i);
            return -1;
        }
    }
    //0,1,2 표준 입출력 fd를 /dev/null 터미널로 리다이렉션
    if(open("/dev/null", O_RDWR) < 0) {
        syslog(LOG_ERR, "open fd0 /dev/null failed\n");
        return -1;
    }
    if(dup(0) < 0){
        syslog(LOG_ERR, "dup fd1 /dev/null failed\n");
        return -1;
    }
    if(dup(0) < 0){
        syslog(LOG_ERR, "dup fd2 /dev/null failed\n");
        return -1;
    }

    //로그 닫기
    closelog();
    return 0;
}