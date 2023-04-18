#include <stdio.h>
#include <stdlib.h>

#define BUFFER_SIZE 1024

int main(void)
{
	char *fname = "ssu_test.dat";
	char name[BUFFER_SIZE];
	FILE *fp;
	int age;

	//읽기 전용으로 파일 오픈
	fp = fopen(fname, "r");
	//파일에서 문자열과 정수 한 라인을 읽어들여 각 메모리에 저장한다.
	fscanf(fp, "%s%d", name, &age);
	fclose(fp);
	//다시 똑같은 파일을 쓰기 전용으로 오픈한다
	fp = fopen(fname, "w");
	//문자열을 위에서 읽어들인 값을 이용해 아래와 같이 쓰기한다.
	fprintf(fp, "%s is %d years old\n", name, age);
	fclose(fp);
	exit(0);
}
