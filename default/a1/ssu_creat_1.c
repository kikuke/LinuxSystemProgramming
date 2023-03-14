#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>

int main(void)
{
	char *fname = "ssu_test.txt";
	int fd;

	//fname이라는 이름의 파일을 생성하고 u,g,o 모두에게 r,w권한을 준다.
	//파일의 기본 권한 값은 0666
	//하지만 실제 생성된 파일은 umask(0002)로 인해 0664권한을 가짐
	//생성 실패 시 에러핸들링
	if ((fd = creat(fname, 0666)) < 0) {
		fprintf(stderr, "creat error for %s\n", fname);
		exit(1);
	}
	else {
		//디스크립터 값은 012를 제외한 다음부터 할당되기에
		//프로그램에서 이전에 열었던 파일이 없어 fd값은 3이 된다
		printf("Success!\nFilename : %s\nDescriptor : %d\n", fname, fd);
		//파일을 닫아줌
		close(fd);
	}
	
	exit(0);
}
