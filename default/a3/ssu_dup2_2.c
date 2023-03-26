#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>

#define BUFFER_SZIE 1024

int main(void)
{
	char buf[BUFFER_SIZE];
	char *fname = "ssu_test.txt";
	int fd;
	int length;

	if((fd = open(fname, O_RDONLY, 0644)) < 0) {
		fprintf(stderr, "open error for %s\n", fname);
		exit(1);
	}

	//Comment: stdout을 4번 디스크립터로 복사함
	if(dup2(1, 4) != 4){
		fprintf(stderr, "dup2 call failed\n");
		exit(1);
	}

	while (1) {
		//Comment: ssu_test.txt파일을 버퍼로 읽어들임.
		length = read(fd, buf, BUFFER_SIZE);

		//Comment: 파일 끝까지 계속 읽어들임
		if (length <= 0)
			break;

		//Comment: 4는 stdout이므로 읽어들인 텍스트가 stdout으로 출력됨.
		write(4, buf, length);
	}

	exit(0);
}
