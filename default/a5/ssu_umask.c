#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/stat.h>

//Comment: 파일을 생성할 때 모든 유형에 대해 읽기, 쓰기만 권한줌.
#define RW_MODE (S_IRUSR|S_IWUSR|S_IRGRP|S_IWGRP|S_IROTH|S_IWOTH)

int main(void)
{
	char *fname1 = "ssu_file1";
	char *fname2 = "ssu_file2";

	//Comment: umask를 0으로 줬으므로 이후 생성은 권한을 준대로 부여됨
	umask(0);

	//Comment: 모든 유형에 대해 읽기 쓰기 권한만 준형태로 생성.
	if(creat(fname1, RW_MODE) < 0) {
		fprintf(stderr, "creat error for %s\n", fname1);
		exit(1);
	}
	//Comment: umask를 0066으로 줬음
	umask(S_IRGRP | S_IWGRP | S_IROTH | S_IWOTH);

	//Comment: 앞서 umaskfmf 0066으로 설정해서 0666 & ~0066이므로 0600의 권한을 가진 파일이 생긴다.
	if(creat(fname2, RW_MODE) < 0) {
		fprintf(stderr, "creat error for %s\n", fname2);
		exit(1);
	}

	//Comment: umask는 프로세스가 종료될 때 까지 유효하므로 이 프로그램이 종료되면 원래 umask로 돌아간다.
	exit(0);
}
