#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>

int main(void)
{
	char *fname = "ssu_test.txt";
	int fd;

	if ((fd = creat(fname, 0666)) < 0) {
		printf("creat error for %s\n", fname);
		exit(1);
	}

	printf("First printf is on the screen.\n");
	//Comment: ssu_test.txt파일을 열었던 디스크립터를 stdout이 있던 디스크립터를 닫고 복사함.
	dup2(fd, 1);
	//Comment: printf는 stdout으로 출력하는 함수이지만 지금은 stdout이 있던 자리가 fd 디스크립터의 복사본으로 되어있어 파일로의 출력이 이루어짐.
	printf("Second printf is in this file.\n");
	exit(0);
}
