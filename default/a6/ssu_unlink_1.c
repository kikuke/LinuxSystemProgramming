#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>

int main(void)
{
	char *fname = "ssu_dump.txt";

	//Comment: 파일이 있는지 확인 용도로 열어봄.
	if (open(fname, O_RDWR) < 0) {
		fprintf(stderr, "open error for %s\n", fname);
		exit(1);
	}

	//Comment: 해당 파일을 언링크함. 링크 계수가 1 감소함.
	if (unlink(fname) < 0) {
		fprintf(stderr, "unlink error for %s\n", fname);
		exit(1);
	}

	printf("File unlinked\n");
	//Comment: 파일의 삭제 실반영시간에는 딜레이가 있기에 기다림.
	sleep(20);
	printf("Done\n");
	exit(0);
}
