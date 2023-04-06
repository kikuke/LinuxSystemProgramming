#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <dirent.h>
#include <fcntl.h>
#include <string.h>
#include <sys/stat.h>

#define DIRECTORY_SIZE MAXNAMLEN

int main(int argc, char *argv[]) {
	struct dirent *dentry;
	struct stat statbuf;
	char filename[DIRECTORY_SIZE+1];
	DIR *dirp;

	//Comment: 인자를 한개 받는다.
	if (argc < 2) {
		fprintf(stderr, "usage: %s <directory>\n", argv[0]);
		exit(1);
	}

	//Comment: 해당 경로의 DIR구조체를 얻어온 뒤, 얻는데 성공했다면(폴더가 있다면) 해당 폴더로 현재 작업디렉토리를 설정
	if((dirp = opendir(argv[1]))==NULL || chdir(argv[1])==-1) {
		fprintf(stderr, "opendir, chdir error for %s\n", argv[1]);
		exit(1);
	}

	//Comment: 해당 디렉토리의 파일들을 끝까지 읽어들임
	while((dentry = readdir(dirp)) != NULL) {
		//Comment: 파일이 삭제된 파일이라면 다음 파일로
		if (dentry->d_ino == 0)
			continue;

		//Comment: 파일 이름 복사
		memcpy(filename, dentry->d_name, DIRECTORY_SIZE);

		//Comment: 스탯구조체를 가져옴
		if(stat(filename, &statbuf) == -1) {
			fprintf(stderr, "stat error for %s\n", filename);
			break;
		}

		//Comment: 일반 파일일 경우 크기까지 출력
		if ((statbuf.st_mode & S_IFMT) == S_IFREG)
			printf("%-14s %ld\n", filename, statbuf.st_size);
		else
			printf("%-14s\n", filename);
	}
	exit(0);
}
