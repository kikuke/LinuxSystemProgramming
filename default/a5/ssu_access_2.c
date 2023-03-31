#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

//Comment: 컴파일러 마다 이 동작은 다르지만 gcc는
//	sizeof에 배열이 온다면 배열의 크기 * 배열의[0]번째 요소의 크기로 계산한다
//	따라서 아래의 수식을 이용하면 배열의 크기를 알 수 있다.
#define TABLE_SIZE (sizeof(table)/sizeof(*table))

int main(int argc, char *argv[])
{
	//Comment: 출력할 이름과, access에 이용할 mode
	struct {
		char *text;
		int mode;
	} table[] = {
		{"exists", 0},
		{"execute", 1},
		{"write", 2},
		{"read", 4},
	};
	int i;

	//Comment: 인자는 두개 이상이어야 한다.
	if (argc < 2) {
		fprintf(stderr, "usage : %s <file>\n", argv[0]);
		exit(1);
	}

	//Comment: 배열의 길이만큼 반복
	for(i = 0; i < TABLE_SIZE; i++) {
		//Comment: 해당 권한들이 있는지 struct를 돌면서 프린트 한다.
		if(access(argv[1], table[i].mode) != -1)
			printf("%s -ok\n", table[i].text);
		else
			printf("%s\n", table[i].text);
	}
	
	exit(0);
}
