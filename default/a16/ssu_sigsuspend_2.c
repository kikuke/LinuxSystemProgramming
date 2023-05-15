#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <signal.h>

//내부에서 ssu_print_mask를 호출함
static void ssu_func(int signo);
//현재 프린트 마스크를 가져와서 출력함
//  이전의 에러상태를 보존함
void ssu_print_mask(const char *str);

int main(void)
{
    sigset_t new_mask, old_mask, wait_mask;

    //현재 프로그램 마스크 상태를 출력
    ssu_print_mask("program start: ");

    //현재 비트 마스크 출력하는 핸들러 들록
    if (signal(SIGINT, ssu_func) == SIG_ERR) {
        fprintf(stderr, "signal(SIGINT) error\n");
        exit(1);
    }

    //suspend용 마스크에 SIGUSR1을 세팅
    sigemptyset(&wait_mask);
    sigaddset(&wait_mask, SIGUSR1);
    //새로 바꿀 마스크에 SIGINT 마스크 세팅
    sigemptyset(&new_mask);
    sigaddset(&new_mask, SIGINT);

    //예전 정보 old_mask에 담고 SIGINT 마스크 세팅
    if (sigprocmask(SIG_BLOCK, &new_mask, &old_mask) < 0) {
        fprintf(stderr, "SIG_BLOCK() error\n");
        exit(1);
    }

    //SIGINT가 세팅된 상태
    ssu_print_mask("in critical region: ");

    //버리고 SIGUSR1마스크를 잠깐 씀
    if (sigsuspend(&wait_mask) != -1) {
        fprintf(stderr, "sigsuspend() error\n");
        exit(1);
    }

    //현재 상태 프린트 suspend 벗어났으므로 SIGINT마스크가 다시 세팅된 상태
    ssu_print_mask("after return from sigsuspend: ");

    //다시 예전으로 돌아감
    if (sigprocmask(SIG_SETMASK, &old_mask, NULL) < 0) {
        fprintf(stderr, "SIG_SETMASK() error\n");
        exit(1);
    }

    //이를 프린트
    ssu_print_mask("proigram exit: ");
    exit(0);
}

void ssu_print_mask(const char *str) {
    sigset_t sig_set;
    int err_num;

    err_num = errno;

    //현재 시그널 마스크를 가져옴
    if (sigprocmask(0, NULL, &sig_set) < 0) {
        fprintf(stderr, "sigprocmask() error\n");
        exit(1);
    }
    //프린트
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
    errno = err_num;
}

static void ssu_func(int signo) {
    ssu_print_mask("\nin ssu_func: ");
}