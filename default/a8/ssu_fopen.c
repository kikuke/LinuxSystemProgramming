#include <stdio.h>
#include <stdlib.h>

int main(void)
{
	char *fname = "ssu_test.txt";
	char *mode = "r";

	//Comment: 파일을 읽기모드로 열음.
	//	파일이 없다면 에러
	if (fopen(fname, mode) == NULL) {
		fprintf(stderr, "fopen error for %s\n", fname);
		exit(1);
	}
	else
		printf("Success!\nFilename: <%s>, mode: <%s>\n", fname, mode);

	exit(0);
}
