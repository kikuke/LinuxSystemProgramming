#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <dirent.h>
#include <limits.h>
#include <string.h>
#include <sys/stat.h>

//Comment: PATH_MAX 매크로가 있다면 패스 길이를 최대 경로 길이로 하고, 정의되지 않았다면 0으로 하라.
#ifdef PATH_MAX
static int pathmax = PATH_MAX;
#else
static int pathmax = 0;
#endif

#define MAX_PATH_GUESSED 1024

//Comment: 라인 최대 길이를 2048로 하라
#ifndef LINE_MAX
#define LINE_MAX 2048
#endif

char *pathname;
char command[LINE_MAX], grep_cmd[LINE_MAX];

int ssu_do_grep(void) {
	struct dirent *dirp;
	struct stat statbuf;
	char *ptr;
	DIR *dp;

	//Comment: lstat이므로 symbolic link일 경우 심볼릭을 가져오게 됨.
	if (lstat(pathname, &statbuf) < 0) {
		fprintf(stderr, "lstat error for %s\n", pathname);
		return 0;
	}

	//Comment: 디렉토리가 아니라면 grep_cmd에 있는 명령어를 앞에 두고, 경로를 합쳐 명령어로 만들겠다.
	//	경로를 출력하고, system 명령어로 실행하겠다.
	if (S_ISDIR(statbuf.st_mode) == 0) {
		sprintf(command, "%s %s", grep_cmd, pathname);
		printf("%s : \n", pathname);
		system(command);
		return 0;
	}

	//Comment: ptr을 pathname의 맨 끝 \0이 있는 자리로 옮기고 이것을 /로 대체 후 뒷자리를 공백으로 채운다.
	//	재귀호출 될 경우 뒤에 디렉토리명을 붙이기 위해.
	ptr = pathname + strlen(pathname);
	//Comment: 해당 위치에 대입연산이 진행된 후 후위연산 증가가 진행됨
	*ptr++ = '/';
	*ptr = '\0';

	//Comment: 명령어가 실행된 디렉토리를 열음.
	if ((dp = opendir(pathname)) == NULL) {
		fprintf(stderr, "opendir error for %s\n", pathname);
		return 0;
	}

	//Comment: 디렉토리를 다 둘러볼 때 까지
	while ((dirp = readdir(dp)) != NULL) {
		//Comment: .과 ..이 아닐 경우
		if (strcmp(dirp->d_name, ".") && strcmp(dirp->d_name, "..")) {
			//Comment: /다음에 해당 파일의 이름이 오게됨.
			strcpy(ptr, dirp->d_name);

			//Comment: 자신을 재귀호출
			if (ssu_do_grep() < 0)
				break;
		}
	}

	//Comment: 이 함수가 다 돌게되면 파일명이 뒤에 붙어있게 될 수 있으므로 /포함 뒤를 잘라줌.
	ptr[-1] = 0;
	closedir(dp);
	return 0;
}

//Comment: grep 뒤에 argv로 들어온 인자들을 집어넣는다.
void ssu_make_grep(int argc, char *argv[]) {
	int i;
	strcpy(grep_cmd, "grep");

	//Comment: 들어온 인자들을 넣어줌
	//	경로는 빠지게 됨.
	for (i = 1; i < argc-1; i++) {
		strcat(grep_cmd, " ");
		strcat(grep_cmd, argv[i]);
	}
}

int main(int argc, char *argv[])
{
	if (argc < 2) {
		fprintf(stderr, "usage: %s <-Cvbchilnsvwx> <-num> <-A num> <-B num> <-f file> \n"
		"<-e> expr <directory>\n", argv[0]);
		exit(1);
	}

	//Comment: 따로 설정된 패스길이가 없을 때
	if (pathmax == 0) {
		//Comment: 시스템에서 다른 파일이 파일 시스템 매개변수에 대해 다른 값을 갖도록 허용하는 경우 값을 가져옴
		//	가져온 패스 길이를 할당함.
		if ((pathmax = pathconf("/", _PC_PATH_MAX)) < 0){
			pathmax = MAX_PATH_GUESSED;
		}
		else
			pathmax++;
	}

	//Comment: 설정한 길이만큼 메모리 할당
	if((pathname = (char *)malloc(pathmax+1)) == NULL) {
		fprintf(stderr, "malloc error\n");
		exit(1);
	}

	//Comment: 경로명을 빼옴
	strcpy(pathname, argv[argc-1]);
	ssu_make_grep(argc, argv);
	ssu_do_grep();
	exit(0);
}
