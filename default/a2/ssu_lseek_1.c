#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>

int main(void)
{
	char *fname = "ssu_test.txt";
	off_t fsize;
	int fd;

	//Comment: 파일을 읽기 전용으로 열었음.
	if((fd = open(fname, O_RDONLY)) < 0){
		fprintf(stderr, "open error for %s\n", fname);
		exit(1);
	}

	//Comment: 파일의 끝으로 이동. 즉 파일의 길이를 알게됨.
	if((fsize = lseek(fd, 0, SEEK_END)) < 0){
		fprintf(stderr, "lseek error\n");
		exit(1);
	}

	printf("The size of<%s> is %ld bytes.\n", fname, fsize);

	exit(0);
}
