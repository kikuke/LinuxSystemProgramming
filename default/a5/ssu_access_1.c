#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int main(int argc, char *argv[])
{
	int i;

	//Comment: 입력 인자가 없으면 사용법 출력
	if (argc < 2) {
		fprintf(stderr, "usage: %s <file1> <file2> .. <fileN>\n", argv[0]);
		exit(1);
	}

	//Comment: 들어온 인자수 만큼 반복
	for (i = 1; i < argc; i++) {
		//Comment: 파일이 존재하지 않는다면 다음 걸로
		if (access(argv[i], F_OK) < 0) {
			fprintf(stderr, "%s doesn't exist.\n", argv[i]);
			continue;
		}

		//Comment: 파일을 읽을 수 있다면 프린트
		if (access(argv[i], R_OK) == 0)
			printf("User can read %s\n", argv[i]);

		//Comment: 파일을 쓸 수 있다면 프린트
		if (access(argv[i], W_OK) == 0)
			printf("User can write %s\n", argv[i]);

		//Comment: 파일을 실행할 수 있다면 프린트
		if (access(argv[i], X_OK) == 0)
			printf("User can execute %s\n", argv[i]);
	}

	exit(0);
}
