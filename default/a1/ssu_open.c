#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>

int main(void)
{
	//open할 파일 이름
	char *fname = "ssu_test.txt";
	//open할 파일의 파일디스크립터
	int fd;
	
	//파일을 읽기전용으로 열고 파일디스크립터를 fd에 저장한다.
	//0보다 작을경우(-1)일경우 에러로 간주하고 에러핸들링
	if ((fd = open(fname, O_RDONLY)) < 0) {
		fprintf(stderr, "open error for %s\n", fname);
		exit(1);
	}
	else
		//파일디스크립터는 012를 제외한 다음값부터 할당되기때문에
		//이전에 열어둔 파일이 없으므로 다음값인 3이 할당된다.
		printf("Success!\nFilename : %s\nDescripter : %d\n", fname, fd);
	
	//main에서는 return과 차이가 없으나(return에서 내부적으로 호출)
	//다른 함수에서 호출시 그 뒤의 명령을 수행하지 않고 프로세스를 종료한다.
	//프로세스 종료 전 atexit에 등록된 함수를 실행한다.
	//_exit()는 atexit에 등록된 함수를 실행하지 않고 종료한다.
	exit(0);
}
