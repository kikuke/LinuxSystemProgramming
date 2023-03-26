#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>

#define S_MODE 0644
#define BUFFER_SIZE 1024

int main(int argc, char *argv[])
{
	char buf[BUFFER_SIZE];
	int fd1, fd2;
	int length;

	//Comment: 인자를 두개 받는 프로그램
	if (argc != 3) {
		fprintf(stderr, "Usage : %s filein fileout\n", argv[0]);
		exit(1);
	}

	//Comment: 첫번째 인자를 읽기전용으로
	if((fd1 = open(argv[1], O_RDONLY)) < 0) {
		fprintf(stderr, "open error for %s\n", argv[1]);
		exit(1);
	}

	//Comment: 두번째 인자를 쓰기전용으로, 파일이 있다면 덮어쓰기
	if((fd2 = open(argv[2], O_WRONLY | O_CREAT | O_TRUNC, S_MODE)) < 0) {
		fprintf(stderr, "open error for %s\n", argv[2]);
		exit(1);
	}

	//Comment: 최대 읽기를 BUFFER_SIZE만큼, 파일 끝까지 읽어가며 두번째 인자의 경로 위치로 쓰기
	while((length = read(fd1, buf, BUFFER_SIZE)) > 0)
		write(fd2, buf, length);

	exit(0);
}
