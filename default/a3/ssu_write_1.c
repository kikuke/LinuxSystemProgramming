#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#define BUFFER_SIZE 1024

int main(void)
{
	char buf[BUFFER_SIZE];
	int length;

	//Comment: 최대 버퍼의 크기만큼 stdin에서 읽어들이고 버퍼에 저장, 입력 길이 저장.
	length = read(0, buf, BUFFER_SZIE);
	//Comment: stdout으로 입력한 길이만큼 버퍼에서 출력
	write(1, buf, length);
	exit(0);
}
