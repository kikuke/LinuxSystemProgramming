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

	//Comment: 두 동일한 파일을 644 권한 읽기 전용으로 연다.
	//	읽기만 하니 뒤의 인자는 생략 가능.
	fd1 = open(fname, O_RDONLY, 0644);
	fd2 = open(fname, O_RDONLY, 0644);

	if(fd1 < 0 || fd2 < 0){
		fprintf(stderr, "open error for %s\n", fname);
		exit(1);
	}

	//Comment: 파일을 25바이트 만큼 읽어들이고 끝을 '\0'으로 채워
	//	문자열 처럼 읽을 수 있게 한다.
	count = read(fd1, buf, 25);
	buf[count] = 0;
	printf("fd1's first printf : %s\n", buf);
	//Comment: 현재위치에서 한칸 이동한다. 공백(엔터)를 건너뛰었다.
	lseek(fd1, 1, SEEK_CUR);
	//Comment: 24바이트 읽고 문자열 처리한다,
	count = read(fd1, buf, 24);
	buf[count] = 0;
	printf("fd1's second printf : %s\n", buf);
	//Comment: 두 디스크립터는 같은 파일을 열었지만, 두 파일 디스크립터는 독립적으로 파일 오프셋을 가진다. 따라서 위와 동일한 결과가 나온다
	count = read(fd2, buf, 25);
	buf[count] = 0;
	printf("fd2's first printf : %s\n", buf);
	lseek(fd2, 1, SEEK_CUR);
	count = read(fd2, buf, 24);
	buf[count] = 0;
	printf("fd2's second printf : %s\n", buf);

	exit(0);
}
