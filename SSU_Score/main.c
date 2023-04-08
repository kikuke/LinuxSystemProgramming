#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/time.h>
#include "ssu_score.h"

#define SECOND_TO_MICRO 1000000

void ssu_runtime(struct timeval *begin_t, struct timeval *end_t);

int main(int argc, char *argv[])
{
	struct timeval begin_t, end_t;
	//현재 시간을 가져옴
	gettimeofday(&begin_t, NULL);

	//ssu_score에 인자를 그대로 넘김
	ssu_score(argc, argv);

	//현재 시간을 가져옴
	gettimeofday(&end_t, NULL);
	//걸린 시간을 측정 및 출력
	ssu_runtime(&begin_t, &end_t);

	exit(0);
}

//걸린 시간을 측정 및 출력
void ssu_runtime(struct timeval *begin_t, struct timeval *end_t)
{
	//종료시간(초) - 시작시간
	end_t->tv_sec -= begin_t->tv_sec;

	//종료 시작시간의 마이크로초가 시작시간보다 작은 경우 초에서 값을 가져옴
	if(end_t->tv_usec < begin_t->tv_usec){
		end_t->tv_sec--;
		end_t->tv_usec += SECOND_TO_MICRO;
	}

	//종료시간(마이크로 초) - 시작시간
	end_t->tv_usec -= begin_t->tv_usec;
	printf("Runtime: %ld:%06ld(sec:usec)\n", end_t->tv_sec, end_t->tv_usec);
}
