#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>

int main(int argc, char *argv[])
{
	int fd;

	if (argc != 3) {
		fprintf(stderr, "usage: %s <oldname> <newname>\n", argv[0]);
		exit(1);
	}

	//Comment: 최초 파일 오픈
	if((fd = open(argv[1], O_RDONLY)) < 0){
		fprintf(stderr, "first open error for %s\n", argv[1]);
		exit(1);
	}
	else
		close(fd);

	//Comment: 해당 파일의 이름을 변경.
	if(rename(argv[1], argv[2]) < 0) {
		fprintf(stderr, "rename error\n");
		exit(1);
	}

	//Comment: 파일의 이름이 변경되었으므로 에러나야 정상
	if((fd = open(argv[1], O_RDONLY)) < 0)
		printf("second open error for %s\n", argv[1]);
	else {
		fprintf(stderr, "it's very stange!\n");
		exit(1);
	}

	//Comment: 바뀐 이름으로 변경
	if ((fd = open(argv[2], O_RDONLY)) < 0) {
		fprintf(stderr, "third open error for %s\n", argv[2]);
		exit(1);
	}

	printf("Everyting is good!\n");
	exit(0);
}
