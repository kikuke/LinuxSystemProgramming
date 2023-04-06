#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#define PATH_MAX 1024

int main(void)
{
    char *pathname;

    //Comment: 작업 디렉토리를 홈 디렉토리로 옮긴다.
    if (chdir("/home/kikuke") < 0) {
        fprintf(stderr, "chdir error\n");
        exit(1);
    }

    pathname = malloc(PATH_MAX);

    //Comment: 현재 작업디렉토리를 가져온다.
    //  홈 디렉토리가 출력된다.
    if (getcwd(pathname, PATH_MAX) == NULL) {
        fprintf(stderr, "getcwd error\n");
        exit(1);
    }

    printf("current directory = %s\n", pathname);
    exit(0);
}