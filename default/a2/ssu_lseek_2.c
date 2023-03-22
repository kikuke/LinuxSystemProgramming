#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>

//Comment: 파일을 644로 생성
#define CREAT_MODE (S_IRUSR | S_IWUSR| S_IRGRP | S_IROTH)

char buf1[] = "1234567890";
char buf2[] = "ABCDEFGHIJ";

int main(void)
{
	char *fname = "ssu_hole.txt";
	int fd;

	//Comment: 파일을 644로 생성한다
	if((fd = creat(fname, CREAT_MODE)) < 0){
		fprintf(stderr, "creat error for %s\n", fname);
		exit(1);
	}

	//Comment: 파일을 12바이트 쓴다. 그러나 buf1은 11바이트이다.
	//	남은 1바이트는 리눅스는 공백처리한다.
	if(write(fd, buf1, 12) != 12){
		fprintf(stderr, "buf1 write error\n");
		exit(1);
	}

	//Comment: 파일의 시작으로부터 15000바이트 건너뛴다.
	//	파일 홀이 생기는데 15000바이트 건너뛰면서 빈 부분을 공백으로채우게 된다.
	if(lseek(fd, 15000, SEEK_SET) < 0){
		fprintf(stderr, "lseek error\n");
		exit(1);
	}

	//Comment: 15000바이트 이후에서 buf2를 쓴다.
	//	이 역시 11바이트인데 12바이트 써서 남은 1바이트는 공백처리한다.
	if(write(fd, buf2, 12) != 12) {
		fprintf(stderr, "buf2 write error\n");
		exit(1);
	}

	exit(0);
}
