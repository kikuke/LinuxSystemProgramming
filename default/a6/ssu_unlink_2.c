#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>

int main(void)
{
	char buf[64];
	char *fname = "ssu_tempfile";
	int fd;
	int length;

	//Comment: 파일을 생성하거나 연다.
	if ((fd = open(fname, O_RDWR | O_CREAT | O_TRUNC, 0600)) < 0) {
		fprintf(stderr, "first open error for %s\n", fname);
		exit(1);
	}

	//Comment: 열린 파일을 unlink한다.
	//	하지만 파일이 삭제되는 시점은 참조카운트가 0일때이다.
	//	이로써 이후 파일들은 해당 파일명으로 접근할 수 없지만
	//	이미 열린 파일은 아직 파일이 삭제되진 않았기에 사용가능하다.
	if(unlink(fname) < 0) {
		fprintf(stderr, "unlink error for %s\n", fname);
		exit(1);
	}

	//Comment: 파일을 쓴다. 12바이트 썼으니 12바이트보다 작아도 0으로 채워서
	//	12바이트 나와야한다. 같지 않으면 에러.
	if (write(fd, "How are you?", 12) != 12) {
		fprintf(stderr, "write error\n");
		exit(1);
	}

	//Comment: 파일 포인터를 맨첨으로 옮긴다
	lseek(fd, 0, 0);

	//Comment: 적었던 글을 읽는다. 파일이 언링크 됐지만
	//	이미 참조하고 있던거라 가능하다
	if((length = read(fd, buf, sizeof(buf))) < 0){
		fprintf(stderr, "buf read error\n");
		exit(1);
	}
	else
		buf[length] = 0;

	printf("%s\n", buf);
	//Comment: 파일을 닫는다
	close(fd);

	//Comment: 위에서 해당 파일을 언링크 했기에 해당 파일 이름으로는 접근할 수 없다.
	if((fd = open(fname, O_RDWR)) < 0) {
		fprintf(stderr, "second open error for %s\n", fname);
		exit(1);
	}
	else
		close(fd);

	exit(0);
}
