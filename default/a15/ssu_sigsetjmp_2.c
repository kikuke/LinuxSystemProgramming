#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <setjmp.h>
#include <signal.h>
#include <time.h>

//현재 마스크 출력
static void ssu_alarm(int signo);
//3초후 알람울리기, 5초동안 멈추기, 마스크 전후로 출력
static void ssu_func(int signo);
//이전 에러를 저장하며 현재 세팅된 마스크를 출력함
void ssu_mask(const char *str);

//원자형 정수 플래그 사용
static volatile sig_atomic_t can_jump;
static sigjmp_buf jump_buf;

int main(void)
{
    if (signal(SIGUSR1, ssu_func) == SIG_ERR) {
        fprintf(stderr, "SIGUSR1 error");
        exit(1);
    }

    if (signal(SIGALRM, ssu_alarm) == SIG_ERR) {
        fprintf(stderr, "SIGALRM error");
        exit(1);
    }

    //현재 마스크 출력
    ssu_mask("starting main: ");

    //ssufunc에서 롱점프 되면 종료됨
    //이전 마스크로 복구
    if (sigsetjmp(jump_buf, 1)) {
        ssu_mask("ending main: ");
        exit(0);
    }

    //이때부터 점프 가능
    can_jump = 1;

    //깨도 다시 재우기
    while (1)
        pause();

    exit(0);
}

void ssu_mask(const char *str) {
    sigset_t sig_set;
    int err_num;

    //함수 호출 전 에러를 저장
    err_num = errno;

    if (sigprocmask(0, NULL, &sig_set) < 0) {
        printf("sigprocmask() error");
        exit(1);
    }

    printf("%s", str);

    if (sigismember(&sig_set, SIGINT))
        printf("SIGINT ");
    
    if (sigismember(&sig_set, SIGQUIT))
        printf("SIGQUIT ");
    
    if (sigismember(&sig_set, SIGUSR1))
        printf("SIGUSR1 ");
    
    if (sigismember(&sig_set, SIGALRM))
        printf("SIGALRM ");
    
    printf("\n");
    //함수 호출 전 에러를 복원
    errno = err_num;
}

static void ssu_func(int signo) {
    time_t start_time;

    if (can_jump == 0)
        return;

    //현재 설정된 마스크들 출력
    ssu_mask("starting ssu_func: ");
    //3초후 알람
    alarm(3);
    start_time = time(NULL);

    //5초 지날때 까지 대기
    while(1)
        if(time(NULL) > start_time +5)
            break;

    //현재 설정된 마스크들 출력
    ssu_mask("ending ssu_func: ");
    //플래그 0으로 설정
    can_jump = 0;
    //점프
    siglongjmp(jump_buf, 1);
}

static void ssu_alarm(int signo) {
    ssu_mask("in ssu_alarm: ");
}