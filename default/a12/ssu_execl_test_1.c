#include <stdio.h>
#include <stdlib.h>

int main(int argc, char *argv[])
{
    extern char **environ;
    char **str;
    int i;

    //받은 인자들을 출력
    for (i = 0; i < argc; i++)
        printf("argv[%d]: %s\n", i, argv[i]);
    
    //상속된 환경 변수값들을 출력
    for(str = environ; *str != 0; str++)
        printf("%s\n", *str);

    exit(0);
}