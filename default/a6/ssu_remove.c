#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int main(int argc, char *argv[])
{
	if(argc != 3) {
		fprintf(stderr, "usage: %s <oldname> <newname>\n", argv[0]);
		exit(1);
	}

	//Comment: 첫번째 인자를 두번째 인자로 하드링크 한다.
	if (link(argv[1], argv[2]) < 0){
		fprintf(stderr, "link error\n");
		exit(1);
	}
	else
		printf("step1 passed.\n");

	//Comment: 디렉토리일 경우 rmdir, 파일일 경우 unlink를 호출한다
	//	디렉토리가 비지 않았을 경우 에러발생.
	//	파일이 완전 삭제되지 않는다.
	//	argv[2]로 하드링크된 파일이 있으므로.
	if(remove(argv[1]) < 0){
		fprintf(stderr, "remove error\n");
		remove(argv[2]);
		exit(1);
	}
	else
		printf("step2 passed.\n");

	printf("Success!\n");
	exit(0);
}
