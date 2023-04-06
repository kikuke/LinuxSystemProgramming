#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int main(void)
{
    //Comment: 공유폴더라 일반 권한도 접근 가능
    if (chdir("/etc") < 0) {
        fprintf(stderr, "chdir error\n");
        exit(1);
    }

    printf("chdir to /etc succeeded.\n");
    exit(0);
}