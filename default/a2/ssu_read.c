#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include "ssu_employee.h"

int main(int argc, char *argv[])
{
	struct ssu_employee record;
	int fd;
	int record_num;

	//Comment: 하나이상의 인자를 필요로한다.
	if(argc < 2) {
		fprintf(stderr, "Usage : %s file\n", argv[0]);
		exit(1);
	}

	//Comment: 첫번째 인자의 값을 읽기 전용으로 오픈한다.
	if((fd = open(argv[1], O_RDONLY)) < 0){
		fprintf(stderr, "open error for %s\n", argv[1]);
		exit(1);
	}

	while(1) {
		printf("Enter record number : ");
		scanf("%d", &record_num);

		if(record_num < 0)
			break;

		//Comment: 파일을 record_num을 인덱스 삼아 record자료형의 크기만큼 건너뛴다.
		//	record단위로 파일을 읽을 수 있게 된다.
		if(lseek(fd, (long)record_num * sizeof(record), 0) < 0){
			fprintf(stderr, "lseek error\n");
			exit(1);
		}

		//Comment: 레코드의 자료형의 크기만큼 파일을 읽어들이는데
		//	이를 record변수에 저장한다.
		//	record에 값이 채워지게 된다.
		if(read(fd, (char *)&record, sizeof(record)) > 0)
			printf("Employee : %s Salary : %d\n", record.name, record.salary);
		else
			printf("Record %d not found\n", record_num);
	}
		close(fd);
		exit(0);
}
