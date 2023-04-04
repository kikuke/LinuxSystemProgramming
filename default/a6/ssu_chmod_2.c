#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/stat.h>

//Comment: 실행 권한만 주는 매크로
#define MODE_EXEC (S_IXUSR|S_IXGRP|S_IXOTH)

int main(int argc, char *argv[])
{
	struct stat statbuf;
	int i;

	if(argc < 2) {
		fprintf(stderr, "usage: %s <file1> <file2> ... <fileN>\n", argv[0]);
		exit(1);
	}

	//Comment: 프로그램 이름을 제외한 인자만큼 반복
	for (i = 1; i < argc; i++){
		if (stat(argv[i], &statbuf) < 0) {
			fprintf(stderr, "%s : stat error\n", argv[i]);
			continue;
		}

		//Comment: 실행 권한들 추가
		statbuf.st_mode |= MODE_EXEC;
		//Comment: 그룹, 기타 실행권한 토글. 위에서 무조건 true가 됐으므로 0이됨.
		statbuf.st_mode ^= (S_IXGRP|S_IXOTH);
		//Comment: 설정한 권한 반영
		if (chmod(argv[i], statbuf.st_mode) < 0)
			fprintf(stderr, "%s : chmod error\n", argv[i]);
		else
			printf("%s : file permission was changed.\n", argv[i]);
	}
	exit(0);
}
