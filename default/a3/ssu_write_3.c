#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include "ssu_employee.h"

int main(int argc, char *argv[])
{
	struct ssu_employee record;
	int fd;
	
	//Comment: 최소 한개의 인자를 받음
	if(argc < 2){
		fprintf(stderr, "usage : %s file\n", argv[0]);
		exit(1);
	}

	//Comment: 인자로 들어온 값의 경로의 파일을 새로 만듦. 있으면 에러.
	if((fd = open(argv[1], O_WRONLY | O_CREAT | O_EXCL, 0640)) < 0){
		fprintf(stderr, "open error for %s\n", argv[1]);
		exit(1);
	}

	while (1){
		//Comment: 이름 입력
		printf("Enter employee name <SPACE> salary: ");
		scanf("%s", record.name);

		//Comment: . 인경우 종료
		if(record.name[0] == '.')
			break;

		//Comment: 레코드에 월급과 현재 프로세스의 pid를 입력
		//	인자로 들어온 경로에 저장
		scanf("%d", &record.salary);
		record.pid = getpid();
		write(fd, (char *)&record, sizeof(record));
	}

	close(fd);
	exit(0);
}
