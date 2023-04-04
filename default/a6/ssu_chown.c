#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>

int main(void)
{
	struct stat statbuf;
	char *fname = "ssu_myfile";
	int fd;

	//Comment: 파일을 읽기/쓰기 전용으로 이어서 쓰고 없으면 생성하여 연다.
	//	권한은 유저만 읽기/쓰기 
	if ((fd = open (fname , O_RDWR | O_CREAT, 0600)) < 0) {
		fprintf(stderr, "open error for %s\n", fname);
		exit(1);
	}

	close(fd);
	stat(fname, &statbuf);
	//Comment: chown으로 소유자 변경 전 uid, gid 체크
	printf("# 1st stat call # UID:%d	GID:%d\n", statbuf.st_uid, statbuf.st_gid);

	//Comment: chown으로 유저 소유자를 501 그룹 소유자를 300으로 변경
	if (chown(fname, 501, 300) < 0) {
		fprintf(stderr, "chown error for %s\n", fname);
		exit(1);
	}

	stat(fname, &statbuf);
	//Comment: 바뀐 결과 프린트 501, 300이 찍혀있다.
	printf("# 2nd stat call # UID:%d	GID:%d\n", statbuf.st_uid, statbuf.st_gid);

	//Comment: 해당 파일 삭제
	if (unlink(fname) < 0) {
		fprintf(stderr, "unlink error for %s\n", fname);
		exit(1);
	}

	exit(0);
}
