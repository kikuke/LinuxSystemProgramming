#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>

#define BUFFER_SIZE 1024

int main(void)
{
	char buf[BUFFER_SIZE];
	char *fname = "ssu_test.txt";
	int count;
	int fd1, fd2;

	if((fd1 = open(fname, O_RDONLY, 0644)) < 0) {
		fprintf(stderr, "open error for %s\n", fname);
		exit(1);
	}

	//Comment: 열었던 파일을 fd2에도 똑같이 참조(디스크립터) 복사
	fd2 = dup(fd1);
	//Comment: 12바이트를 읽는다.
	count = read(fd1, buf, 12);
	buf[count] = 0;
	printf("fd1's printf : %s\n", buf);
	//Comment: 현재 포인터에서 한칸 이동.
	lseek(fd1, 1, SEEK_CUR);
	//Comment: fd1의 포인터 이동이 fd2디스크립터에도 반영이 된것을 알수있음.
	//	두 디스크립터가 같은 디스크립터임을 알수 있음.
	count = read(fd2, buf, 12);
	buf[count] = 0;
	printf("fd2's printf : %s\n", buf);
	exit(0);
}
