#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/stat.h>

int main(void)
{
	struct stat statbuf;
	char *fname1 = "ssu_file1";
	char *fname2 = "ssu_file2";

	if (stat(fname1, &statbuf) < 0)
		fprintf(stderr, "stat error %s\n", fname1);

	//Comment: 그룹의 실행권한을 삭제하고, setuserid 비트를 활성화 한다.
	//	suid활성화로 인해 user에 실행권한이 없다면 S로 표기, user에 실행권한이 있다면 s로 대체되어 표기된다.
	if (chmod(fname1, (statbuf.st_mode & ~S_IXGRP) | S_ISUID) < 0)
		fprintf(stderr, "chmod error %s\n", fname1);

	//Comment: 유저 읽기 쓰기 그룹 읽기, 기타 읽기 실행으로 접근권한을 변경한다.
	if (chmod(fname2, S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH | S_IXOTH) < 0)
		fprintf(stderr, "chmode error %s\n", fname2);

	exit(0);
}
