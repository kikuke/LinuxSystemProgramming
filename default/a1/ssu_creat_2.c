#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>

int main(void)
{
	char *fname = "ssu_test.txt";
	int fd;

	//fname으로 파일을 0666권한으로 생성한다.
	//이때 umask는 0002라서 0664로 파일의 권한이 설정된다.
	//u:rw, g:rw, o:r
	//0보다 작을경우(-1) 파일 생성에 실패했다는 의미이고
	//그럴경우 에러발생을 알리며 프로그램을 종료한다.
	if ((fd = creat(fname, 0666)) < 0) {
		fprintf(stderr, "creat error for %s\n", fname);
		exit(1);
	}
	else {
		//파일을 닫는다. creat로 생성된 파일은 쓰기전용만 되기때문에
		//읽기위해서 이후 open을 이용해 읽기권한을 추가시킨다
		//파일은 바로 닫히지만 디스크립터는 바로 반환되지 않는다.
		close(fd);
		//이후 생성된 파일을 읽고, 쓰기 모드로 연다
		//반환 값은 파일디스크립터
		fd = open(fname, O_RDWR);
		printf("Succeeded!\n<%s> is new readable and writable\n", fname);
	}

	exit(0);
}
